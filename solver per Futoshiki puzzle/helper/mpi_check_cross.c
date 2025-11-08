#include "mpi_check_cross.h"

UpdateLine cross_rules(char col[P], char row[P], int rank, int number_of_column)
{
    UpdateLine result;

    char possible_number[5];

    switch (rank){
        case 11:
            if(col[0] == '0'){
                //char check_valid_number(col, row,possible_number);
   
                printf("%s\n", col);
                printf("%s\n", row);
                printf("%d\n", rank);
                printf("%d\n", number_of_column);           
                
            }
            break;
    
        case 12:
            if(col[2] == '0'){
                //char check_valid_number(col, row,possible_number);
   
                printf("%s\n", col);
                printf("%s\n", row);
                printf("%d\n", rank);
                printf("%d\n", number_of_column);     
            }      
                break;

        case 13:
            if(col[4] == '0'){
                //char check_valid_number(col, row,possible_number);
            
                printf("%s\n", col);
                printf("%s\n", row);
                printf("%d\n", rank);
                printf("%d\n", number_of_column);            
                }
            break;

        case 14:
            if(col[6] == '0'){
                //char check_valid_number(col, row,possible_number);
   
                printf("%s\n", col);
                printf("%s\n", row);
                printf("%d\n", rank);
                printf("%d\n", number_of_column);            
                }
            break;

        case 15:
            if(col[8] == '0'){
                //char check_valid_number(col, row,possible_number);
   
                printf("%s\n", col);
                printf("%s\n", row);
                printf("%d\n", rank);
                printf("%d\n", number_of_column);            
                }
            break;
    
        default:
            break;
    }
    //IMPLEMENTARE LA LOGICA E AGGIORNARE RESULT.ROW E RESULT.COL
    //SOLO PER TEST
    //char row_test[9] = {'0','<','0','<','0','<','0','<','0'};
    //char col_test[9] = {'0','<','0','<','0','<','0','<','0'};

    for(int i = 0; i < P/2; i++){
        result.row[i] = row[i*2];
        result.col[i] = col[i*2]; 
    }

    
                                                                                                            
    return result;
}