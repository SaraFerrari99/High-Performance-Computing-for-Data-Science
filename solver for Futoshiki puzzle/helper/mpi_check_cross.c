#include "mpi_check_cross.h"
#include "log.h"

void check_valid_number(char col[P], char row[P], PossibleNumber *number_possible){

    LOG_INFO(0,"Checking for valid numbers in the line");

    number_possible->count = 0;
    //For each number from 1 to 5, check if it is present in either the row
    for (int i = 1; i < 6; i++) {
        bool found = false;
        for (int j = 0; j < 5; j++) {
            if (col[j*2] == '0' + i || row[j*2] == '0' + i) {
                found = true;
                break;
            } 
        }

        if (!found) {
            number_possible->possible_number[number_possible->count] = '0' + i;
            number_possible->count++;
        }
    }
}

int is_digit(char c) {
    return (c >= '1' && c <= '5');
}

void extract_range_from_constraints(char line[P], int pos, int *minV, int *maxV)
{
    int dist;
    int i;

    //RIGHT
    dist = 0;
    i = pos + 1;
    while (i < P) {
        if (line[i] == '<') {
            dist++;
            i++;
        }
        else if (line[i] == '>') {
            dist++;
            i++;
        }
        else if (is_digit(line[i])) {
            if (line[i - dist * 2] == '<') {
                *maxV = (*maxV < (line[i] - '0' - dist)) ?
                        *maxV : (line[i] - '0' - dist);
            }
            else {
                *minV = (*minV > (line[i] - '0' + dist)) ?
                        *minV : (line[i] - '0' + dist);
            }
            break;
        }
        else break;
    }

    //LEFT
    dist = 0;
    i = pos - 1;
    while (i >= 0) {
        if (line[i] == '<') {
            dist++;
            i--;
        }
        else if (line[i] == '>') {
            dist++;
            i--;
        }
        else if (is_digit(line[i])) {
            if (line[i + dist * 2] == '<') {
                *minV = (*minV > (line[i] - '0' + dist)) ?
                        *minV : (line[i] - '0' + dist);
            }
            else {
                *maxV = (*maxV < (line[i] - '0' - dist)) ?
                        *maxV : (line[i] - '0' - dist);
            }
            break;
        }
        else break;
    }

    if (*minV < 1) *minV = 1;
    if (*maxV > 5) *maxV = 5;
}

//Check if a number respects the neighboring < and > constraints in the line
bool check_constraints(char line[P], int pos, int num) {
    
    //Check immediate left
    if (pos >= 2) {
        if (line[pos - 1] == '<') {
            if (is_digit(line[pos - 2]) && (line[pos - 2] - '0') >= num) return false;
        } else if (line[pos - 1] == '>') {
            if (is_digit(line[pos - 2]) && (line[pos - 2] - '0') <= num) return false;
        }
    }

    // Check immediate right
    if (pos <= P - 3) {
        if (line[pos + 1] == '<') {
            if (is_digit(line[pos + 2]) && (line[pos + 2] - '0') <= num) return false;
        } else if (line[pos + 1] == '>') {
            if (is_digit(line[pos + 2]) && (line[pos + 2] - '0') >= num) return false;
        }
    }

    return true; //if passed all checks, the number respects the constraints
}

bool check_full_line(char line[P]) {
    for(int i = 1; i < P-1; i += 2) {
        if(line[i] == '<') {
            if(is_digit(line[i-1]) && is_digit(line[i+1])) {
                if(line[i-1] >= line[i+1]) return false;
            }
        }
        else if(line[i] == '>') {
            if(is_digit(line[i-1]) && is_digit(line[i+1])) {
                if(line[i-1] <= line[i+1]) return false;
            }
        }
    }
    return true;
}

char take_final_value(char row[P], int pos, char col[P], int *rMin, int *rMax, int *count) {

    int used_row[6] = {0};
    int used_col[6] = {0};

    // Sign numbers already present in the row
    for(int i = 0; i < P; i += 2) {
        if(row[i] >= '1' && row[i] <= '5' && i != pos) {
            used_row[row[i]-'0'] = 1;
        }
    }

    // Sign numbers already present in the column
    for(int i = 0; i < P; i += 2) {
        if(col[i] >= '1' && col[i] <= '5') {
            used_col[col[i]-'0'] = 1;
        }
    }

    char final_value = 0;
    *count = 0;

    // Check all possible numbers between rMin and rMax
    for(int n = *rMin; n <= *rMax; n++) {
        row[pos] = '0' + n;
        if(!used_row[n] && !used_col[n] && check_constraints(row, pos, n) && check_full_line(row)) {
            final_value = '0' + n;
            (*count)++;
        }
    }

    row[pos] = '0'; // restore

    if(*count > 1) {
        final_value = '0'; // more than one valid number found
    }

    return final_value;
}


