#include <lucix/fs/file_system.h>

#include <lucix/string.h>

/*static struct file_system_list {
    struct list_head list;
} file_system_list = {
    .list = LIST_HEAD_INIT(file_system_list.list)
};*/

struct list_head file_system_list = LIST_HEAD_INIT(file_system_list);

void file_system_register(struct file_system* fs)
{
    list_add_tail(&fs->list_entry, &file_system_list);
}

void file_system_unregister(const char *name)
{
    struct list_head *pos = NULL;
    list_for_each(pos, &file_system_list) {
    	struct file_system* fs = list_entry(pos, struct file_system, list_entry);

        if (strcmp(fs->name, name) == 0) {
            list_del(&fs->list_entry);
            break;
        }
    }
}

struct file_system *file_system_lookup(const char* name) {
    struct list_head *pos = NULL;
    list_for_each(pos, &file_system_list) {
    	struct file_system* fs = list_entry(pos, struct file_system, list_entry);
        if (strcmp(fs->name, name) == 0) {
            return fs;
            break;
        }
    }

    return NULL;
}
