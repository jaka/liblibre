CFLAGS += -I ../include

all: config

clean:
	rm -f config test.o config.o

config: test.o config.o
	$(CC) -o $@ $^
	./$@
