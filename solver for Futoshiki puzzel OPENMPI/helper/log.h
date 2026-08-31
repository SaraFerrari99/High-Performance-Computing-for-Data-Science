#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"

#define LOG_INFO(rank, fmt, ...) \
    printf(COLOR_GREEN "[INFO][OMP] " fmt COLOR_RESET "\n", ##__VA_ARGS__)

#define LOG_WARN(rank, fmt, ...) \
    printf(COLOR_YELLOW "[WARN][OMP] " fmt COLOR_RESET "\n", ##__VA_ARGS__)

#define LOG_ERROR(rank, fmt, ...) \
    fprintf(stderr, COLOR_RED "[ERROR][OMP] " fmt COLOR_RESET "\n", ##__VA_ARGS__)

#endif // LOG_H