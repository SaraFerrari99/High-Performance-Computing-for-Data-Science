#include "mpi_check_void_helper.h"
#include "log.h"

bool check_only_one_void(char line[6], char line_sign[10])
{

    LOG_INFO(0,"Checking for only one void in the line");

    int zero_index = -1;
    bool found[6] = {false}; // Index 1..5

    // Count sign and find the index of the only '0' in the line
    for (int i = 0; i < 5; i++)
    {
        if (line[i] == '0')
        {
            if (zero_index != -1)
            {
                // More than one '0' found, return false
                LOG_WARN(0,"More than one '0' found in the line, returning false");
                return false;
            }
            zero_index = i;
        }
        else
        {
            int num = line[i] - '0';
            if (num >= 1 && num <= 5)
                found[num] = true;
        }
    }

    if (zero_index == -1)
    {
        LOG_WARN(0,"No '0' found in the line, returning false");
        return false;
    }

    // IF there is only one '0', find the missing number and replace it
    for (int num = 1; num <= 5; num++)
    {
        if (!found[num])
        {
            line[zero_index] = '0' + num; // substitute the '0' with the missing number
            for (int i = 0; i < 5; i++)
            {
                line_sign[i * 2] = line[i];
            }

            return true; // substituted successfully
        }
    }

    return false;
}