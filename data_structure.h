#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__


#include <stdio.h>
#include <stdlib.h>


// struct for sparse matrix elements
typedef struct SparseMatElem
{
    int row;
    int col;
    int val;
} SparseMatElem;


// struct for square matrices
typedef struct SquareMatrix
{
    int size;
    int nonzero_cnt;
    int* dMatrix;
    SparseMatElem* sMatrix;
} SquareMatrix;


// Initializes SquareMatrix instance.
// All SquareMatrix instances should be declared this way.
SquareMatrix* SquareMatrixInit(void)
{
    SquareMatrix* matrix = (SquareMatrix*)malloc(sizeof(SquareMatrix));
    matrix->dMatrix = NULL;
    matrix->sMatrix = NULL;
    matrix->size = 0;
    matrix->nonzero_cnt = 0;
    return matrix;
}


// Read the matrix and return the number of nonzero elements.
// Return -1 if read operation was not successful.
int ReadMatrix(FILE* input, int dim)
{
    int nZeroCnt = 0;
    int nNonZeroCnt = 0;
    int idx = 0;
    char cStrBuf[11];
    int nBuf;

    // Count the number of zero and nonzero elements
    for (idx = 0; idx < dim*dim; idx++)
    {
        fscanf(input, "%s", cStrBuf);
        nBuf = (int)atoi(cStrBuf);

        if (nBuf == 0)
            nZeroCnt++;
        else
            nNonZeroCnt++;
    }

    // Return error sign if not counted properly
    if (nNonZeroCnt + nZeroCnt != dim * dim)
        return -1;
    
    return nNonZeroCnt;
}


int CreateDenseMatix(FILE* input, SquareMatrix* mat)
{
    // Setup buffers for function operation
    int idx = 0;
    char cStrBuf[11];
    int nBuf;

    // Check the size of given matrix
    int nDim = mat->size;
    
    // Allocate 1D array as the matrix data
    int* npMatrix =  (int*)malloc(sizeof(int) * nDim * nDim);

    // Read data from file and store to matrix
    for (idx = 0; idx < nDim*nDim; idx++)
    {
        fscanf(input, "%s", cStrBuf);
        nBuf = (int)atoi(cStrBuf);
        npMatrix[idx] = nBuf;
    }

    // Set the matrix pointer of SquareMatrix instance
    mat->dMatrix = npMatrix;

    return 0;
}


int CreateSparseMatrix(FILE* input, SquareMatrix* mat)
{
    // Setup buffers for function operation
    int row, col = 0;
    char cStrBuf[11];
    int nBuf, idx;

    // Check the size and # of nonzero elements
    int nDim = mat->size;
    int nNonZeroCnt = mat->nonzero_cnt;

    // Allocate array of SparseMatElem's
    SparseMatElem* pMatrix = (SparseMatElem*)malloc(sizeof(SparseMatElem)*nNonZeroCnt);
    int nMatrixIdx = 0;
    
    // Read matrix from file and store to RAM
    for (row = 0; row < nDim; row++)
    {
        for (col = 0; col < nDim; col++)
        {
            fscanf(input, "%s", cStrBuf);
            nBuf = (int)atoi(cStrBuf);
            pMatrix[nMatrixIdx++] = (SparseMatElem){ .row = row, .col = col, .val = nBuf };
        }
    }

    // The sparse matrix is currently in row-major state.
    // To make column-major'ed, the matrix should be transposed.
    
    // Allocate two 1D arrays for counting the numbers and offsets
    int* npCntByCol = (int*)malloc(sizeof(int) * nDim);
    int* npOffsets = (int*)malloc(sizeof(int) * nDim);

    // Initialize two arrays to all 0.
    for (idx = 0; idx < nDim * nDim; idx++)
        npCntByCol[idx] = 0;
    for (idx = 0; idx < nDim * nDim; idx++)
        npOffsets[idx] = 0;

    // Sweep the matrix and count
    for (idx = 0; idx < nDim * nDim; idx++)
        npCntByCol[pMatrix[idx].col]++;
    
    // Calculate offsets
    npOffsets[0] = 0;
    for (idx = 0; idx < nDim - 1; idx++)
        npOffsets[idx + 1] = npOffsets[idx] + npCntByCol[idx];

    // Allocate array with same dimension with pMatrix
    SparseMatElem* pMatrix_T = (SparseMatElem*)malloc(sizeof(SparseMatElem)*nNonZeroCnt);

    // Sweep the original matrix and parse values in column-major'ed order
    for (idx = 0; idx < nDim * nDim; idx++)
        pMatrix_T[npOffsets[pMatrix[idx].col]++] = (SparseMatElem){ .row = pMatrix[idx].row,
                                                                    .col = pMatrix[idx].col,
                                                                    .val = pMatrix[idx].val };
    
    // Free the counter arrays
    free(pMatrix);
    pMatrix = NULL;
    free(npCntByCol);
    npCntByCol = NULL;
    free(npOffsets);
    npOffsets = NULL;

    // Parse the pointer to tansposed matrix
    mat->sMatrix = pMatrix_T;
}


