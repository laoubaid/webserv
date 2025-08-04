
# Compiler and flags
CC = clang++ -g3 #-std=c++98 #-fsanitize=address
CFLAGS = -Wall -Wextra# -Werror -g3

SRC_DIR = src/*/
OBJ_DIR = obj/

# Source files
SRC = src/main.cpp src/server/Socket.cpp \
		src/server/Server.cpp src/server/Client.cpp \
		src/req/HTTPRequestParser.cpp src/req/HTTPRequestParserTools.cpp src/req/host.cpp\
		src/req/strMatchers.cpp src/req/strValidators.cpp src/req/test_fields.cpp \
		src/uvec/Uvec.cpp src/config/parser.cpp \
		src/resp/HTTPResponse.cpp

# OBJ = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))
OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)%.o, $(SRC))

# Target executable
NAME = webserv

# Default target
all: OBJ_STP $(NAME)

# Link object files to create executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files

OBJ_STP:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)server
	mkdir -p $(OBJ_DIR)req
	mkdir -p $(OBJ_DIR)uvec
	mkdir -p $(OBJ_DIR)config
	mkdir -p $(OBJ_DIR)resp

$(OBJ_DIR)%.o: src/%.cpp
	# mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(OBJ_DIR)
	rm -rf $(NAME)

re: fclean all

# Phony targets
.PHONY: all clean fclean re OBJ_STP

