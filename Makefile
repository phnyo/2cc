CFLAGS := -std=c11 -g -static

2cc: 2cc.c

test: 2cc
	sh test.sh

clean:
	rm -f 2cc *.o *~ tmp*

.PHONY: test clean
