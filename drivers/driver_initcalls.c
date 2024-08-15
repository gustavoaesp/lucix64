#include <lucix/driver_initcalls.h>

static int (*init_call_list[])(void) = {
};

void driver_initcalls()
{
    int num = sizeof(init_call_list)/sizeof(int(*)(void));

    for (int i = 0; i < num; ++i) {
        init_call_list[i]();
    }
}