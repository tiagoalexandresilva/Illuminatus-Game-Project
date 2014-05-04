OPT_OR_DEBUG=-O2 
CFLAGS=-Wall -Wextra -ansi -pedantic $(OPT_OR_DEBUG)
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
LIBS=-lreadline

illuminatus: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
clean:
	rm illuminatus $(wildcard *.o) $(wildcard *~)
