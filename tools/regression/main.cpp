#include "argument_parser.h"
#include "elo_utils.h"
#include "endgame.h"
#include "engine_wrapper.h"
#include "movegen.h"
#include "polyglot.h"
#include "timer.h"
#include "uci.h"
#include "zobrist_hash.h"

#include "ctpl_stl.h"

#include <fstream>
#include <set>

constexpr int WIN_WHITE = 0;
constexpr int WIN_BLACK = 1;
constexpr int DRAW = 2;

const std::string DRAW_NAME = "__draw__";

// result, pgn string
using GameResult = std::tuple<int, std::string>;

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

GameResult game(int id, GameParams params)
{
    EngineWrapper engine1(params.engine_white.command, params.engine_white.name,
                          id, params.debug);
    EngineWrapper engine2(params.engine_black.command, params.engine_black.name,
                          id, params.debug);

    engine1.uci();
    engine2.uci();

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
              << params.time_format.time_increment_ms << std::endl;

    engines[WHITE]->ucinewgame();
    engines[BLACK]->ucinewgame();

    Position position;
    std::vector<Move> moves;
    // bookmove, time left, engine eval
    std::vector<std::tuple<bool, std::string, std::string>> moves_info;

    uint32_t depth = 0;

    while (!position.is_checkmate() && !position.is_draw() &&
           timers[WHITE].get_time_left_ms() > 0 &&
           timers[BLACK].get_time_left_ms() > 0)
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
        moves.push_back(move);
        moves_info.push_back(std::make_tuple(bookmove, timers[side].get_time_left_human(), score));
        position.do_move(move);

        depth++;
    }

    engine1.quit();
    engine2.quit();

    int result = DRAW;
    if (timers[WHITE].get_time_left_ms() == 0) result = WIN_BLACK;
    if (timers[BLACK].get_time_left_ms() == 0) result = WIN_WHITE;
    if (position.is_checkmate())
        result = position.color() == WHITE ? WIN_BLACK : WIN_WHITE;

    std::string resultString = "";
    if (result == DRAW)
        resultString = "1/2-1/2";
    else if (result == WIN_WHITE)
        resultString = "1-0";
    else if (result == WIN_BLACK)
        resultString = "0-1";

    std::stringstream ss;
    ss << "[Event \"?\"]" << std::endl;
    ss << "[Site \"?\"]" << std::endl;
    ss << "[White \"" << engines[WHITE]->get_name() << "\"]" << std::endl;
    ss << "[Black \"" << engines[BLACK]->get_name() << "\"]" << std::endl;
    ss << "[Result \"" << resultString << "\"]" << std::endl;
    ss << std::endl;
    Position temp_position;
    for (uint32_t i = 0; i < moves.size(); i++)
    {
        if (i % 2 == 0)
        {
            ss << (i / 2) + 1 << ". ";
        }
        ss << temp_position.san(moves[i]) << " ";
        if (std::get<0>(moves_info[i]))
            ss << "{ book } ";

        ss << "{ ";
        if (!std::get<0>(moves_info[i]))
            ss << "[\%eval " << std::get<2>(moves_info[i]) << "] ";

        ss << "[\%clk " << std::get<1>(moves_info[i]) << "] } ";

        temp_position.do_move(moves[i]);
    }
    ss << resultString << std::endl << std::endl;

    return std::make_tuple(result, ss.str());
}

int main(int argc, char** argv)
{
    Args args = parse_args(argc, argv);

    if (args.engines.size() < 2)
    {
        std::cerr << "At least 2 engines necessary" << std::endl;
        return 1;
    }

    std::ofstream fd(args.pgn_file);

    init_move_bitboards();
    init_zobrist();
    bitbase::init();
    endgame::init();

    PolyglotBook polyglot(args.polyglot, args.seed);
    GameParams params(args.engines[0], args.engines[1], args.debug);
    params.time_format = args.time_format;

    std::map<std::string, double> points;
    points[args.engines[0].name] = 0.0;
    points[args.engines[1].name] = 0.0;
    points[DRAW_NAME] = 0.0;

    ctpl::thread_pool p(args.num_threads);
    std::vector<std::future<GameResult>> results(args.num_games);
    std::vector<GameParams> gameParams;
    std::set<OpeningLine> openingLines;

    for (int i = 0; i < args.num_games; ++i)
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
                    Move move = polyglot.sample_move(hash, position);
                    position.do_move(move);
                    params.intial_moves.push_back(move);
                    hash = PolyglotBook::hash(position);
                    depth++;
                }
            } while (openingLines.find(params.intial_moves) != openingLines.end());
            openingLines.insert(params.intial_moves);
        }

        gameParams.push_back(params);
        results[i] = p.push(game, params);

        std::swap(params.engine_white, params.engine_black);
    }

    for (int i = 0; i < args.num_games; ++i)
    {
        GameResult gameResult = results[i].get();

        if (std::get<0>(gameResult) == WIN_WHITE)
        {
            points[gameParams[i].engine_white.name] += 1.0;
            std::cout << gameParams[i].engine_white.name << "(white) won" << std::endl;
        }
        else if (std::get<0>(gameResult) == WIN_BLACK)
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

        fd << std::get<1>(gameResult);
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
