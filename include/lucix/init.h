#ifndef _LUCIX_INIT_H_
#define _LUCIX_INIT_H_
#include <lucix/start.h>

struct initramfs {
	void *addr;
	int64_t size;
};

extern struct initramfs initramfs_info;

void start_kernel(struct lucix_startup_data* startup_data);

void kinit_task(void *__unused);

#endif
