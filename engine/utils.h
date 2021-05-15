#ifndef CHESS_ENGINE_UTILS_H_
#define CHESS_ENGINE_UTILS_H_

namespace engine
{
#ifdef DEBUG

#define LOG_DEBUG(msg, ...) fprintf(stderr, msg "\n", ##__VA_ARGS__);
#define RETURN_DEBUG(val)                \
    {                                    \
        Value ret = (val);               \
        LOG_DEBUG("exit with %ld", ret); \
        return ret;                      \
    }

#else

#define LOG_DEBUG(msg, ...) \
    {                       \
    }
#define RETURN_DEBUG(val) return (val);

#endif

}  // namespace engine

#endif  // CHESS_ENGINE_UTILS_H_
