#include "rules_helper.h"
#include <string.h>
#include <ctype.h>

bool check_only_one_void(char line[6], char line_sign[10]) {
    int zero_index = -1;
    bool found[6] = {false};
    for (int i = 0; i < 5; i++) {
        if (line[i] == '0') {
            if (zero_index != -1) return false;
            zero_index = i;
        } else {
            int num = line[i] - '0';
            if (num >= 1 && num <= 5) found[num] = true;
        }
    }
    if (zero_index == -1) return false;

    for (int num = 1; num <= 5; num++) {
        if (!found[num]) {
            line[zero_index] = '0' + num;
            for (int i = 0; i < 5; i++) line_sign[i * 2] = line[i];
            return true; 
        }
    }
    return false;
}

bool check_minor_major(char line_sign[10], char line[6]) {
    char save_array[11];
    memcpy(save_array, line_sign, 10);
    save_array[10] = '\0';
    bool modified = false;
    int len = strlen(save_array);

    for (int i = 0; i < len; i++) {
        if(i == 0 && i < len -2 ){
            if (save_array[i] == '2' && save_array[i + 1] == '>' && save_array[i + 2] == '0') {
                save_array[i + 2] = '1'; modified = true;
            } else if (save_array[i] == '4' && save_array[i + 1] == '<' && save_array[i + 2] == '0') {
                save_array[i + 2] = '5'; modified = true;
            }
        } else if (i >= 2) {
            if (save_array[i] == '2' && save_array[i - 1] == '<' && save_array[i - 2] == '0') {
                save_array[i - 2] = '1'; modified = true;
            } else if (save_array[i] == '4' && save_array[i - 1] == '>' && save_array[i - 2] == '0') {
                save_array[i - 2] = '5'; modified = true;
            }
        }
    }

    if(modified){
        int pos = 0;
        for (int i = 0; save_array[i] != '\0'; i++) {
            if (isdigit((unsigned char)save_array[i])) {
                line[pos++] = save_array[i];
                if (pos >= 5) break; 
            }
        }
        line[pos] = '\0';
        strcpy(line_sign, save_array);
        for(int i = 0; i < 5; i++) line[i] = line_sign[i*2];
    }
    return modified;
}

int deduceBetween(int a, int b, char leftOp, char rightOp) {
    if (leftOp == '<' && rightOp == '<' && b - a == 2) return a + 1;
    if (leftOp == '>' && rightOp == '>' && a - b == 2) return b + 1;
    return 0;
}

bool check_number_between(char line_sign[10], char line[6]) {
    bool modified = false;
    for (int i = 0; i < strlen(line_sign); i++) {
        if (line_sign[i] == '0') {
            if (i > 0 && isdigit(line_sign[i-2]) && i+2 < strlen(line_sign) && isdigit(line_sign[i+2]) && 
               ((line_sign[i-1] == '<' && line_sign[i+1] == '<') || (line_sign[i-1] == '>' && line_sign[i+1] == '>'))) {
                int left = line_sign[i-2] - '0';
                int right = line_sign[i+2] - '0';
                int val = deduceBetween(left, right, line_sign[i-1], line_sign[i+1]);
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

UpdateMessage apply_rules(char line[6], char line_sign[P]) {
    UpdateMessage result;
    result.changed = false;
    strcpy(result.line, line);
    bool local_change;
    do {
        local_change = false;
        if (check_only_one_void(result.line, line_sign) ||
            check_minor_major(line_sign, result.line) ||
            check_number_between(line_sign, result.line)) {
            local_change = true;
            result.changed = true;
        }
    } while (local_change);
    return result;
}