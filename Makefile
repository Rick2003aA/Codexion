NAME := codexion

CC := cc
CFLAGS := -Wall -Wextra -Werror -pthread
CFLAGS += -I src

SRC := src/app/main.c \
	src/core/coder.c \
	src/core/monitor.c \
	src/init/parse.c \
	src/init/sim_init.c \
	src/common/time.c \
	src/common/log.c \
	src/common/utils.c \
	src/common/cleanup.c

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
