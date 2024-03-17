#include <lucix/device.h>

struct list_head g_device_table = LIST_HEAD_INIT(g_device_table);
obj_mem_cache_t* g_device_mem_cache = NULL;

void init_device_table()
{
	g_device_mem_cache = create_obj_mem_cache(sizeof(struct device_table_element), 0, 1, "device-table");
}

void add_device_to_table(struct device_table_element* device)
{
	list_add(&device->list, &g_device_table);
};
