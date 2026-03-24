CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Isrc
LDFLAGS = -lncurses -lm

VPATH   = src
TARGET  = raycaster

SRCS    = main.c map.c ray.c gun.c render.c title.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Header dependencies
main.o:   main.c   defs.h map.h player.h gun.h render.h
map.o:    map.c    map.h  defs.h
ray.o:    ray.c    ray.h  map.h
gun.o:    gun.c    gun.h  defs.h
render.o: render.c render.h defs.h map.h ray.h gun.h player.h
title.o:  title.c  title.h defs.h

clean:
	rm -f $(OBJS) $(TARGET)