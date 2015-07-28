CPPFLAGS	:= -I.. -Ifake -DF_CPU=1000000UL -DTESTSUITE=1
CFLAGS		:= -std=c11 -g -O2
LDFLAGS		:=

TESTS		:= test_pid

TEST_BINS	:= $(foreach t,$(TESTS),$(t).run)

%.o: %.c test.h
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

test_%.run: test_%.o fake/fake.o
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

all: $(TEST_BINS)
	./test_pid.run
	@echo
	@echo "All test succeeded"

clean:
	rm -f *.o
	rm -f fake/*.o
	rm -f $(TEST_BINS)

.PHONY: all clean
