#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "../helper/log.h"
#include "../helper/common.h"
#include "../helper/update_puzzle.h"
#include "../helper/rules_helper.h"
#include "../helper/check_cross.h"

void set_puzzle(char puzzle[N][M], char last_array[N][M], char puzzle_without_sign[5][M], char puzzle_without_sign_reverse[5][6], char puzzle_reverse[N][M]) {
    FILE *fp = fopen("../puzzle/puzzle1.txt", "r");
    if (!fp) {
        LOG_ERROR(0,"Error in opening the file");
        exit(1);
    }
    for (int i = 0; i < N; i++) {
        if (fgets(puzzle[i], M, fp) == NULL) continue;
        char *p = puzzle[i];
        while (*p) { if (*p == '\n' || *p == '\r') { *p = '\0'; break; } p++; }
    }
    for (int i = 0; i < N; i += 2) {
        int k = 0;
        for (int j = 0; puzzle[i][j] != '\0'; j++)
            if (isdigit((unsigned char)puzzle[i][j])) puzzle_without_sign[i / 2][k++] = puzzle[i][j];
        puzzle_without_sign[i / 2][k] = '\0';
    }
    for (int c = 0; c < 5; c++) {
        for (int r = 0; r < 5; r++) puzzle_without_sign_reverse[c][r] = puzzle_without_sign[r][c];
        puzzle_without_sign_reverse[c][5] = '\0';
    }
    int cols_lenght = strlen(puzzle[0]);
    for (int c = 0; c < N; c++) {
        for (int r = 0; r < N; r++) puzzle_reverse[c][r] = puzzle[r][c];
            puzzle_reverse[c][N] = '\0';
    }
    for (int c = 0; c < cols_lenght; c = c +2){
        for (int r = 0; puzzle_reverse[c][r] != '\0'; r++) {
            if (puzzle_reverse[c][r] == '^') puzzle_reverse[c][r] = '<';
            if (puzzle_reverse[c][r] == 'v') puzzle_reverse[c][r] = '>';
        }
    }
    fclose(fp);
    for(int i = 0; i < 9; i++) strcpy(last_array[i], puzzle[i]);
}

void print_debug_puzzles(char puzzle[N][M], char puzzle_without_sign[5][M], char puzzle_without_sign_reverse[5][6], char puzzle_reverse[N][M]) {
    printf(COLOR_YELLOW "\n--- DEBUG: PRINCIPAL PUZZLE (9x9) ---" COLOR_RESET "\n");
    for (int i = 0; i < N; i++) {
        printf("%s\n", puzzle[i]);
    }

    printf(COLOR_YELLOW "\n--- DEBUG: PUZZLE ONLY NUMBERS (5x5) ---" COLOR_RESET "\n");
    for (int i = 0; i < 5; i++) {
        printf("%s\n", puzzle_without_sign[i]);
    }

    printf(COLOR_YELLOW "\n--- DEBUG: PUZZLE REVERSED ONLY NUMBERS (5x5) ---" COLOR_RESET "\n");
    for (int i = 0; i < 5; i++) {
        printf("%s\n", puzzle_without_sign_reverse[i]);
    }

    printf(COLOR_YELLOW "\n--- DEBUG: PUZZLE REVERSED COMPLETE (9x9) ---" COLOR_RESET "\n");
    for (int i = 0; i < N; i++) {
        printf("%s\n", puzzle_reverse[i]);
    }
    printf("\n");
}

