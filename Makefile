CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Isrc
LDFLAGS = -lncursesw -lm

VPATH   = src
TARGET  = raycaster
SERVER  = server

SRCS        = main.c map.c ray.c gun.c render.c title.c entity.c
SRCS_SERVER = server.c map.c entity.c ray.c
OBJS        = $(SRCS:.c=.o)
OBJS_SERVER = $(SRCS_SERVER:.c=.o)

.PHONY: all server clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SERVER): $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Header dependencies
main.o:   main.c   defs.h map.h player.h gun.h render.h
map.o:    map.c    map.h  defs.h
ray.o:    ray.c    ray.h  map.h
gun.o:    gun.c    gun.h  defs.h
render.o: render.c render.h defs.h map.h ray.h gun.h player.h
server.o: server.c network.h map.h entity.h
title.o:  title.c  title.h defs.h

clean:
	rm -f $(OBJS) $(OBJS_SERVER) $(TARGET) $(SERVER)
