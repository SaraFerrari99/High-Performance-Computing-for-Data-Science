#include "mpi_rules_helper.h"
#include "log.h"

UpdateMessage apply_rules(char line[6], char line_sign[P])
{
    UpdateMessage result;
    result.changed = false;
    strcpy(result.line, line);  //Copy the original line to result.line

    bool local_change;

    do {
        local_change = false;
        bool two_start = false;

        // 1.First rule: check for only one void
        if (check_only_one_void(result.line, line_sign)) {
            local_change = true;
            result.changed = true;
            continue; //Restart the loop to check for further changes
        }
        
        // 2.Second rule: check for minor/major constraints
        if (check_minor_major(line_sign, result.line)) {
            local_change = true;
            result.changed = true;
            continue;
        }
        
        // 3. Third rule: check for number between constraints
        if (check_number_between(line_sign,result.line)) {
            local_change = true;
            result.changed = true;
            continue;
        }
        
    } while (local_change);

    return result;
}