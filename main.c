/* Insertion Sort - demonstracao e benchmark em 5 cenarios.
 * Uso: ./insertion_sort demo            (passo a passo)
 *      ./insertion_sort bench [N]       (tempo nos 5 cenarios; padrao N=40000)
 *      ./insertion_sort csv             (varios N, saida CSV para os graficos)
 * Cada tempo e a MEDIANA de REPS execucoes (reduz ruido de medicao).
 *
 * Compilar:  make
 *   ou sem make (Windows/Linux/Mac):
 *   gcc -std=c11 -Wall -Wextra -O0 -o insertion_sort main.c insertion_sort.c
 */
#ifdef __MINGW32__
#define __USE_MINGW_ANSI_STDIO 1   /* faz o printf("%zu") funcionar no MinGW */
#endif
#ifndef _WIN32
#define _POSIX_C_SOURCE 199309L    /* clock_gettime (Linux/Mac) */
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "insertion_sort.h"

#define REPS 5
#define N_PADRAO 40000

/* ---------- Versao didatica: imprime o vetor a cada passo ---------- */
static void imprime(const int *v, size_t n, size_t ordenado_ate) {
    for (size_t k = 0; k < n; k++)
        printf(k == ordenado_ate ? "| %d " : "%d ", v[k]);
    printf("\n");
}

static void insertion_sort_passos(int *v, size_t n) {
    printf("Inicial: "); imprime(v, n, n);
    for (size_t i = 1; i < n; i++) {
        int chave = v[i];
        size_t j = i;
        while (j > 0 && v[j - 1] > chave) { v[j] = v[j - 1]; j--; }
        v[j] = chave;
        printf("i=%zu (chave=%d): ", i, chave);
        imprime(v, n, i + 1);
    }
}

/* ---------- Geradores de cenarios ---------- */
/* rand() no Windows so chega a 32767; juntamos duas chamadas (30 bits). */
static int aleat(void) {
    return (int)((((unsigned)rand() << 15) ^ (unsigned)rand()) & 0x7fffffffu);
}
static void gera_aleatorio(int *v, size_t n)   { for (size_t i = 0; i < n; i++) v[i] = aleat(); }
static void gera_ordenado(int *v, size_t n)    { for (size_t i = 0; i < n; i++) v[i] = (int)i; }
static void gera_decrescente(int *v, size_t n) { for (size_t i = 0; i < n; i++) v[i] = (int)(n - i); }
static void gera_quase(int *v, size_t n) {
    gera_ordenado(v, n);
    size_t trocas = n / 100 + 1;              /* ~1% de perturbacoes */
    for (size_t t = 0; t < trocas; t++) {
        size_t a = (size_t)aleat() % n, b = (size_t)aleat() % n;
        int tmp = v[a]; v[a] = v[b]; v[b] = tmp;
    }
}
static void gera_duplicadas(int *v, size_t n)  { for (size_t i = 0; i < n; i++) v[i] = rand() % 10; }

typedef void (*gerador)(int *, size_t);
static const char *NOMES[5] = {"aleatorio", "ordenado", "decrescente", "quase_ordenado", "duplicadas"};
static const gerador GER[5] = {gera_aleatorio, gera_ordenado, gera_decrescente, gera_quase, gera_duplicadas};

static int esta_ordenado(const int *v, size_t n) {
    for (size_t i = 1; i < n; i++) if (v[i - 1] > v[i]) return 0;
    return 1;
}

#ifdef _WIN32
static double agora(void) {                /* Windows: contador de alta resolucao */
    LARGE_INTEGER freq, cont;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cont);
    return (double)cont.QuadPart / (double)freq.QuadPart;
}
#else
static double agora(void) {                /* Linux/Mac */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}
#endif

static int cmp_double(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

/* Tempo (s) de um cenario: mediana de REPS execucoes; -1 em erro. */
static double mede(int cenario, size_t n) {
    if (n == 0 || n > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "Erro: N invalido\n");
        return -1.0;
    }
    int *v = malloc(n * sizeof *v);
    if (v == NULL) { fprintf(stderr, "Erro: malloc falhou\n"); return -1.0; }

    double tempos[REPS];
    srand(42);                                 /* semente fixa: reproduzivel */
    for (int r = 0; r < REPS; r++) {
        GER[cenario](v, n);                    /* regenera: o sort altera o vetor */
        double t0 = agora();
        insertion_sort(v, n);
        tempos[r] = agora() - t0;
        if (!esta_ordenado(v, n)) {
            fprintf(stderr, "Erro: vetor nao ordenado!\n");
            free(v);
            return -1.0;
        }
    }
    free(v);
    qsort(tempos, REPS, sizeof tempos[0], cmp_double);
    return tempos[REPS / 2];
}

int main(int argc, char **argv) {
    const char *modo = argc > 1 ? argv[1] : "demo";

    if (strcmp(modo, "demo") == 0) {
        int v[] = {5, 2, 4, 6, 1, 3};
        insertion_sort_passos(v, sizeof v / sizeof v[0]);
    } else if (strcmp(modo, "bench") == 0) {
        size_t n = N_PADRAO;
        if (argc > 2) {
            char *fim;
            n = (size_t)strtoul(argv[2], &fim, 10);
            if (*fim != '\0' || n == 0) { fprintf(stderr, "N invalido\n"); return 1; }
        }
        printf("N = %zu (mediana de %d execucoes)\n%-16s %s\n", n, REPS, "cenario", "tempo (s)");
        for (int c = 0; c < 5; c++) {
            double t = mede(c, n);
            if (t < 0) return 1;
            printf("%-16s %.6f\n", NOMES[c], t);
        }
    } else if (strcmp(modo, "csv") == 0) {
        size_t tam[] = {1000, 2000, 5000, 10000, 20000, 40000};
        printf("N,cenario,tempo_s\n");
        for (size_t k = 0; k < sizeof tam / sizeof tam[0]; k++)
            for (int c = 0; c < 5; c++) {
                double t = mede(c, tam[k]);
                if (t < 0) return 1;
                printf("%zu,%s,%.6f\n", tam[k], NOMES[c], t);
            }
    } else {
        fprintf(stderr, "Uso: %s [demo | bench [N] | csv]\n", argv[0]);
        return 1;
    }
    return 0;
}
