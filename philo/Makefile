NAME	= philosopher
CC		= cc -g -Wall -Werror -Wextra
SRC		= main.c philosophers.c routine.c timer.c mini_libft.c watcher.c
OBJ		= $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ)
	@echo $(NAME)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: clean all

.PHONY: all clean fclean re
