# Target device.
# May be one of: m88, m328p
DEV			:= m88

streq			= $(and $(filter 1,$(words $2)),$(filter $1,$(firstword $2)))
deveq			= $(call streq,$1,$(DEV))
ifeq ($(strip $(call deveq,m88)$(call deveq,m328p)),)
$(error "$$DEV:=$(DEV) is not supported")
endif

# Build configuration
CONF_CALIB		:= 0
CONF_DEBUG		:= 1
CONF_KCONF		:= $(if $(call deveq,m328p),1,0)

# Project name
NAME			:= xytronic-lf

# Project source files
SRCS			:= bitmap.c \
			   buttons.c \
			   $(if $(call streq,1,$(CONF_CALIB)),calib_current.c) \
			   controller_current.c \
			   controller_temp.c \
			   $(if $(call streq,1,$(CONF_DEBUG)),debug_uart.c) \
			   display.c \
			   filter.c \
			   fixpt.c \
			   main.c \
			   measure.c \
			   measure_current.c \
			   measure_temp.c \
			   menu.c \
			   pid.c \
			   pwm_current.c \
			   settings.c \
			   scale.c \
			   sseg.c \
			   timer.c \
			   util.c

# Project fuse bits
# 8 MHz, BOD 2.7 V
ifeq ($(DEV),m88)
LFUSE			:= 0xE2
HFUSE			:= 0xDD
EFUSE			:= 0x01
endif
ifeq ($(DEV),m328p)
LFUSE			:= 0xE2
HFUSE			:= 0xD9
EFUSE			:= 0x05
endif

# CPU speed, in Hz
F_CPU			:= 8000000UL

# Architecture configuration
GCC_ARCH		:= $(patsubst m%,atmega%,$(DEV))
AVRDUDE_ARCH		:= $(DEV)
FUNC_STACK_LIMIT	:= 64

# Programmer selection.
# Values can be:  avrisp2, mysmartusb
PROGRAMMER		:= avrisp2

# Additional compiler flags
CFLAGS			:= $(if $(call streq,1,$(CONF_CALIB)),-DCONF_CALIB) \
			   $(if $(call streq,1,$(CONF_DEBUG)),-DCONF_DEBUG) \
			   $(if $(call streq,1,$(CONF_KCONF)),-DCONF_KCONF)
LDFLAGS			:=

# Additional "clean" and "distclean" target files
CLEAN_FILES		:=
DISTCLEAN_FILES		:=


include avrmakelib.mk


tests: all
	$(MAKE) -C tests clean
	$(MAKE) -C tests all

.PHONY: tests
