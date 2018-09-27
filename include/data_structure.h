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


SquareMatrix* SquareMatrixInit(void);
int ReadMatrix(FILE* input, int dim);
int CreateDenseMatix(FILE* input, SquareMatrix* mat);
int CreateSparseMatrix(FILE* input, SquareMatrix* mat);
void Sparse2Dense(SquareMatrix* mat);
void Dense2Sparse(SquareMatrix* mat);
int MatAdd(SquareMatrix* matResult, SquareMatrix* matNext);
int MatMul(SquareMatrix* mat1, SquareMatrix* mat2);
int MatTranspose(SquareMatrix* mat);
int IdxCmp(SparseMatElem* elem1, SparseMatElem* elem2);
int CheckSpaceEfficiency(SquareMatrix* mat);


// Initializes SquareMatrix instance.
// All SquareMatrix instances should be declared this way.
SquareMatrix* SquareMatrixInit(void)
{
    SquareMatrix* matrix = (SquareMatrix*)malloc(sizeof(SquareMatrix));
    matrix->dMatrix = NULL;
    matrix->sMatrix = NULL;
    matrix->size = 0;
    matrix->nonzero_cnt = 0;

    // printf("Initialized SquareMatrix object at 0x%p \n", (void*)matrix);
    return matrix;
}


// Create dense matrix
int CreateDenseMatrix(FILE* input, SquareMatrix* mat)
{
    // Setup buffers for function operation
    int idx = 0;
    char cStrBuf[11];
    int nBuf;
    int nNonZeroCnt = 0, nZeroCnt = 0;

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
        if (nBuf != 0)
            nNonZeroCnt++;
        else
            nZeroCnt++;
    }
    
    // Check if any error
    if (nZeroCnt + nNonZeroCnt != nDim * nDim)
        return -1;

    // Set the matrix pointer of SquareMatrix instance
    (mat->dMatrix) = npMatrix;
    (mat->nonzero_cnt) = nNonZeroCnt;
    return 0;
}


// Create sparse matrix
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
    for (idx = 0; idx < nDim; idx++)
        npCntByCol[idx] = 0;
    for (idx = 0; idx < nDim; idx++)
        npOffsets[idx] = 0;

    // Sweep the matrix and count
    for (idx = 0; idx < (mat->nonzero_cnt); idx++)
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

    // printf("Sparse matrix created at %p \n", (void*)mat);
    return 0;
}


