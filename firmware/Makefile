# Project name
NAME			:= xylf

# Project source files
SRCS			:= buttons.c \
			   controller_current.c \
			   controller_temp.c \
			   debug_uart.c \
			   display.c \
			   main.c \
			   measure.c \
			   measure_current.c \
			   measure_temp.c \
			   pid.c \
			   pwm_current.c \
			   scale.c \
			   sseg.c \
			   timer.c \
			   util.c

# Project fuse bits
# 8 MHz, BOD 2.7 V
LFUSE			:= 0xE2
HFUSE			:= 0xDD
EFUSE			:= 0xF9

# CPU speed, in Hz
F_CPU			:= 8000000UL

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
