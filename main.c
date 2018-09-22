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
    input = fopen("./hw1_input.txt", "r");
    FILE_INFO FileInfo;

    // Preprocess the file
    PreProcess(input, &FileInfo);

    // Create three instances of struct SquareMatrix
    SquareMatrix* matA = SquareMatrixInit();
    SquareMatrix* matB = SquareMatrixInit();
    SquareMatrix* matResult = SquareMatrixInit();

    // Parse inputs
    ParseInput(input, &FileInfo, matA, matB, matResult);
    // PrintMatrix(matA);
    // PrintMatrix(matB);

    // Parse the formula
    ParseFormula(input, &FileInfo, matA, matB, matResult);

    // Print the matrix
    printf("matResult: "); PrintMatrix(matResult);
    fclose(input);
    return 0;
}