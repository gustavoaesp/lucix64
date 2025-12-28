#include <lucix/slab.h>
#include <lucix/mm.h>
#include <lucix/page.h>
#include <lucix/printk.h>
#include <arch/paging.h>

#define FREE_LIST_END		(0xdeadbeef)
#define SLAB_FREE_LISTP(s)	((uint32_t*)((slab_t*)((s) + 1)))

struct malloc_prop
{
	size_t			size;
	obj_mem_cache_t*	cache;
	uint32_t		order;
	char			*name;
};

#define	KMALLOC_SIZES	(11)
static struct malloc_prop malloc_sizes[] = {
	{ .size = 32,  .cache = NULL, .name = "kmalloc-32", .order = 0 },
	{ .size = 64,  .cache = NULL, .name = "kmalloc-64", .order = 0 },
	{ .size = 128, .cache = NULL, .name = "kmalloc-128",.order = 0 },
	{ .size = 256, .cache = NULL, .name = "kmalloc-256",.order = 1 },
	{ .size = 512, .cache = NULL, .name = "kmalloc-512",.order = 1 },
	{ .size = 1024,.cache = NULL, .name = "kmalloc-1024", .order = 1 },
	{ .size = 2048,.cache = NULL, .name = "kmalloc-2048", .order = 1 },
	{ .size = 4096,.cache = NULL, .name = "kmalloc-4096", .order = 1 },
	{ .size = 8192,.cache = NULL, .name = "kmalloc-8192", .order = 2 },
	{ .size = 16*1024, .cache = NULL, .name = "kmalloc-16384", .order = 4},
	{ .size = 32*1024, .cache = NULL, .name = "kmalloc-32768", .order = 4},
	{ .size = 0, .cache = NULL, .name = NULL, .order = 0 } // null element
};


struct slab
{
	struct list_head	list;
	void			*mem;
	uint32_t		in_use;
	uint32_t		next_free;
};
typedef struct slab slab_t;

enum obj_mem_cache_flags {
	OBJMEM_CACHE_NOFLAGS = 0,
	OBJMEM_CACHE_SLAB_META_IN_BLOCK = 1,
};

struct obj_mem_cache_s
{
	struct list_head	list;
	struct list_head	slabs_free;
	struct list_head	slabs_full;
	struct list_head	slabs_partial;

	uint32_t	flags;

	uint32_t	object_size;
	uint32_t	objs_per_slab;

	uint32_t	page_order;

	char		name[CACHE_NAME_SIZE];
};

static obj_mem_cache_t	root_cache;

static void __calc_num_objects(obj_mem_cache_t* cache)
{
	uint32_t slab_size = PAGE_SIZE << cache->page_order;
	uint32_t count = 0;

	if (!cache->flags & OBJMEM_CACHE_SLAB_META_IN_BLOCK) {
		cache->objs_per_slab = slab_size / cache->object_size;
		return;
	}

	while (++count < slab_size) {
		int desc_size = sizeof(slab_t) + count * sizeof(uint32_t);
		int objs_size = cache->object_size * count;

		if (desc_size + objs_size > slab_size) {
			cache->objs_per_slab = count - 1;
			return;
		}
	}
}

static void __initialize_cache(obj_mem_cache_t* cache,
	uint32_t object_size,
	uint32_t page_order,
	const char* name)
{
	strcpy(cache->name, name);
	cache->object_size = object_size;
	cache->page_order = page_order;
	cache->flags = 0;

	if (object_size <= (PAGE_SIZE << cache->page_order) / 8) {
		cache->flags |= OBJMEM_CACHE_SLAB_META_IN_BLOCK;
	};

	__calc_num_objects(cache);

	INIT_LIST_HEAD(&cache->slabs_free);
	INIT_LIST_HEAD(&cache->slabs_full);
	INIT_LIST_HEAD(&cache->slabs_partial);
	list_add(&cache->list, &root_cache.list);
}

void obj_mem_cache_init()
{
	INIT_LIST_HEAD(&root_cache.list);
	__initialize_cache(&root_cache, sizeof(obj_mem_cache_t), 0, "root_cache");

	for (int i = 0; i < KMALLOC_SIZES; ++i) {
		malloc_sizes[i].cache = create_obj_mem_cache(
			malloc_sizes[i].size, 0,
			malloc_sizes[i].order,
			malloc_sizes[i].name
		);
	}
}

