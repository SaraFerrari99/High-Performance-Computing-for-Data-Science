#include "mpi_number_between.h"
#include <unistd.h>
#include "log.h"

int deduceBetween(int a, int b, char leftOp, char rightOp) {

    LOG_INFO(0,"Deducing number between");

    // Case a < X < b
    if (leftOp == '<' && rightOp == '<') {
        if (b - a == 2) return a + 1;
    }
    // Case a > X > b
    if (leftOp == '>' && rightOp == '>') {
        if (a - b == 2) return b + 1;
    }
    LOG_WARN(0,"Unable to deduce number between, returning 0");
    return 0;
}


bool check_number_between(char line_sign[10], char line[6])
{
    bool modified = false;

    // Scan characters in line_sign to find '0' and check for patterns
    for (int i = 0; i < strlen(line_sign); i++) {
        if (line_sign[i] == '0') {
            // Check pattern between: num < 0 < num
            if (i > 0 && isdigit(line_sign[i-2]) && i+2 < strlen(line_sign) && isdigit(line_sign[i+2]) && ((line_sign[i-1] == '<' && line_sign[i+1] == '<') || (line_sign[i-1] == '>' && line_sign[i+1] == '>'))) {

                int left = line_sign[i-2] - '0';
                int right = line_sign[i+2] - '0';
                char left_sign = line_sign[i-1];
                char right_sign = line_sign[i+1];

                int val = deduceBetween(left, right, left_sign, right_sign);
                if (val != 0) {
                    line_sign[i] = val + '0';
                    line[i/2] = val + '0';
                    modified = true;
                }
            }
        }
    }
    
    return modified;
}