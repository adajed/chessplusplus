#!/bin/bash

trap "{ exit 255; }" INT

PROGRAM="./build/chessplusplus"

run_perft_test()

{
    fen=$1
    depth=$2
    nodes=$3

    NUMBER_OF_PERFT_TESTS=$((NUMBER_OF_PERFT_TESTS + 1))
    expect tests/scripts/perft.exp ${PROGRAM} "${fen}" ${depth} ${nodes} >/dev/null
    if [ $? -eq 0 ]
    then
        PERFT_TESTS_PASSED=$((PERFT_TESTS_PASSED + 1))
        echo "Perft test \"${fen}\" ${depth} passed"
    else
        echo "Perft test \"${fen}\" ${depth} failed"
    fi
}

NUMBER_OF_PERFT_TESTS=0
PERFT_TESTS_PASSED=0

run_perft_test "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 1 20
run_perft_test "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 2 400
run_perft_test "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 3 8902
run_perft_test "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 4 197281
run_perft_test "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 5 4865609
run_perft_test "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 6 119060324


run_perft_test "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - " 1 48
run_perft_test "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - " 2 2039
run_perft_test "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - " 3 97862
run_perft_test "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - " 4 4085603
run_perft_test "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - " 5 193690690


run_perft_test "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - " 1 14
run_perft_test "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - " 2 191
run_perft_test "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - " 3 2812
run_perft_test "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - " 4 43238
run_perft_test "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - " 5 674624
run_perft_test "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - " 6 11030083
run_perft_test "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - " 7 178633661


run_perft_test "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - " 1 6
run_perft_test "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - " 2 264
run_perft_test "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - " 3 9467
run_perft_test "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - " 4 422333
run_perft_test "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - " 5 15833292
run_perft_test "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - " 6 706045033


run_perft_test "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1" 1 6
run_perft_test "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1" 2 264
run_perft_test "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1" 3 9467
run_perft_test "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1" 4 422333
run_perft_test "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1" 5 15833292
run_perft_test "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1" 6 706045033


run_perft_test "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - " 1 44
run_perft_test "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - " 2 1486
run_perft_test "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - " 3 62379
run_perft_test "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - " 4 2103487
run_perft_test "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - " 5 89941194


run_perft_test "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - " 1 46
run_perft_test "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - " 2 2079
run_perft_test "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - " 3 89890
run_perft_test "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - " 4 3894594
run_perft_test "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - " 5 164075551


echo "Perft tests passed: ${PERFT_TESTS_PASSED}/${NUMBER_OF_PERFT_TESTS}"
if [ ${PERFT_TESTS_PASSED} -ne ${NUMBER_OF_PERFT_TESTS} ]
then
    exit 1
fi

