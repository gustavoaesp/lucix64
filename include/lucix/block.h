#ifndef _LUCIX_BLOCK_H_
#define _LUCIX_BLOCK_H_

#include <stdint.h>

typedef uint32_t sector_t;

struct block_device {
};

void register_block_device(struct block_device);

#endif