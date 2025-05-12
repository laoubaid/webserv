# Compiler and flags
CC = clang++ -g3 #-std=c++98 #-fsanitize=address
CFLAGS = #-Wall -Wextra -O2

SRC_DIR = ./
OBJ_DIR = obj/

# Source files
SRC = HTTPRequestParser.cpp HTTPRequestParserTools.cpp\
		 test_tobedel.cpp strMatchers.cpp strValidators.cpp test_fields.cpp

OBJ = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC))

# Target executable
NAME = testing

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

