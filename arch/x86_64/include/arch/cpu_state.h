#ifndef _LUCIX_ARCH_X86_CPU_STATE_
#define _LUCIX_ARCH_X86_CPU_STATE_

#include <stdint.h>

#include <lucix/compiler.h>
#include <lucix/cpu.h>

struct arch_x86_cpu_state
{
    /* These are pushed by the interrupt */
	uint64_t	rip;
	uint64_t	cs;
	uint64_t	ss;
	uint64_t	rflags;
	uint64_t	rsp;

	uint64_t	rdi;
	uint64_t	rsi;
	uint64_t	rbp;
	uint64_t	rbx;
	uint64_t	rdx;
	uint64_t	rcx;
	uint64_t	rax;

	uint64_t	r8;
	uint64_t	r9;
	uint64_t	r10;
	uint64_t	r11;
	uint64_t	r12;
	uint64_t	r13;
	uint64_t	r14;
	uint64_t	r15;
};

struct tss_entry64
{
	uint32_t _rsvd0;
	uint64_t rsp[3];
	uint64_t _rsvd1;
	uint64_t IST[7];
	uint64_t _rsvd2;
	uint16_t _rsvd3;
	uint16_t iopb;
} __packed;

typedef struct arch_x86_percpu_private
{
	struct tss_entry64 tss;
} per_cpu_state_t;

extern struct cpu up_cpu;

#endif
