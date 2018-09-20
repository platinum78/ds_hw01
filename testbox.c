#include <stdio.h>
#include <stdlib.h>
#include "./data_structure.h"
#include "./utils.h"

int main(void)
{
    FILE* input;
    input = fopen("./hw1_input.txt", "r");
    int i = 0, j = 0;

    // Read the dimension of matrix
    char cStrBuf[10];
    fscanf(input, "%s", cStrBuf);
    int nDim = (int)atoi(cStrBuf);

    int nDollarPos[3] = { 0, };
    nDollarPos[0] = FindDollar(input);

    int nBuf = ReadMatrix(input, nDim);
    printf("Nonzero elements in matrix A: %d \n", nBuf);

    nDollarPos[1] = FindDollar(input);

    nBuf = ReadMatrix(input, nDim);
    printf("Nonzero elements in matrix B: %d \n", nBuf);

    return 0;
}