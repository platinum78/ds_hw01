#include <stdio.h>
#include <stdlib.h>
#include "./data_structure.h"
#include "./utils.h"

#define TRUE 1
#define FALSE 0


int main(void)
{
    char cStrBuf[10];                   // Character buffer, which is to store each number string
    unsigned char nBufCursor = 0;       // Cursor in buffer, which is to operate the buffer string
    char cCharBuf = '\0';               // Character buffer
    int nDim;                           // Dimension of two matrices
    int nBuf;                           // Integer buffer
    int nDollarPos[3] = { 0, };         // Array to store location of each $'s


    // Open file and initiate FILE struct
    FILE* input;
    input = fopen("./input.txt", "r");

    // Create three instances of struct SquareMatrix
    SquareMatrix* matA = SquareMatrixInit();
    SquareMatrix* matB = SquareMatrixInit();
    SquareMatrix* matResult = SquareMatrixInit();

    // Read the dimension of matrix
    fscanf(input, "%s", cStrBuf);
    nDim = (int)atoi(cStrBuf);
    (matResult->size) = nDim;

    // Read while met first $
    nDollarPos[0] = FindDollar(input);

    // Read the first matrix, and return error signal if not read properly
    nBuf = ReadMatrix(input, matA);
    if (nBuf == -1)
        return -1;
    matA->size = nDim;
    matA->nonzero_cnt = nBuf;
    
    // Read while met second $
    nDollarPos[1] = FindDollar(input);

    // Read the second matrix, and return error signal if not read properly
    nBuf = ReadMatrix(input, matB);
    if (ReadMatrix(input, matB) == -1)
        return -1;
    matB->size = nDim;
    matB->nonzero_cnt = nBuf;

    // Read while met third $
    nDollarPos[2] = FindDollar(input);

    // Find the point where the formula starts
    if (FindFormula(input) == -1)
        return -1;

    // Read the formula and do whatever operation needed
    // 001 for add, 010 for mul, 100 for transpose
    SquareMatrix* pNextOperand = NULL;      // Next operand
    char cNextOperator = '\0';              // Next operator
    nBuf = 0;               // nBuf will be used for operation triggering here.
    
    // TODO: Allocate a zero matrix to matResult and add the first operand.

    while (1)
    {
        // Stop reading formula when reached EOF
        if (fread(&cCharBuf, 1, 1, input) == 0)
            break;

        // nBuf++ if found operand
        switch (cCharBuf)
        {
            case ' ': // Ignore white space
                continue;
            case 'A': // Set next operand to matA and add 1 to nBuf
                pNextOperand = matA;
                nBuf++;
            case 'B': // Set next operand to matB and add 1 to nBuf
                pNextOperand = matB;
                nBuf++;
            case '+': // Set next operator to + and add 1 to nBuf
                cNextOperator = cCharBuf;
                nBuf++;
            case '*': // Set next operator to * and add 1 to nBuf
                cNextOperator = cCharBuf;
                nBuf++;
            case '`': // Set next operator to ` and add 2 to nBuf
                cNextOperator = cCharBuf;
                nBuf += 2;
        }

        // Execute the corresponding operation when triggered.
        if (nBuf == 2)
        {
            switch (cNextOperator)
            {
                case '+':
                    MatAdd(matResult, pNextOperand);
                case '*':
                    MatMul(matResult, pNextOperand);
                case '`':
                    MatTranspose(matResult);
            }
        }
    }

    return 0;
}