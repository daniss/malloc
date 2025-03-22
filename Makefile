# Check for HOSTTYPE
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Library names
NAME = libft_malloc_$(HOSTTYPE).so
SYM_LINK = libft_malloc.so

# Compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -fPIC

# Source files
SRC_DIR = srcs
SRCS = $(SRC_DIR)/malloc.c \
       $(SRC_DIR)/free.c \
       $(SRC_DIR)/realloc.c \
       $(SRC_DIR)/show_alloc_mem.c

# Object files
OBJ_DIR = obj
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Include directory
INC_DIR = includes
INC = -I$(INC_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -shared -o $(NAME) $(OBJS)
	@ln -sf $(NAME) $(SYM_LINK)
	@echo "Library $(NAME) created successfully!"
	@echo "Symbolic link $(SYM_LINK) created successfully!"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "Object files removed!"

fclean: clean
	@rm -f $(NAME) $(SYM_LINK)
	@echo "Library and symbolic link removed!"

re: fclean all

.PHONY: all clean fclean re