void Sparse2Dense(SquareMatrix* mat)
{
    printf("Sparse2Dense() called! \n");

    // Verify if square matrix is sparse
    if (mat->sMatrix == NULL)
        return;

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


void Dense2Sparse(SquareMatrix* mat)
{
    printf("Dense2Sparse() called! \n");

    // Verify if square matrix is actually dense
    if (mat->sMatrix != NULL)
        return;

    // Set buffers for function operation
    int idx, row, col;
    int nDim = mat->size;
    int nNonZeroCnt = 0;
    
    // Count number of nonzero elements in the dense matrix
    for (idx = 0; idx < nDim * nDim; idx++)
        if ((mat->dMatrix)[idx] != 0)
            nNonZeroCnt++;
    
    // Allocate memory for sparse matrix
    SparseMatElem* pMatrix = (SparseMatElem*)malloc(sizeof(SparseMatElem) * nNonZeroCnt);

    // Read the matrix in column-major order and copy values to sparse matrix
    idx = 0;
    for (col = 0; col < nDim; col++)
        for (row = 0; row < nDim; row++)
        {
            if ((mat->dMatrix)[row*nDim + col] != 0)
            {
                pMatrix[idx] = (SparseMatElem){ .col = col, .row = row,
                                                .val = (mat->dMatrix)[row*nDim + col] };
                ++idx;
            }
        }
    // Now dense matrix is obsolete. Free the array.
    free(mat->dMatrix);
    mat->dMatrix = NULL;
    
    // Connect sparse matrix to the SquareMatrix instance
    mat->sMatrix = pMatrix;
}


int MatAdd(SquareMatrix* matResult, SquareMatrix* matNext)
{
    // MatAdd should be responsible for all four cases: S+S, S+D, D+S, D+D
    printf("MatAdd() Called! \n");

    // printf("MatAdd called! \n");
    int idx = 0;
    int nBuf;  // Integer buffer for general purpose
    int nDim = matResult->size;
    int nNonZeroCnt = 0;
    
    // Handle the case that matResult is an empty matrix (most likely in the initial state)
    if ((matResult->dMatrix) == NULL && (matResult->sMatrix) == NULL)
    {
        // Case when matResult is a zero matrix. Copy all values to matResult

        if ((matNext->sMatrix) != NULL)
        {
            printf("None + Sparse \n");
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
            printf("None + Dense \n");
            int* pMatrix = (int*)malloc(sizeof(int) * nBuf);

            // Copy value while counting nonzero values
            nNonZeroCnt = 0;
            for (idx = 0; idx < nDim * nDim; idx++)
            {
                pMatrix[idx] = (matNext->dMatrix)[idx];
                nNonZeroCnt++;
            }
            (matResult->dMatrix) = pMatrix;
            pMatrix = NULL;
        }
        (matResult->nonzero_cnt) += (matNext->nonzero_cnt);
        printf("\n");
        return 0;
    }

    // Iterate both sparse matrices and merge them into one.
    if ((matResult->sMatrix) != NULL && (matNext->sMatrix) != NULL)
    {
        // Sparse + Sparse Case
        printf("Sparse + Sparse \n");

        int nIdxCmp = 0, idxResult = 0, idxNext = 0;  // *CR: column+row / idx*: index buffer
        int nTotalElemCnt = (matResult->nonzero_cnt) + (matNext->nonzero_cnt);
        
        // printf("Total elements before repetition elimination: %d \n", nTotalElemCnt);
        int nRepElemCnt = 0;  // Variable to count the number of repeated elements

        // First loop iteration: count the overall elements considering repetition
        while (idxResult + idxNext != nTotalElemCnt)
        {
            if (idxResult == (matResult->nonzero_cnt) && idxNext != (matNext->nonzero_cnt))
            {
                // Index of matResult reached the top
                idxNext++;
            }
            else if (idxResult != (matResult->nonzero_cnt) && idxNext == (matNext->nonzero_cnt))
            {
                // Index of matNext reached the top
                idxResult++;
            }
            else
            {
                // Compare two indices
                nIdxCmp = IdxCmp((matResult->sMatrix) + idxResult, (matNext->sMatrix) + idxNext);

                // Start Comparing
                if (nIdxCmp == 1)
                {
                    // matResult precedes matNext
                    // printf("matResult precedes \n");
                    idxResult++;
                }
                else if (nIdxCmp == -1)
                {
                    // matNext precedes matResult
                    // printf("matNext precedes \n");
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
        }
        idxResult = idxNext = 0;
        // Calculate total element count removing repetitions,
        // and allocate memory for the new sparse matrix pMatrix
        SparseMatElem* pMatrix = (SparseMatElem*)malloc(sizeof(SparseMatElem) * (nTotalElemCnt - nRepElemCnt));
        printf("After repetition elimination: %d \n", nTotalElemCnt - nRepElemCnt);
        (matResult->nonzero_cnt) = nTotalElemCnt - nRepElemCnt;

        // Second loop iteration: actually copy values to pMatrix
        int idxNew = 0;
        while (idxResult + idxNext != nTotalElemCnt)
        {
            // printf("nexIndex: %d \n", idxNew);
            if (idxResult == (matResult->nonzero_cnt) && idxNext != (matNext->nonzero_cnt))
            {
                // Index of matResult reached the top
                pMatrix[idxNew++] = (SparseMatElem){ .col = (matNext->sMatrix)[idxNext].col, 
                                                   .row = (matNext->sMatrix)[idxNext].row, 
                                                   .val = (matNext->sMatrix)[idxNext].val };
                idxNext++;
            }
            else if (idxResult != (matResult->nonzero_cnt) && idxNext == (matNext->nonzero_cnt))
            {
                // Index of matNext reached the top
                pMatrix[idxNew++] = (SparseMatElem){ .col = (matResult->sMatrix)[idxResult].col, 
                                                   .row = (matResult->sMatrix)[idxResult].row, 
                                                   .val = (matResult->sMatrix)[idxResult].val };
                idxResult++;
            }
            else
            {
                // Compare two indices
                nIdxCmp = IdxCmp((matResult->sMatrix) + idxResult, (matNext->sMatrix) + idxNext);

                // Start Comparing
                if (nIdxCmp == 1)
                {
                    // matResult precedes matNext
                    pMatrix[idxNew++] = (SparseMatElem){ .col = (matResult->sMatrix)[idxResult].col, 
                                                        .row = (matResult->sMatrix)[idxResult].row, 
                                                        .val = (matResult->sMatrix)[idxResult].val };
                    idxResult++;
                }
                else if (nIdxCmp == -1)
                {
                    // matNext precedes matResult
                    pMatrix[idxNew++] = (SparseMatElem){ .col = (matNext->sMatrix)[idxNext].col, 
                                                        .row = (matNext->sMatrix)[idxNext].row, 
                                                        .val = (matNext->sMatrix)[idxNext].val };
                    idxNext++;
                }
                else if (nIdxCmp == 0)
                {
                    // Found identical index. Add 1 to nRepElemCnt
                    pMatrix[idxNew++] = (SparseMatElem){ .col = (matResult->sMatrix)[idxResult].col,
                                                        .row = (matResult->sMatrix)[idxResult].row,
                                                        .val = (matResult->sMatrix)[idxResult].val
                                                                    + (matNext->sMatrix)[idxNext].val };
                    idxResult++;
                    idxNext++;
                }
            }
        }

        // Now SparseMatElem array of matResult is obsolete. Free it and connect to pMatrix.
        free(matResult->sMatrix);
        (matResult->sMatrix) = pMatrix;
        (matResult->nonzero_cnt) = nTotalElemCnt - nRepElemCnt;
        if ((matResult->nonzero_cnt) > (matResult->size) * (matResult->size) / 3)
            Sparse2Dense(matResult);
    }
    else if ((matResult->sMatrix) != NULL && (matNext->dMatrix) != NULL)
    {
        // Sparse + Dense Case
        printf("Sparse + Dense \n");

        // Convert matResult from sparse to dense matrix
        Sparse2Dense(matResult);
        nBuf = (matResult->size); // nBuf is used to store matrix size

        // Add all elements 
        for (idx = 0; idx < nBuf * nBuf; idx++)
            (matResult->dMatrix)[idx] += (matNext->dMatrix)[idx];
        
        // Count number of nonzero elements
        nNonZeroCnt = 0;
        for (idx = 0; idx < nDim * nDim; idx++)
            if ((matResult->dMatrix)[idx] != 0)
                nNonZeroCnt++;
        matResult->nonzero_cnt = nNonZeroCnt;

        // Check space efficiency
        CheckSpaceEfficiency(matResult);
    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) != NULL)
    {
        // Dense + Sparse Case
        printf("Dense + Sparse \n");
        
        SparseMatElem* pMatElem;
        for (idx = 0; idx < (matNext->size); idx++)
        {
            pMatElem = (matNext->sMatrix);  // Get the pointer of SparseMatElem array
            nBuf = pMatElem[idx].row * (matNext->size) + pMatElem[idx].col;  // Calculate the index for 1D array
            (matResult->dMatrix)[nBuf] += pMatElem[idx].val;  // Add values to corresponding position
        }

        // Count number of nonzero elements
        nNonZeroCnt = 0;
        for (idx = 0; idx < nDim * nDim; idx++)
            if ((matResult->dMatrix)[idx] != 0)
                nNonZeroCnt++;
        matResult->nonzero_cnt = nNonZeroCnt;

        // Check space efficiency
        CheckSpaceEfficiency(matResult);
    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) == NULL)
    {
        // Both matrices are dense
        printf("Dense + Dense \n");

        nBuf = (matResult->size); // nBuf is used to store matrix size
        // Add all elements 
        for (idx = 0; idx < nBuf * nBuf; idx++)
            (matResult->dMatrix)[idx] += (matNext->dMatrix)[idx];
        
        // Count number of nonzero elements
        nNonZeroCnt = 0;
        for (idx = 0; idx < nDim * nDim; idx++)
            if ((matResult->dMatrix)[idx] != 0)
                nNonZeroCnt++;
        matResult->nonzero_cnt = nNonZeroCnt;

        // Check space efficiency
        CheckSpaceEfficiency(matResult);
    }
    printf("\n");
    return 0;
}


// Matrix product of two given matrices
int MatMul(SquareMatrix* matResult, SquareMatrix* matNext)
{
    printf("MatMul() called! \n");

    // Buffers for function operation
    int idx, idx1, idx2, row, col;
    int nNonZeroCnt = 0;

    // Create and calibrate a dense matrix first. 
    // This might be less space-efficient, but enhance mental health LMAO :)
    int nDim = (matResult->size);
    int* pMatrix = (int*)malloc(sizeof(int) * nDim * nDim);
    for (idx = 0; idx < nDim * nDim; idx++)
        pMatrix[idx] = 0;
    
    if ((matResult->sMatrix) != NULL && (matNext->sMatrix) != NULL)
    {
        // Sparse * Sparse
        printf("Sparse * Sparse \n");

        for (idx1 = 0; idx1 < (matResult->nonzero_cnt); idx1++)
        {
            for (idx2 = 0; idx2 < (matNext->nonzero_cnt); idx2++)
            {
                if ((matResult->sMatrix)[idx1].col == (matNext->sMatrix)[idx2].row)
                {
                    // In matrix multiplication A * B,
                    // Elements from A and B should have the same index for column and row each.
                    idx = ((matResult->sMatrix)[idx1].row) * nDim + (matNext->sMatrix)[idx2].col; 
                    pMatrix[idx] += (matResult->sMatrix)[idx1].val * (matNext->sMatrix)[idx2].val;
                }
            }
        }
        
        // Now the sparse matrix is obsolete.
        free(matResult->sMatrix);
        matResult->sMatrix = NULL;
        matResult->dMatrix = pMatrix;
        
        // Count number of nonzero elements
        nNonZeroCnt = 0;
        for (idx = 0; idx < nDim * nDim; idx++)
            if (pMatrix[idx] != 0)
                nNonZeroCnt++;
        matResult->nonzero_cnt = nNonZeroCnt;

        // Check space efficiency
        CheckSpaceEfficiency(matResult);
    }
    else if ((matResult->sMatrix) != NULL && (matNext->sMatrix) == NULL)
    {
        // Sparse * Dense
        printf("Sparse * Dense");

        // Set buffers for function operation
        int idx, row, col, colIdx;
        int nDim = matResult->size;
        
        for (idx = 0; idx < (matResult->nonzero_cnt); idx++)
        {
            for (colIdx = 0; colIdx < nDim; colIdx++)
            {
                row = (matResult->sMatrix)[idx].row;
                col = (matResult->sMatrix)[idx].col;
                pMatrix[row * nDim + colIdx] += (matResult->sMatrix)[idx].val
                                                * (matNext->dMatrix)[col * nDim + colIdx];
            }
        }
        
        // Now the sparse matrix is obsolete.
        free(matResult->sMatrix);
        matResult->sMatrix = NULL;
        matResult->dMatrix = pMatrix;
        
        // Count number of nonzero elements
        nNonZeroCnt = 0;
        for (idx = 0; idx < nDim * nDim; idx++)
            if (pMatrix[idx] != 0)
                nNonZeroCnt++;
        matResult->nonzero_cnt = nNonZeroCnt;

        // Check space efficiency
        CheckSpaceEfficiency(matResult);
    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) != NULL)
    {
        // Dense * Sparse Case
        printf("Dense * Sparse");

        int idx, row, col, rowIdx;
        int nDim = matResult->size;

        for (idx = 0; idx < nDim; idx++)
        {
            for (rowIdx = 0; rowIdx < nDim; rowIdx++)
            {
                row = (matNext->sMatrix)[idx].row;
                col = (matNext->sMatrix)[idx].col;
                pMatrix[row * nDim + rowIdx] += (matResult->sMatrix)[idx].val
                                                * (matNext->dMatrix)[col * nDim + rowIdx];
            }
        }

        // Now the dense matrix is obsolete.
        free(matResult->dMatrix);
        matResult->dMatrix = pMatrix;
        
        // Count number of nonzero elements
        nNonZeroCnt = 0;
        for (idx = 0; idx < nDim * nDim; idx++)
            if (pMatrix[idx] != 0)
                nNonZeroCnt++;
        matResult->nonzero_cnt = nNonZeroCnt;

        // Check space efficiency
        CheckSpaceEfficiency(matResult);
    }
    else if ((matResult->sMatrix) == NULL && (matNext->sMatrix) == NULL)
    {
        // Dense * Dense Case
        printf("Dense * Dense \n");

        int* pMatResult = matResult->dMatrix;
        int* pMatNext = matNext->dMatrix;
        // Do matrix multiplication
        for (row = 0; row < nDim; row++)
            for (col = 0; col < nDim; col++)
                for (idx = 0; idx < nDim; idx++)
                    pMatrix[row*nDim + col] += pMatResult[row*nDim + idx]
                                                * pMatNext[idx*nDim + col];
        printf("Dense * Dense finished \n");

        // Now the dense matrix is obsolete.
        free(matResult->dMatrix);
        matResult->dMatrix = pMatrix;
        
        // Count number of nonzero elements
        nNonZeroCnt = 0;
        for (idx = 0; idx < nDim * nDim; idx++)
            if (pMatrix[idx] != 0)
                nNonZeroCnt++;
        matResult->nonzero_cnt = nNonZeroCnt;

        // Check space efficiency
        CheckSpaceEfficiency(matResult);
    }
    printf("\n");
    return 0;
}


