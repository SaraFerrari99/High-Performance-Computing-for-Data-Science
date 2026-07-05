#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

#define LOG_INFO(rank, fmt, ...) \
    printf(COLOR_GREEN "[INFO][%d] " fmt COLOR_RESET "\n", rank, ##__VA_ARGS__)

#define LOG_WARN(rank, fmt, ...) \
    printf(COLOR_YELLOW "[WARN][%d] " fmt COLOR_RESET "\n", rank, ##__VA_ARGS__)

#define LOG_ERROR(rank, fmt, ...) \
    fprintf(stderr, COLOR_RED "[ERROR][%d] " fmt COLOR_RESET "\n", rank, ##__VA_ARGS__)

#define N 9  // dimensione effettiva della griglia letta (numeri + simboli)
#define M 64 // caratteri per riga (N + '\0')