obj_mem_cache_t* create_obj_mem_cache(uint32_t object_size, uint32_t flags, uint32_t page_order, const char* name)
{
	obj_mem_cache_t* cache = mem_cache_alloc_obj(&root_cache);
	__initialize_cache(cache, object_size, page_order, name);
	return cache;
}

slab_t* __new_slab(obj_mem_cache_t* cache)
{
	struct page* slab_pages = alloc_pages(PGALLOC_KERNEL, cache->page_order);
	void *slab_pages_vaddr = get_page_vaddr(slab_pages);
	slab_t* slab;
	int mem_offset = 0;

	if (cache->flags & OBJMEM_CACHE_SLAB_META_IN_BLOCK) {
		slab = slab_pages_vaddr;
		mem_offset = sizeof(slab_t) + sizeof(uint32_t) * cache->objs_per_slab;
	} else {
		slab = (slab_t*)kmalloc(sizeof(slab_t) + sizeof(uint32_t) * cache->objs_per_slab, 0);
	}

	for (int p = 0; p < (1 << cache->page_order); ++p) {
		slab_pages[p].list.next = (struct list_head*)cache;
		slab_pages[p].list.prev = (struct list_head*)slab;
	}

	for (int i = 0; i < cache->objs_per_slab; ++i) {
		SLAB_FREE_LISTP(slab)[i] = i + 1;
	}

	SLAB_FREE_LISTP(slab)[cache->objs_per_slab - 1] = FREE_LIST_END;
	slab->next_free = 0;
	slab->in_use = 0;
	slab->mem = slab_pages_vaddr + mem_offset;

	return slab;
}

void* __alloc_from_slab(slab_t* slab)
{
	struct page* page = get_page_from_vaddr(slab->mem);
	obj_mem_cache_t* cache = (obj_mem_cache_t*)page->list.next;

	// This should never happen
	if (slab->next_free == FREE_LIST_END) {
		return NULL;
	}

	void* objp = slab->mem + slab->next_free * cache->object_size;
	slab->next_free = SLAB_FREE_LISTP(slab)[slab->next_free];

	if (slab->next_free == FREE_LIST_END) {
		list_del_init(&slab->list);
		list_add(&slab->list, &cache->slabs_full);
	}

	slab->in_use++;
	return objp;
}

void* mem_cache_alloc_obj(obj_mem_cache_t* cache)
{
	slab_t* slab;
	void* objp;
	if (!list_empty(&cache->slabs_partial)) {
		return __alloc_from_slab((slab_t*)cache->slabs_partial.next);
	}

	if (!list_empty(&cache->slabs_free)) {
		slab = (slab_t*)cache->slabs_free.next;
		objp = __alloc_from_slab(slab);

		list_del_init(&slab->list);
		list_add(&slab->list, &cache->slabs_partial);

		return objp;
	}

	slab = __new_slab(cache);
	objp = __alloc_from_slab(slab);
	list_add(&slab->list, &cache->slabs_partial);

	return objp;
}

void __free_from_slab(slab_t* slab, uint32_t offset)
{
	SLAB_FREE_LISTP(slab)[offset] = slab->next_free;
	slab->next_free = offset;
	slab->in_use--;
}

void mem_cache_free_obj(obj_mem_cache_t* cache, void* obj)
{
	struct page* page = get_page_from_vaddr(obj);
	struct slab* slab = (struct slab*)page->list.prev;
	uint64_t mem_phys, obj_phys, offset;

	mem_phys = VA2PA(slab->mem);
	obj_phys = VA2PA(obj);

	offset = obj_phys - mem_phys;
	offset /= cache->object_size;

	__free_from_slab(slab, offset);

	if (!slab->in_use) {
		list_del(&slab->list);
		list_add(&slab->list, &cache->slabs_free);
	}
}

void *kmalloc(size_t size, uint32_t flags)
{
	for (struct malloc_prop* malloc = malloc_sizes; malloc->size; malloc++) {
		if (size > malloc->size)
			continue;
		return mem_cache_alloc_obj(malloc->cache);
	}

	return NULL;
}

void kfree(void* ptr)
{
	struct page* page = get_page_from_vaddr(ptr);
	obj_mem_cache_t* cache = (obj_mem_cache_t*)page->list.next;
	mem_cache_free_obj(cache, ptr);
}