void deterministic_logic(char puzzle_without_sign[5][M], char puzzle[N][M], char puzzle_without_sign_reverse[5][6], char puzzle_reverse[N][M], char last_array[N][M]) {
    LOG_INFO(0,"Starting deterministic logic with OpenMP");

    double t_omp_start, t_omp_end;
    double tempo_totale_parallel = 0.0;
    int regioni_parallele = 0;

    bool repeat = true;
    while (repeat) {
        char snapshot[5][M];
        for (int i = 0; i < 5; i++) strcpy(snapshot[i], puzzle_without_sign[i]);

        bool first_check = false;
        while(!first_check) {
            char local_rows[5][6];
            char local_cols[5][6];
            char local_puzzle[N][M];
            char local_puzzle_reverse[N][M];
            
            memcpy(local_puzzle, puzzle, sizeof(local_puzzle));
            memcpy(local_puzzle_reverse, puzzle_reverse, sizeof(local_puzzle_reverse));
            for (int i=0; i<5; i++) {
                strcpy(local_rows[i], puzzle_without_sign[i]);
                strcpy(local_cols[i], puzzle_without_sign_reverse[i]);
            }

        print_debug_puzzles(puzzle, puzzle_without_sign, puzzle_without_sign_reverse, puzzle_reverse);

            t_omp_start = omp_get_wtime();

            #pragma omp parallel
            {
                #pragma omp for nowait
                for (int r = 0; r < 5; r++) {
                    UpdateMessage res = apply_rules(local_rows[r], local_puzzle[r * 2]);
                    strcpy(local_rows[r], res.line);
                }
                #pragma omp for nowait
                for (int c = 0; c < 5; c++) {
                    UpdateMessage res = apply_rules(local_cols[c], local_puzzle_reverse[c * 2]);
                    strcpy(local_cols[c], res.line);
                }
            }

            t_omp_end = omp_get_wtime();
            tempo_totale_parallel += (t_omp_end - t_omp_start);
            regioni_parallele++;

            for(int i=0; i<5; i++) {
                strcpy(puzzle_without_sign[i], local_rows[i]);
                strcpy(puzzle_without_sign_reverse[i], local_cols[i]);
                strcpy(puzzle[i*2], local_puzzle[i*2]);
                strcpy(puzzle_reverse[i*2], local_puzzle_reverse[i*2]);
            }

            char array_transposed[5][6];
            first_check = update_puzzle_unsigned(puzzle_without_sign, puzzle_without_sign_reverse, array_transposed, puzzle, last_array, puzzle_reverse);
        }

        repeat = check_if_array_completed(puzzle_without_sign);

        for (int i = 0; i < 5; i++) {
            char local_cross_rows[5][6];
            char local_cross_cols[5][6];

            #pragma omp parallel for
            for(int j = 0; j < 5; j++){
                char col_copy[10];
                char row_copy[M];
                strcpy(col_copy, puzzle_reverse[j*2]);
                strcpy(row_copy, puzzle[i*2]);
                
                // NOTA: j è la colonna, i è la riga. La tua funzione in check_cross.c 
                // usa 'j' per indicare l'indice della colonna, e 'number_of_column' per indicare la riga.
                UpdateLine res = cross_rules(col_copy, row_copy, j, i);
                strcpy(local_cross_cols[j], res.col);
                strcpy(local_cross_rows[j], res.row);
            }

            // ORA RIMETTIAMO INSIEME I PEZZI CON CURA
            for(int j = 0; j < 5; j++){
                // Controlliamo l'incrocio esatto (riga i, colonna j)
                char nuovo_valore = local_cross_rows[j][j]; 
                
                if (nuovo_valore != '0' && puzzle_without_sign[i][j] == '0') {
                    puzzle_without_sign[i][j] = nuovo_valore;
                    puzzle_without_sign_reverse[j][i] = nuovo_valore;
                }
            }
        }
        char array_transposed_cross[5][6];
        update_puzzle_unsigned(puzzle_without_sign, puzzle_without_sign_reverse, array_transposed_cross, puzzle, last_array, puzzle_reverse);
        

        bool changed = false;
        for (int i = 0; i < 5; i++) {
            if (strcmp(snapshot[i], puzzle_without_sign[i]) != 0) {
                changed = true; break;
            }
        }
        if (!changed) { repeat = false; break; }
        repeat = check_if_array_completed(puzzle_without_sign);
    }
}

bool has_duplicates(char puzzle[5][M]) {
    for (int i = 0; i < 5; i++) {
        int seen_row[6] = {0}, seen_col[6] = {0};
        for (int j = 0; j < 5; j++) {
            if (puzzle[i][j] != '0') {
                if (seen_row[puzzle[i][j] - '0']) return true;
                seen_row[puzzle[i][j] - '0'] = 1;
            }
            if (puzzle[j][i] != '0') {
                if (seen_col[puzzle[j][i] - '0']) return true;
                seen_col[puzzle[j][i] - '0'] = 1;
            }
        }
    }
    return false;
}

bool is_safe_guess(char p[5][M], int r, int c, char val) {
    for (int j = 0; j < 5; j++) if (j != c && p[r][j] == val) return false;
    for (int i = 0; i < 5; i++) if (i != r && p[i][c] == val) return false;
    return true;
}

bool check_inequality_constraints(char puzzle[N][M], int r, int c, char val) {
    int v = val - '0';
    if (c > 0 && puzzle[r*2][(c-1)*2] != '0') {
        int lv = puzzle[r*2][(c-1)*2] - '0'; char sign = puzzle[r*2][(c*2)-1];
        if (sign == '<' && !(lv < v)) return false; 
        if (sign == '>' && !(lv > v)) return false;
    }
    if (c < 4 && puzzle[r*2][(c+1)*2] != '0') {
        int rv = puzzle[r*2][(c+1)*2] - '0'; char sign = puzzle[r*2][(c*2)+1];
        if (sign == '<' && !(v < rv)) return false; 
        if (sign == '>' && !(v > rv)) return false;
    }
    if (r > 0 && puzzle[(r-1)*2][c*2] != '0') {
        int uv = puzzle[(r-1)*2][c*2] - '0'; char sign = puzzle[(r*2)-1][c*2];
        if (sign == '^' && !(uv < v)) return false; 
        if (sign == 'v' && !(uv > v)) return false;
    }
    if (r < 4 && puzzle[(r+1)*2][c*2] != '0') {
        int dv = puzzle[(r+1)*2][c*2] - '0'; char sign = puzzle[(r*2)+1][c*2];
        if (sign == '^' && !(v < dv)) return false; 
        if (sign == 'v' && !(v > dv)) return false;
    }
    return true;
}

