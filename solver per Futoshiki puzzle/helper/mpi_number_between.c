#include "mpi_number_between.h"
#include <unistd.h>

int deduceBetween(int a, int b, char leftOp, char rightOp) {
    // Caso a < X < b
    if (leftOp == '<' && rightOp == '<') {
        if (b - a == 2) return a + 1;
    }
    // Caso a > X > b
    if (leftOp == '>' && rightOp == '>') {
        if (a - b == 2) return b + 1;
    }
    return 0; // non determinabile
}


bool check_number_between(char line_sign[10], char line[6])
{
    bool modified = false;

    // Scansiona caratteri
    for (int i = 0; i < strlen(line_sign); i++) {
        if (line_sign[i] == '0') {
            // Controllo pattern vicino: num < 0 < num
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