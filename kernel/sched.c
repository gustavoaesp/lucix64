#include <lucix/task.h>

static obj_mem_cache_t* task_cache;
static obj_mem_cache_t* vm_area_cache;
static obj_mem_cache_t* procmm_cache;

static uint32_t pid_count = 1;

void sched_init()
{
	task_cache = create_obj_mem_cache(sizeof(struct task), 0, 1, "task-cache");
	vm_area_cache = create_obj_mem_cache(sizeof(struct vm_area), 0, 1, "vma-cache");
	procmm_cache = create_obj_mem_cache(sizeof(struct procmm), 0, 1, "procmm-cache");

}

