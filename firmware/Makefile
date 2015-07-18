# Project name
NAME			:= xylf

# Project source files
SRCS			:= main.c \
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
