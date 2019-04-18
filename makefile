
SRC = frontend.c backend.c
NAME = 2048

COMPILE_FLAGS = -g
LINK_FLAGS = -lSDL2 -lm -lSDL2_image -lSDL2_ttf

all : $(SRC)
	gcc backend.c -c $(COMPILE_FLAGS) -lm
	gcc frontend.c -o 2048 backend.o $(COMPILE_FLAGS) $(LINK_FLAGS)

clean : 
	rm -f backend.o 2048

