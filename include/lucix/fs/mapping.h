#ifndef _FS_MAP_H_
#define _FS_MAP_H_

/*
 * Holds page mappings of the pages that hold the contents of an
 * inode file.
 */
struct file_page_mapping
{
	struct inode *host;

	struct page **pages;
	size_t nr_pages;

	struct file_page_mapping_ops *ops;
};

struct file_page_mapping_ops
{
	int (*readpage)(struct file*, struct file_page_mapping *, struct page*, int64_t offs);
	int (*write_begin)(struct file*, struct file_page_mapping*,
				size_t pos, size_t len, struct page**,
				void *fs_data);
	int (*write_end)(struct file*, struct file_page_mapping*,
			size_t pos, size_t len, size_t copied,
			struct page*, void *fs_data);
};

int file_page_mapping_evict_page(struct file_page_mapping *, struct page *);

#endif
