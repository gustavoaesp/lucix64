CC		:=	x86_64-elf-gcc
AS		:=	x86_64-elf-as
CFLAGS		:=	-std=gnu99 -ffreestanding -m64 -mcmodel=kernel -g

SRCFILES	:=	main.c
ASMFILES	:=	syscall.s
LINKERLD	:=	link.ld

OBJFILES	:=	$(patsubst %.c,%.o,$(SRCFILES))
ASMOBJFILES	:=	$(patsubst %.s,%.o,$(ASMFILES))
ALLOBJ		:=	$(OBJFILES) $(ASMOBJFILES)

INCLUDEDIRS	:=	.
INC_PARAMS	:=	$(foreach d, $(INCLUDEDIRS), -I$d)

%.o: %.c Makefile
	$(CC) $(CLFLAGS) $(INC_PARAMS) -c $< -o $@
%.o: %.s Makefile
	$(AS) --64 -g $< -o $@

testbin: $(OBJFILES) $(ASMOBJFILES)
	$(CC) -T $(LINKERLD) -o testbin -ffreestanding -nostdlib -nostdinc -nostartfiles -z max-page-size=0x1000 \
		$(ALLOBJ) -lgcc

all: testbin
