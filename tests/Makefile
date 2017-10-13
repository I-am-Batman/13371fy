# 13371fy
# By I-am-Batman, 2017

all: test

test: test.o
	gcc -Wall -lcrypto test.o -o test

test.o:
	gcc -c test.c

clean:
	rm -f test *.o
