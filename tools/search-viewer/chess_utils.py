import chess


def parseMove(fen: str, move_uci: None) -> str:
    if not isinstance(move_uci, str):
        return move_uci
    board = chess.Board(fen=fen)
    return board.san(board.parse_uci(move_uci))


def color(board: chess.Board) -> str:
    return "WHITE" if board.turn() else "BLACK"

