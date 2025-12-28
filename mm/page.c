#include <lucix/mm.h>

void set_page_dirty(struct page *p)
{
	p->flags |= PAGE_CACHE_FLAG_DIRTY;
}
