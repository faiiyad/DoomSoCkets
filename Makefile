CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Isrc
LDFLAGS = -lncurses -lm
VPATH   = src:src/server
TARGET  = raycaster
SERVER  = server

SRCS        = main.c map.c ray.c gun.c render.c ui.c title.c entity.c client.c client_socket.c title_death.c render_hit_indicator.c render_kill_indicator.c
SRCS_SERVER = server.c server_ui.c server_socket.c client_manager.c map.c entity.c ray.c

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
main.o:           main.c           defs.h map.h player.h gun.h render.h
map.o:            map.c            map.h defs.h
ray.o:            ray.c            ray.h map.h
gun.o:            gun.c            gun.h defs.h
render.o:         render.c         render.h defs.h map.h ray.h gun.h player.h ui.h
title.o:          title.c          title.h defs.h
title_death.o:    title_death.c    title.h defs.h
ui.o:             ui.c             ui.h defs.h map.h entity.h
server.o:         server.c              network.h client_manager.h server_socket.h server_ui.h
server_ui.o:       server_ui.c            server_ui.h client_manager.h map.h
server_socket.o:  server_socket.c        server_socket.h network.h
client_manager.o: client_manager.c       client_manager.h entity.h
client.o:        client.c        client.h network.h client_socket.h
client_socket.o: client_socket.c client_socket.h network.h
render_hit_indicator.o: render_hit_indicator.c   defs.h render.h
render_hit_indicator.o: render_kill_indicator.c   defs.h render.h

clean:
	rm -f $(OBJS) $(OBJS_SERVER) $(TARGET) $(SERVER)