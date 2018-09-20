#ifndef __UTILS_H__
#define __UTILS_H__


#include <stdio.h>
#include <stdlib.h>
#include "./data_structure.h"


int FindDollar(FILE* input)
{
    char cCharBuf;
    while (1)
    {
        fread(&cCharBuf, 1, 1, input);
        if (cCharBuf == '$')
            return ftell(input);
    }
}


int FindFormula(FILE* input)
{
    char cCharBuf;
    // The formula will start from either A or B at any case.
    // Therefore, find the point where A or B first appears.
    while (1)
    {
        if (fread(&cCharBuf, 1, 1, input) == 0)
            return -1;
        if (cCharBuf == 'A' || cCharBuf == 'B')
        {
            fseek(input, -1, SEEK_CUR);
            return 0;
        }
    }
}


#endif