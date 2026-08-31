#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

#define N 9  
#define M 64 
#define P 10 

typedef struct {
    char line[6];
    bool changed;
} UpdateMessage;

typedef struct {
    char row[6];
    char col[6];
} UpdateLine;

typedef struct {
    char possible_number[5];
    int count;
} PossibleNumber;

#endif // COMMON_H