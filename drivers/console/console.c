#include <lucix/cdev.h>
#include <lucix/console.h>
#include <lucix/initcall.h>
#include <lucix/printk.h>
#include <lucix/fs/file.h>

#define CONSOLE_DEV_MAJ	5

static int64_t console_write(struct file *file, const void *src, uint64_t bytes, size_t *pos)
{
	for(int i = 0; i < bytes; ++i) {
		console_putchar(*((char*)src++));
	}

	return bytes;
}

static struct file_ops console_fileops = {
	.write = console_write
};

struct char_device_driver dev_console_device_driver = {
	.fops = &console_fileops,
	.minor_count = 1,
	.minor_start = CONSOLE_DEV_MAJ
};

int console_cdev_init()
{
	printf("Registering console char device\n");
	register_char_device_driver(CONSOLE_DEV_MAJ, &dev_console_device_driver);
	return 0;
}

int console_cdev_unload()
{
	return 0;
}

initcall_device(console_cdev_init);
