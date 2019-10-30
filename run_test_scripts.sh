#!/bin/bash

error()
{
  echo "perft testing failed on line $1"
  exit 1
}
trap 'error ${LINENO}' ERR

expect tests/scripts/uci.exp >/dev/null

expect tests/scripts/mate_search.exp "6k1/5ppp/8/8/8/8/8/1RK5 w - - 0 1" 1 b1b8 >/dev/null
expect tests/scripts/mate_search.exp "r5k1/5ppp/8/8/8/8/1R6/1RK5 w - - 0 1" 2 b2b8 >/dev/null
expect tests/scripts/mate_search.exp "rr4k1/5ppp/8/8/8/2R5/2R5/2RK4 w - - 0 1" 3 c3c8 >/dev/null
