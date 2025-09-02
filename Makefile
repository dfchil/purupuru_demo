TARGET = drxlax.elf
KOS_CPPSTD	:= -std=c++20
KOS_CCSTD	:= -std=gnu11
ROMDIR=cdrom/drxlax
CC=kos-cc


# List all of your C files here, but change the extension to ".o"
OBJS := $(shell find ./code/ -name '*.c' -not -path "./.git/*" | sed -e 's/\(.*\).c/\1.o/g')
LHEADERS = $(shell find . -name '*.h' -not -path "./.git/*")
LDLIBS 	:= -lkosutils -lm


DEFINES=	
ifdef DEBUG
	DEFINES += -DDEBUG
endif

KOS_CFLAGS=\
		-I$(KOS_BASE)/include \
		-I$(KOS_BASE)/kernel/arch/dreamcast/include \
		-I$(KOS_BASE)/addons/include \
		-I$(KOS_BASE)/../kos-ports/include \
		-I$(KOS_BASE)/utils \
		-I$(shell pwd)/include \
		-fno-strict-aliasing \
		-fno-builtin  -ffunction-sections -fdata-sections -ftls-model=local-exec \
		-D__DREAMCAST__  \
		-D_arch_dreamcast -D_arch_sub_pristine -Wall \
		-ffast-math \
		-fstack-protector-all \
		-flto=auto \
		-DFRAME_POINTERS \
		-fomit-frame-pointer \
		-ffunction-sections \
		-Wall -Wextra  \
		${DEFINES} \
		-m4-single-only -ml -matomic-model=soft-imask \

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS) $(basename $(TARGET)).cdi

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS) $(DTTEXTURES) $(SFXFILES)
	$(CC) $(OBJS) $(KOS_CFLAGS) -L$(KOS_BASE)/lib -o $@  $(LDLIBS)

