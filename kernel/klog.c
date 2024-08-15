#include <lucix/klog.h>

#include <stdint.h>

#define _KLOG_SIZE (128*1024)

static char __klog_buffer[_KLOG_SIZE] = {};

static uint32_t _count = 0;

void klog_putchar(char c)
{
    __klog_buffer[(_count++) % _KLOG_SIZE] = c;
}