#include <lucix/cpu.h>
#include <lucix/user.h>
#include <lucix/sched.h>
#include <lucix/task.h>
#include <lucix/vma.h>

int64_t copy_from_user(void *dst, const void *usr, size_t count)
{
	int64_t total = 0;
	struct cpu *cpu = cpu_get_cpu();
	struct procmm *mm = cpu->current->task->mm;
	uint8_t *dst_u8 = dst;
	const uint8_t *usr_u8 = usr;

	while (total <= count) {
		/* Soo inefficient I feel ashamed */
		struct vm_area *vma = find_vma_for_address((uintptr_t)(usr + total), mm);

		if (!vma) {
			/* TODO kill current task */
			return -1;
		}

		*dst_u8++ = *usr_u8++;
		total++;
	}

	return total;
}

int64_t strncpy_from_user(char *dst, const char *usr, size_t n)
{
	struct cpu *cpu = cpu_get_cpu();
	int64_t total = 0;

	struct procmm *mm = cpu->current->task->mm;

	while (total <= n) {
		/* Soo inefficient I feel ashamed */
		struct vm_area *vma = find_vma_for_address((uintptr_t)(usr + total), mm);

		if (!vma) {
			/* TODO kill current task */
			return -1;
		}

		total++;
		if (!(*dst++ = *usr++)) break;
	}

	return total;
}
