CPPFLAGS	:= -I.. -Ifake -DF_CPU=1000000UL \
		   -DTESTSUITE=1 \
		   -DFIXPT_SIZE=16 -DFIXPT_SHIFT=5 -DFIXPTFLOAT_SIZE=64
CFLAGS		:= -std=c11 -g -O2 -Wall -Wextra -Wno-unused-parameter -Wswitch-enum \
		   -Wsuggest-attribute=noreturn -Wundef -Wpointer-arith \
		   -Wcast-qual -Wlogical-op -Wshadow -Wconversion
LDFLAGS		:=

TESTS		:= test_fixpt \
		   test_pid \
		   test_scale

TEST_BINS	:= $(foreach t,$(TESTS),$(t).run)

%.o: %.c test.h
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

test_%.run: test_%.o fake/fake.o
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

all: $(TEST_BINS)
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
