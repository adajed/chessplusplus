<h1 align="center">chessplusplus</h1>

<p align="center">
  A UCI chess engine written in C++.
</p>

## Building

### Using CMake
```
cmake -S. -Bbuild
cd build
make chessplusplus -j8
```

### Using Make
There is also Makefile available (for those who can't use CMake).  
It only compiles the engine in Release mode.  
Tested on msys2 with g++.

```
make
```

Engine binary will be generated in `.\build` directory.

### LOG\_LEVEL
To add additional logs during runtime:
`cmake -S. -Bbuild -DLOG_LEVEL=2`

LEVELS:
- 0 - No logging.
- 1 - Additional logging in 'info' output.
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
