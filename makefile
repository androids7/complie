C_SRC = error.c input.c ucl.c
OBJS = $(C_SRC:.c=.o)
cc = gcc
CFLAGS = -g
all: $(OBJS) 
	$(CC) -o ucl $(CFLAGS) $(OBJS)

