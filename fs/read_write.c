#include <lucix/vfs.h>
#include <lucix/fs/inode.h>
#include <lucix/fs/mapping.h>
#include <lucix/mm.h>
#include <arch/paging.h>

int64_t generic_file_write(struct file *file, const void *src, size_t count, size_t *pos)
{
	struct inode *ino = file->inode;
	int64_t total = 0;
	size_t start_pos = *pos;

	int64_t total_pages = count / PAGE_SIZE;
	total_pages += (count % PAGE_SIZE) ? 1 : 0;

	for (size_t idx = *pos; idx < start_pos + count;) {
		size_t remaining = count - total;
		size_t pg_offset = *pos % PAGE_SIZE;
		struct page *dst_page = NULL;

		size_t to_write = (remaining >= PAGE_SIZE) ? (PAGE_SIZE - pg_offset) : remaining;

		ino->f_map->ops->write_begin(file, ino->f_map,
			*pos,
			to_write,
			&dst_page,
			NULL
		);

		memcpy(dst_page->vaddr + pg_offset, src, to_write);

		dst_page->flags |= PAGE_CACHE_FLAG_DIRTY;

		ino->f_map->ops->write_end(file, ino->f_map,
			*pos, to_write,
			to_write,
			dst_page,
			NULL);

		total += to_write;
		src += to_write;
		*pos += to_write;
		idx += to_write;
	}

	return total;
}

int64_t generic_file_read(struct file *file, void *dst, size_t count, size_t *pos)
{
	struct inode *ino = file->inode;
	int64_t total = 0;
	size_t start_pos = *pos;

	if (*pos >= ino->size) return 0;

	for (int64_t idx = *pos; idx < count + start_pos;) {
		int64_t pg_index = idx / PAGE_SIZE;
		int64_t pg_offset = idx % PAGE_SIZE;
		int64_t remaining_bytes = count - total;
		int64_t to_read = 0;

		if (!ino->f_map->pages[pg_index]) {
			struct page *new_page = alloc_pages(PGALLOC_KERNEL, 0);
			new_page->page_cache_attr.owner = ino->f_map;
			new_page->flags = PAGE_USAGE_CACHE;
			ino->f_map->pages[pg_index] = new_page;
			ino->f_map->ops->readpage(file, ino->f_map, new_page, pg_index * PAGE_SIZE);
			page_ref(new_page);
		}

		to_read = (remaining_bytes >= PAGE_SIZE) ? (PAGE_SIZE - pg_offset) : remaining_bytes;

		/* check that we do not pass beyond the size of the file */
		if (*pos + to_read > ino->size) {
			to_read = ino->size - (*pos);
		}

		memcpy(dst, ino->f_map->pages[pg_index]->vaddr + pg_offset, to_read);

		*pos += to_read;
		dst += to_read;
		total += to_read;

		idx += to_read;
	}

	return total;
}
