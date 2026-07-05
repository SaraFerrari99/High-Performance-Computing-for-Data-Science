#include "mpi_check_major_minor_helper.h"
#include <unistd.h>
#include "log.h"


bool check_minor_major(char line_sign[10], char line[6])
{

    LOG_INFO(0,"Checking for minor/major constraints in the line");

    char save_array[11];

    memcpy(save_array, line_sign, 10);
    save_array[10] = '\0';

    bool modified = false;
    int len = strlen(save_array);

    for (int i = 0; i < len; i++)
    {
        if(i == 0 && i < len -2 ){
            if (save_array[i] == '2' && save_array[i + 1] == '>' && save_array[i + 2] == '0')
            {
                save_array[i + 2] = '1';
                modified = true;
            }
            else if (save_array[i] == '4' && save_array[i + 1] == '<' && save_array[i + 2] == '0')
            {
                save_array[i + 2] = '5';
                modified = true;
            }
        }else if (i !=0){
            if (save_array[i] == '2' && save_array[i - 1] == '<' && save_array[i - 2] == '0')
            {
                save_array[i - 2] = '1';
                modified = true;
            }
            else if (save_array[i] == '4' && save_array[i - 1] == '>' && save_array[i - 2] == '0')
            {
                save_array[i - 2] = '5';
                modified = true;
            }
        }
    }

    if(modified){
        int pos = 0;
        for (int i = 0; save_array[i] != '\0'; i++)
        {
            if (isdigit((unsigned char)save_array[i]))
            {
                line[pos++] = save_array[i];
                if (pos >= 5)
                    break; //Max 5 numbers
            }
        }
        line[pos] = '\0';

        strcpy(line_sign, save_array);
        for(int i = 0; i < 5; i++){
            line[i] = line_sign[i*2];
        }

    }
    return modified;
}