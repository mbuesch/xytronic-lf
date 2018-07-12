CPPFLAGS	:= -I.. -Ifake -DF_CPU=8000000UL \
		   -DTESTSUITE=1 -D_DEFAULT_SOURCE
CFLAGS		:= -std=c11 -g -O2 -Wall -Wextra -Wno-unused-parameter -Wswitch-enum \
		   -Wsuggest-attribute=noreturn -Wundef -Wpointer-arith \
		   -Wcast-qual -Wlogical-op -Wshadow -Wconversion \
		   -Wno-shift-negative-value

LDFLAGS		:=

TESTS		:= test_common \
		   test_fixpt \
		   test_pid \
		   test_scale \
		   test_timer

TEST_BINS	:= $(foreach t,$(TESTS),$(t).run)

%.o: %.c test.h
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

test_%.run: test_%.o fake/fake.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(LDFLAGS) $^

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
