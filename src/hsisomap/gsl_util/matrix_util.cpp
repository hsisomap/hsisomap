//
// Created by Can on 1/18/16.
//

#include <hsisomap/gsl_util/matrix_util.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_vector_double.h>

namespace gsl {

void MakeBasesSameDirectionAs(Matrix &bases, const Matrix &optional_datum) {
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

std::shared_ptr<gsl::Matrix> GetRows(const std::shared_ptr<const gsl::Matrix> matrix_ptr, std::vector<Index> rows) {

  auto result = std::make_shared<gsl::Matrix>(rows.size(), matrix_ptr->cols());
  for (Index r = 0; r < rows.size(); ++r) {
    for (Index c = 0; c < matrix_ptr->cols(); ++c) {
      gsl_matrix_set(result->m_, r, c, gsl_matrix_get(matrix_ptr->m_, rows[r], c));
    }
  }
  return result;
}

std::shared_ptr<gsl::Matrix> GetCols(const std::shared_ptr<const gsl::Matrix> matrix_ptr, std::vector<Index> cols) {

  auto result = std::make_shared<gsl::Matrix>(matrix_ptr->rows(), cols.size());
  for (Index r = 0; r < matrix_ptr->rows(); ++r) {
    for (Index c = 0; c < cols.size(); ++c) {
      gsl_matrix_set(result->m_, r, c, gsl_matrix_get(matrix_ptr->m_, r, cols[c]));
    }
  }
  return result;
}

Scalar RowVectorDistance(const Matrix &matrix, Index a, Index b) {
  gsl_vector *va = gsl_vector_alloc(matrix.cols());
  gsl_matrix_get_row(va, matrix.m_, a);
  gsl_vector_const_view vb_view = gsl_matrix_const_row(matrix.m_, b);
  gsl_vector_sub(va, &vb_view.vector);
  Scalar result = gsl_blas_dnrm2(va);
  gsl_vector_free(va); // Remember to deallocate memory when using GSL C API.
  return result;
}

Scalar RowVectorDistanceSquare(const Matrix &matrix, Index a, Index b) {
//  gsl_vector *va = gsl_vector_alloc(matrix.cols());
//  gsl_matrix_get_row(va, matrix.m_, a);
//  gsl_vector_const_view vb_view = gsl_matrix_const_row(matrix.m_, b);
//  gsl_vector_sub(va, &vb_view.vector);
//  gsl_vector_mul(va, va);
//  Scalar result = gsl_blas_dasum(va);
//  gsl_vector_free(va);

  Scalar result = 0;
  for (Index i = 0; i < matrix.cols(); ++i) {
    result += pow(matrix(a, i) - matrix(b, i), 2);
  }
  return result;
}

Scalar RowVectorDistanceToRowVector(const Matrix &matrix_a, const Matrix &matrix_b, Index a, Index b) {
  gsl_vector *va = gsl_vector_alloc(matrix_a.cols());
  gsl_matrix_get_row(va, matrix_a.m_, a);
  gsl_vector_const_view vb_view = gsl_matrix_const_row(matrix_b.m_, b);
  gsl_vector_sub(va, &vb_view.vector);
  Scalar result = gsl_blas_dnrm2(va);
  gsl_vector_free(va);
  return result;
}

Scalar RowVectorDistanceSquareToRowVector(const Matrix &matrix_a, const Matrix &matrix_b, Index a, Index b) {
//  gsl_vector *va = gsl_vector_alloc(matrix_a.cols());
//  gsl_matrix_get_row(va, matrix_a.m_, a);
//  gsl_vector_const_view vb_view = gsl_matrix_const_row(matrix_b.m_, b);
//  gsl_vector_sub(va, &vb_view.vector);
//  gsl_vector_mul(va, va);
//  Scalar result = gsl_blas_dasum(va);
//  gsl_vector_free(va);

  Scalar result = 0;
  for (Index i = 0; i < matrix_a.cols(); ++i) {
    result += pow(matrix_a(a, i) - matrix_b(b, i), 2);
  }

  return result;
}

} // namespace gsl
