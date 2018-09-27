#include <stdio.h>
#include <stdlib.h>
#include "./data_structure.h"
#include "./utils.h"


void FreeIt(SquareMatrix* mat)
{
    free(mat->sMatrix);
    free(mat->dMatrix);
}


int main(void)
{
    SquareMatrix* mat = SquareMatrixInit();
    int* pMatrix = (int*)malloc(sizeof(int) * 100);
    mat->size = 10;
    mat->dMatrix = pMatrix;
    Dense2Sparse(mat);
    FreeIt(mat);
    printf("Freed! \n");
    return 0;
}