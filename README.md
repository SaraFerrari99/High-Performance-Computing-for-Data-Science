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

1. Ensure the puzzle/ directory exists in the project root.

2. Place your puzzle configuration file in puzzle/puzzle1.txt.

    - Note: Ensure the file format matches the input requirements expected by the source code.