// Transpose given matrix
int MatTranspose(SquareMatrix* mat)
{
    printf("MatTranspose called! \n");

    if ((mat->sMatrix) == NULL)
    {
        printf("Dense \n");
        int row, col, buf;
        int nDim = (mat->size);
        for (row = 0; row < nDim - 1; row++)
        {
            for (col = row + 1; col < nDim; col++)
            {
                buf = (mat->dMatrix)[row * nDim + col];
                (mat->dMatrix)[row * nDim + col] = (mat->dMatrix)[col * nDim + row];
                (mat->dMatrix)[col * nDim + row] = buf;
            }
        }
    }
    else
    {
        printf("Sparse \n");
        // Set buffers for function operation
        int nBuf, idx;
        int nDim = mat->size;   
        SparseMatElem* pMatrix = mat->sMatrix;
        for (idx = 0; idx < nDim; idx++)
        {
            nBuf = pMatrix[idx].row;
            pMatrix[idx].row = pMatrix[idx].col;
            pMatrix[idx].col = nBuf;
        }

        // Allocate two 1D arrays for counting the numbers and offsets
        int* npCntByCol = (int*)malloc(sizeof(int) * nDim);
        int* npOffsets = (int*)malloc(sizeof(int) * nDim);

        // Initialize two arrays to all 0.
        for (idx = 0; idx < nDim; idx++)
            npCntByCol[idx] = 0;
        for (idx = 0; idx < nDim; idx++)
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
    }
    printf("\n");
    return 0;
}


// Compare indices of two elements lexiographically
int IdxCmp(SparseMatElem* elem1, SparseMatElem* elem2)
{
    // printf("IdxCmp called! \n");
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


int CheckSpaceEfficiency(SquareMatrix* mat)
{
    // Get information of the matrix
    int nDim = (mat->size);
    int nNonZeroCnt = (mat->nonzero_cnt);

    // Judge the proper form of the matrix
    if (nNonZeroCnt <= nDim * nDim / 3)
        Dense2Sparse(mat);
    else
        Sparse2Dense(mat);
}


#endif