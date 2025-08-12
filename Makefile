UNAME := $(shell uname)
DEBUG := #-g -fsanitize=address

# Collecting Sorce files
SRC_C := $(wildcard src/*.c)
SRC_CPP := $(wildcard src/*.cpp)
SRC := $(SRC_C) $(SRC_CPP) glad.c

# Object file list
OBJ := $(SRC:.c=.o)
OBJ := $(OBJ:.cpp=.o)

#Compiler
CC := cc
CXX := c++

# Linker
ifeq ($(UNAME), Darwin)
	CFLAGS := -I/opt/homebrew/include -Iinclude -Wall -Wextra -Werror -O2 -g
	LDFLAGS := -L/opt/homebrew/lib
	LIBS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lglfw
else
	CFLAGS := -I/usr/include -Iinclude -Wall -Wextra -Werror -O2 -g
	LDFLAGS := -L/usr/lib/x86_64-linux-gnu
    LIBS = -lGL -lglfw -ldl
endif

# Build
NAME = app

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS) $(LIBS) $(DEBUG)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(DEBUG)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(DEBUG)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
