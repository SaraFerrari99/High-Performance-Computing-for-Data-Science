#ifndef MPI_RULES_HELPER
#define MPI_RULES_HELPER

#include <mpi.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define M 64 // deve corrispondere al main
#define N 9

typedef struct {
    bool changed;     // indica se la regola ha fatto modifiche
    char line[6];     // qui metti la riga o colonna (5 + terminatore '\0')
} UpdateMessage;

UpdateMessage apply_rules(char line[6], char line_reverse[N]);

#endif