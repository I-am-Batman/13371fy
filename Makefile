# 13371fy
# By I-am-Batman, 2017

all: 13371fy

13371fy: 13371fy.o
	gcc -Wall 13371fy.o -o 13371fy

13371fy.o:
	gcc -c 13371fy.c

afl:
	afl-gcc 13371fy.c -o 13371fy-afl -Wall

test: all
	make -C tests all
	./tests/test

clean:
	rm -f 13371fy 13371fy-afl *.o
	make -C tests clean
