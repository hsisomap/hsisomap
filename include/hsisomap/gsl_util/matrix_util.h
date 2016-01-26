//
// Created by Can on 1/18/16.
//

#ifndef HSISOMAP_MATRIX_UTIL_H
#define HSISOMAP_MATRIX_UTIL_H

#include <hsisomap/Matrix.h>

namespace gsl {

void MakeBasesSameDirectionAs(Matrix &bases, const Matrix &optional_datum = Matrix(1, 1, 1.0));

void SortMatrixRows(Matrix &matrix, Matrix &also_arrange, bool do_not_also_arrange = false);

void SortMatrixRows(Matrix &matrix);

// GetRows generates a new matrix with the rows of the original matrix specified.
// The different with the Matrix::GetRows is that this function uses shared_ptr.
std::shared_ptr<gsl::Matrix> GetRows(const std::shared_ptr<const gsl::Matrix> matrix_ptr, std::vector<Index> rows);

// GetCols generates a new matrix with the columns of the original matrix specified.
// The different with the Matrix::GetCols is that this function uses shared_ptr.
std::shared_ptr<gsl::Matrix> GetCols(const std::shared_ptr<const gsl::Matrix> matrix_ptr, std::vector<Index> cols);


Scalar RowVectorDistance(const Matrix &matrix, Index a, Index b);
Scalar RowVectorDistanceSquare(const Matrix &matrix, Index a, Index b);
Scalar RowVectorDistanceToRowVector(const Matrix &matrix_a, const Matrix &matrix_b, Index a = 0, Index b = 0);
Scalar RowVectorDistanceSquareToRowVector(const Matrix &matrix_a, const Matrix &matrix_b, Index a = 0, Index b = 0);


} // namespace gsl

#endif //HSISOMAP_MATRIX_UTIL_H
