#!/usr/bin/python3

import chess
import sys
import re

if __name__ == "__main__":
    for line in sys.stdin.readlines():
        if (m := re.match(r"^run_search_test \"(.*)\" (.*)$", line)):
            fen = m.group(1)
            move_san = m.group(2)
            board = chess.Board(fen=fen)
            move_uci = board.uci(board.parse_san(move_san))
            print(f"run_search_test \"{fen}\" {move_uci}")
