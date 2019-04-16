
SRC = 2048.c
NAME = 2048

COMPILE_FLAGS = -g
LINK_FLAGS = -lSDL2 -lm -lSDL2_image 

all : $(SRC)
	gcc $(SRC) $(COMPILE_FLAGS) $(LINK_FLAGS) -o $(NAME) 

