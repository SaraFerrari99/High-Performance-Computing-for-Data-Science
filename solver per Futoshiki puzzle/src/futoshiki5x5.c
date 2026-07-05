#include <mpi.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include "../helper/mpi_send_helpers.h"
#include "../helper/mpi_update_puzzle.h"
#include "../helper/mpi_check_void_helper.h"
#include "../helper/mpi_rules_helper.h"
#include "../helper/mpi_check_cross.h"
#include "../helper/log.h"


void set_puzzle(char puzzle[N][M], char last_array[N][M],char puzzle_without_sign[5][M], char puzzle_without_sign_reverse[5][6], char puzzle_reverse[N][M]){
    
    LOG_INFO(0,"Puzzle setting start");
    LOG_INFO(0,"Opening of the puzzle file puzzle1.txt");

    
    FILE *fp = fopen("../puzzle/puzzle1.txt", "r");
    if (!fp)
    {
        LOG_ERROR(0,"Error in opening the file");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    //check every row and save it
    for (int i = 0; i < N; i++){

        //check if the row is correctly readable
        if (fgets(puzzle[i], M, fp) == NULL)
        {
            LOG_ERROR(0, "Error on reading void row %d\n", i);
            fclose(fp);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Remove eventually new line in the end of the row
        char *p = puzzle[i];
        while (*p)
        {
            if (*p == '\n')
            {
                *p = '\0';
                break;
            }
            p++;
        }
    }

    //start padding the array without signs
    for (int i = 0; i < N; i += 2){
        int k = 0;
        for (int j = 0; puzzle[i][j] != '\0'; j++)
        {
            if (isdigit((unsigned char)puzzle[i][j]))
            {
                puzzle_without_sign[i / 2][k++] = puzzle[i][j];
            }
        }
        puzzle_without_sign[i / 2][k] = '\0'; //end of string/row
    }

    //start padding the reverse array without signs
    for (int c = 0; c < 5; c++)
    {
        for (int r = 0; r < 5; r++)
        {
            puzzle_without_sign_reverse[c][r] = puzzle_without_sign[r][c];
        }
        puzzle_without_sign_reverse[c][5] = '\0';//end of string/row
    }

    //start padding the reverse array with signs
    int cols_lenght = strlen(puzzle[0]); // 11
    for (int c = 0; c < cols_lenght; c++)
    {
        for (int r = 0; r < N; r++)
        {
            puzzle_reverse[c][r] = puzzle[r][c];
        }
        puzzle_reverse[c][N] = '\0';
    }

    //adjust the sign because if i reverte it i have something like:
    //1-3^4-2-5
    for (int c = 0; c < cols_lenght; c = c +2){
        for (int r = 0; puzzle_reverse[c][r] != '\0'; r++)
        {
            if (puzzle_reverse[c][r] == '^')
            {
                puzzle_reverse[c][r] = '<';
            }
            if (puzzle_reverse[c][r] == 'v')
            {
                puzzle_reverse[c][r] = '>';
            }
        }
    }

    fclose(fp);

    for(int i = 0; i < 9; i++){
        strcpy(last_array[i], puzzle[i]);
    }

    LOG_INFO(0,"Puzzle setting completed");
}

void check_if_array_completed(char puzzle_without_sign[5][M], bool repeat)
{
    LOG_INFO(0,"Checking if the array is completed");
    repeat = false;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (puzzle_without_sign[i][j] == '0') {
                repeat = true;
                LOG_INFO(0,"Found a '0' at position (%d, %d)\n", i, j);
                return;  // exit when you find the first '0'
            }
        }
    }

    LOG_WARN(0,"No '0' found\n");
}

