#include "check_cross.h"
#include <string.h>

void check_valid_number(char col[P], char row[P], PossibleNumber *number_possible){
    number_possible->count = 0;
    for (int i = 1; i < 6; i++) {
        bool found = false;
        for (int j = 0; j < 5; j++) {
            if (col[j*2] == '0' + i || row[j*2] == '0' + i) { found = true; break; }
        }
        if (!found) {
            number_possible->possible_number[number_possible->count++] = '0' + i;
        }
    }
}

int is_digit(char c) { return (c >= '1' && c <= '5'); }

void extract_range_from_constraints(char line[P], int pos, int *minV, int *maxV) {
    int dist = 0, i = pos + 1;
    while (i < P) {
        if (line[i] == '<' || line[i] == '>') { dist++; i += (line[i]=='<')?1:-1; if(line[i]=='>') i+=2;} 
        if (line[i] == '<') { dist++; i++; }
        else if (line[i] == '>') { dist++; i++; }
        else if (is_digit(line[i])) {
            if (line[i - dist * 2] == '<') *maxV = (*maxV < (line[i] - '0' - dist)) ? *maxV : (line[i] - '0' - dist);
            else *minV = (*minV > (line[i] - '0' + dist)) ? *minV : (line[i] - '0' + dist);
            break;
        } else break;
    }
    dist = 0; i = pos - 1;
    while (i >= 0) {
        if (line[i] == '<') { dist++; i--; }
        else if (line[i] == '>') { dist++; i--; }
        else if (is_digit(line[i])) {
            if (line[i + dist * 2] == '<') *minV = (*minV > (line[i] - '0' + dist)) ? *minV : (line[i] - '0' + dist);
            else *maxV = (*maxV < (line[i] - '0' - dist)) ? *maxV : (line[i] - '0' - dist);
            break;
        } else break;
    }
    if (*minV < 1) *minV = 1;
    if (*maxV > 5) *maxV = 5;
}

bool check_constraints(char line[P], int pos, int num) {
    if (pos >= 2) {
        if (line[pos - 1] == '<' && is_digit(line[pos - 2]) && (line[pos - 2] - '0') >= num) return false;
        if (line[pos - 1] == '>' && is_digit(line[pos - 2]) && (line[pos - 2] - '0') <= num) return false;
    }
    if (pos <= P - 3) {
        if (line[pos + 1] == '<' && is_digit(line[pos + 2]) && (line[pos + 2] - '0') <= num) return false;
        if (line[pos + 1] == '>' && is_digit(line[pos + 2]) && (line[pos + 2] - '0') >= num) return false;
    }
    return true;
}

bool check_full_line(char line[P]) {
    for(int i = 1; i < P-1; i += 2) {
        if(line[i] == '<' && is_digit(line[i-1]) && is_digit(line[i+1]) && line[i-1] >= line[i+1]) return false;
        if(line[i] == '>' && is_digit(line[i-1]) && is_digit(line[i+1]) && line[i-1] <= line[i+1]) return false;
    }
    return true;
}

char take_final_value(char row[P], int pos, char col[P], int *rMin, int *rMax, int *count) {
    int used_row[6] = {0}, used_col[6] = {0};
    for(int i = 0; i < P; i += 2) {
        if(row[i] >= '1' && row[i] <= '5' && i != pos) used_row[row[i]-'0'] = 1;
        if(col[i] >= '1' && col[i] <= '5') used_col[col[i]-'0'] = 1;
    }
    char final_value = 0; *count = 0;
    for(int n = *rMin; n <= *rMax; n++) {
        row[pos] = '0' + n;
        if(!used_row[n] && !used_col[n] && check_constraints(row, pos, n) && check_full_line(row)) {
            final_value = '0' + n; (*count)++;
        }
    }
    row[pos] = '0';
    return (*count > 1) ? '0' : final_value;
}

UpdateLine cross_rules(char col[P], char row[P], int j, int number_of_column) {
    UpdateLine result;
    PossibleNumber number;
    int pos = j * 2;
    int col_index = number_of_column * 2;

    if (col[col_index] == '0') {
        check_valid_number(col, row, &number);
        if (number.count == 1) {
            row[pos] = number.possible_number[0];
            col[col_index] = number.possible_number[0];
        } else {
            int rMin = 1, rMax = 5, count = 0;
            extract_range_from_constraints(row, pos, &rMin, &rMax);
            char final_value = take_final_value(row, pos, col, &rMin, &rMax, &count);
            if (count == 1 && row[pos] == '0') {
                row[pos] = final_value;
                col[col_index] = final_value;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        result.row[i] = row[i * 2];
        result.col[i] = col[i * 2];
    }
    result.row[5] = '\0'; result.col[5] = '\0';
    return result;
}