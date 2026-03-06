NAME := codexion

CC := cc
CFLAGS := -Wall -Wextra -Werror -pthread
CFLAGS += -I src

SRC := src/app/main.c \
	src/core/sim_stop.c \
	src/core/dongle.c \
	src/core/scheduler.c \
	src/core/coder_actions.c \
	src/core/coder_routine.c \
	src/core/monitor.c \
	src/core/monitor_checks.c \
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
