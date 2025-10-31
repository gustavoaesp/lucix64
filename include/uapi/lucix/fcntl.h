#ifndef _UAPI_LINUX_FCNTL_H
#define _UAPI_LINUX_FCNTL_H

#define O_RDONLY    0x0
#define O_WRONLY    0x1
#define O_RDWR      0x2
#define O_ACCMODE   0x3

#define O_CREAT     0x100
#define O_EXCL      0x200
#define O_TRUNC     0x400
#define O_APPEND    0x800

#endif