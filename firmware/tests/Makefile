CPPFLAGS	:= -I.. -Ifake -DF_CPU=8000000UL \
		   -DTESTSUITE=1 -D_DEFAULT_SOURCE -DCONF_DEBUG=0

CXXFLAGS	:= -std=c++17 -fconcepts -g -O2 -Wall -Wextra -Wno-unused-parameter -Wswitch-enum \
		   -Wsuggest-attribute=noreturn -Wundef -Wpointer-arith \
		   -Wcast-qual -Wlogical-op -Wshadow -Wconversion \
		   -Wno-shift-negative-value

CFLAGS		:=

LDFLAGS		:=

TESTS		:= test_common \
		   test_fixpt \
		   test_pid \
		   test_scale \
		   test_timer

TEST_BINS	:= $(foreach t,$(TESTS),$(t).run)

%.o: %.c test.h
	$(CXX) -o $@ -c $(CPPFLAGS) $(CXXFLAGS) $<

test_%.run: test_%.o fake/fake.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(LDFLAGS) $^

all: $(TEST_BINS)
	./test_common.run
	./test_timer.run
	./test_fixpt.run
	./test_pid.run
	./test_scale.run
	@echo
	@echo "All test succeeded"

clean:
	rm -f *.o
	rm -f fake/*.o
	rm -f $(TEST_BINS)

.PHONY: all clean
