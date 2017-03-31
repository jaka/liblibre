OPTS	:= -O2
CFLAGS	+= -Iinclude -Wall -Werror -Wextra -s -pedantic -fdata-sections -ffunction-sections -fno-exceptions -fstrict-aliasing -fPIC -std=c99
LDFLAGS	+= -Wl,--gc-sections
SFLAGS	:= -R .comment -R .gnu.version -R .gnu.version_r -R .note -R .note.ABI-tag

CC	?= cc

SOURCES	:= $(wildcard src/*.c)
OBJECTS	:= $(patsubst %.c,%.o,$(SOURCES))
TARGETS := liblibre.so

all: $(OBJECTS) $(TARGETS)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(OPTS) -c -o $@ $<

$(TARGETS): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OPTS) -shared -o $@ $^

install: $(TARGETS)
	install -m 755 $(TARGETS) /usr/lib/

clean:
	rm -fr $(OBJECTS)
	rm -f $(TARGETS)
