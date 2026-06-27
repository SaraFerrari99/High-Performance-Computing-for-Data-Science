#include "mpi_rules_helper.h"

UpdateMessage apply_rules(char line[6], char line_sign[P])
{
    UpdateMessage result;
    result.changed = false;
    strcpy(result.line, line);  // copia lo stato iniziale

    bool local_change;

    do {
        local_change = false;
        bool two_start = false;

        // 1. void
        if (check_only_one_void(result.line, line_sign)) {
            local_change = true;
            result.changed = true;
            continue; // torna a ricontrollare dalla void
        }
        
        //VA MESSO STUDIATO UN BLOCCO CHE ASPETTA CHE TUTTI I PROCESSI ARRIVINO QUI E CHE POI PROSEGUE

        // 2. prima regola nuova
        if (check_minor_major(line_sign, result.line)) {
            local_change = true;
            result.changed = true;
            continue;
        }
        
        // 3. seconda regola nuova
        if (check_number_between(line_sign,result.line)) {
            local_change = true;
            result.changed = true;
            continue;
        }
        

       //DEVO SEMPRE RìTORNAR EPER OGNI FUNZIONE QUALCOSA TIPO 1 2 3 4 5 
       //SEMPLICEMENTE PER FARLO MI BASTA MODIFICARE IL CONTENUTO DI RESULT.LINE


    } while (local_change);

    return result;
}