NAME	= codexion
SRC		= main.c \
		  parse.c \
		  time_utils.c \
		  log.c \
		  heap.c \
		  heap_utils.c \
		  init.c \
		  cleanup.c \
		  dongle.c \
		  coder.c \
		  monitor.c

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

# Test targets
test_invalid: $(NAME)
	@echo "=== Testing invalid arguments ==="
	@! ./$(NAME) > /dev/null 2>&1 || (echo "Failed: missing args" && exit 1)
	@! ./$(NAME) 1 -100 100 100 100 1 0 fifo > /dev/null 2>&1 || (echo "Failed: negative arg" && exit 1)
	@! ./$(NAME) 1 100 abc 100 100 1 0 fifo > /dev/null 2>&1 || (echo "Failed: non-numeric arg" && exit 1)
	@! ./$(NAME) 1 100 100 100 100 1 0 invalid_sched > /dev/null 2>&1 || (echo "Failed: invalid scheduler" && exit 1)
	@echo "All invalid argument tests passed!"

test_single: $(NAME)
	@echo "=== Testing single coder (should burn out cleanly at ~300ms) ==="
	./$(NAME) 1 300 100 100 100 5 0 fifo

test_fifo: $(NAME)
	@echo "=== Testing FIFO with 4 coders, 2 compiles ==="
	./$(NAME) 4 1500 100 100 100 2 0 fifo
	@echo "=== Testing FIFO with 5 coders, 2 compiles, cooldown 10ms ==="
	./$(NAME) 5 1500 100 100 100 2 10 fifo

test_edf: $(NAME)
	@echo "=== Testing EDF with 4 coders, 2 compiles ==="
	./$(NAME) 4 1500 100 100 100 2 0 edf
	@echo "=== Testing EDF with 5 coders, 2 compiles, cooldown 10ms ==="
	./$(NAME) 5 1500 100 100 100 2 10 edf

test_burnout: $(NAME)
	@echo "=== Testing burnout detection under FIFO (burnout at 310ms) ==="
	./$(NAME) 4 310 200 100 100 5 0 fifo
	@echo "=== Testing burnout detection under EDF (burnout at 310ms) ==="
	./$(NAME) 4 310 200 100 100 5 0 edf

test_stress: $(NAME)
	@echo "=== Testing 50 coders under EDF ==="
	./$(NAME) 50 2000 100 100 100 1 0 edf
	@echo "=== Testing 100 coders under FIFO ==="
	./$(NAME) 100 2000 100 100 100 1 0 fifo

test_leaks: $(NAME)
	@echo "=== Testing memory leaks with Valgrind (single coder burnout) ==="
	valgrind --quiet --leak-check=full ./$(NAME) 1 200 100 100 100 5 0 fifo
	@echo "=== Testing memory leaks with Valgrind (4 coders completion) ==="
	valgrind --quiet --leak-check=full ./$(NAME) 4 1500 80 80 80 2 0 edf

test_helgrind: $(NAME)
	@echo "=== Testing data races with Helgrind ==="
	valgrind --quiet --tool=helgrind ./$(NAME) 4 1500 80 80 80 2 0 edf

test: test_invalid test_single test_fifo test_edf test_burnout
	@echo "=== ALL BASIC TESTS COMPLETED SUCCESSFULLY ==="

.PHONY: all clean fclean re test_invalid test_single test_fifo test_edf test_burnout test_stress test_leaks test_helgrind test
