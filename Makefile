# To build an executable from `example.c`, run `make example`

CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: $(patsubst %.c,%,$(wildcard *.c))

%: %.c
	$(CC) $(CFLAGS) -o $* $<

clean:
	rm -f $(patsubst %.c,%,$(wildcard *.c))

