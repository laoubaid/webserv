
# Compiler and flags
CC = clang++ -g3 #-std=c++98 #-fsanitize=address
CFLAGS = -Wall -Wextra# -Werror -g3

OBJ_DIR = obj/

# Source files
SRC = src/main.cpp src/server/Socket.cpp src/server/webServ.cpp \
		src/server/Server.cpp src/server/Client.cpp \
		src/req/HTTPRequestParser.cpp src/req/HTTPRequestParserTools.cpp src/req/host.cpp\
		src/req/strMatchers.cpp src/req/strValidators.cpp src/req/test_fields.cpp \
		src/uvec/Uvec.cpp src/config/configParser.cpp \
		src/config/serverConf.cpp src/config/locationConf.cpp \
		src/resp/HTTPResponse.cpp src/resp/process_resp.cpp \
		src/cgi/Cgi.cpp

# OBJ = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))
OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)%.o, $(SRC))

# Target executable
NAME = webserv
EXEC := ./$(NAME)

# printing colors
GREEN       := \033[0;32m
RED         := \033[0;31m
RESET       := \033[0m

# execution variables and flages
PID_FILE    := .webserv.pid
CONFIG_FILE ?= ./conf/default.conf
VALGRIND_FLAGS ?=
LEAK_KIND = --show-leak-kinds=



# Default target
all: $(OBJ_DIR) $(NAME)

# Link object files to create executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR) $(OBJ_DIR)server $(OBJ_DIR)req \
	$(OBJ_DIR)uvec $(OBJ_DIR)config $(OBJ_DIR)resp $(OBJ_DIR)cgi

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

run: all
	@if [ -f $(PID_FILE) ]; then \
		echo "$(RED)Server already running (PID file exists)$(RESET)"; \
	else \
		nohup $(EXEC) $(CONFIG_FILE) >/dev/null 2>logs & \
		PID=$$!; \
		sleep 1; \
		kill -0 $$PID 2>/dev/null; \
		#  could use a better algo to handle this? \
		if [ $$? -eq 0 ]; then \
			echo $$PID > $(PID_FILE); \
			echo "$(GREEN)Starting webserv...$(RESET)"; \
		else \
			echo "$(RED)Failed to start webserv$(RESET)"; \
			cat logs; \
		fi \
	fi

stop:
	@if [ -f $(PID_FILE) ]; then \
		kill `cat $(PID_FILE)` 2>/dev/null || true; \
		rm -f $(PID_FILE); \
		echo "$(GREEN)Stopped webserv$(RESET)"; \
	else \
		echo "$(RED)No PID file found$(RESET)"; \
	fi

restart: stop run

crun:
	@clear
	@$(MAKE) -s run
	# -s silent in the recursive call to make

valgrind: all
	valgrind $(VALGRIND_FLAGS) $(EXEC) $(CONFIG_FILE)

leaks: all
	valgrind --leak-check=full $(LEAK_KIND) $(EXEC) $(CONFIG_FILE)

# Phony targets
.PHONY: all clean fclean re OBJ_STP run crun stop valgrind leaks

.SECONDARY: OBJ_STP $(OBJ)

