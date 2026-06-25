NAME        = codexion
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread
INC         = includes
OBJ_DIR     = obj
SRC_DIR     = src

SRCS        = main.c $(shell find $(SRC_DIR) -type f -name '*.c' | sort)
OBJS        = $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS        = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -I$(INC) -o $(NAME)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -I$(INC) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
