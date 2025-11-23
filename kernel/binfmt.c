#include <lucix/binfmt.h>
#include <lucix/errno.h>
#include <lucix/string.h>

struct list_head binfmt_list = LIST_HEAD_INIT(binfmt_list);

int register_binfmt(struct binfmt *fmt)
{
	struct list_head *pos = NULL;

	if (list_empty(&binfmt_list)) {
		list_add(&fmt->list, &binfmt_list);
		return 0;
	}

	list_for_each(pos, &binfmt_list) {
		struct binfmt *list_fmt = (struct binfmt*)pos;
		if (!strcmp(fmt->name, list_fmt->name)) {
			return -EINVAL;
		}
	}

	list_add(&fmt->list, &binfmt_list);

	return 0;
}
