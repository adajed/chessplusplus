#!/bin/bash

ENGINE1_COMMAND=$(pwd)/build/chessplusplus
ENGINE1_NAME=chessplusplus
ENGINE1="command=${ENGINE1_COMMAND} name=${ENGINE1_NAME}"

# ENGINE2_COMMAND=$(pwd)/master_build/chessplusplus
ENGINE2_COMMAND=$(pwd)/../shallow-blue/shallowblue
ENGINE2_NAME=shallowblue
ENGINE2="command=${ENGINE2_COMMAND} name=${ENGINE2_NAME}"

TIME_FORMAT=$1
NUM_GAMES=$2

POLYGLOT=$(pwd)/../polyglot/Book.bin
BOOKDEPTH=10

./build/regression \
    --engine ${ENGINE1} \
    --engine ${ENGINE2} \
    --time ${TIME_FORMAT} \
    --numgames ${NUM_GAMES} \
    --polyglot ${POLYGLOT} \
    --bookdepth ${BOOKDEPTH} \
    --debug