int MatAdd(SquareMatrix* matResult, SquareMatrix* matNext)
{
    int idx = 0;
    if (matResult->dMatrix != NULL && matNext->dMatrix != NULL)
    {
        for (idx = 0; idx < (matResult->size) * (matResult->size); idx++)
        {
            (matResult->dMatrix)[idx] += (matNext->dMatrix)[idx];
        }
    }
    
    return 0;
}


void Sparse2Dense(SquareMatrix* mat)
{
    // Verify if square matrix is sparse
    if (mat->dMatrix == NULL)
        return -1;

    // Set buffers for function operation
    int idx, nDim, nBuf;
    int row, col;

    // Get size of matrix, allocate matrix, and initialize
    nDim = mat->size;
    int* pMatrix = (int*)malloc(sizeof(int) * nDim * nDim);
    for (idx = 0; idx < nDim * nDim; idx++)
        pMatrix[idx] = 0;

    for (row = 0; row < nDim; row++)
    {
        for (col = 0; col < nDim; col++)
        {
            pMatrix[row*nDim+col] = 
        }
    }
}


int MatMul(SquareMatrix* mat1, SquareMatrix* mat2)
{
    // Define function for matrix multiplication

    return 0;
}


int MatTranspose(SquareMatrix* mat)
{
    // Set buffers for function operation
    int nBuf, idx;

    // Allocate two 1D arrays for counting the numbers and offsets
    int* npCntByCol = (int*)malloc(sizeof(int) * nDim);
    int* npOffsets = (int*)malloc(sizeof(int) * nDim);

    // Initialize two arrays to all 0.
    for (idx = 0; idx < nDim * nDim; idx++)
        npCntByCol[idx] = 0;
    for (idx = 0; idx < nDim * nDim; idx++)
        npOffsets[idx] = 0;

    // Sweep the matrix and count
    for (idx = 0; idx < nDim * nDim; idx++)
        npCntByCol[pMatrix[idx].col]++;
    
    // Calculate offsets
    npOffsets[0] = 0;
    for (idx = 0; idx < nDim - 1; idx++)
        npOffsets[idx + 1] = npOffsets[idx] + npCntByCol[idx];

    // Allocate array with same dimension with pMatrix
    SparseMatElem* pMatrix_T = (SparseMatElem*)malloc(sizeof(SparseMatElem)*nNonZeroCnt);

    // Sweep the original matrix and parse values in column-major'ed order
    for (idx = 0; idx < nDim * nDim; idx++)
        pMatrix_T[npOffsets[pMatrix[idx].col]++] = (SparseMatElem){ .row = pMatrix[idx].row,
                                                                    .col = pMatrix[idx].col,
                                                                    .val = pMatrix[idx].val };
    
    // Free the counter arrays
    free(pMatrix);
    pMatrix = NULL;
    free(npCntByCol);
    npCntByCol = NULL;
    free(npOffsets);
    npOffsets = NULL;

    // Parse the pointer to tansposed matrix
    mat->sMatrix = pMatrix_T;
    return 0;
}


#endif