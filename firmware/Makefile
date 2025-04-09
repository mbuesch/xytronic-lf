# Target device.
# May be one of: m88, m88p, m328p
DEV			:= m88p

# Target hardware.
# May be one of: legacy, smd
HW			:= smd


streq			= $(and $(filter 1,$(words $2)),$(filter $1,$(firstword $2)))
deveq			= $(call streq,$1,$(DEV))
hweq			= $(call streq,$1,$(HW))

ifeq ($(strip $(call deveq,m88)$(call deveq,m88p)$(call deveq,m328p)),)
$(error "$$DEV:=$(DEV) is not supported")
endif
ifeq ($(strip $(call hweq,legacy)$(call hweq,smd)),)
$(error "$$HW:=$(HW) is not supported")
endif


# Build configuration
CONF_DEBUG		:= $(if $(call deveq,m328p),1,0)
CONF_BOOST		:= 1
CONF_IDLE		:= 1
CONF_IDLETOGGLE		:= 1
CONF_PRESETS		:= 1
CONF_EERING		:= 1
CONF_ADJ		:= 1
CONF_KCONF		:= $(if $(call deveq,m328p),1,0)
CONF_CURRCUTOFFHYST	:= 0.1
CONF_CURRCUTOFF		:= 1.8


# Project name
NAME			:= xytronic-lf

# Project source files
SRCS			:= bitmap.c \
			   buttons.c \
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
			   presets.c \
			   pwm_current.c \
			   ring.c \
			   settings.c \
			   scale.c \
			   sseg.c \
			   timer.c \
			   util.c
GEN_SRCS		:=

# Bootloader source files
BOOT_SRCS		:=
BOOT_GEN_SRCS		:=
BOOT_OFFSET		:=

# Project fuse bits
# 8 MHz, BOD 2.7 V, WDT on
M88_LFUSE		:= 0xE2
M88_HFUSE		:= 0xCD
M88_EFUSE		:= 0xF9
M88P_LFUSE		:= 0xE2
M88P_HFUSE		:= 0xCD
M88P_EFUSE		:= 0xF9
M328P_LFUSE		:= 0xE2
M328P_HFUSE		:= 0xC9
M328P_EFUSE		:= 0xFD
ifeq ($(DEV),m88)
LFUSE			:= $(M88_LFUSE)
HFUSE			:= $(M88_HFUSE)
EFUSE			:= $(M88_EFUSE)
endif
ifeq ($(DEV),m88p)
LFUSE			:= $(M88P_LFUSE)
HFUSE			:= $(M88P_HFUSE)
EFUSE			:= $(M88P_EFUSE)
endif
ifeq ($(DEV),m328p)
LFUSE			:= $(M328P_LFUSE)
HFUSE			:= $(M328P_HFUSE)
EFUSE			:= $(M328P_EFUSE)
endif

# CPU speed, in Hz
F_CPU			:= 8000000UL

# Architecture configuration
GCC_ARCH		:= $(patsubst m%,atmega%,$(DEV))
AVRDUDE_ARCH		:= $(DEV)
FUNC_STACK_LIMIT	:=

# Programmer selection.
# Values can be:  avrisp2, mysmartusb
PROGRAMMER		:= avrisp2

# Instrumentation
INSTRUMENT_FUNC		:=
BOOT_INSTRUMENT_FUNC	:=

# Additional compiler flags
CFLAGS			:= $(if $(call hweq,legacy),-DHW_LEGACY) \
			   $(if $(call hweq,smd),-DHW_SMD) \
			   -DCONF_DEBUG=$(CONF_DEBUG) \
			   -DCONF_BOOST=$(CONF_BOOST) \
			   -DCONF_IDLE=$(CONF_IDLE) \
			   -DCONF_IDLETOGGLE=$(CONF_IDLETOGGLE) \
			   -DCONF_PRESETS=$(CONF_PRESETS) \
			   -DCONF_EERING=$(CONF_EERING) \
			   -DCONF_ADJ=$(CONF_ADJ) \
			   -DCONF_KCONF=$(CONF_KCONF) \
			   -DCONF_CURRCUTOFF=$(CONF_CURRCUTOFF) \
			   -DCONF_CURRCUTOFFHYST=$(CONF_CURRCUTOFFHYST)
LDFLAGS			:=

# Additional "clean" and "distclean" target files
CLEAN_FILES		:=
DISTCLEAN_FILES		:=


include avrmakelib.mk


tests: all
	$(MAKE) -C tests clean
	$(MAKE) -C tests all

.PHONY: tests
