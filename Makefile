
OBJS	= main.o cli.o colors.o parse.o fs.o
SOURCE	= ./main.c, ./cli.c, ./colors.c, ./parse.c, ./fs.c,
HEADER	= cli.h, colors.h, parse.h, fs.h
OUT	= ColorApp
CC	 = gcc
FLAGS	 = -g -c -Wall 
LFLAGS	 = -lm

all:	$(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) 
 


clean:
	rm -f $(OBJS) $(OUT)
