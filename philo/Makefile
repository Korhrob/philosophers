NAME    = philosopher
CC      = cc
CFLAGS  = -g -Wall -Werror -Wextra #-fsanitize=thread 
DEBUG   = -DDEBUG=1
SRC     = main.c threads.c philos_refactor.c timer.c mini_libft.c watcher.c get.c set.c
OBJ     = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug: CFLAGS += $(DEBUG)
debug: fclean $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo "$(NAME) (debug)"

.PHONY: all clean fclean re debug
