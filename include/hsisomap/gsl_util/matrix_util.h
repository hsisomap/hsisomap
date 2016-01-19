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


} // namespace gsl

#endif //HSISOMAP_MATRIX_UTIL_H
