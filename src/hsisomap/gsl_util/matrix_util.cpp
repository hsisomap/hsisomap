//
// Created by Can on 1/18/16.
//

#include <hsisomap/gsl_util/matrix_util.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_vector_double.h>

namespace gsl {

void MakeBasesSameDirectionAs(Matrix &bases, const Matrix &optional_datum) {
  std::cout << "AAA";
  Matrix datum = optional_datum;
  if (datum.cols() != bases.rows()) {
    datum = Matrix(1, bases.rows(), 0.0);
    datum(0, 0) = 1.0;
  }
  gsl_vector_const_view datum_view = gsl_matrix_const_row(datum.m_, 0);
  for (Index col = 0; col < bases.cols(); col++) {
    gsl_vector_view column_view = gsl_matrix_column(bases.m_, col);
    double inner_product = 0;
    gsl_blas_ddot(&column_view.vector, &datum_view.vector, &inner_product);
    if (inner_product == 0) {
      std::cerr << "Basis orthogonal to the datum vector; try another datum vector." << std::endl;
    }
    if (inner_product < 0) {
      gsl_vector_scale(&column_view.vector, - 1.0);
    }
  }
};

void SortMatrixRows(Matrix &matrix, Matrix &also_arrange, bool do_not_also_arrange) {
  for (Index r = 0; r < matrix.rows(); ++r) {
    gsl_vector_view row_view = gsl_matrix_row(matrix.m_, r);
    if (do_not_also_arrange) {
      gsl_sort_vector(&row_view.vector);
    } else {
      gsl_vector_view row_view_also_arrange = gsl_matrix_row(also_arrange.m_, r);
      gsl_sort_vector2(&row_view.vector, &row_view_also_arrange.vector);
    }
  }
}

void SortMatrixRows(Matrix &matrix) {
  Matrix pseudo(1, 1);
  SortMatrixRows(matrix, pseudo, true);
}

} // namespace gsl