bool is_puzzle_valid_with_signs(char puzzle[N][M]) {
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
            char val = puzzle[r*2][c*2];
            if (val == '0') continue; 
            if (!check_inequality_constraints(puzzle, r, c, val)) return false;
        }
    }
    return true;
}

bool solve_with_guessing(char p_ns[5][M], char p_ns_rev[5][6], char p[N][M], char p_rev[N][M], char last[N][M]) {
    int r = -1, c = -1;
    char row[6], col[6];

    for (int i = 0; i < 5 && r == -1; i++) {
        for (int j = 0; j < 5; j++) {
            if (p_ns[i][j] == '0') {
                r = i; c = j;
                memcpy(row, p_ns[i], 5); row[5] = '\0';
                memcpy(col, p_ns_rev[j], 5); col[5] = '\0';
                break;
            }
        }
    }

    if (r == -1) return true; 

    int candidates[5], count = 0;
    for (int n = 1; n <= 5; n++) {
        bool found = false;
        for (int k = 0; k < 5; k++) if (row[k] == '0' + n || col[k] == '0' + n) { found = true; break; }
        if (!found) candidates[count++] = n;
    }

    for (int k = 0; k < count; k++) {
        char guess = '0' + candidates[k];
        if (!is_safe_guess(p_ns, r, c, guess) || !check_inequality_constraints(p, r, c, guess)) continue;

        char bu[5][M], bur[5][6], bp[N][M], br[N][M], bl[N][M];
        for (int i = 0; i < 5; i++) { strcpy(bu[i], p_ns[i]); strcpy(bur[i], p_ns_rev[i]); }
        for (int i = 0; i < N; i++) { strcpy(bp[i], p[i]); strcpy(br[i], p_rev[i]); strcpy(bl[i], last[i]); }

        p_ns[r][c] = guess; p_ns_rev[c][r] = guess;
        p[r*2][c*2] = guess; p_rev[c*2][r*2] = guess; last[r*2][c*2] = guess;

        deterministic_logic(p_ns, p, p_ns_rev, p_rev, last);

        if (!has_duplicates(p_ns) && is_puzzle_valid_with_signs(p)) {
            if (solve_with_guessing(p_ns, p_ns_rev, p, p_rev, last)) return true;
        }

        for (int i = 0; i < 5; i++) { strcpy(p_ns[i], bu[i]); strcpy(p_ns_rev[i], bur[i]); }
        for (int i = 0; i < N; i++) { strcpy(p[i], bp[i]); strcpy(p_rev[i], br[i]); strcpy(last[i], bl[i]); }
    }
    return false;
}

void save_puzzle_to_file(const char *filename, char puzzle[N][M]) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    for (int i = 0; i < N; i++) fprintf(fp, "%s\n", puzzle[i]);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    omp_set_dynamic(0);     
    omp_set_num_threads(16); 
    
    LOG_INFO(0, "Starting Futoshiki resolution using OpenMP Shared Memory");

    char puzzle[N][M];
    char last_array[N][M];
    char puzzle_reverse[N][M];
    char puzzle_without_sign[5][M];
    char puzzle_without_sign_reverse[5][6];

    double start_time, end_time;

    set_puzzle(puzzle, last_array, puzzle_without_sign, puzzle_without_sign_reverse, puzzle_reverse);
            
    start_time = omp_get_wtime();

    deterministic_logic(puzzle_without_sign, puzzle, puzzle_without_sign_reverse, puzzle_reverse, last_array);

    bool completato = solve_with_guessing(puzzle_without_sign, puzzle_without_sign_reverse, puzzle, puzzle_reverse, last_array);

    end_time = omp_get_wtime();

    if (completato) {
        LOG_INFO(0, "PUZZLE Solved Successfully!");
        LOG_INFO(0, "Execution Time: %f seconds", end_time - start_time);
    } else {
        LOG_ERROR(0, "Impossible to solve the puzzle.");
    }
    
    struct stat st = {0};
    if (stat("../Solution", &st) == -1) {
        mkdir("../Solution", 0777);
    }
    save_puzzle_to_file("../Solution/solution.txt", puzzle);

    LOG_INFO(0, "Execution Terminate!");
    return 0;
}