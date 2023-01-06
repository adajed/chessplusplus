#include "argument_parser.h"
#include "elo_utils.h"
#include "endgame.h"
#include "engine_wrapper.h"
#include "extended_position.h"
#include "movegen.h"
#include "polyglot.h"
#include "timer.h"
#include "uci.h"
#include "zobrist_hash.h"

#include "ctpl_stl.h"

#include <fstream>
#include <set>

const std::string DRAW_NAME = "__draw__";

// result, pgn string
using Result = std::tuple<GameResult, std::string>;

using OpeningLine = std::vector<Move>;

struct GameParams
{
    GameParams(const EngineParams& white, const EngineParams& black, bool debug)
        : engine_white(white),
          engine_black(black),
          time_format(),
          intial_moves(),
          debug(debug)
    {
    }

    EngineParams engine_white, engine_black;
    TimeFormat time_format;
    OpeningLine intial_moves;
    bool debug;
};

Result game(int id, GameParams params)
{
    EngineWrapper engine1(params.engine_white.command, params.engine_white.name,
                          id, params.debug);
    EngineWrapper engine2(params.engine_black.command, params.engine_black.name,
                          id, params.debug);

    engine1.uci();
    engine1.ucinewgame();

    engine2.uci();
    engine2.ucinewgame();

    Timer timers[COLOR_NUM] = {Timer(params.time_format.time_initial_ms,
                                     params.time_format.time_increment_ms),
                               Timer(params.time_format.time_initial_ms,
                                     params.time_format.time_increment_ms)};
    EngineWrapper* engines[COLOR_NUM];

    engines[WHITE] = &engine1;
    engines[BLACK] = &engine2;

    std::cout << "New game: white=" << engines[WHITE]->get_name()
              << " black=" << engines[BLACK]->get_name()
              << " time=" << params.time_format.time_initial_ms << "+"
              << params.time_format.time_increment_ms
              << " moves=";
    engine::Position p{};
    for (Move m : params.intial_moves)
    {
        std::cout << p.san(m) << " ";
        p.do_move(m);
    }
    std::cout << std::endl;

    engines[WHITE]->ucinewgame();
    engines[BLACK]->ucinewgame();

    ExtendedPosition position(engines[WHITE]->get_name(),
                              engines[BLACK]->get_name(),
                              params.time_format.time_initial_ms,
                              params.time_format.time_increment_ms);
    std::vector<Move> moves;
    uint32_t depth = 0;

    while (!position.is_checkmate()
             && !position.is_draw()
             && !position.is_stalemate()
             && timers[WHITE].get_time_left_ms() > 0
             && timers[BLACK].get_time_left_ms() > 0)
    {
        const Color side = position.color();
        engines[side]->position(Position::STARTPOS_FEN, moves);

        Move move = NO_MOVE;
        timers[side].start();
        bool bookmove = depth < params.intial_moves.size();
        std::string score = "";
        if (bookmove)
        {
            move = params.intial_moves[depth];
        }
        else
        {
            CommandGoParams params;
            params.time[WHITE] = timers[WHITE].get_time_left_ms();
            params.time_inc[WHITE] = timers[WHITE].get_time_inc_ms();
            params.time[BLACK] = timers[BLACK].get_time_left_ms();
            params.time_inc[BLACK] = timers[BLACK].get_time_inc_ms();
            move = engines[side]->go(params, side, score);
        }
        timers[side].end();

        if (move == NO_MOVE)
        {
            std::cerr << "ERROR: got NO_MOVE in \"" << position.fen() << "\"" << std::endl;
            std::exit(1);
        }

        std::string timeLeft = timers[side].get_time_left_human();
        position.push_back({move, score, timeLeft, bookmove, position.san(move)});
        position.do_move(move);
        moves.push_back(move);

        depth++;
    }

    engine1.quit();
    engine2.quit();

    GameResult result = GameResult::DRAW;
    if (timers[WHITE].get_time_left_ms() == 0) result = GameResult::BLACK_WIN;
    if (timers[BLACK].get_time_left_ms() == 0) result = GameResult::WHITE_WIN;
    if (position.is_checkmate())
        result = position.color() == WHITE ? GameResult::BLACK_WIN : GameResult::WHITE_WIN;

    position.setGameResult(result);

    return std::make_tuple(result, position.pgn());
}

