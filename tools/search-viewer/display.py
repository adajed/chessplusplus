import curses
import chess

from curses_utils import (print_centered,
                          window_split_horizontally,
                          window_split_vertically,
                          get_centered_position)
from db import CHUNK_FIELDS
from chess_utils import parseMove


def clip(x, min_value, max_value):
    # clips x to be between min_value and max_value
    return min(max(x, min_value), max_value)


class View:
    def __init__(self, window, useBorder=False):
        self.__window = window
        self.__useBorder = useBorder
        if useBorder:
            y, x = self.__window.getmaxyx()
            self.__insideWindow = self.__window.derwin(y - 2, x - 2, 1, 1)

    @property
    def window(self):
        if self.__useBorder:
            return self.__insideWindow
        return self.__window

    @property
    def height(self):
        return self.window.getmaxyx()[0]

    @property
    def width(self):
        return self.window.getmaxyx()[1]

    def refresh(self):
        self.__window.refresh()

    def display(self):
        self.__window.clear()
        self.__window.border()


class LogBarView(View):
    def __init__(self, window):
        View.__init__(self, window, useBorder=True)
        self.__text = None

    def setText(self, text):
        self.__text = text

    def display(self):
        super().display()
        if (self.__text is not None):
            print_centered(self.window, 0, self.__text)


class TopBarView(View):
    def __init__(self, window):
        View.__init__(self, window)
        self.__moves = []

    def addMove(self, move):
        self.__moves.append(move)

    def popMove(self):
        if len(self.__moves) > 0:
            self.__moves.pop()

    def display(self):
        self.window.clear()
        print_centered(self.window, 0, " ".join(self.__moves))


class NodeInfoView(View):
    def __init__(self, window):
        View.__init__(self, window, useBorder=True)
        self.__fen = None

    def setFEN(self, fen):
        self.__fen = fen

    def display(self):
        super().display()
        if self.__fen is not None:
            board = chess.Board(fen=self.__fen)
            board_str = board.__str__()
            ranks = board_str.split("\n")
            print_centered(self.window, 0, f"FEN: {self.__fen}")
            for i, rank in enumerate(ranks):
                print_centered(self.window, 1 + i, rank)
            side = "WHITE" if board.turn == chess.WHITE else "BLACK"
            print_centered(self.window, 10, f"{side} TO MOVE")


class PreviewView(View):
    def __init__(self, window):
        View.__init__(self, window, useBorder=True)
        for field in CHUNK_FIELDS:
            self.__setattr__(field, None)

    def setChunkInfo(self, chunkInfo):
        for field in CHUNK_FIELDS:
            self.__setattr__(field, chunkInfo[field])

    def display(self):
        super().display()
        if self.fen is not None:
            print_centered(
                self.window, 1, f"NODE best move : {parseMove(self.fen, self.best_move)}")
            print_centered(
                self.window, 2, f"ply : {self.ply}  depth : {self.depth}")
            print_centered(
                self.window, 3, f"alpha = {self.alpha}  beta = {self.beta}")
            print_centered(
                self.window, 4, f"result = {self.score} position = {self.static_eval}")
            if (self.pv_list is not None):
                pv_list_san = []
                board = chess.Board(fen=self.fen)
                for move_uci in self.pv_list.split():
                    pv_list_san.append(board.san(board.parse_uci(move_uci)))
                    board.push_uci(move_uci)
                pv_list_san = " ".join(pv_list_san)
                print_centered(self.window, 5, f"pv = {pv_list_san}")

            print_centered(self.window, 6, f"FEN {self.fen}")

            board = chess.Board(fen=self.fen)
            board_str = board.__str__()
            ranks = board_str.split("\n")
            for i, rank in enumerate(ranks):
                print_centered(self.window, 7 + i, rank)
            if board.turn == chess.WHITE:
                print_centered(self.window, 16, "WHITE TO MOVE")
            else:
                print_centered(self.window, 16, "BLACK TO MOVE")

            if (self.cache_move is None):
                print_centered(self.window, 18, "CACHE MISS")
            else:
                flag = "UNKNOWN"
                if (self.cache_flag == 0):
                    flag = "EXACT"
                elif (self.cache_flag == 1):
                    flag = "LOWER BOUND"
                elif (self.cache_flag == 2):
                    flag = "UPPER BOUND"
                print_centered(self.window, 18,
                               f"CACHE Move {parseMove(self.fen, self.cache_move)} Depth {self.cache_depth} Score {self.cache_score} Flag {flag}")


class MenuView(View):
    def __init__(self, window, useIndex):
        View.__init__(self, window, useBorder=True)
        self.__items = None
        self.__position = 0
        self.__useIndex = useIndex

    def setItems(self, items):
        self.__items = items
        self.__position = 0

    def setPosition(self, pos):
        self.__position = pos

    def _move(self, diff):
        self.__position += diff
        self.__position = clip(self.__position, 0, len(self.__items) - 1)

    @property
    def position(self):
        return self.__position

    def display(self):
        super().display()
        if self.__items is not None:
            start_index = max(0, self.__position - self.height // 2)
            end_index = min(len(self.__items), start_index + self.height)
            for index in range(start_index, end_index):
                i = index - start_index
                item = self.__items[index]
                if index == self.__position:
                    mode = curses.A_REVERSE
                else:
                    mode = curses.A_NORMAL

                if self.__useIndex:
                    msg = f"{index}. {item}"
                else:
                    msg = item
                start_pos = get_centered_position(len(msg), self.width)
                self.window.addstr(i, start_pos, msg, mode)


class Display(View):
    def __init__(self, window):
        View.__init__(self, window)

        log_bar, main_window = window_split_horizontally(window, 3)
        top_bar, main_window = window_split_horizontally(main_window, 1)
        menu_window, preview_window = window_split_vertically(main_window, 0.3)
        top_window, bottom_window = window_split_horizontally(
            preview_window, 0.5)

        self.__logBarView = LogBarView(log_bar)
        self.__topBarView = TopBarView(top_bar)
        self.__nodeInfoView = PreviewView(top_window)
        self.__menuView = MenuView(menu_window, useIndex=True)
        self.__previewView = PreviewView(bottom_window)

        # self.loading_panel = panel.new_panel(self.window)
        # self.panel.hide()
        # panel.update_panels()

    @property
    def logBarView(self):
        return self.__logBarView

    @property
    def topBarView(self):
        return self.__topBarView

    @property
    def nodeInfoView(self):
        return self.__nodeInfoView

    @property
    def menuView(self):
        return self.__menuView

    @property
    def previewView(self):
        return self.__previewView

    def display(self):
        self.logBarView.display()
        self.topBarView.display()
        self.nodeInfoView.display()
        self.menuView.display()
        self.previewView.display()