UNAME := $(shell uname)
CFLAGS := -I/opt/homebrew/include -Iinclude -Wall -Wextra -O2 -g
LDFLAGS := -L/opt/homebrew/lib
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
    LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lglfw
else
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
