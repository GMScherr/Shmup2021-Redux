OBJS	= main.o physics2d.o
SOURCE	= main.c physics2d.c
HEADER	= physics2d.h
OUT	= TP1
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lGL -lGLU -lglut -lm -lSOIL

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c $(LFLAGS)

physics2d.o: physics2d.c
	$(CC) $(FLAGS) physics2d.c $(LFLAGS)


clean:
	rm -f $(OBJS) $(OUT)