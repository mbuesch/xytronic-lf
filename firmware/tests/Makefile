CPPFLAGS	:= -I.. -Ifake -DF_CPU=1000000UL
CFLAGS		:= -std=c11 -g -O2
LDFLAGS		:=

TESTS		:= test_pid

all: $(TESTS)
	./test_pid
	@echo
	@echo "All test succeeded"

%.o: %.c
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

test_%: test_%.o fake/fake.o
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

clean:
	rm -f *.o
	rm -f fake/*.o
	rm -f $(TESTS)

.PHONY: clean
