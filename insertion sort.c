#ifndef INSERTION_SORT_H
#define INSERTION_SORT_H
#include <stddef.h>
/* Ordena v[0..n-1] em ordem crescente, no proprio vetor.
 * Estavel, adaptativo, espaco auxiliar O(1).
 * Aceita n == 0 (nao faz nada); v pode ser NULL apenas se n == 0. */
void insertion_sort(int *v, size_t n);
#endif
