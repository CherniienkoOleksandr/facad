# Универсальный Makefile для маленьких C-проектов.
# Все *.c в папке собираются в один бинарник, имя = имя папки.
# Переопределение: make PROG=window_change  |  make SRCS="main.c window_change.c"

CC     ?= cc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra
DBG    := -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer

PROG   ?= $(notdir $(CURDIR))
SRCS   := $(wildcard *.c)
OBJS   := $(SRCS:.c=.o)
DEPS   := $(OBJS:.o=.d)
DOBJS  := $(SRCS:.c=.dbg.o)
DDEPS  := $(DOBJS:.o=.d)

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

# Отладочная сборка в отдельные объектники и отдельный бинарник,
# чтобы не путать её с релизной.
$(PROG)-dbg: $(DOBJS)
	$(CC) $(CFLAGS) $(DBG) -o $@ $^

%.dbg.o: %.c
	$(CC) $(CFLAGS) $(DBG) -MMD -MP -c -o $@ $<

debug: $(PROG)-dbg

run: $(PROG)
	./$(PROG)

bear:
	bear -- $(MAKE) --no-print-directory clean all

clean:
	rm -f $(OBJS) $(DEPS) $(DOBJS) $(DDEPS) $(PROG) $(PROG)-dbg \
	      compile_commands.json

-include $(DEPS) $(DDEPS)

.PHONY: all debug run bear clean
