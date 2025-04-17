# Compiler and flags
CC = clang++ -g3 #-fsanitize=address
CFLAGS = #-Wall -Wextra -O2

# Target executable
TARGET1 = svr
TARGET2 = clt

# Source files
SRCS1 = server.c
SRCS2 = client.c
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)

# Default targets
all: $(TARGET1) $(TARGET2)

# Link object files to create executables
$(TARGET1): $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $^
$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS1) $(OBJS2)
fclean: clean
	rm -f $(TARGET1) $(TARGET2)
re: fclean all

# Phony targets
.PHONY: all clean