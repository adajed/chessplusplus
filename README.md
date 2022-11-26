<h1 align="center">chessplusplus</h1>

<p align="center">
  A UCI chess engine written in C++.
</p>

## Building

#### All targets
```
make -j8
```

#### Build only engine
Release mode
```
make engine -j8
```

Debug mode
```
make engine_debug -j8
```

#### Build tools (regression)
Release mode
```
make tools -j8
```

Debug mode
```
make tools_debug
```

#### Build tests
Tests require Google Test

Release mode
```
make test -j8
```

Debug mode
```
make test_debug -j8
```

#### LOG\_LEVEL
To add additional logs during runtime:
```
make LOG_LEVEL=2 -j8
```

LEVELS:
- 0 - No logging.
- 1 - Small amout of logging, mainly about iter search.
- 2 - Full logging, prints info from whole search tree. This causes massive slowdown!!!

## Implemented non-UCI commands
- `printboard`
  - Prints current position in human friendly way.
- `perft <depth>`
  - Prints the perft value for the current position (and for each move separately).
- `hash`
  - Prints zobrist hash of current position.
- `moves <move [move [move...]]>`
  - Adds moves to current position (doesn't check if moves are legal).
- `staticeval`
  - Prints static eval of current position.
