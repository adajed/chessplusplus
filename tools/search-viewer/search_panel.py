import curses
from curses import panel
from menu import MenuItem, Menu, get_centered_position
import chess


def print_centered(window, line, text):
    (height, width) = window.getmaxyx()
    start_x = get_centered_position(len(text), width)
    window.addstr(line, start_x, text)


def create_node_menu_item(window, node, fen, name, moves):
    p = SearchPanel(window, node)
    board = chess.Board(fen=fen)
    san = board.san(board.parse_uci(name))
    if name in moves:
        name = f"{name} ({san}) Order: {moves[name]}"
    else:
        name = f"{name} ({san})"
    return MenuItem(name, p)


class SearchPanel:
    def __init__(self, window, node):
        self.window = window.derwin(0, 0)
        self.window.keypad(1)

        self.panel = panel.new_panel(self.window)
        self.panel.hide()
        panel.update_panels()

        self.node = node

    def display(self):
        self.panel.top()
        self.panel.show()
        self.window.clear()

        menu_list = list(map(lambda n : create_node_menu_item(self.window, n[1], self.node.fen, n[0], dict(self.node.moves)), self.node.nodes))
        main_menu = Menu(menu_list, self.window, use_index=False)
        main_menu.display()

        self.window.clear()
        self.panel.hide()
        panel.update_panels()
        curses.doupdate()

    def preview(self, window):
        (height, width) = window.getmaxyx()
        window.clear()

        print_centered(window, 1, "NODE")
        print_centered(window, 2, f"ply : {self.node.ply}  depth : {self.node.depth}")
        print_centered(window, 3, "")
        print_centered(window, 4, f"alpha = {self.node.alpha}  beta = {self.node.beta}")
        print_centered(window, 5, "")
        print_centered(window, 6, f"result = {self.node.result}")
        print_centered(window, 7, "")
        print_centered(window, 8, "FEN")
        print_centered(window, 9, f"{self.node.fen}")

        board = chess.Board(fen=self.node.fen)
        board_str = board.__str__()
        ranks = board_str.split("\n")
        for i, rank in enumerate(ranks):
            print_centered(window, 11 + i, rank)
        if board.turn == chess.WHITE:
            print_centered(window, 20, "WHITE TO MOVE")
        else:
            print_centered(window, 20, "BLACK TO MOVE")

def create_search_menu_item(window, node, name=None):
    p = SearchPanel(window, node)
    if name is None:
        name = f"Search depth={node.depth}"
    return MenuItem(name, p)


