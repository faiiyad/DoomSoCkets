CC     = gcc
CFLAGS = -Wall -lm -lncurses

SRCS = main.c render.c player.c input.c map.c console.c
OBJS = $(SRCS:.c=.o)

doom: $(OBJS)
	$(CC) -o doom $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS) doom