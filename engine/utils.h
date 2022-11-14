#ifndef CHESS_ENGINE_UTILS_H_
#define CHESS_ENGINE_UTILS_H_

namespace engine
{
#ifdef DEBUG

#define ASSERT(cond)                                     \
    do                                                             \
    {                                                              \
        if (!(cond))                                               \
        {                                                          \
            fprintf(stderr, "ASSERT FAILED %s:%d:\"" #cond "\"\n", __FILE__, __LINE__);                                \
            std::terminate();                                      \
        }                                                          \
    } while (false)

#define ASSERT_WITH_MSG(cond, msg, ...)                                     \
    do                                                             \
    {                                                              \
        if (!(cond))                                               \
        {                                                          \
            fprintf(stderr, "ASSERT FAILED %s:%d:\"" #cond "\" " msg "\n", __FILE__, __LINE__, \
                    ##__VA_ARGS__);                                \
            std::terminate();                                      \
        }                                                          \
    } while (false)

#define LOG_DEBUG(msg, ...) fprintf(stderr, msg "\n", ##__VA_ARGS__);

#define RETURN_DEBUG(val)                \
    {                                    \
        Value ret = (val);               \
        LOG_DEBUG("exit with %ld", ret); \
        return ret;                      \
    }

#define EXIT_SEARCH(val)                                            \
    {                                                               \
        Value ret = (val);                                          \
        LOG_DEBUG("EXIT SEARCH ply=%d score=%ld", info->_ply, ret); \
        return ret;                                                 \
    }

#define EXIT_QSEARCH(val)                                                       \
    {                                                                           \
        Value ret = (val);                                                      \
        LOG_DEBUG("EXIT QUIESCENCE_SEARCH ply=%d score=%ld", info->_ply, ret);  \
        return ret;                                                             \
    }

#else

#define LOG_DEBUG(msg, ...) \
    do \
    {                       \
    } while(false)

#define RETURN_DEBUG(val) return (val);

#define EXIT_SEARCH(val) return (val);
#define EXIT_QSEARCH(val) return (val);

#define ASSERT(cond) \
    do \
    { \
    } while(false)

#define ASSERT_WITH_MSG(cond, msg, ...) \
    do                         \
    {                          \
    } while (false)

#endif

}  // namespace engine

#endif  // CHESS_ENGINE_UTILS_H_
