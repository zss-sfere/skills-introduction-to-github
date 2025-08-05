#ifndef __SELF_DRIVER_LOG_H__
#define __SELF_DRIVER_LOG_H__

#include "stdio.h"

#define DBG_LEVEL        DBG_INFO


#define _DBG_LOG_X_END         printf("\r\n")
#define dbg_log_line_Debug(lvl, file, line, fmt, ...)    \
do                                                       \
{                                                        \
    printf("(%s:%s:%u) ", lvl, file, line);             \
    printf(fmt, ##__VA_ARGS__);                          \
    _DBG_LOG_X_END;                                      \
}                                                        \
    while (0)

#define dbg_log_line_Info(lvl, fmt, ...)                 \
do                                                       \
{                                                        \
    printf("(%s ) ", lvl);                               \
    printf(fmt, ##__VA_ARGS__);                          \
    _DBG_LOG_X_END;                                      \
}                                                        \
    while (0)



#if (DBG_LEVEL >= DBG_LOG)
#define LOG_D(fmt, ...)      dbg_log_line_Debug("D",__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_D(...)
#endif

#if (DBG_LEVEL >= DBG_INFO)
#define LOG_I(fmt, ...)      dbg_log_line_Info("I",fmt, ##__VA_ARGS__)
#else
#define LOG_I(...)
#endif

#if (DBG_LEVEL >= DBG_WARNING)
#define LOG_W(fmt, ...)      dbg_log_line_Debug("W",__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_W(...)
#endif

#if (DBG_LEVEL >= DBG_ERROR)
#define LOG_E(fmt, ...)      dbg_log_line_Debug("E",__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_E(...)
#endif

#endif /* __SELF_DRIVER_LOG_H__ */