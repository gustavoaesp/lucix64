CC		:=	x86_64-elf-gcc
CFLAGS		:=	-std=gnu99 -ffreestanding -m64 -mcmodel=kernel

SRCFILES	:=	main.c
LINKERLD	:=	link.ld

OBJFILES	:=	$(patsubst %.c,%.o,$(SRCFILES))

%.o: %.c Makefile
	$(CC) $(CLFLAGS) -c $< -o $@

testbin: $(OBJFILES)
	$(CC) -T $(LINKERLD) -o testbin -ffreestanding -nostdlib -z max-page-size=0x1000 $(OBJFILES) -lgcc

all: testbin
