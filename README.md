<h1 align="center">chessplusplus</h1>

[![Build Status](https://travis-ci.com/adajed/chessplusplus.svg?branch=master)](https://travis-ci.com/adajed/chessplusplus)

<p align="center">
  A UCI chess engine written in C++.
</p>

## Building
```
make
```

## Implemented non-UCI commands
- `printboard`
  - Prints current position in human friendly way.
- `perft <depth>`
  - Prints the perft value for the current position (and for each move separately).
- `hash`
  - Prints zobrist hash of current position.
