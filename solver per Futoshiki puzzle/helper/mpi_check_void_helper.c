#include "mpi_check_void_helper.h"

bool check_only_one_void(char line[6])
{
    int zero_index = -1;
    bool found[6] = {false}; // indice 1..5

    // Conta zeri e segna i numeri presenti
    for (int i = 0; i < 5; i++)
    {
        if (line[i] == '0')
        {
            if (zero_index != -1)
            {
                // più di uno zero, non fare nulla
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
        // nessuno zero → nulla da fare
        return false;
    }

    // Se c'è esattamente uno zero, trova il numero mancante
    for (int num = 1; num <= 5; num++)
    {
        if (!found[num])
        {
            line[zero_index] = '0' + num; // sostituisci lo 0
            printf("Sostituito 0 con %d nella posizione %d\n", num, zero_index);
            return true; // ho fatto una sostituzione
        }
    }

    return false;
}