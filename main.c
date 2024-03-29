#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "./include/data_structure.h"
#include "./include/utils.h"

#define TRUE 1
#define FALSE 0


int main(int argc, char** argv)
{   
    clock_t tic = clock();

    // Open file and initiate FILE struct
    tic = clock();
    FILE* input;
    FILE* output;
    if (argc == 1)
    {
        input = fopen("./io/hw1_input.txt", "r");
    }
    else if (argc == 2)
    {
        input = fopen(argv[1], "r");
        if (input == NULL)
        {
            printf("No such file exists. Try with no path option. \n");
            return -1;
        }
    }
    else
    {
        printf("Invalid arguments! \n");
        return -1;
    }
    output = fopen("./io/hw1_output.txt", "w");

    // Create three instances of struct SquareMatrix
    SquareMatrix* matA = SquareMatrixInit();
    SquareMatrix* matB = SquareMatrixInit();
    SquareMatrix* matResult = SquareMatrixInit();

    // Parse inputs
    ParseInput(input, matA, matB, matResult);

    // Parse the formula
    // printf("Checkpoint before ParseFormula() passed! \n");
    ParseFormula(input, matA, matB, matResult);

    // Print the matrices
    WriteMatrix(output, matA);
    fprintf(output, "$ \n");
    WriteMatrix(output, matB);
    fprintf(output, "$ \n");
    WriteMatrix(output, matResult);

    // Close file handler
    fclose(input);

    // Finish measuring time and print the result
    clock_t toc = clock();
    printf("%lf miliseconds elapsed. \n", (double)(toc-tic)/CLOCKS_PER_SEC*1000);

    return 0;
}