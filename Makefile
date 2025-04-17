
# Compiler and flags
CC = clang++ -g3 #-fsanitize=address
CFLAGS = #-Wall -Wextra -O2

SRC_DIR = src/
OBJ_DIR = obj/

# Source files
SRC = src/main.cpp src/Client.cpp

OBJ = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))

# Target executable
NAME = webserv

# Default target
all: $(NAME)

# Link object files to create executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ)
fclean: clean
	rm -rf $(OBJ_DIR)
	rm -rf $(NAME)

re: fclean all

# Phony targets
.PHONY: all clean

