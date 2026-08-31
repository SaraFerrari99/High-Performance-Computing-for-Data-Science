# --- Variables ---
CC = gcc
CFLAGS = -Wall -O3 -fopenmp
       ../helper/update_puzzle.c \
       ../helper/rules_helper.c \
       ../helper/check_cross.c

TARGET = futoshiki_omp

# --- Principal Rules ---
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)