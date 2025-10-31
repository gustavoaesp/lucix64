#include <fs/inocache.h>
#include <lucix/slab.h>
#include <lucix/fs/super.h>

struct list_head ino_cache_list;

static obj_mem_cache_t *ino_cache_mem_cache;

int ino_cache_init()
{
    ino_cache_mem_cache = create_obj_mem_cache(sizeof(struct ino_cache_entry), 0, 0, "ino-cache-entry");
    INIT_LIST_HEAD(&ino_cache_list);
}

struct inode *ino_cache_get(const struct super_block *sb, ino_t inode_number)
{
    struct list_head *pos = NULL;
    list_for_each(pos, &ino_cache_list) {
        struct ino_cache_entry *entry = (struct ino_cache_entry*)pos;
        if (entry->ino->id == inode_number && entry->sb == sb) {
            return entry->ino;
        }
    }

    return NULL;
}

int ino_cache_insert(struct super_block *sb, struct inode *ino)
{
    struct ino_cache_entry *new_entry = mem_cache_alloc_obj(ino_cache_mem_cache);
    uint64_t irq_state = cpu_irq_save();

    new_entry->ino = ino;
    new_entry->sb = sb;

    list_add(&new_entry->list, &ino_cache_list);

    cpu_irq_restore(irq_state);
    return 0;
}

int ino_cache_reap()
{
    struct list_head *pos = NULL;
    uint64_t irq_state = cpu_irq_save();

    list_for_each(pos, &ino_cache_list) {
        struct ino_cache_entry *entry = (struct ino_cache_entry*)pos;
        if (entry->ino->refcnt) {
            continue;
        }
        /* TODO check if dirty as well */

        list_del(&entry->list);
        entry->sb->ops->inode_mem_free(entry->sb, entry->ino);
        mem_cache_free_obj(ino_cache_mem_cache, entry);
    }

    cpu_irq_restore(irq_state);
    return 0;
}