CC=gcc
CFLAGS=-Wall

exec_name=fujiraw

src = $(wildcard src/*.c)
obj = $(src:.c=.o)

%.o: %.c 
	$(CC) $(CFLAGS) -c -g -o $@ $< 

debug: $(obj) 
	$(CC) $(CFLAGS) -g -I ./src -o $(exec_name) $^ -lm

.PHONY: clean
clean:
	rm -f $(obj) $(exec_name)
