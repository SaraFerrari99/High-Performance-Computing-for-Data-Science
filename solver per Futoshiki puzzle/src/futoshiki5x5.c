#include <mpi.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "../helper/mpi_send_helpers.h"
#include "../helper/mpi_update_puzzle.h"
#include "../helper/mpi_check_void_helper.h"
#include "../helper/mpi_rules_helper.h"
#include "../helper/mpi_check_cross.h"


#define N 9  // dimensione effettiva della griglia letta (numeri + simboli)
#define M 64 // caratteri per riga (N + '\0')

void set_puzzle(char puzzle[N][M], char last_array[N][M],char puzzle_without_sign[5][M], char puzzle_without_sign_reverse[5][6], char puzzle_reverse[N][M])
{
    FILE *fp = fopen("../puzzle/puzzle1.txt", "r");
    if (!fp)
    {
        perror("Errore apertura file");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < N; i++)
    {
        if (fgets(puzzle[i], M, fp) == NULL)
        {
            fprintf(stderr, "Errore lettura riga %d\n", i);
            fclose(fp);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Rimuove eventuale newline alla fine
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

    // Adesso riempiamo puzzle_without_sign SOLO con i numeri
    for (int i = 0; i < N; i += 2) // solo le righe "con numeri"
    {
        int k = 0;
        for (int j = 0; puzzle[i][j] != '\0'; j++)
        {
            if (isdigit((unsigned char)puzzle[i][j]))
            {
                puzzle_without_sign[i / 2][k++] = puzzle[i][j];
            }
        }
        puzzle_without_sign[i / 2][k] = '\0'; // termina la stringa
    }

    for (int c = 0; c < 5; c++)
    {
        for (int r = 0; r < 5; r++)
        {
            puzzle_without_sign_reverse[c][r] = puzzle_without_sign[r][c];
        }
        puzzle_without_sign_reverse[c][5] = '\0'; // terminatore di stringa
    }

    int cols_lenght = strlen(puzzle[0]); // 11

    for (int c = 0; c < cols_lenght; c++)
    {
        for (int r = 0; r < N; r++)
        {
            puzzle_reverse[c][r] = puzzle[r][c];
        }
        puzzle_reverse[c][N] = '\0';

    }

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
}

void check_if_array_completed(char puzzle_without_sign[5][M], bool *repeat, char end[10])
{
    *repeat = false; // default
    strcpy(end, "NOTHING");

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (puzzle_without_sign[i][j] == '0') {
                printf("TROVATO UNO 0\n");
                strcpy(end, "0 FOUND");
                *repeat = true;
                return;  // esci appena trovi uno zero
            }
        }
    }
    printf("NESSUNO 0 TROVATO\n");

    strcpy(end,"NOTHING");
}

