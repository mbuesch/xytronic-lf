# Build configuration
CONF_DEBUG		:= 1
CONF_BOOST		:= 0
CONF_IDLE		:= 1
CONF_IDLETOGGLE		:= 0
CONF_PRESETS		:= 1
CONF_EERING		:= 1
CONF_ADJ		:= 1
CONF_KCONF		:= 1
CONF_CURRCUTOFFHYST	:= 0.1
CONF_CURRCUTOFF		:= 1.8


BIN		:= pyxytronic.so

SRCS		:= pyxytronic.cxx \
		   simulator.cxx \
		   firmware.cxx

CPPFLAGS	:= -I.. -I../tests/fake \
		   $(shell python3-config --includes) \
		   -include ../tests/fake/fake.h \
		   -include simulator.h \
		   -include sparse.h \
		   -DNDEBUG \
		   -D_DEFAULT_SOURCE -D_GNU_SOURCE \
		   -DF_CPU=8000000UL \
		   -Dirq_disable=sim_irq_disable \
		   -Dirq_enable=sim_irq_enable \
		   -Dirq_disable_save=sim_irq_disable_save \
		   -Dirq_restore=sim_irq_restore \
		   -DSIMULATOR \
		   -DHW_SMD \
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

CXXFLAGS	:= -std=c++17 -g -O2 -pthread -fPIC \
		   -Wall -Wextra -Wno-unused-parameter -Wswitch-enum \
		   -Wsuggest-attribute=noreturn -Wundef -Wpointer-arith \
		   -Wno-shift-negative-value \
		   -Wno-suggest-attribute=noreturn \
		   -Wno-attributes

LDFLAGS		:= $(shell python3-config --libs)


V		= @             # Verbose build:  make V=1
Q		= $(V:1=)
QUIET_CXX	= $(Q:@=@echo '     CXX      '$@;)$(CXX)
QUIET_DEPEND	= $(Q:@=@echo '     DEPEND   '$@;)$(CXX)

DEPS		= $(patsubst %.cxx,dep/%.d,$(1))
OBJS		= $(patsubst %.cxx,obj/%.o,$(1))


.SUFFIXES:
.PHONY: all install clean
.DEFAULT_GOAL := all

# Generate dependencies
$(call DEPS,$(SRCS)): dep/%.d: %.cxx
	@mkdir -p $(dir $@)
	$(QUIET_DEPEND) -o $@.tmp -MM -MT "$@ $(patsubst dep/%.d,obj/%.o,$@)" $(CXXFLAGS) $< && mv -f $@.tmp $@

-include $(call DEPS,$(SRCS))

# Generate object files
$(call OBJS,$(SRCS)): obj/%.o:
	@mkdir -p $(dir $@)
	$(QUIET_CXX) -o $@ -c $(CPPFLAGS) $(CXXFLAGS) $<

$(BIN): $(call OBJS,$(SRCS))
	$(QUIET_CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(LDFLAGS) -shared $(LIBS) $(call OBJS,$(SRCS))

all: $(BIN)

clean:
	rm -Rf dep obj core *~ $(BIN)
