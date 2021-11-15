#ifndef COMMUNITY_H_
#define COMMUNITY_H_

#include <stdint.h>

struct FLAGS_BIT
{
    uint32_t Led_TODO : 1; // LED 模式检查2
    uint32_t rsvd : 31;    // 保留标志位自行拓展
};

typedef union community
{
    uint32_t all;
    struct FLAGS_BIT bit;
} COMM_t;

extern COMM_t comm_pipe;

#endif

