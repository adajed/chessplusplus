#ifndef CHESS_ENGINE_SEARCH_UTILS_H_
#define CHESS_ENGINE_SEARCH_UTILS_H_

namespace engine {

// how much to reduce search depth for later moves
int late_move_reduction(int depth, int move_number);

} // namespace engine

#endif  // CHESS_ENGINE_SEARCH_UTILS_H
