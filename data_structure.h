#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__


#include <stdio.h>
#include <stdlib.h>

#define FROM_FILE 1
#define FROM_ARRAY 2


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
    int size;  // Size of axis1
    int nonzero_cnt;
    int* dMatrix;  // Pointer to dense matrix; NULL if matrix is sparse
    SparseMatElem* sMatrix;  // Pointer to sparse matrix; NULL if matrix is dense
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


int ReadMatrix(FILE* input, int dim);
int CreateDenseMatix(FILE* input, SquareMatrix* mat);
int CreateSparseMatrix(FILE* input, SquareMatrix* mat);
void Sparse2Dense(SquareMatrix* mat);
int MatAdd(SquareMatrix* matResult, SquareMatrix* matNext);
int MatMul(SquareMatrix* mat1, SquareMatrix* mat2);
int MatTranspose(SquareMatrix* mat);
int IdxCmp(SparseMatElem* elem1, SparseMatElem* elem2);



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
    // This array will store direct inputs from the text file.
    SparseMatElem* pMatrix = (SparseMatElem*)malloc(sizeof(SparseMatElem)*nNonZeroCnt);
    int nMatrixIdx = 0;
    
    // Read matrix from file and store to pMatrix
    for (row = 0; row < nDim; row++)
    {
        for (col = 0; col < nDim; col++)
        {
            fscanf(input, "%s", cStrBuf);
            nBuf = (int)atoi(cStrBuf);
            // Store to pMatrix only if the value is nonzero
            if (nBuf != 0)
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
    // This array will store the transpose of pMatrix
    SparseMatElem* pMatrix_T = (SparseMatElem*)malloc(sizeof(SparseMatElem)*nNonZeroCnt);

    // Sweep pMatrix and parse values in pMatrix_T
    for (idx = 0; idx < nNonZeroCnt; idx++)
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


void Sparse2Dense(SquareMatrix* mat)
{
    // Verify if square matrix is sparse
    if (mat->sMatrix == NULL)
        return -1;

    // Set buffers for function operation
    int idx, nDim, nBuf;
    int row, col;
    SparseMatElem* pMatrix = mat->sMatrix;

    // Get size of matrix, allocate matrix, and initialize.
    nDim = mat->size;
    int* pMatrix_D = (int*)malloc(sizeof(int) * nDim * nDim);
    for (idx = 0; idx < nDim * nDim; idx++)
        pMatrix_D[idx] = 0;

    // Sweep pMatrix, and copy values to corresponding locations in pMatrix_D
    for (idx = 0; idx < (mat->nonzero_cnt); idx++)
    {
        nBuf = pMatrix[idx].row * nDim + pMatrix[idx].col;
        pMatrix_D[nBuf] = pMatrix[idx].val;
    }

    // Free the sparse matrix
    free(pMatrix);
    pMatrix = NULL;
    (mat->sMatrix) = NULL;

    // Change the SquareMatrix into dense one
    (mat->dMatrix) = pMatrix_D;
    pMatrix_D = NULL;
}


int MatAdd(SquareMatrix* matResult, SquareMatrix* matNext)
{
    // MatAdd should be responsible for all four cases: S+S, S+D, D+S, D+D
    int idx = 0;
    int nBuf;  // Integer buffer for general purpose
    if ((matResult->dMatrix) == NULL && (matResult->sMatrix) == NULL)
    {
        // Case when matResult is a zero matrix. Copy all values to matResult
        if ((matNext->sMatrix) != NULL)
        {
            nBuf = (matNext->nonzero_cnt);
            SparseMatElem* pMatrix = (SparseMatElem*)malloc(sizeof(SparseMatElem) * nBuf);
            for (idx = 0; idx < nBuf; idx++)
                pMatrix[idx] = (SparseMatElem){ .row = (matNext->sMatrix)[idx].row,
                                                .col = (matNext->sMatrix)[idx].col,
                                                .val = (matNext->sMatrix)[idx].val };
            (matResult->sMatrix) = pMatrix;
            pMatrix = NULL;
        }
        else if ((matNext->dMatrix) != NULL)
        {
            nBuf = (matNext->size);
            int* pMatrix = (int*)malloc(sizeof(int) * nBuf * nBuf);
            nBuf *= nBuf;
            for (idx = 0; idx < nBuf; idx++)
                pMatrix[idx] = (matNext->dMatrix)[idx];
            (matResult->dMatrix) = pMatrix;
            pMatrix = NULL;
        }
    }

    if ((matResult->sMatrix) != NULL && (matNext->sMatrix) != NULL)
    {
        int nIdxCmp = 0, idxResult = 0, idxNext = 0;  // *CR: column+row / idx*: index buffer
        int nTotalElemCnt = (matResult->nonzero_cnt) + (matNext->nonzero_cnt);
        int nRepElemCnt = 0;  // Variable to count the number of repeated elements

        // First loop iteration: count the overall elements considering repetition
        while (idxResult + idxNext == nTotalElemCnt)
        {
            // Compare two indices
            nIdxCmp = IdxCmp((matResult->sMatrix) + idxResult, (matNext->sMatrix) + idxNext);

            // Start Comparing
            if (nIdxCmp == 1)
            {
                // matResult precedes matNext
                idxResult++;
            }
            else if (nIdxCmp == -1)
            {
                // matNext precedes matResult
                idxNext++;
            }
            else if (nIdxCmp == 0)
            {
                // Found identical index. Add 1 to nRepElemCnt
                nRepElemCnt++;
                idxResult++;
                idxNext++;
            }
        }
        idxResult = idxNext = 0;
        // Calculate total element count removing repetitions,
        // and allocate memory for the new sparse matrix pMatrix
        nTotalElemCnt =- nRepElemCnt;
        SparseMatElem* pMatrix = (SparseMatElem*)malloc(sizeof(SparseMatElem) * (nTotalElemCnt));

        // Second loop iteration: actually copy values to pMatrix
        int idxNew = 0;
        while (idxResult + idxNext == nTotalElemCnt)
        {
            // Compare two indices
            nIdxCmp = IdxCmp((matResult->sMatrix) + idxResult, (matNext->sMatrix) + idxNext);

            // Start Comparing
            if (nIdxCmp == 1)
            {
                // matResult precedes matNext
                // Copy contents from matResult to pMatrix
                pMatrix[idxNew] = (SparseMatElem){ .col = (matResult->sMatrix)[idxResult].col, 
                                                   .row = (matResult->sMatrix)[idxResult].row, 
                                                   .val = (matResult->sMatrix)[idxResult].val };
                idxResult++;
            }
            else if (nIdxCmp == -1)
            {
                // matNext precedes matResult
                // Copy contents from matNext to pMatrix
                pMatrix[idxNew] = (SparseMatElem){ .col = (matNext->sMatrix)[idxNext].col, 
                                                   .row = (matNext->sMatrix)[idxNext].row, 
                                                   .val = (matNext->sMatrix)[idxNext].val };
                idxNext++;
            }
            else if (nIdxCmp == 0)
            {
                // Found identical index. Add 1 to nRepElemCnt
                // Copy added value of matResult and matNext to pMatrix
                pMatrix[idxNew] = (SparseMatElem){ .col = (matResult->sMatrix)[idxResult].col
                                                            + (matNext->sMatrix)[idxNext].col, 
                                                   .row = (matResult->sMatrix)[idxResult].row
                                                            + (matNext->sMatrix)[idxNext].row, 
                                                   .val = (matResult->sMatrix)[idxResult].val
                                                            + (matNext->sMatrix)[idxNext].val };
                idxResult++;
                idxNext++;
            }
        }

        // Now SparseMatElem array of matResult is obsolete. Free it and connect to pMatrix.
        free((matResult->sMatrix));
        (matResult->sMatrix) = pMatrix;
    }
    else if ((matResult->sMatrix) != NULL && (matNext->sMatrix) == NULL)
    {
        // matResult is sparse and matNext is dense
        // Convert matResult from sparse to dense matrix
        Sparse2Dense(matResult);
        nBuf = (matResult->size); // nBuf is used to store matrix size
        // Add all elements 
        for (idx = 0; idx < nBuf * nBuf; idx++)
            (matResult->dMatrix)[idx] += (matNext->dMatrix)[idx];
    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) != NULL)
    {
        // Dense + Sparse Case

        SparseMatElem* pMatElem;
        for (idx = 0; idx < (matNext->size); idx++)
        {
            pMatElem = (matNext->sMatrix);  // Get the pointer of SparseMatElem array
            nBuf = pMatElem[idx].row * (matNext->size) + pMatElem[idx].col;  // Calculate the index for 1D array
            (matResult->dMatrix)[nBuf] += pMatElem[idx].val;  // Add values to corresponding position
        }
    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) == NULL)
    {
        // Both matrices are dense
        nBuf = (matResult->size); // nBuf is used to store matrix size
        // Add all elements 
        for (idx = 0; idx < nBuf * nBuf; idx++)
            (matResult->dMatrix)[idx] += (matNext->dMatrix)[idx];
    }
    
    return 0;
}


