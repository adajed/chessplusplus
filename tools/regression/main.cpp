#include "engine_wrapper.h"

#include "endgame.h"
#include "movegen.h"
#include "polyglot.h"
#include "uci.h"
#include "zobrist_hash.h"

#include "argument_parser.h"
#include "timer.h"

constexpr int WIN_WHITE = 0;
constexpr int WIN_BLACK = 1;
constexpr int DRAW = 2;

struct GameParams
{
    GameParams(EngineWrapper* white, EngineWrapper* black, const PolyglotBook& polyglot)
        : engine_white(white), engine_black(black), time_format(), polyglot(polyglot), book_depth(0), intial_moves()
    {
    }

    EngineWrapper* engine_white;
    EngineWrapper* engine_black;
    TimeFormat time_format;
    PolyglotBook polyglot;
    int book_depth;
    std::vector<Move> intial_moves;
};

int game(GameParams& params)
{
    Timer timers[COLOR_NUM] = {
        Timer(params.time_format.time_initial_ms, params.time_format.time_increment_ms),
        Timer(params.time_format.time_initial_ms, params.time_format.time_increment_ms)
    };
    EngineWrapper* engines[COLOR_NUM];

    engines[WHITE] = params.engine_white;
    engines[BLACK] = params.engine_black;

    std::cout << "New game: white=" << engines[WHITE]->get_name()
              << " black=" << engines[BLACK]->get_name()
              << " time=" << params.time_format.time_initial_ms << "+" << params.time_format.time_increment_ms
              << std::endl;

    engines[WHITE]->ucinewgame();
    engines[BLACK]->ucinewgame();

    Position position;
    std::vector<Move> moves;

    int depth = 0;

    while (!position.is_checkmate() &&
            !position.is_draw() &&
            timers[WHITE].get_time_left_ms() > 0 &&
            timers[BLACK].get_time_left_ms() > 0)
    {
        const Color side = position.side_to_move();
        engines[side]->position(Position::STARTPOS_FEN, moves);

        Move move = NO_MOVE;
        uint64_t hash = PolyglotBook::hash(position);
        timers[side].start();
        if (depth < params.book_depth && params.polyglot.contains(hash))
        {
            move = params.polyglot.sample_move(hash, position);
        }
        else
        {
            CommandGoParams params;
            params.time[WHITE] = timers[WHITE].get_time_left_ms();
            params.time_inc[WHITE] = timers[WHITE].get_time_inc_ms();
            params.time[BLACK] = timers[BLACK].get_time_left_ms();
            params.time_inc[BLACK] = timers[BLACK].get_time_inc_ms();
            move = engines[side]->go(params);
        }
        timers[side].end();
        moves.push_back(move);
        position.do_move(move);

        depth++;
    }

    std::cout << "[Event \"?\"]" << std::endl;
    std::cout << "[Site \"?\"]" << std::endl;
    std::cout << "[White \"" << engines[WHITE]->get_name() << "\"]" << std::endl;
    std::cout << "[Black \"" << engines[BLACK]->get_name() << "\"]" << std::endl;
    std::cout << "[Result \"?\"]" << std::endl;
    std::cout << std::endl;
    Position temp_position;
    for (int i = 0; i < moves.size(); i++) {
        if (i % 2 == 0)
        {
            std::cout << (i / 2) + 1 << ". ";
        }
        std::cout << temp_position.san(moves[i]) << " ";
        temp_position.do_move(moves[i]);
    }
    std::cout << "*" << std::endl;


    if (timers[WHITE].get_time_left_ms() == 0)
        return WIN_BLACK;
    if (timers[BLACK].get_time_left_ms() == 0)
        return WIN_WHITE;
    if (position.is_checkmate())
        return position.side_to_move() == WHITE ? WIN_BLACK : WIN_WHITE;
    return DRAW;
}

int main(int argc, char** argv)
{
    Args args = parse_args(argc, argv);

    if (args.engines.size() < 2)
    {
        std::cerr << "At least 2 engines necessary" << std::endl;
        return 1;
    }

    init_move_bitboards();
    init_zobrist();
    bitbase::init();
    endgame::init();

    EngineWrapper engine1(args.engines[0].command, args.engines[0].name, args.debug);
    EngineWrapper engine2(args.engines[1].command, args.engines[1].name, args.debug);

    engine1.uci();
    engine2.uci();

    PolyglotBook polyglot(args.polyglot);
    GameParams params(&engine1, &engine2, polyglot);
    params.book_depth = args.book_depth;
    params.time_format = args.time_format;

    std::map<std::string, double> points;
    points[engine1.get_name()] = 0.;
    points[engine2.get_name()] = 0.;

    for (int i = 0; i < args.num_games; ++i)
    {
        int result = game(params);

        if (result == WIN_WHITE)
        {
            points[params.engine_white->get_name()] += 1.;
            std::cout << params.engine_white->get_name() << "(white) won" << std::endl;
        }
        else if (result == WIN_BLACK)
        {
            points[params.engine_black->get_name()] += 1.;
            std::cout << params.engine_black->get_name() << "(black) won" << std::endl;
        }
        else
        {
            points[params.engine_white->get_name()] += 0.5;
            points[params.engine_black->get_name()] += 0.5;
            std::cout << "draw" << std::endl;
        }

        std::cout << "Current score: "
                  << engine1.get_name() << "=" << points[engine1.get_name()] << " "
                  << engine2.get_name() << "=" << points[engine2.get_name()] << std::endl;

        std::swap(params.engine_white, params.engine_black);
    }

    engine1.quit();
    engine2.quit();

    std::cout << "Final score:" << std::endl;
    std::cout << engine1.get_name() << " : " << points[engine1.get_name()] << std::endl;
    std::cout << engine2.get_name() << " : " << points[engine2.get_name()] << std::endl;

    return 0;
}
