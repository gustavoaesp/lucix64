#ifndef _SLAB_H_
#define _SLAB_H_

#include <lucix/list.h>
#include <stdint.h>

#define	CACHE_NAME_SIZE	(16)

typedef struct obj_mem_cache_s obj_mem_cache_t;

void obj_mem_cache_init();
obj_mem_cache_t* create_obj_mem_cache(uint32_t object_size, uint32_t flags, uint32_t page_order, const char* name);

void* mem_cache_alloc_obj(obj_mem_cache_t* cache);
void mem_cache_free_obj(obj_mem_cache_t* cache, void* obj);

void *kmalloc(size_t size, uint32_t flags);
void kfree(void*);

#endif
