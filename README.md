<h1 align="center">chessplusplus</h1>

<p align="center">
  <a href="https://travis-ci.com/adajed/chess-engine"><img src="https://travis-ci.com/adajed/chess-engine.svg?branch=master"></a>
</p>
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
