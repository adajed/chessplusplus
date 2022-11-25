import sys
import curses
import re

from chess_utils import parseMove
from display import Display
from db import Database, CHUNK_FIELDS

FILEPATH = None
NODES = []


def getItemName(mainChunk: dict, chunk: dict, move_order: dict) -> str:
    if chunk["move"] is None:
        depth = chunk["depth"]
        return f"Search depth={depth}"
    if chunk["move"] == "nullmove":
        return "nullmove"
    move_uci = chunk["move"]
    move_san = parseMove(mainChunk["fen"], move_uci)
    if move_uci in move_order:
        s = move_order[move_uci]
        return f"{move_san} ({move_uci})  S={s}"
    return f"{move_san} ({move_uci})"


def run(display: Display, database: Database, chunk_id: int) -> None:
    display.window.clear()

    display.logBarView.setText("Loading chunks...")
    display.display()
    display.refresh()
    curses.doupdate()

    chunk = None if chunk_id is None else database.select_chunk(chunk_id)

    move_order = dict()
    if chunk["move_order"] is not None:
        for m in re.findall(r"\((\w+),(\w+)\)", chunk["move_order"]):
            move_order[m[0]] = int(m[1])

    child_ids = list(map(lambda p: p[0], database.select_child_ids(chunk_id)))
    child_chunks = list(map(lambda id_: database.select_chunk(id_), child_ids))

    items = list(map(lambda c: getItemName(
        chunk, c, move_order), child_chunks)) + ["Exit"]

    display.nodeInfoView.setChunkInfo(chunk)
    if chunk is not None and chunk["move"] is not None:
        display.topBarView.addMove(chunk["move"])

    display.logBarView.setText("")
    display.menuView.setItems(items)

    while True:
        pos = display.menuView.position
        if (pos < len(child_chunks)):
            display.previewView.setChunkInfo(child_chunks[pos])
        else:
            display.previewView.setChunkInfo(
                {field: None for field in CHUNK_FIELDS})

        display.display()
        display.refresh()
        curses.doupdate()

        key = display.window.getch()

        if key in [curses.KEY_ENTER, curses.KEY_RIGHT, ord("\n"), ord('l'), ord('L')]:
            pos = display.menuView.position
            if pos == len(items) - 1:
                break
            else:
                run(display, database, child_ids[pos])
                display.nodeInfoView.setChunkInfo(chunk)
                display.menuView.setItems(items)
                display.menuView.setPosition(pos)

        elif key in [curses.KEY_UP, ord('k'), ord('K')]:
            display.menuView._move(-1)

        elif key in [curses.KEY_DOWN, ord('j'), ord('J')]:
            display.menuView._move(1)

        elif key in [curses.KEY_BACKSPACE, curses.KEY_LEFT, ord('h'), ord('H'), ord('p'), ord('P')]:
            break

        elif key in [ord('u'), ord('U')]:
            display.menuView._move(-1)

        elif key in [ord('d'), ord('D')]:
            display.menuView._move(1)

    display.topBarView.popMove()


def main(stdscr):
    curses.curs_set(0)
    (max_y, max_x) = stdscr.getmaxyx()

    x = max_x
    y = max_y
    start_x = 0
    start_y = 0

    window = stdscr.derwin(y, x, start_y, start_x)
    display = Display(window)
    database = Database(FILEPATH)
    root_id = database.get_root_id()
    run(display, database, root_id)


if __name__ == "__main__":
    FILEPATH = sys.argv[1]
    curses.wrapper(main)
