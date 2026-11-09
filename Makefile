NAME	= codexion
SRC		= main.c \
		  parse.c \
		  time_utils.c \
		  log.c \
		  heap.c \
		  heap_utils.c

OBJ  = $(SRC:.c=.o)
CC   = cc
CFLAGS = -Wall -Wextra -Werror -pthread

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME) 1 2 3 4 5 6 7 8
