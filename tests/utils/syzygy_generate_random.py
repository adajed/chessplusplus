import chess
import chess.syzygy
import random
from typing import List


def generate_random_piece() -> chess.Piece:
    return chess.Piece.from_symbol(
            random.choice(['P', 'p', 'N', 'n', 'B', 'b', 'R', 'r', 'Q', 'q']))


def generate_random_position(num_pieces=5) -> chess.Board:
    while True:
        pieces: List[chess.Piece] = [chess.Piece(chess.KING, chess.WHITE),
                                     chess.Piece(chess.KING, chess.BLACK)]
        squares: List[chess.Square] = []

        for _ in range(2, num_pieces):
            pieces.append(generate_random_piece())

        for _ in pieces:
            sq = random.randint(0, 63)
            while sq in squares:
                sq = random.randint(0, 63)
            squares.append(sq)

        board = chess.Board(fen=None)
        for p, sq in zip(pieces, squares):
            board.set_piece_at(sq, p)

        board.turn = chess.WHITE if random.randint(0, 1) == 1 else chess.BLACK

        if (board.is_valid()):
            return board


def wdl_to_str(wdl: int) -> str:
    return ["WDLLoss", "WDLCursedLoss", "WDLDraw", "WDLCursedDraw", "WDLWin"][wdl + 2]


random.seed(42)

with chess.syzygy.open_tablebase("./tests/syzygy/") as tb:
    print("\nWDL\n")
    for _ in range(100):
        board = generate_random_position(num_pieces=5)
        wdl_score = wdl_to_str(tb.get_wdl(board))
        print(f"{{\"{board.fen()}\", TB::ProbeState::OK, TB::WDLScore::{wdl_score}}},")

    print("\nDTZ\n")
    for _ in range(100):
        board = generate_random_position(num_pieces=5)
        dtz = tb.get_dtz(board)
        print(f"{{\"{board.fen()}\", TB::ProbeState::OK, {dtz}}},")
