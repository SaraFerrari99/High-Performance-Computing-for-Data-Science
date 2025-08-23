#include "mpi_rules_helper.h"
#include "mpi_check_void_helper.h"
#include "mpi_check_major_minor_helper.h"

UpdateMessage apply_rules(char line[6], char line_reverse[N])
{
    UpdateMessage result;
    result.changed = false;
    strcpy(result.line, line);  // copia lo stato iniziale

    bool local_change;

    do {
        local_change = false;

        // 1. void
        if (check_only_one_void(result.line)) {
            local_change = true;
            result.changed = true;
            continue; // torna a ricontrollare dalla void
        }

        // 2. prima regola nuova
        if (check_minor_major(line_reverse, result.line)) {
            local_change = true;
            result.changed = true;
            continue;
        }

        // 3. seconda regola nuova
        /*
        if (check_second_rule(result.line)) {
            local_change = true;
            result.changed = true;
            continue;
        }
        */

       //DEVO SEMPRE RìTORNAR EPER OGNI FUNZIONE QUALCOSA TIPO 1 2 3 4 5 
       //SEMPLICEMENTE PER FARLO MI BASTA MODIFICARE IL CONTENUTO DI RESULT.LINE


        local_change = false;
    } while (local_change);

    return result;
}