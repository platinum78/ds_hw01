# Programming Assignment #1 - Matrix Operations

Data Structure SWE2015-41, School of Software, SKKU <br>
Susung Park (2014311254), School of Mechanical Engineering, SKKU


## I. Objectives and Considerations

In this assignment, basic arithmetic and transposition operations of matrices are evaluated.
While writing the codes, the followings are considered, with the priority follwing the numbers.

    1. Reduction of time complexity
    2. Reduction of space complexity
    3. Capsulization & interoperability between different types of matrices
    4. Modulization through utilization of functions

It is clear that sparse matrices and dense matrices have different representation of their data.
Therefore, it is also clear that a function that adds sparse and dense matrices can work for adding dense and dense matrices,
since the method should be different.
Therefore, operation between *sparse matrix* and *dense matrix* should be defined case-by-case (4 cases in total), since C does not support overloading.

To resolve this problem, two unique `struct`s are defined; `SparseMatElem` and `SquareMatrix`. <br>
`SquareMatrix` is a C struct that contains **dimension**, **# of nonzeros**, and **pointer to matrix**,
and `SparseMatElem` is a C struct that contains **row and column indices**, and the **value** for there.


One thing to point out is that `SquareMatrix` instance includes pointers for both sparse matrix and dense matrix.
This is indeed a useless(?) data overhead. However, considering the interoperability between `SquareMatrix` instances,
this was inevitable. One comforting thing is that, by setting one of these `NULL`, it can be used as a flag variable
that indicates whether the `SquareMatrix` object is sparse or dense.
This way, both sparse matrix and dense matrix can be capsulized as `SquareMatrix`. 

Now, we consider about the operations between matrices.

<br>

## II. Operations

Operations are given in a string form. Therefore, this formula should be parsed into set of operations.
Fortunately, there is no condition to follow the priority rules, so serial reading is enough.
Therefore, it would be efficient to create a separate matrix for the results so far,
and do all the operations upcoming to that matrix.
This will help both the workflow and data integrity of two originally-given matrices.
Here, the '`+`' and '`*`' operators involve two terms, and '\`' operator involves only one term.
The matrix for 'result' will be called `matResult` and for 'upcoming operand' will be called `matNext` from now on.

### 1. Addition

Addition between two dense matrices is either easy; since the length of two matrices are equal,
simply iterating two matrices and add the values is enough.
Since both matrices are already confirmed that dense matrix representation is more efficient,
the output could be represented in dense matrix form.

If `matResult` is dense and `matNext` is sparse, we simply can iterate through indices and values of `matNext`,
and copy the value into proper locations.

Else if `matResult` is sparse and `matNext` is dense, we can convert `matResult` into dense matrix, and add the two dense matrices.

Dealing with addition between two sparse matrices is a real problem. It is somewhat similar to a *union* in set theories.
We should merge two matrices, while finding and adding elements that have identical indices.
Fortunately, thanks to the nature of the process creating sparce matrices, ***both matrices are already sorted in column-major order***. We can simply consider this problem as merging two dictionaries.

When we try to merge two dictionaries into one in lexiographical order (which actually seems quite silly), we might open the first page of both dictionaries and start comparing which word will come prior. In the exactly same method, we can merge two sparse matrices.


### 2. Multiplication

Multiplication was either a hard job to create. (TBD)


### 3. Transposition

Transposition was realized exactly the same way discussed in class.

<br>

## III. Time & Space Compexity

(TBD)

<br>

## IV. How to Use

Usage of this program is exactly same with the requirements in the PDF file, which was preliminarily provided. <br>
**ONE THING TO BE CAUTIOUS: USE \` FOR TRANSPOSITION SYMBOL. OTHERWISE THE PROGRAM WON'T ** 