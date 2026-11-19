CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O0
# -O0: o compilador nao altera o laco que esta sendo medido
OBJ = main.o insertion_sort.o

all: insertion_sort

insertion_sort: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c insertion_sort.h
	$(CC) $(CFLAGS) -c $<

demo: insertion_sort
	./insertion_sort demo

bench: insertion_sort
	./insertion_sort bench 40000

csv: insertion_sort
	./insertion_sort csv > resultados.csv

# make san: compila com AddressSanitizer + UBSan e roda
san:
	$(CC) $(CFLAGS) -g -fsanitize=address,undefined -o insertion_sort_san main.c insertion_sort.c
	./insertion_sort_san demo
	./insertion_sort_san bench 5000

clean:
	rm -f *.o insertion_sort insertion_sort_san

.PHONY: all demo bench csv san clean
