#ifndef CHESS_ENGINE_UTILS_H_
#define CHESS_ENGINE_UTILS_H_

namespace engine
{

#if LOG_LEVEL > 0
#define LOG_INFO(msg, ...) do { fprintf(stderr, msg "\n", ##__VA_ARGS__); } while(false)
#else
#define LOG_INFO(msg, ...) do { } while(false)
#endif

#if LOG_LEVEL > 1
#define LOG_DEBUG(msg, ...) do { fprintf(stderr, msg "\n", ##__VA_ARGS__); } while(false)
#else
#define LOG_DEBUG(msg, ...) do { } while(false)
#endif

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