UpdateLine cross_rules(char col[P], char row[P], int rank, int number_of_column)
{
    UpdateLine result;
    PossibleNumber number;
    bool changed = 1;

    switch (rank){
        case 11:
            while(changed == 1){

                if(col[0] == '0'){
                    check_valid_number(col, row, &number);

                    if(number.count == 1){
                        row[number_of_column *2] = number.possible_number[0];
                        col[0] = number.possible_number[0];
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                        changed = 0;
                        break;
                    } 

                    int pos = number_of_column * 2;
                    int rMin = 1;
                    int rMax = 5;

                    //range obtain from < and > of the row
                    extract_range_from_constraints(row, pos, &rMin, &rMax);

                    int count = 0;
                    char final_value = take_final_value(row, pos, col, &rMin, &rMax, &count);

                    //  certain deduction
                    if(count == 1 && row[pos] == '0'){
                        row[pos] = final_value; // THE ONLY MODIFICATION TO THE ROW
                        col[0] = final_value;
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                    }

                    changed = 0; 
                    break;
                }
                changed = 0;
            };
            break;
    
        case 12:
            while(changed == 1){

                if(col[2] == '0'){
                    check_valid_number(col, row, &number);   

                    if(number.count == 1){
                        row[number_of_column*2] = number.possible_number[0];
                        col[2] = number.possible_number[0];
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                        changed = 0;
                        break;
                    }  
                    int pos = number_of_column * 2;
                    int rMin = 1;
                    int rMax = 5;

                    //range obtain from < and > of the row
                    extract_range_from_constraints(row, pos, &rMin, &rMax);

                    int count = 0;
                    char final_value = take_final_value(row, pos, col, &rMin, &rMax, &count);

                    //  certain deduction
                    if(count == 1 && row[pos] == '0'){

                        row[pos] = final_value;  //the only modification to the row
                        col[2] = final_value;
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                    }

                    changed = 0;   
                    break;            
                }   
                changed = 0;
            };  
            break;

        case 13:
            while(changed == 1){
                if(col[4] == '0'){
                    check_valid_number(col, row, &number);
                    
                    if(number.count == 1){
                        row[number_of_column*2] = number.possible_number[0];
                        col[4] = number.possible_number[0];
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                        changed = 0;                        
                        break;
                    } 

                    int pos = number_of_column * 2;
                    int rMin = 1;
                    int rMax = 5;

                    //range obtain from < and > of the row
                    extract_range_from_constraints(row, pos, &rMin, &rMax);

                    int count = 0;
                    char final_value = take_final_value(row, pos, col, &rMin, &rMax, &count);

                    //  certain deduction
                    if(count == 1 && row[pos] == '0'){
                        row[pos] = final_value;  //the only modification to the row
                        col[4] = final_value;
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                    }

                    changed = 0;
                    break;
                }
                changed = 0;
            };
            break;

        case 14:
            while(changed == 1){
                if(col[6] == '0'){
                    check_valid_number(col, row, &number);

                    if(number.count == 1){
                        row[number_of_column*2] = number.possible_number[0];
                        col[6] = number.possible_number[0];
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                        changed = 0;
                        break;
                    } 

                    
                    int pos = number_of_column * 2;
                    int rMin = 1;
                    int rMax = 5;

                    //range obtain from < and > of the row
                    extract_range_from_constraints(row, pos, &rMin, &rMax);

                    int count = 0;
                    char final_value = take_final_value(row, pos, col, &rMin, &rMax, &count);

                    //  certain deduction
                    if(count == 1 && row[pos] == '0'){
                        row[pos] = final_value;  //THE ONLY MODIFICATION TO THE ROW
                        col[6] = final_value;
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                    }

                    changed = 0;  
                    break;
                }
                changed = 0;

            };
            break;

        case 15:
            while(changed == 1){

                if(col[8] == '0'){
                    check_valid_number(col, row, &number);
                
                    if(number.count == 1){
                        row[number_of_column*2] = number.possible_number[0];
                        col[8] = number.possible_number[0];
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                        changed = 0;
                        break;
                    } 

                    int pos = number_of_column * 2;
                    int rMin = 1;
                    int rMax = 5;

                    //range obtain from < and > of the row
                    extract_range_from_constraints(row, pos, &rMin, &rMax);

                    int count = 0;
                    char final_value = take_final_value(row, pos, col, &rMin, &rMax, &count);

                    //  certain deduction
                    if(count == 1 && row[pos] == '0'){
                        row[pos] = final_value;  //THE ONLY MODIFICATION TO THE ROW
                        col[8] = final_value;
                        result.row[5] = '\0';
                        result.col[5] = '\0';
                    }

                    changed = 0;
                    break;
                }

                changed = 0;
            };
            break;
    
        default:
            break;
    }
    //IMPLEMENTING THE LOGIC AND UPDATING RESULT.ROW AND RESULT.COL

    for(int i = 0; i < 5; i++){
        result.row[i] = row[i*2];
        result.col[i] = col[i*2]; 

    }  
    result.row[5] = '\0';
    result.col[5] = '\0'; 

    return result;
}