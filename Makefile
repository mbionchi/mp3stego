CC=gcc
COPTS=-Wall -Wextra -g

pbhide: src/pbhide.c extract.o embed.o info.o common.o
	$(CC) $(COPTS) src/pbhide.c extract.o embed.o info.o common.o -o pbhide

embed.o: src/embed.c src/embed.h
	$(CC) $(COPTS) -c src/embed.c

info.o: src/info.c src/info.h
	$(CC) $(COPTS) -c src/info.c

extract.o: src/extract.c src/extract.h
	$(CC) $(COPTS) -c src/extract.c

common.o: src/common.c src/common.h
	$(CC) $(COPTS) -c src/common.c
