CC = gcc
CFLAGS = `pkg-config libxml-2.0 glib-2.0 libcurl gthread-2.0 --cflags --libs` -lncursesw -ggdb -Wall

all:
	$(CC) $(CFLAGS) -o foo main.c
debug:
	$(CC) $(CFLAGS) -DDEBUG -DVERSION=0.1 -o foo main.c
