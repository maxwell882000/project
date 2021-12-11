all: test

test: test.c
        gcc -g test.c -o ./test