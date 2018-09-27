#ifndef __UTILS_H__
#define __UTILS_H__


#include <stdio.h>
#include <stdlib.h>
#include "./data_structure.h"


void PrintMatrix(SquareMatrix* mat);

// Struct that describes the file
typedef struct FILE_INFO
{
    int dollar_pos[3];
    int dimension;
    int matA_nonzeros;
    int matB_nonzeros;
} FILE_INFO;


void FindDollar(FILE* input)
{
    char cCharBuf;
    while (1)
    {
        fread(&cCharBuf, 1, 1, input);
        if (cCharBuf == '$')
            return;
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

int ParseInput(FILE* input, SquareMatrix* matA, SquareMatrix* matB, SquareMatrix* matResult)
{
    // Buffers
    char cStrBuf[11];
    int nBuf;  // Integer buffer

    // Read the dimension info of the file
    fseek(input, 0, SEEK_SET);
    fscanf(input, "%s", cStrBuf);
    (matA->size) = (matB->size) = (matResult->size) = (int)atoi(cStrBuf);

    // Find the first dollar mark
    FindDollar(input);
    fseek(input, ftell(input)+1, SEEK_SET);

    // Create first matrix
    CreateDenseMatrix(input, matA);
    CheckSpaceEfficiency(matA);

    // Find the second dollar mark
    FindDollar(input);
    fseek(input, ftell(input)+1, SEEK_SET);

    // Create second matrix
    CreateDenseMatrix(input, matB);
    CheckSpaceEfficiency(matB);

    // Find the third dollar mark
    FindDollar(input);
    fseek(input, ftell(input)+1, SEEK_SET);
}


int ParseFormula(FILE* input, SquareMatrix* matA, SquareMatrix* matB, SquareMatrix* matResult)
{
    char cCharBuf;
    SquareMatrix* pNextOperand = NULL;      // Next operand
    char cNextOperator = '+';              // Next operator
    int nOpTrigger = 1;  // nBuf will be used for operation triggering here.
    
    // fseek(input, 1, SEEK_CUR);

    while (1)
    {
        // Stop reading formula when reached EOF
        if (fread(&cCharBuf, 1, 1, input) == 0)
            break;

        switch (cCharBuf)
        {
            case 'A': // Set next operand to matA and add 1 to nOpTrigger
                pNextOperand = matA;
                nOpTrigger++;
                break;
            case 'B': // Set next operand to matB and add 1 to nOpTrigger
                pNextOperand = matB;
                nOpTrigger++;
                break;
            case '+': // Set next operator to + and add 1 to nOpTrigger
                cNextOperator = cCharBuf;
                nOpTrigger++;
                break;
            case '*': // Set next operator to * and add 1 to nOpTrigger
                cNextOperator = cCharBuf;
                nOpTrigger++;
                break;
            case '`': // Set next operator to ` and add 2 to nOpTrigger
                cNextOperator = cCharBuf;
                nOpTrigger += 2;
                break;
            default:
                continue;
        }
        // printf("Read %c, current trigger %d \n", cCharBuf, nOpTrigger);

        // Execute the corresponding operation when triggered.
        if (nOpTrigger == 2)
        {
            switch (cNextOperator)
            {
                case '+':
                    // printf("Addition \n");
                    MatAdd(matResult, pNextOperand);
                    break;
                case '*':
                    // printf("Multiplication \n");
                    MatMul(matResult, pNextOperand);
                    break;
                case '`':
                    // printf("Transpose \n");
                    MatTranspose(matResult);
                    break;
            }
            // PrintMatrix(matResult);
            nOpTrigger = 0;
        }
    }
}


void PrintMatrix(SquareMatrix* mat)
{
    int idx = 0;
    if ((mat->sMatrix) != NULL)
    {
        printf("Sparse Matrix \n");

        // Print columns
        printf("%10s", "Column");
        for (idx = 0; idx < (mat->nonzero_cnt); idx++)       
            printf("%5d", (mat->sMatrix)[idx].col);
        printf("\n");

        // Print rows
        printf("%10s", "Row");
        for (idx = 0; idx < (mat->nonzero_cnt); idx++)       
            printf("%5d", (mat->sMatrix)[idx].row);
        printf("\n");

        // Print values
        printf("%10s", "Value");
        for (idx = 0; idx < (mat->nonzero_cnt); idx++)       
            printf("%5d", (mat->sMatrix)[idx].val);
        printf("\n\n");
        // printf("Total nonzero elements: %d \n", (mat->nonzero_cnt));
    }
    else
    {
        printf("Dense Matrix \n\n");
        int i, j;
        for (i = 0; i < (mat->size); i++)
        {
            for (j = 0; j < (mat->size); j++)
                printf("%5d", (mat->dMatrix)[i * (mat->size) + j]);
            printf("\n");
        }
        printf("\n");
    }
        
}


void WriteMatrix(FILE* output, SquareMatrix* mat)
{
    int idx = 0;
    if ((mat->sMatrix) != NULL)
    {
        // Sparse matrix
        
        fprintf(output, "S \n");

        // Print columns
        for (idx = 0; idx < (mat->nonzero_cnt); idx++)       
            fprintf(output, "%-5d", (mat->sMatrix)[idx].col);
        fprintf(output, "\n");

        // Print rows
        for (idx = 0; idx < (mat->nonzero_cnt); idx++)       
            fprintf(output, "%-5d", (mat->sMatrix)[idx].row);
        fprintf(output, "\n");

        // Print values
        for (idx = 0; idx < (mat->nonzero_cnt); idx++)       
            fprintf(output, "%-5d", (mat->sMatrix)[idx].val);
        fprintf(output, "\n");
        // printf("Total nonzero elements: %d \n", (mat->nonzero_cnt));
    }
    else
    {
        fprintf(output, "D \n");
        int i, j;
        for (i = 0; i < (mat->size); i++)
        {
            for (j = 0; j < (mat->size); j++)
                fprintf(output, "%-5d", (mat->dMatrix)[i * (mat->size) + j]);
            fprintf(output, "\n");
        }
    }
        
}


#endif