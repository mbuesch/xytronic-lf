# Project name
NAME			:= xylf

# Project source files
SRCS			:= main.c \
			   measure.c \
			   measure_current.c \
			   measure_temp.c \
			   pid.c \
			   timer.c \
			   util.c

# Project fuse bits
LFUSE			:= 0x62
HFUSE			:= 0xDF
EFUSE			:= 0xF9

# CPU speed, in Hz
F_CPU			:= 1000000UL

# Architecture configuration
GCC_ARCH		:= atmega88
AVRDUDE_ARCH		:= m88
FUNC_STACK_LIMIT	:= 64

# Programmer selection.
# Values can be:  avrisp2, mysmartusb
PROGRAMMER		:= avrisp2

# Additional compiler flags
CFLAGS			:=
LDFLAGS			:=

# Additional "clean" and "distclean" target files
CLEAN_FILES		:=
DISTCLEAN_FILES		:=


include avrmakelib.mk


tests: all
	$(MAKE) -C tests clean
	$(MAKE) -C tests all

.PHONY: tests
