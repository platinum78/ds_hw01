#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "./include/data_structure.h"
#include "./include/utils.h"

#define TRUE 1
#define FALSE 0


int main(void)
{
    
    clock_t tic = clock();
    char cStrBuf[10];                   // Character buffer, which is to store each number string
    unsigned char nBufCursor = 0;       // Cursor in buffer, which is to operate the buffer string
    char cCharBuf = '\0';               // Character buffer
    int nDim;                           // Dimension of two matrices
    int nBuf;                           // Integer buffer
    int nDollarPos[3] = { 0, };         // Array to store location of each $'s
    float fTotalTime = 0.0;

    // Open file and initiate FILE struct
    tic = clock();
    FILE* input;
    input = fopen("./io/hw1_input.txt", "r");
    FILE* output;
    output = fopen("./io/hw1_output.txt", "w");

    // Create three instances of struct SquareMatrix
    SquareMatrix* matA = SquareMatrixInit();
    SquareMatrix* matB = SquareMatrixInit();
    SquareMatrix* matResult = SquareMatrixInit();

    // Parse inputs
    ParseInput(input, matA, matB, matResult);

    // Parse the formula
    printf("Checkpoint before ParseFormula() passed! \n");
    ParseFormula(input, matA, matB, matResult);

    // Print the matrix

    WriteMatrix(output, matA);
    fprintf(output, "$ \n");
    WriteMatrix(output, matB);
    fprintf(output, "$ \n");
    WriteMatrix(output, matResult);
    fclose(input);
    clock_t toc = clock();
    printf("%lf miliseconds elapsed. \n", (double)(toc-tic)/CLOCKS_PER_SEC*1000);
    return 0;
}