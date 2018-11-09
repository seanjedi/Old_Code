targets := libuthread.a
obs := context.o uthread.o preempt.o queue.o
objs := context.d uthread.d preempt.d queue.d
CC := gcc
CFLAGS := -Wall -Werror -g -c -o
ARCHIVE := ar rcs

all: $(targets)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

libuthread.a: $(obs)
	$(ARCHIVE) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $@ $< $(DEPFLAGS)

clean:
	rm -f $(targets) $(obs) $(deps) $(objs)
