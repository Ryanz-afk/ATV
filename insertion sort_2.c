#include "insertion_sort.h"
void insertion_sort(int *v, size_t n) {
    for (size_t i = 1; i < n; i++) {
        int chave = v[i];
        size_t j = i;
        /* j > 0 vem ANTES de v[j-1] (curto-circuito: nunca le fora do vetor).
         * '>' estrito => estavel (nao passa iguais).
         * size_t nao tem sinal: por isso j > 0 e nao j >= 0. */
        while (j > 0 && v[j - 1] > chave) {
            v[j] = v[j - 1];
            j--;
        }
        v[j] = chave;
    }
}
