.PHONY: all clean

all:
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread santaProblem.c -o santaProblem

clean:
	rm santaProblem santaProblem.out
