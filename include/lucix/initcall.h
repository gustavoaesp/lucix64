#ifndef _LUCIX_INITCALL_H_
#define _LUCIX_INITCALL_H_

#include <lucix/compiler.h>

typedef int (*initcall_t)();

#define ___define_initcall(fn, id, __sec) \
    static initcall_t __initcall_##fn##id \
    __attribute__((__used__)) \
    __attribute__((__section__(#__sec ".init"))) = fn;

#define __define_initcall(fn, id)   ___define_initcall(fn, id, .initcall##id)

#define initcall_early(fn)      __define_initcall(fn, 0)
#define initcall_core(fn)       __define_initcall(fn, 1)
#define initcall_postcore(fn)   __define_initcall(fn, 2)
#define initcall_arch(fn)       __define_initcall(fn, 3)
#define initcall_subsys(fn)     __define_initcall(fn, 4)
#define initcall_fs(fn)         __define_initcall(fn, 5)
#define initcall_rootfs(fn)     __define_initcall(fn, rootfs)
#define initcall_device(fn)     __define_initcall(fn, 6)
#define initcall_late(fn)       __define_initcall(fn, 7)

void do_initcalls();

#endif