// Matrix product of two given matrices
int MatMul(SquareMatrix* matResult, SquareMatrix* matNext)
{
    if ((matResult->sMatrix) != NULL && (matNext->sMatrix) != NULL)
    {
        
    }
    else if ((matResult->sMatrix) != NULL && (matNext->sMatrix) == NULL)
    {

    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) != NULL)
    {

    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) == NULL)
    {
        // Dense * Dense Case

        int row, col, idx;
        int nDim = (matResult->size);

        // Allocate new matrix and initialize to zero
        int* pMatrix = (int*)malloc(sizeof(int) * nDim * nDim);
        for (idx = 0; idx < nDim * nDim; idx++)
            pMatrix[idx] = 0;

        // Do matrix multiplication
        for (row = 0; row < nDim; row++)
            for (col = 0; col < nDim; col++)
                for (idx = 0; idx < nDim; idx++)
                    pMatrix[row*nDim + col] += (matResult->dMatrix)[row*nDim + idx]
                                                * (matNext->dMatrix)[idx*nDim + col];
        
        // Now matResult->dMatrix is obsolete. Free it.
        free(matResult->dMatrix);
        matResult->dMatrix = pMatrix;
        pMatrix = NULL;
    }
    return 0;
}


// Transpose given matrix
int MatTranspose(SquareMatrix* mat)
{
    // Set buffers for function operation
    int nBuf, idx;
    int nDim = mat->size;   
    SparseMatElem* pMatrix = mat->sMatrix;

    // Allocate two 1D arrays for counting the numbers and offsets
    int* npCntByCol = (int*)malloc(sizeof(int) * nDim);
    int* npOffsets = (int*)malloc(sizeof(int) * nDim);

    // Initialize two arrays to all 0.
    for (idx = 0; idx < nDim * nDim; idx++)
        npCntByCol[idx] = 0;
    for (idx = 0; idx < nDim * nDim; idx++)
        npOffsets[idx] = 0;

    // Sweep the matrix and count
    for (idx = 0; idx < (mat->nonzero_cnt); idx++)
        npCntByCol[pMatrix[idx].col]++;
    
    // Calculate offsets
    npOffsets[0] = 0;
    for (idx = 0; idx < nDim - 1; idx++)
        npOffsets[idx + 1] = npOffsets[idx] + npCntByCol[idx];

    // Allocate array with same dimension with pMatrix
    // pMatrix_T will store the transpose of pMatrix
    SparseMatElem* pMatrix_T = (SparseMatElem*)malloc(sizeof(SparseMatElem)*(mat->nonzero_cnt));

    // Sweep pMatrix and parse values to pMatrix_T
    for (idx = 0; idx < (mat->nonzero_cnt); idx++)
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


// Compare indices of two elements lexiographically
int IdxCmp(SparseMatElem* elem1, SparseMatElem* elem2)
{
    // Return 1 if index of elem 1 precedes,0 if identical,
    // -1 if index of elem 2 precedes

    if ((elem1->col) < (elem2->col))
        return 1;
    else if ((elem1->col) == (elem2->col))
        if ((elem1->row) < (elem2->row))
            return 1;
        else if ((elem1->row) == (elem2->row))
            return 0;
        else
            return -1;
    else
        return -1;
}


#endif