void deterministic_logic(char puzzle_without_sign[5][M],
                         char puzzle[N][M], char puzzle_without_sign_reverse[5][6],
                         char puzzle_reverse[N][M],
                         char last_array[N][M]){

    LOG_INFO(0,"Starting deterministic logic");

    //initialization of the repeat variable to do a cicle 
    bool repeat = true;
    bool first_check = false;
    bool second_check = false;
     while (repeat)
        {

            char snapshot[5][M];
            for (int i = 0; i < 5; i++) {
                strcpy(snapshot[i], puzzle_without_sign[i]);
            }

            LOG_INFO(0,"Starting update puzzle");
            while(!first_check){
                // Send rows and columns to workers
                send_row(puzzle_without_sign, puzzle, "1");
                send_column(puzzle_without_sign_reverse, puzzle_reverse);

                // Receive rows updated
                for (int r = 1; r <= 5; r++)
                {

                    char updated_row[6];
                    bool local_repeat;
                    MPI_Recv(updated_row, 6, MPI_CHAR, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&local_repeat, 1, MPI_C_BOOL, r, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    updated_row[5] = '\0';

                    strcpy(puzzle_without_sign[r - 1], updated_row);
                }

                // Receive updated columns
                for (int r = 6; r < 11; r++)
                {
                    char updated_col[6];
                    bool local_repeat;
                    MPI_Recv(updated_col, 6, MPI_CHAR, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&local_repeat, 1, MPI_C_BOOL, r, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    strcpy(puzzle_without_sign_reverse[r - 6], updated_col);
                }

                char array_transposed[5][6];
                first_check = update_puzzle_unsigned(puzzle_without_sign, puzzle_without_sign_reverse, array_transposed,puzzle,last_array, puzzle_reverse);
            }

            LOG_INFO(0,"End Update puzzle");

            //IMPLEMENT X LOGIC ! ROW ALL COLUMS 
            //send to project between 11 and 15 row and column to understand + 0
            // Send rows and columns to workers
            check_if_array_completed(puzzle_without_sign,repeat);
            
            for(int i = 0; i < 5; i++){
                send_row(puzzle_without_sign, puzzle, "2");
                send_only_one_column(puzzle_without_sign_reverse[i], puzzle_reverse[i*2]);
                
                for(int j = 0; j < 5; j++){
                    MPI_Send(&i, 1, MPI_INT, j + 11, 11, MPI_COMM_WORLD);
                }

                for(int m = 0; m < 5; m++){
                    char update_col_cross[6];
                    char update_row_cross[6];
                    MPI_Recv(update_col_cross, 6, MPI_CHAR, m+11, 9, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(update_row_cross, 6, MPI_CHAR, m+11, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                    strcpy(puzzle_without_sign[m], update_row_cross);

                    // FIX DANGEROUS! Update only if update_col_cross has a number!
                    for (int k = 0; k < 5; k++) {
                        if (update_col_cross[k] != '0') {
                            puzzle_without_sign_reverse[i][k] = update_col_cross[k];
                        }
                    }
                }
                char array_transposed_cross[5][6];
                update_puzzle_unsigned(puzzle_without_sign, puzzle_without_sign_reverse, array_transposed_cross, puzzle, last_array, puzzle_reverse);
            }

            first_check = false;

            bool changed = false;
            for (int i = 0; i < 5; i++) {
                if (strcmp(snapshot[i], puzzle_without_sign[i]) != 0) {
                    changed = true;
                    break;
                }
            }

            if (!changed) {
                LOG_WARN(0,"No changes detected\n");
                repeat = false;
                break;
            }

            check_if_array_completed(puzzle_without_sign,repeat);

        }

        LOG_INFO(0, "End deterministic logic");
}

bool is_complete(char puzzle[5][M]) {
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (puzzle[i][j] == '0')
                return false;
    return true;
}

bool has_duplicates(char puzzle[5][M]) {
    for (int i = 0; i < 5; i++) {
        int seen_row[6] = {0};
        int seen_col[6] = {0};

        for (int j = 0; j < 5; j++) {
            if (puzzle[i][j] != '0') {
                int v = puzzle[i][j] - '0';
                if (seen_row[v]) return true;
                seen_row[v] = 1;
            }
            if (puzzle[j][i] != '0') {
                int v = puzzle[j][i] - '0';
                if (seen_col[v]) return true;
                seen_col[v] = 1;
            }
        }
    }
    return false;
}

bool is_safe_guess(
    char puzzle_without_sign[5][M],
    int r, int c,
    char val
) {
    //check rows
    for (int j = 0; j < 5; j++) {
        if (j != c && puzzle_without_sign[r][j] == val)
            return false;
    }

    //check colums
    for (int i = 0; i < 5; i++) {
        if (i != r && puzzle_without_sign[i][c] == val)
            return false;
    }

    return true;
}

bool check_inequality_constraints(
    char puzzle[N][M],
    int r, int c,
    char val
) {
    int v = val - '0';

    // Check right
    if (c > 0) {
        char left = puzzle[r*2][(c-1)*2];
        char sign = puzzle[r*2][(c*2)-1];

        if (left != '0') {
            int lv = left - '0';
            if (sign == '<' && !(lv < v)) return false;
            if (sign == '>' && !(lv > v)) return false;
        }
    }

    //Check left
    if (c < 4) {
        char right = puzzle[r*2][(c+1)*2];
        char sign = puzzle[r*2][(c*2)+1];

        if (right != '0') {
            int rv = right - '0';
            if (sign == '<' && !(v < rv)) return false;
            if (sign == '>' && !(v > rv)) return false;
        }
    }

    // Check up
    if (r > 0) {
        char up = puzzle[(r-1)*2][c*2];
        char sign = puzzle[(r*2)-1][c*2];

        if (up != '0') {
            int uv = up - '0';
            if (sign == '^' && !(uv < v)) return false;  // ^ significa sopra < sotto
            if (sign == 'v' && !(uv > v)) return false;  // v significa sopra > sotto
        }
    }

    //Check down
    if (r < 4) {
        char down = puzzle[(r+1)*2][c*2];
        char sign = puzzle[(r*2)+1][c*2];

        if (down != '0') {
            int dv = down - '0';
            if (sign == '^' && !(v < dv)) return false;
            if (sign == 'v' && !(v > dv)) return false;
        }
    }

    return true;
}

bool is_puzzle_valid_with_signs(char puzzle[N][M]) {
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
            char val = puzzle[r*2][c*2];
            if (val == '0') continue; // Jump void cells. check only present number
            
            int v = val - '0';
            
            //Check left sign (if not in the last column) 
            if (c < 4) {
                char right = puzzle[r*2][(c+1)*2];
                char sign = puzzle[r*2][(c*2)+1];
                if (right != '0') {
                    int rv = right - '0';
                    if (sign == '<' && !(v < rv)) return false;
                    if (sign == '>' && !(v > rv)) return false;
                }
            }
            
            //Check down sign (if not in the last column) 
            if (r < 4) {
                char down = puzzle[(r+1)*2][c*2];
                char sign = puzzle[(r*2)+1][c*2];
                if (down != '0') {
                    int dv = down - '0';
                    if (sign == '^' && !(v < dv)) return false;
                    if (sign == 'v' && !(v > dv)) return false;
                }
            }
        }
    }
    return true; //NO violation found!
}


bool solve_with_guessing(
    char puzzle_without_sign[5][M],
    char puzzle_without_sign_reverse[5][6],
    char puzzle[N][M],
    char puzzle_reverse[N][M],
    char last_array[N][M]
) {

    LOG_INFO(0, "Start guessing");

    int r = -1, c = -1;
    char row[6], col[6];

    //FInd first 0
    for (int i = 0; i < 5 && r == -1; i++) {
        for (int j = 0; j < 5; j++) {
            if (puzzle_without_sign[i][j] == '0') {
                r = i;
                c = j;
                memcpy(row, puzzle_without_sign[i], 5);
                row[5] = '\0';
                memcpy(col, puzzle_without_sign_reverse[j], 5);
                col[5] = '\0';
                break;
            }
        }
    }

    if (r == -1) {
        LOG_INFO(0,"Grid complete! Solution found!\n");
        return true; // <--- FIX: Return true if finish!
    }

    //FOund cndidate numbers
    int candidates[5];
    int count = 0;

    for (int n = 1; n <= 5; n++) {
        bool found = false;
        for (int k = 0; k < 5; k++) {
            if (row[k] == '0' + n || col[k] == '0' + n) {
                found = true;
                break;
            }
        }
        if (!found)
            candidates[count++] = n;
    }

    //Try every candidate
    for (int k = 0; k < count; k++) {
        char guess = '0' + candidates[k];

        if (!is_safe_guess(puzzle_without_sign, r, c, guess)) continue;
        if (!check_inequality_constraints(puzzle, r, c, guess)) continue;
        
        LOG_INFO(0,"Trying guess %c in (%d,%d)\n", guess, r, c);

        // BACKUP Complete
        char bu[5][M], bur[5][6], bp[N][M], br[N][M], bl[N][M];
        for (int i = 0; i < 5; i++) {
            strcpy(bu[i], puzzle_without_sign[i]);
            strcpy(bur[i], puzzle_without_sign_reverse[i]);
        }
        for (int i = 0; i < N; i++) {
            strcpy(bp[i], puzzle[i]);
            strcpy(br[i], puzzle_reverse[i]);
            strcpy(bl[i], last_array[i]);
        }

        //added
        puzzle_without_sign[r][c] = guess;
        puzzle_without_sign_reverse[c][r] = guess;
        puzzle[r*2][c*2] = guess;
        puzzle_reverse[c*2][r*2] = guess;
        last_array[r*2][c*2] = guess;

        //deterministic logic
        deterministic_logic(puzzle_without_sign, puzzle, puzzle_without_sign_reverse, puzzle_reverse, last_array);

    
        // FIX: We go after only if there is no duplicates in the grid and if the grid is valid with signs. 
        //Otherwise, we backtrack immediately.    

        if (!has_duplicates(puzzle_without_sign) && is_puzzle_valid_with_signs(puzzle)) {
            
            if (solve_with_guessing(puzzle_without_sign, puzzle_without_sign_reverse, puzzle, puzzle_reverse, last_array)) {
                return true; 
            }
        }

        //BACKTRACK
        LOG_WARN(0,"Guess %c in (%d,%d) FAILED → restore\n", guess, r, c);
        for (int i = 0; i < 5; i++) {
            strcpy(puzzle_without_sign[i], bu[i]);
            strcpy(puzzle_without_sign_reverse[i], bur[i]);
        }
        for (int i = 0; i < N; i++) {
            strcpy(puzzle[i], bp[i]);
            strcpy(puzzle_reverse[i], br[i]);
            strcpy(last_array[i], bl[i]);
        }
    }

    LOG_WARN(0,"No valid guess in (%d,%d) → FALSE BACKTRACK!\n", r, c);
    return false; //Dangerous FIX: If all the candidates fail, return FALSE!
}

void save_puzzle_to_file(const char *filename, char puzzle[N][M]) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        LOG_ERROR(0, "Fatal error: unable to open file %s\n", filename);
        return;
    }

    for (int i = 0; i < N; i++) {
        fprintf(fp, "%s\n", puzzle[i]);
    }

    fflush(fp);
    fclose(fp);
    LOG_INFO(0, "Puzzle saved correctly in %s", filename);
}

