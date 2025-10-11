PING = ft_ping
PING_SRCS = srcs/*.c
PING_OBJS = $(:.c=.o)
PING_FLAGS = -Wall -Wextra -Werror -I includes

$(PING): $(PING_OBJS)
	gcc $(PING_FLAGS) $(PING_SRCS) $(PING_FLAGS) -o $(PING)

clean:
	rm -rf srcs/*.o

fclean: clean
	rm -rf $(PING)

all: $(PING)

re: fclean all
