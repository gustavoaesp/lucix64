#include <lucix/cdev.h>
#include <lucix/errno.h>

struct char_device_driver *char_device_drivers[MAX_MAJOR];

int register_char_device_driver(uint32_t major, struct char_device_driver *driver)
{
	if (char_device_drivers[major]) {
		return -EEXIST;
	}

	char_device_drivers[major] = driver;

	return 0;
}

int remove_char_device_driver(uint32_t major)
{
	if (major >= MAX_MAJOR) {
		return -EINVAL;
	}

	if (!char_device_drivers[major]) {
		return -ENOENT;
	}

	/* TODO more verification to avoid removing a driver used by opened devices */

	char_device_drivers[major] = NULL;

	return 0;
}

struct char_device_driver *get_driver(uint32_t major)
{
	if (major >= MAX_MAJOR) {
		return NULL;
	}

	return char_device_drivers[major];
}
