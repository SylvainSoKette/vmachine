CC=gcc
STD=-std=c11

CFLAGS=-O3
LFLAGS=-lSDL2

SRC=main.c
PROGRAM=bin/vmachine

all:
	${CC} ${STD} ${SRC} ${CFLAGS} ${LFLAGS} -o ${PROGRAM}
