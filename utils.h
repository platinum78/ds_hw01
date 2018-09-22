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
            return ftell(input) - 1;
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


int PreProcess(FILE* input, FILE_INFO* FileInfo)
{
    // Buffers
    char cStrBuf[11];

    // Read the dimension info of the file
    fseek(input, 0, SEEK_SET);
    fscanf(input, "%s", cStrBuf);
    (FileInfo->dimension) = (int)atoi(cStrBuf);
    printf("Dimension: %d \n", (FileInfo->dimension));
    // Find the first dollar mark
    (FileInfo->dollar_pos)[0] = FindDollar(input);
    // Analyze the first matrix
    (FileInfo->matA_nonzeros) = AnalyzeMatrix(input, FileInfo->dimension, ftell(input));
    // Find the second dollar mark
    (FileInfo->dollar_pos)[1] = FindDollar(input);
    // Analyze the second matrix
    (FileInfo->matB_nonzeros) = AnalyzeMatrix(input, FileInfo->dimension, ftell(input));
    // Find the third dollar mark
    (FileInfo->dollar_pos)[2] = FindDollar(input);

    return 0;
}


int ParseInput(FILE* input, FILE_INFO* FileInfo, SquareMatrix* matA, SquareMatrix* matB, SquareMatrix* matResult)
{
    int nDim = (FileInfo->dimension);
    int nBuf;  // Integer buffer

    // Read the dimension of matrix
    fseek(input, 0, SEEK_SET);
    printf("Dimension: %d \n", nDim);
    (matResult->size) = nDim;

    // Initialize SquareMatrix instances
    (matA->size) = (matB->size) = nDim;
    (matA->nonzero_cnt) = (FileInfo->matA_nonzeros);
    (matB->nonzero_cnt) = (FileInfo->matB_nonzeros);

    // Create first matrix
    fseek(input, (FileInfo->dollar_pos)[0] + 1, SEEK_SET);  // Go to where first matrix starts
    printf("Nonzero_A: %d \n", (matA->nonzero_cnt));
    if ((matA->nonzero_cnt) <= nDim * nDim / 3)
        CreateSparseMatrix(input, matA);
    else
        CreateDenseMatrix(input, matA);

    // Create second matrix
    fseek(input, (FileInfo->dollar_pos)[1] + 1, SEEK_SET);  // Go to where second matrix starts
    printf("Nonzero_B: %d \n", (matB->nonzero_cnt));
    if ((matB->nonzero_cnt) <= nDim * nDim / 3)
        CreateSparseMatrix(input, matB);
    else
        CreateDenseMatrix(input, matB);
}


int ParseFormula(FILE* input, FILE_INFO* FileInfo, SquareMatrix* matA, SquareMatrix* matB, SquareMatrix* matResult)
{
    char cCharBuf;
    SquareMatrix* pNextOperand = NULL;      // Next operand
    char cNextOperator = '+';              // Next operator
    int nOpTrigger = 1;  // nBuf will be used for operation triggering here.
    
    fseek(input, (FileInfo->dollar_pos)[2] + 1, SEEK_SET);

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
            nOpTrigger = 0;
        }
    }
}


void PrintMatrix(SquareMatrix* mat)
{
    int idx = 0;
    if ((mat->sMatrix) != NULL)
    {
        printf("Sparse Matrix \n\n");

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
        printf("\n");
        printf("Total nonzero elements: %d \n", (mat->nonzero_cnt));
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


#endif