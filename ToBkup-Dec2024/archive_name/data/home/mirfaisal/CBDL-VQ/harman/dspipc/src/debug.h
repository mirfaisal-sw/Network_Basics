#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_HEAD "[dev-dspipc]"

#define LEVEL_V 1
#define LEVEL_VV 2
#define LEVEL_VVV 3
#define LEVEL_VVVV 4

#define DEBUG_INFO_LEVEL(level, fmt, ...)                    \
        {                                                    \
                if (level <= current_debug_level)            \
                        printk(DEBUG_HEAD fmt, __VA_ARGS__); \
        }

#define DEBUG_INFO(fmt, args...) printk(DEBUG_HEAD fmt, ##args)
#define DEBUG_ERROR(fmt, args...) printk(DEBUG_HEAD fmt, ##args)

#define DEBUG_TEST(fmt, args...) printk(DEBUG_HEAD fmt, ##args)

#define hb_slogf_error(fmt, args...) \
        do {                         \
                printk(DEBUG_HEAD);  \
                printk(fmt, ##args); \
                printk("\n");        \
        } while (0)

extern int current_debug_level;

#endif
