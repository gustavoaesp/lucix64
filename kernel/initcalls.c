#include <lucix/initcall.h>

extern initcall_t __initcall_start, __initcall_end;

/*
extern initcall_t __initcall0_start;
extern initcall_t __initcall1_start;
extern initcall_t __initcall2_start;
extern initcall_t __initcall3_start;
extern initcall_t __initcall4_start;
extern initcall_t __initcall5_start;
extern initcall_t __initcallrootfs_start;
extern initcall_t __initcall6_start;
extern initcall_t __initcall7_start;*/

/*static initcall_t *initcall_levels[9] = {
    __initcall0_start,
    __initcall1_start,
    __initcall2_start,
    __initcall3_start,
    __initcall4_start,
    __initcall5_start,
    __initcallrootfs_start,
    __initcall6_start,
    __initcall7_start
};*/

void do_initcalls()
{
    void *call;
    call = &__initcall_start;
    while(((initcall_t*)call) < &__initcall_end) {
        initcall_t initcall_idx = *((initcall_t*)call);
        (*initcall_idx)();
        call += sizeof(initcall_t);
    };
}
