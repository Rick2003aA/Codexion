NAME := codexion

CC := cc
CFLAGS := -Wall -Wextra -Werror -pthread

SRC := src/main.c \
	src/coder.c \
	src/parse.c \
	src/sim_init.c \
	src/time.c \
	src/log.c \
	src/utils.c \
	src/cleanup.c

OBJ := $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
