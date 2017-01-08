//***************************************************************************************
//
//! \file matrix_util.h
//!  A collection of helper functions for simple matrix manipulations.
//!
//! \author    Can
//! \version   1.0
//! \date      2016-01-18
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef HSISOMAP_MATRIX_UTIL_H
#define HSISOMAP_MATRIX_UTIL_H

#include <hsisomap/Matrix.h>

namespace gsl {

//! The function makes a set of orthogonal base vectors the same direction as the datum vector.
//! The function takes an input matrix that represents an orthogonal base vectors as column vectors, and convert it in-place to a set of equivalent base vectors that has the same direction as the datum vector.
//! This usually means that the absolute values of the elements in the matrix are not changed, but the signs of each column might be changed.
//!
//! This is useful for unit test so that the equivalent output as a base vectors have the same representation.
//! \param bases input matrix representing base vectors as column vectors for an orthogonal space. Note that the conversion is in-place of the input variable.
//! \param optional_datum (Optional) the datum vector to represent the direction the new base vectors should be pointing at.
void MakeBasesSameDirectionAs(Matrix &bases, const Matrix &optional_datum = Matrix(1, 1, 1.0));

//! Sort the elements of the each row of the matrix separately to ascending order, and also arrange the second matrix using the same moving operation.
//! The function takes a matrix, and sort the elements of each row of the matrix separately to ascending order. It will also re-arrange the order of the elements of each row of the second matrix, with the same operation of the sorting. The also-rearranged matrix is useful to keep the indices of the elements.
//!
//! The function will sort and arrange the elements of the two input matrices in-place.
//! \param matrix the input matrix to be sort in-place.
//! \param also_arrange the input matrix to be also re-arranged in-place.
//! \param do_not_also_arrange if it is true, disable the also_arrange operation as if the parameter also_arrange is not supplied. It is useful for the overriding function without also-arrange operation. By default it is false.
void SortMatrixRows(Matrix &matrix, Matrix &also_arrange, bool do_not_also_arrange = false);

//! Sort the elements of the each row of the matrix separately to ascending order.
//! The function takes a matrix, and sort the elements of each row of the matrix separately to ascending order.
//!
//! The function will sort and arrange the elements of the input matrix in-place.
//! \param matrix the input matrix to be sort in-place.
void SortMatrixRows(Matrix &matrix);

//! Sort the elements of the each column of the matrix separately to ascending order, and also arrange the second matrix as the same moving operation.
//! The function takes a matrix, and sort the elements of each column of the matrix separately to ascending order. It will also re-arrange the order of the elements of each column of the second matrix, with the same operation of the sorting. The also-rearranged matrix is useful to keep the indices of the elements.
//!
//! The function will sort and arrange the elements of the two input matrices in-place.
//! \param matrix the input matrix to be sort in-place.
//! \param also_arrange the input matrix to be also re-arranged in-place.
//! \param do_not_also_arrange if it is true, disable the also_arrange operation as if the parameter also_arrange is not supplied. It is useful for the overriding function without also-arrange operation. By default it is false.
void SortMatrixCols(Matrix &matrix, Matrix &also_arrange, bool do_not_also_arrange = false);

//! Sort the elements of the each column of the matrix separately to ascending order.
//! The function takes a matrix, and sort the elements of each column of the matrix separately to ascending order.
//!
//! The function will sort and arrange the elements of the input matrix in-place.
//! \param matrix the input matrix to be sort in-place.
void SortMatrixCols(Matrix &matrix);

//! Generate a new matrix with the specified rows of the original matrix.
//! It generates a new matrix with the specified rows of the original matrix. The different with the Matrix::GetRows is that this function generates a new matrix as a copy, and uses shared_ptr.
//! \param matrix_ptr smart pointer to the input matrix.
//! \param rows vector of indices of the selected rows to be copied.
//! \return smart pointer to the resulting matrix.
std::shared_ptr<gsl::Matrix> GetRows(const std::shared_ptr<const gsl::Matrix> matrix_ptr, std::vector<Index> rows);

//! Generate a new matrix with the specified columns of the original matrix.
//! It generates a new matrix with the specified columns of the original matrix. The different with the Matrix::GetCols is that this function generates a new matrix as a copy, and uses shared_ptr.
//! \param matrix_ptr smart pointer to the input matrix.
//! \param cols vector of indices of the selected columns to be copied.
//! \return smart pointer to the resulting matrix.
std::shared_ptr<gsl::Matrix> GetCols(const std::shared_ptr<const gsl::Matrix> matrix_ptr, std::vector<Index> cols);

//! Calculate the distance between two specified row vectors of a matrix.
//! It calculates the distance between two specified row vectors of the input matrix. The two row vectors is specified by the row indices.
//! \param matrix the input matrix. Since the row vectors are calculated, it is useful when the matrix is representing a collection of row vectors, such that each row represents a sample, and each column represents a dimension.
//! \param a the index of the first row.
//! \param b the index of the second row.
//! \return distance of the two specified row vectors in the matrix.
Scalar RowVectorDistance(const Matrix &matrix, Index a, Index b);

//! Calculate the square distance between two specified row vectors of a matrix.
//! It calculates the square of the distance between two specified row vectors of the input matrix. The two row vectors is specified by the row indices.
//!
//! Since the square of the distance is calculated, the square-root operation in a distance calculation is not involved to save time for large scale data. It can be useful when only the comparison is involved, while the actual value of the distances are not as important.
//! \param matrix the input matrix. Since the row vectors are calculated, it is useful when the matrix is representing a collection of row vectors, such that each row represents a sample, and each column represents a dimension.
//! \param a the index of the first row.
//! \param b the index of the second row.
//! \return square distance of the two specified row vectors in the matrix.
Scalar RowVectorDistanceSquare(const Matrix &matrix, Index a, Index b);

//! Calculate the distance between two specified row vectors from two matrix.
//! It calculates the distance between two specified row vectors from two input matrices. The two row vectors is specified by the row indices.
//! The two indices specified the indices of the row vector to be computed from each of the two input matrices.
//!
//! By default the indices are zero. Therefore it is useful to quickly calculate the distance between two row vectors represented as one-row matrices.
//! \param matrix the input matrix. Since the row vectors are calculated, it is useful when the matrix is representing a collection of row vectors, such that each row represents a sample, and each column represents a dimension.
//! \param a (Optional) the index of the first row. By default it is 0, useful to quickly calculate for two row vectors represented by matrices.
//! \param b the index of the second row. By default it is 0.
//! \return distance of the two specified row vectors from each of the matrices.
Scalar RowVectorDistanceToRowVector(const Matrix &matrix_a, const Matrix &matrix_b, Index a = 0, Index b = 0);


//! Calculate the square distance between two specified row vectors from two matrix.
//! It calculates the square of the distance between two specified row vectors from two input matrices. The two row vectors is specified by the row indices.
//! The two indices specified the indices of the row vector to be computed from each of the two input matrices.
//!
//! By default the indices are zero. Therefore it is useful to quickly calculate the distance between two row vectors represented as one-row matrices.
//!
//! Since the square of the distance is calculated, the square-root operation in a distance calculation is not involved to save time for large scale data. It can be useful when only the comparison is involved, while the actual value of the distances are not as important.
//! \param matrix the input matrix. Since the row vectors are calculated, it is useful when the matrix is representing a collection of row vectors, such that each row represents a sample, and each column represents a dimension.
//! \param a (Optional) the index of the first row. By default it is 0, useful to quickly calculate for two row vectors represented by matrices.
//! \param b the index of the second row. By default it is 0.
//! \return square distance of the two specified row vectors from each of the matrices.
Scalar RowVectorDistanceSquareToRowVector(const Matrix &matrix_a, const Matrix &matrix_b, Index a = 0, Index b = 0);


} // namespace gsl

#endif //HSISOMAP_MATRIX_UTIL_H
