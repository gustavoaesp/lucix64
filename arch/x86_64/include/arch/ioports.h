#ifndef _IO_PORTS_H_
#define _IO_PORTS_H_

/* input */
static inline void out8(uint32_t port, uint8_t data)
{
	__asm__ volatile ("outb	%b0,	%w1" : : "a" (data), "Nd" (port));
}
static inline void out16(uint32_t port, uint16_t data)
{
	__asm__ volatile ("outw %w0,	%w1" : : "a" (data), "Nd" (port));
}
static inline void out32(uint32_t port, uint32_t data)
{
	__asm__ volatile ("outl %0,	%w1" : : "a" (data), "Nd" (port));
}

/* output */
static inline uint8_t in8(uint32_t port)
{
	uint8_t data;
	__asm__ volatile ("inb	%w1,	%b0" : "=a" (data) : "Nd" (port));
	return data;
}
static inline uint16_t in16(uint32_t port)
{
	uint16_t data;
	__asm__ volatile ("inw	%w1,	%w0" : "=a" (data) : "Nd" (port));
	return data;
}
static inline uint32_t in32(uint32_t port)
{
	uint32_t data;
	__asm__ volatile ("inl	%w1,	%0" : "=a" (data) : "Nd" (port));
	return data;
}

#endif