void stop_all_processes() {
    char stop6[6] = "STOP";
    char stop9[9] = "STOP";
    char dummy_puzzle[45] = {0};
    int dummy_int = -1;

    for (int p = 1; p <= 5; p++) {
        MPI_Send(stop6, 6, MPI_CHAR, p, 0, MPI_COMM_WORLD);
        MPI_Send(stop9, 9, MPI_CHAR, p, 2, MPI_COMM_WORLD);
        MPI_Send(dummy_puzzle, 45, MPI_CHAR, p, 6, MPI_COMM_WORLD);
    }

    for (int p = 6; p <= 10; p++) {
        MPI_Send(stop6, 6, MPI_CHAR, p, 0, MPI_COMM_WORLD);
        MPI_Send(stop9, 9, MPI_CHAR, p, 2, MPI_COMM_WORLD);
    }

    for (int p = 11; p <= 15; p++) {
        MPI_Send(stop9, 9, MPI_CHAR, p, 8, MPI_COMM_WORLD);
        MPI_Send(stop9, 9, MPI_CHAR, p, 7, MPI_COMM_WORLD);
        MPI_Send(&dummy_int, 1, MPI_INT, p, 11, MPI_COMM_WORLD);
    }

    LOG_INFO(0, "STOP sent to all processes\n");
}

