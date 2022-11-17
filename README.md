<h1 align="center">chessplusplus</h1>

<p align="center">
  A UCI chess engine written in C++.
</p>

## Building
```
make -j8 // builds all targets

make engine // builds engine in realese mode
make engine_debug // builds engine in debug mode

make tools // builds tools in release mode
make tools_debug // builds tools in debug mode

// Tests require google test
make test // builds tests in release mode
make test_debug // builds tests in debug mode
```

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
