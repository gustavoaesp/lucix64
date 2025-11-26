#include <lucix/string.h>
#include <lucix/utils.h>
#include <lucix/vfs.h>

#include <fs/namecache.h>
#include <fs/lookup.h>

struct namei
{
	const char *remaining_path;
	struct inode *parent;
	struct inode *current;
};

static void get_next_component(const char *path, struct qstr *str)
{
	int count = 0;
	for (count = 0; path[count] && path[count] != '/'; count++);

	str->str = path;
	str->len = count;
}

int vfs_lookup(const char *path, uint32_t flags, struct inode **res)
{
	struct namei nd = {
		.remaining_path = path,
		.parent = NULL,
		.current = NULL
	};
	struct qstr str = {};
	int last_code = 0;

	if (nd.remaining_path[0] == '/') {
		nd.current = mnt_root->root_ino;
		nd.parent = mnt_root->root_ino;
		nd.remaining_path++;
	}

	ino_ref(nd.current);
	ino_ref(nd.parent);

	for (;;) {
		struct inode *ino = NULL;
		int ret = 0;
		while (*nd.remaining_path == '/') nd.remaining_path++;

		get_next_component(nd.remaining_path, &str);
		if (!str.len) {
			/* TODO handle lookup for parent */
			/* This means that we reached the end of the chain and we return the
			* parent in this case */
			if (flags & LOOKUP_PARENT) {
				*res = nd.parent;
				if (nd.current) {
					ino_deref(nd.current);
				}
				return 0;
			}
			*res = nd.current;
			if (nd.parent) {
				ino_deref(nd.parent);
			}
			return last_code;
		} else if (last_code) {
			if (nd.parent) ino_deref(nd.parent);
			if (nd.current) ino_deref(nd.current);
			return last_code;
		}
		nd.remaining_path += str.len;

		ino = fs_name_cache_fast_lookup(nd.current, &str);
		if (!ino) {
			ret = nd.current->ops->lookup(nd.current, str.str, str.len, &ino);
			if (ret) {
				ino_deref(nd.parent);
				nd.parent = nd.current;
				nd.current = NULL;
				last_code = ret;
				continue;
			}
			else {
				fs_name_cache_add_inode(nd.current, &str, ino);
			}
		}
		ino_deref(nd.parent);
		nd.parent = nd.current;
		nd.current = ino;
	}

	return 0;
}
