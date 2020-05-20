CC = clang
CFLAGS = -g
#DEBUG = -DBINARYDEBUG

SRCS = linkedlist.c main.c talloc.c tokenizer.c parser.c
HDRS = linkedlist.h value.h talloc.h tokenizer.h parser.h
OBJS = $(SRCS:.c=.o)

parser: $(OBJS)
	$(CC)  -rdynamic $(CFLAGS) $^  -o $@

%.o : %.c $(HDRS)
	$(CC)  $(CFLAGS) $(DEBUG) -c $<  -o $@

clean:
	rm *.o
	rm parser
