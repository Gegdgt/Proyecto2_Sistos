CC = gcc
CFLAGS = -Wall -std=c99 `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_ttf

OBJS = main.o fifo.o utils.o

simulador: $(OBJS)
	$(CC) -o simulador $(OBJS) $(LIBS)

main.o: main.c fifo.h utils.h
	$(CC) $(CFLAGS) -c main.c

fifo.o: fifo.c fifo.h utils.h
	$(CC) $(CFLAGS) -c fifo.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o simulador