void deterministic_logic(bool *repeat, bool *first_check, char puzzle_without_sign[5][M],
                         char puzzle[N][M], char puzzle_without_sign_reverse[5][6],
                         char puzzle_reverse[N][M], bool *second_check, char end[10],
                         char last_array[N][M]){

     while (*repeat)
        {

            char snapshot[5][M];
            for (int i = 0; i < 5; i++) {
                strcpy(snapshot[i], puzzle_without_sign[i]);
            }

            printf("NUOVO CICLO\n");
            while(!*first_check){
                // Manda righe e colonne ai worker
                send_row(puzzle_without_sign, puzzle, "1");
                send_column(puzzle_without_sign_reverse, puzzle_reverse);

                // Riceve righe aggiornate
                for (int r = 1; r <= 5; r++)
                {

                    char updated_row[6];
                    bool local_repeat;
                    MPI_Recv(updated_row, 6, MPI_CHAR, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&local_repeat, 1, MPI_C_BOOL, r, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    updated_row[5] = '\0';

                    strcpy(puzzle_without_sign[r - 1], updated_row);
                }

                // Riceve colonne aggiornate
                for (int r = 6; r < 11; r++)
                {
                    char updated_col[6];
                    bool local_repeat;
                    MPI_Recv(updated_col, 6, MPI_CHAR, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&local_repeat, 1, MPI_C_BOOL, r, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    strcpy(puzzle_without_sign_reverse[r - 6], updated_col);
                }

                char array_transposed[5][6];
                *first_check = update_puzzle_unsigned(puzzle_without_sign, puzzle_without_sign_reverse, array_transposed,puzzle,last_array, puzzle_reverse);

                for(int i = 0; i < 5; i++){
                    printf("array final %s\n",puzzle_without_sign[i]);
                }

                for(int i = 0; i < 5; i++){
                    printf("array final reverse%s\n",puzzle_without_sign_reverse[i]);
                }

                for(int i = 0; i < 9; i++){
                    printf("array final with sign%s\n",puzzle[i]);
                }

                for(int i = 0; i < 9; i++){
                    printf("array final puzzle reverse with sign%s\n",puzzle_reverse[i]);
                }

                printf("END First ARRay update\n");
            }

            printf("FUBE PRIMO CICLO\n");


            //IMPLEMENT X LOGIC ! ROW ALL COLUMS 
            //send to project between 11 and 15 row and column to understand + 0
            // Manda righe e colonne ai worker

            check_if_array_completed(puzzle_without_sign,repeat,end);
            
            *second_check = false;

            while(!*second_check){
                bool result = true;
                char update_col_cross[6];
                char update_row_cross[6];
                for(int i = 0; i < 5; i++){
                    send_row(puzzle_without_sign, puzzle, "2");
                    send_only_one_column(puzzle_without_sign_reverse[i],puzzle_reverse[i*2]);
                    for(int j = 0; j < 5; j++){
                        MPI_Send(&i, 1, MPI_INT, j + 11, 11, MPI_COMM_WORLD);
                    }

                    for(int m = 0; m < 5; m++){
                        MPI_Recv(update_col_cross, 6, MPI_CHAR, m+11, 9, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        MPI_Recv(update_row_cross, 6, MPI_CHAR, m+11, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        strcpy(puzzle_without_sign[m], update_row_cross);

                        //PRIMA DI FARE L UPDATE DELLA COLONNA EFFETTIVA VA AGGIORNATA LA COLONNA TOTALE PER TUTTI
                        for (int k = 0; k < 5; k++) {
                            if (puzzle_without_sign_reverse[i][k] != '0'  && update_col_cross[k] != '0') {
                                puzzle_without_sign_reverse[i][k] = update_col_cross[k];
                            }
                        }
                    }
                    char array_transposed_cross[5][6];
                    result = update_puzzle_unsigned(puzzle_without_sign, puzzle_without_sign_reverse, array_transposed_cross,puzzle, last_array, puzzle_reverse);
                    printf("UPDATED");

                    if(i == 4){
                        printf("arrivato al quinto \n");
                        *second_check = result;
                    }
                }
            }

            *first_check = false;

            //DOVREI AVER AGGIORNATO TUTTO GIA !! VERIFICA!!

            for(int i = 0; i < 5; i++){
                printf("array final %s\n",puzzle_without_sign[i]);
            }

            bool changed = false;
            for (int i = 0; i < 5; i++) {
                if (strcmp(snapshot[i], puzzle_without_sign[i]) != 0) {
                    changed = true;
                    break;
                }
            }

            if (!changed) {
                printf("NESSUN CAMBIAMENTO: esco dal ciclo deterministico\n");
                *repeat = false;
                break;
            }

            check_if_array_completed(puzzle_without_sign,repeat,end);

        }
        //DA METTERE IN CHECK NUMBER TEST
        //char stop_msg[6] = "STOP0"; // esattamente 6 caratteri
        //for (int i = 1; i <= 15; i++)
        //{
            //MPI_Send(stop_msg, 6, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        //}
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
    // controllo riga
    for (int j = 0; j < 5; j++) {
        if (j != c && puzzle_without_sign[r][j] == val)
            return false;
    }

    // controllo colonna
    for (int i = 0; i < 5; i++) {
        if (i != r && puzzle_without_sign[i][c] == val)
            return false;
    }

    return true;
}

bool solve_with_guessing(
    char puzzle_without_sign[5][M],
    char puzzle_without_sign_reverse[5][6],
    char puzzle[N][M],
    char puzzle_reverse[N][M],
    char last_array[N][M]
) {
    printf("IN GUESSING\n");

    int r = -1, c = -1;
    char row[6], col[6];

    /* 1️⃣ trova il primo 0 */
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
        printf("Griglia completa, niente guessing\n");
        return false;
    }

    printf("Prima cella vuota: (%d,%d)\n", r, c);
    printf("Row: %s\n", row);
    printf("Col: %s\n", col);

    /* 2️⃣ calcola numeri candidati */
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

    printf("Candidati: ");
    for (int i = 0; i < count; i++)
        printf("%d ", candidates[i]);
    printf("\n");

    /* 3️⃣ prova ogni candidato */
    for (int k = 0; k < count; k++) {

        char guess = '0' + candidates[k];

        if (!is_safe_guess(puzzle_without_sign, r, c, guess)) {
            printf("Scarto %c (doppioni immediati)\n", guess);
            continue;
        }

        printf("Provo guess %c in (%d,%d)\n", guess, r, c);

        /* ===== BACKUP COMPLETO ===== */
        char bu[5][M], bur[5][6];
        char bp[N][M], br[N][M], bl[N][M];

        for (int i = 0; i < 5; i++) {
            strcpy(bu[i], puzzle_without_sign[i]);
            strcpy(bur[i], puzzle_without_sign_reverse[i]);
        }
        for (int i = 0; i < N; i++) {
            strcpy(bp[i], puzzle[i]);
            strcpy(br[i], puzzle_reverse[i]);
            strcpy(bl[i], last_array[i]);
        }

        /* ===== INSERIMENTO ===== */
        puzzle_without_sign[r][c] = guess;
        puzzle_without_sign_reverse[c][r] = guess;
        puzzle[r*2][c*2] = guess;
        puzzle_reverse[c*2][r*2] = guess;
        last_array[r*2][c*2] = guess;

        /* ===== LOGICA DETERMINISTICA ===== */
        bool repeat = true;
        bool first_check = false;
        bool second_check = false;
        char end[10];

        deterministic_logic(
            &repeat,
            &first_check,
            puzzle_without_sign,
            puzzle,
            puzzle_without_sign_reverse,
            puzzle_reverse,
            &second_check,
            end,
            last_array
        );

        /* ===== VERIFICA ===== */
        if (!has_duplicates(puzzle_without_sign)) {
            printf("Guess %c ACCETTATO\n", guess);
            return true;
        }

        /* ===== BACKTRACK ===== */
        printf("Guess %c FALLITO → ripristino\n", guess);

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

    printf("Tutti i guess falliti → backtracking\n");
    return true;
}

int main(int argc, char *argv[])
{
    int rank, size;
    char puzzle[N][M];
    char last_array[N][M];
    char puzzle_reverse[N][M];
    char puzzle_without_sign[5][M];
    char puzzle_without_sign_reverse[5][6];
    bool repeat = true;
    bool first_check = false;
    bool second_check = false;
    char end[10];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {

        bool c = true;
        printf("SET PUZZLE\n");
        set_puzzle(puzzle, last_array, puzzle_without_sign, puzzle_without_sign_reverse, puzzle_reverse);
        
        while(c){

            printf("DETERMNISTIC LOGIC\n");
            deterministic_logic(&repeat, &first_check, puzzle_without_sign, puzzle, puzzle_without_sign_reverse, puzzle_reverse, &second_check, end, last_array);

            printf("SOLVE GUESSING\n");
            c = solve_with_guessing(puzzle_without_sign,puzzle_without_sign_reverse, puzzle, puzzle_reverse,last_array);

            printf("fuori dal while and END\n");
        }

        
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
            my_row_sign[9] = '\0';  // aggiungi terminatore

            for(int i = 0; i < 5; i++){
                puzzle_char[i][9] = '\0';  // aggiungi terminatore alla fine di ogni riga
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
            my_col_reverse[9] = '\0';  // aggiungi terminatore
            
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


    MPI_Finalize();
    return 0;
}
