# --- Variables ---
CC = mpicc
CFLAGS = -Wall -I./helper

# --- File Sources ---
SRCS = futoshiki5x5.c \
       ../helper/mpi_send_helpers.c \
       ../helper/mpi_check_void_helper.c \
       ../helper/mpi_update_puzzle.c \
       ../helper/mpi_rules_helper.c \
       ../helper/mpi_check_major_minor_helper.c \
       ../helper/mpi_number_between.c \
       ../helper/mpi_check_cross.c

TARGET = futoshiki5x5

# --- Principal Rules ---
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	mpirun --oversubscribe -n 16 ./$(TARGET)

clean:
	rm -f $(TARGET)