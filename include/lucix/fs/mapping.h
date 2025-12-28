#ifndef _FS_MAP_H_
#define _FS_MAP_H_

#include <stddef.h>
#include <stdint.h>

#include <lucix/fs/file.h>

/*struct page_mapping_section
{
	struct list_head list;
	size_t pfn_start;
	struct page **pages;
	size_t nr_pages;
};*/

/*
 * Holds page mappings of the pages that hold the contents of an
 * inode file.
 */
struct page_mapping
{
	void *owner;

	struct page **pages;
	size_t nr_pages;

	struct page_mapping_ops *ops;
};

struct page_mapping_ops
{
	int (*readpage)(struct file*, struct page_mapping *, struct page*, int64_t offs);
	int (*write_begin)(struct file*, struct page_mapping*,
				size_t pos, size_t len, struct page**,
				void *fs_data);
	int (*write_end)(struct file*, struct page_mapping*,
			size_t pos, size_t len, size_t copied,
			struct page*, void *fs_data);
};

int page_mapping_evict_page(struct page_mapping *, struct page *);

#endif
