#include <fs/namecache.h>

#include <lucix/fs/super.h>
#include <lucix/fs/inode.h>
#include <lucix/string.h>
#include <lucix/utils.h>

static obj_mem_cache_t *name_cache_slab;

struct list_head name_cache_list;

int fs_name_cache_init()
{
	name_cache_slab = create_obj_mem_cache(sizeof(struct name_entry), 0, 0, "fs-name-cache");
	INIT_LIST_HEAD(&name_cache_list);

	return 0;
}

struct inode *fs_name_cache_fast_lookup(struct inode *parent, const struct qstr *name)
{
	struct list_head *pos = NULL;
	uint64_t irq_state = cpu_irq_save();

	list_for_each(pos, &name_cache_list) {
		struct name_entry *entry = (struct name_entry*)pos;
		if (entry->parent != parent) {
			continue;
		}
		if (memcmp(entry->name, name->str, name->len)) {
			continue;
		}

		cpu_irq_restore(irq_state);
		ino_ref(entry->ino);
		return entry->ino;
	}

	cpu_irq_restore(irq_state);
	return NULL;
}

int fs_name_cache_add_inode(struct inode *parent, const struct qstr *name, struct inode *ino)
{
	struct name_entry *entry = NULL;
	uint64_t irq_state = 0;
	entry = mem_cache_alloc_obj(name_cache_slab);
	memset(entry, 0, sizeof(struct name_entry));
	ino_ref(ino);
	ino_ref(parent);

	entry->parent = parent;
	entry->ino = ino;
	memcpy(entry->name, name->str, name->len);

	irq_state = cpu_irq_save();
	list_add(&entry->list, &name_cache_list);
	cpu_irq_restore(irq_state);

	return 0;
}

int fs_name_cache_reap()
{
	struct list_head to_delete = LIST_HEAD_INIT(to_delete);
	struct list_head *pos = NULL;
	struct list_head *tmp = NULL;
	uint64_t irq_state = cpu_irq_save();

	list_for_each_safe(pos, tmp, &name_cache_list) {
		struct name_entry *entry = (struct name_entry*)pos;
		ino_deref(entry->ino);
		ino_deref(entry->parent);
		mem_cache_free_obj(name_cache_slab, entry);
	}

	cpu_irq_restore(irq_state);
	return 0;
}
