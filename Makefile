ARCHDIR		:=	arch/x86_64
CC		:=	x86_64-elf-gcc
AS		:=	x86_64-elf-as
CFLAGS		:=	-std=gnu99 -ffreestanding -m64 -mcmodel=kernel -g
LINKERLD	:=	linker.ld

PROJDIRS	:=	$(ARCHDIR) mm kernel drivers

SRCFILES	:=	$(shell find $(PROJDIRS) -type f -name "*.c")
ASMFILES	:=	$(shell find $(PROJDIRS) -type f -name "*.s")

OBJFILES	:=	$(patsubst %.c,%.o,$(SRCFILES))
ASMOBJFILES	:=	$(patsubst %.s,%.o,$(ASMFILES))
ALLOBJ		:=	$(OBJFILES) $(ASMOBJFILES)
INCLUDEDIRS	:=	include $(ARCHDIR)/include drivers

#================================================================
#
#	This generates the commandline -I include args for gcc
#
#===============================================================
INC_PARAMS	:= $(foreach d, $(INCLUDEDIRS), -I$d)

%.o: %.c Makefile
	$(CC) $(CFLAGS) $(INC_PARAMS) -c $< -o $@

%.o: %.s Makefile
	$(AS) --64 -g $< -o $@

lucix: $(OBJFILES) $(ASMOBJFILES)
	$(CC) -T $(LINKERLD) -o lucix -ffreestanding -nostdlib -z max-page-size=0x1000 $(ALLOBJ) -lgcc

all: lucix 

clean:
	rm $(ALLOBJ)
	rm lucix
