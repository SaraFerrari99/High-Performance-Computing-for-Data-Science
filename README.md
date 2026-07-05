# Parallel Futoshiki Solver (MPI)

This project is a high-performance **Futoshiki puzzle solver** developed using C and the **Message Passing Interface (MPI)**. The application utilizes a master-worker architecture to distribute constraint checking and grid validation across multiple processes, significantly speeding up the resolution of complex puzzles.

## Prerequisites

To compile and execute this project, you need an MPI library (such as OpenMPI) installed on your system.

**Install the required dependencies:**
```bash
sudo apt update && sudo apt install openmpi-bin libopenmpi-dev -y
```

## Setup

Before running the solver, you must provide the puzzle input:

1. Ensure the puzzle directory exists in the project root.

2. Place your puzzle configuration file in puzzle/puzzle1.txt.

    - Note: Ensure the file format matches the input requirements expected by the source code.

# Compilation and Execution
1. Navigate to the src folder:
    ```bash
    cd src
    ```
2. Compile the source code:
    ```bash
    make
    ```
3. Run the application:
    ```bash
    make run
    ```

# Viewing Results
Once the execution is finished, the program will generate the result in the Solution/ directory.

    - Output File: Solution/solution.txt

# Architecture Overview
The solver follows a master-worker communication pattern. The master process handles the main backtracking logic and file I/O, while worker processes compute specific row, column, and cross-constraint validations in parallel.

# Project Structure
- ``` src/ ```: Main source code and Makefile.

- ``` helper/ ```: Auxiliary libraries for MPI communication, rules, and logging.

- ``` puzzle/ ```: Input directory for puzzle files.

- ``` Solution/ ```: Output directory for generated solutions.

Developed for High-Performance Computing (HPC) environments.