int main(int argc, char** argv)
{
    Args args = parse_args(argc, argv);

    if (args.engines.size() != 2)
    {
        std::cerr << "Currently only 2 engines are supported" << std::endl;
        return 1;
    }

    std::ofstream fd(args.pgn_file);

    move_bitboards::init();
    zobrist::init();
    bitbase::init();
    endgame::init();
    eco::init();

    PolyglotBook polyglot(args.polyglot, args.seed);
    GameParams params(args.engines[0], args.engines[1], args.debug);

    std::map<std::string, double> points;
    points[args.engines[0].name] = 0.0;
    points[args.engines[1].name] = 0.0;
    points[DRAW_NAME] = 0.0;

    ctpl::thread_pool p(args.num_threads);
    std::vector<std::future<Result>> results(args.getTotalNumGames());
    std::vector<GameParams> gameParams;
    std::set<OpeningLine> openingLines;

    int gameCounter = 0;
    for (const auto& format : args.game_format)
    {
        params.time_format = format.first;
        int numGames = format.second;

        for (int i = 0; i < numGames; ++i)
        {
            if (!args.repeat || i % 2 == 0)
            {
                do
                {
                    params.intial_moves.clear();
                    Position position;
                    uint64_t hash = PolyglotBook::hash(position);
                    int depth = 0;
                    while (depth < args.book_depth && polyglot.contains(hash))
                    {
                        Move move = polyglot.get_random_move(hash, position);
                        position.do_move(move);
                        params.intial_moves.push_back(move);
                        hash = PolyglotBook::hash(position);
                        depth++;
                    }
                } while (openingLines.find(params.intial_moves) != openingLines.end());
                openingLines.insert(params.intial_moves);
            }

            gameParams.push_back(params);
            results[gameCounter++] = p.push(game, params);

            std::swap(params.engine_white, params.engine_black);
        }
    }

    for (int i = 0; i < args.getTotalNumGames(); ++i)
    {
        Result result = results[i].get();

        if (std::get<0>(result) == GameResult::WHITE_WIN)
        {
            points[gameParams[i].engine_white.name] += 1.0;
            std::cout << gameParams[i].engine_white.name << "(white) won" << std::endl;
        }
        else if (std::get<0>(result) == GameResult::BLACK_WIN)
        {
            points[gameParams[i].engine_black.name] += 1.0;
            std::cout << gameParams[i].engine_black.name << "(black) won" << std::endl;
        }
        else
        {
            points[DRAW_NAME] += 1.0;
            std::cout << "draw" << std::endl;
        }

        std::cout << "Current score: "
                  << args.engines[0].name << "=" << points[args.engines[0].name] << " "
                  << args.engines[1].name << "=" << points[args.engines[1].name] << " "
                  << "draw=" << points[DRAW_NAME] << std::endl;
        double eloDiff = computeEloDifferenceFromScores(points[args.engines[0].name], points[DRAW_NAME], points[args.engines[1].name]);
        double marginError = computeErrorMargin(points[args.engines[0].name], points[DRAW_NAME], points[args.engines[1].name]);
        std::cout << "ELO difference = " << eloDiff << " +- " << marginError << " ELO" << std::endl;

        fd << std::get<1>(result);
    }

    std::cout << "Final score: "
              << args.engines[0].name << "=" << points[args.engines[0].name] << " "
              << args.engines[1].name << "=" << points[args.engines[1].name] << " "
                  << "draw=" << points[DRAW_NAME] << std::endl;
    double eloDiff = computeEloDifferenceFromScores(points[args.engines[0].name], points[DRAW_NAME], points[args.engines[1].name]);
    double marginError = computeErrorMargin(points[args.engines[0].name], points[DRAW_NAME], points[args.engines[1].name]);
    std::cout << "ELO difference = " << eloDiff << " +- " << marginError << " ELO" << std::endl;

    return 0;
}
