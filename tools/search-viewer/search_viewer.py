from parser import *
import sys

import curses
from curses import panel
from menu import MenuItem, Menu

from search_panel import create_search_menu_item

NODES = []


def main(stdscr):
    curses.curs_set(0)
    (max_y, max_x) = stdscr.getmaxyx()

    x = max_x
    y = max_y
    start_x = 0
    start_y = 0

    window = stdscr.derwin(y, x, start_y, start_x)
    menu_list = list(map(lambda n : create_search_menu_item(window, n), NODES))
    main_menu = Menu(menu_list, window)
    main_menu.display()

if __name__ == "__main__":
    NODES = parse(sys.argv[1])
    curses.wrapper(main)