int main(int argc, char *argv[])
{

    LOG_INFO(NULL, "Starting of the futoshiki resolution");

    char puzzle[N][M];//normal complete puzzle
    char last_array[N][M];//save the last modify puzzle
    char puzzle_reverse[N][M];//reverse complete puzzle (inversion of row with column)
    char puzzle_without_sign[5][M];//normal puzzle without simbol
    char puzzle_without_sign_reverse[5][6];//reverse puzzle without simbol


    //MPI SETTING
    int rank, size;
    MPI_Init(&argc, &argv);//initialization of MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);//setting of MPI RANKING
    MPI_Comm_size(MPI_COMM_WORLD, &size);//establish connection

    //PRINCIPAL PROCESS 0
    if (rank == 0)
    {

        LOG_INFO(rank, "Starting the principal process");

        //variable to cicle until the puzzle is not complete
        LOG_INFO(rank,"SET PUZZLE");
        set_puzzle(puzzle, last_array, puzzle_without_sign, puzzle_without_sign_reverse, puzzle_reverse);
                
        deterministic_logic(puzzle_without_sign, puzzle, puzzle_without_sign_reverse, puzzle_reverse, last_array);

        bool completato = solve_with_guessing(puzzle_without_sign, puzzle_without_sign_reverse, puzzle, puzzle_reverse, last_array);

        if (completato) {
            LOG_INFO(0, "PUZZLE Solved Successfully!\n");
        } else {
            LOG_ERROR(0, "Impossible to solve the puzzle.\n");
        }
        
        struct stat st = {0};

        if (stat("../Solution", &st) == -1) {
            if (mkdir("../Solution", 0777) == -1) {
                LOG_ERROR(0, "Error creating directory");
            } else {
                LOG_INFO(0, "Directory created: %s\n", "../Solution");
            }
        }

        save_puzzle_to_file("../Solution/solution.txt", puzzle);

        stop_all_processes();
        
    }
    else if (rank >= 1 && rank <= 5)
    {
        while (1)
        {
            char puzzle_char[5][9];
            char my_row[6];
            char my_row_sign[10];
            MPI_Recv(my_row, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(my_row_sign, 9, MPI_CHAR, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(puzzle_char, 45, MPI_CHAR, 0, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            my_row_sign[9] = '\0';  // add terminator

            for(int i = 0; i < 5; i++){
                puzzle_char[i][9] = '\0';  //add terminator to every row of the puzzle
            }
            

            if (strncmp(my_row, "STOP", 4) == 0)
            {
                break;
            }
            UpdateMessage res = apply_rules(my_row, my_row_sign);

            MPI_Send(res.line, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&res.changed, 1, MPI_C_BOOL, 0, 1, MPI_COMM_WORLD);
        }
    }
    else if (rank >= 6 && rank <= 10)
    {
        while (1)
        {
            char my_col[6];
            char my_col_reverse[10];
            MPI_Recv(my_col, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(my_col_reverse, 9, MPI_CHAR, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            my_col_reverse[9] = '\0';  // add terminator
            
            if (strncmp(my_col, "STOP", 4) == 0)
            {
                break;
            }
            UpdateMessage res = apply_rules(my_col, my_col_reverse);

            MPI_Send(res.line, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&res.changed, 1, MPI_C_BOOL, 0, 1, MPI_COMM_WORLD);
        }
    }else if (rank >=11 && rank <= 15){
        while(1){

            char my_col_reverse[10];
            char my_row[10];
            int number_of_column;
            MPI_Recv(my_col_reverse, 9, MPI_CHAR, 0, 8, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(my_row, 9, MPI_CHAR, 0, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&number_of_column, 1, MPI_INT, 0, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            my_col_reverse[9] = '\0';
            my_row[9] = '\0';

            if ((strncmp(my_col_reverse, "STOP", 4) == 0) & (strncmp(my_row, "STOP", 4) == 0) )
            {
                break;
            }

            UpdateLine res = cross_rules(my_col_reverse, my_row, rank, number_of_column);//ADD HELPER + FUNCTION
            MPI_Send(res.col, 6, MPI_CHAR, 0, 9, MPI_COMM_WORLD);
            MPI_Send(res.row, 6, MPI_CHAR, 0, 10, MPI_COMM_WORLD);
            //ADD SEND TO 0

        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
