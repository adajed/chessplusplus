import re

class Node:
    def __init__(self, fen, moves, alpha, beta, result, depth, ply, nodes):
        self.fen = fen
        self.moves = moves
        self.alpha = alpha
        self.beta = beta
        self.result = result
        self.depth = depth
        self.ply = ply
        self.nodes = nodes


class Chunk:
    def __init__(self, lines):
        self._pos = 0
        self._lines = lines

    def hasNext(self):
        return self._pos < len(self._lines)

    def getNext(self):
        line = self._lines[self._pos]
        self._pos += 1
        return line

    def prev(self):
        assert(self._pos > 0)
        self._pos -= 1


ENTER_SEARCH_REGEX = r"^ENTER (SEARCH|QUIESCENCE_SEARCH) ply=(\d+) depth=(\d+) alpha=(-?\d+) beta=(-?\d+) fen=(.*)$"
EXIT_SEARCH_REGEX = r"^EXIT (SEARCH|QUIESCENCE_SEARCH) ply=(\d+) score=(-?\d+)$"
DO_MOVE_REGEX = r"^DO MOVE (\w+) alpha=(-?\d+) beta=(-?\d+)$"
UNDO_MOVE_REGEX = r"UNDO MOVE (\w+) score=(-?\d+)$"


def print_node(node, tab=""):
    print(f"{tab}Node [{node.ply}] depth={node.depth} alpha={node.alpha} beta={node.beta} result={node.result} fen={node.fen}")
    for (move, n) in node.nodes:
        print(f"{tab}{move}")
        print_node(n, tab + "\t")

# Splits 'lines' on lines that match pattern
def split_into_chunks(lines, pattern):
    chunks = []
    current_chunk = []
    for line in lines:
        if (re.match(pattern, line)):
            chunks.append(current_chunk)
            current_chunk = [line]
        else:
            current_chunk.append(line)
    if len(current_chunk) > 0:
        chunks.append(current_chunk)
    return chunks


def parse_move_order(line):
    moves = re.findall(r"\((\w+),(\d+)\)", line)
    return list(map(lambda m: (m[0], int(m[1])), moves))


def parse_node(chunk, moves):
    line = chunk.getNext()
    enter_search_match = re.match(ENTER_SEARCH_REGEX, line)

    q = False if enter_search_match.group(1) == "SEARCH" else True
    ply = int(enter_search_match.group(2))
    depth = int(enter_search_match.group(3))
    alpha = int(enter_search_match.group(4))
    beta = int(enter_search_match.group(5))
    fen = enter_search_match.group(6)
    move_order = None
    nodes = []

    while chunk.hasNext():
        line = chunk.getNext()

        if re.match(ENTER_SEARCH_REGEX, line):
            chunk.prev()
            nodes.append((moves[-1], parse_node(chunk, moves)))
        elif (m := re.match(DO_MOVE_REGEX, line)):
            moves.append(m.group(1))
        elif (m := re.match(UNDO_MOVE_REGEX, line)):
            assert(len(moves) > 0)
            assert(moves[-1] == m.group(1))
            moves.pop()
        elif re.match(r"^Move order: .*$", line):
            move_order = parse_move_order(line)
        elif (m := re.match(EXIT_SEARCH_REGEX, line)):
            assert(q == (m.group(1) == "QUIESCENCE_SEARCH"))
            assert(int(m.group(2)) == ply)
            return Node(fen=fen, ply=ply, depth=depth, alpha=alpha, beta=beta, result=int(m.group(3)), nodes=nodes, moves=move_order)



def parse_search_chunk(chunk):
    while chunk.hasNext():
        line = chunk.getNext()
        if (re.match(ENTER_SEARCH_REGEX, line)):
            chunk.prev()
            return parse_node(chunk, [])

def parse(filepath):
    print("Loading logs...")
    with open(filepath, "r") as fd:
        lines = fd.readlines()

    search_chunks = split_into_chunks(lines, r"^Search depth=.*$")[1:]
    print("Parsing logs...")
    nodes = list(map(lambda chunk: parse_search_chunk(Chunk(chunk)), search_chunks))
    return nodes
