//
// Created by Can on 1/18/16.
//

#include <hsisomap/gsl_util/embedding.h>
#include <hsisomap/gsl_util/gsl_util.h>
#include <gsl/gsl_eigen.h>
#include <hsisomap/gsl_util/matrix_util.h>

namespace gsl {

Embedding PCA(const Matrix &data, Index reduced_dimensions) {
  if (reduced_dimensions == 0) reduced_dimensions = data.cols();
  if (reduced_dimensions > data.cols()) throw std::invalid_argument("Reduced dimensions should be less than or equal to the total dimensions.");

  Embedding result;
  Index dimensions = data.cols(), samples = data.rows();

  Matrix covariance(dimensions, dimensions);
  Matrix centered_data(samples, dimensions);
  gsl_util_covariance_matrix(data.m_, covariance.m_, GSL_UTIL_COVARIANCE_MATRIX_UNBIASED, centered_data.m_);

  gsl_vector* eigenvalues = gsl_vector_alloc(dimensions);
  std::shared_ptr<Matrix> vectors = std::make_shared<Matrix>(dimensions, dimensions);
  gsl_eigen_symmv_workspace* workspace = gsl_eigen_symmv_alloc(dimensions);
  gsl_eigen_symmv(covariance.m_, eigenvalues, vectors->m_, workspace);
  gsl_eigen_symmv_free(workspace);

  gsl_eigen_symmv_sort(eigenvalues, vectors->m_, GSL_EIGEN_SORT_ABS_DESC);

  std::shared_ptr<Matrix> space = std::make_shared<Matrix>(samples, reduced_dimensions);
  std::shared_ptr<Matrix> values = std::make_shared<Matrix>(1, dimensions);
  gsl_matrix_set_row(values->m_, 0, eigenvalues);
  gsl_vector_free(eigenvalues);

#ifdef TEST_ENABLE_EIGENVECTOR_DIRECTION_CORRECTION
  // Make the eigenvectors have similar direction as the vector (1, 0, ..., 0);
  // It doesn't matter for the results unless compared with results from other
  // programs. The operation is disabled by default for performance considerations.
  // If any test cases failed, enable this operation when testing, and make sure
  // the ground-truth eigenvectors received the same handling.
  gsl::MakeBasesSameDirectionAs(*vectors);
#endif

  // Note that PCA returns the score in a centered coordinates.
  gsl_matrix_view reduced_eigenvectors = gsl_matrix_submatrix(vectors->m_, 0, 0, dimensions, reduced_dimensions);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, centered_data.m_, &reduced_eigenvectors.matrix, 0.0, space->m_);

  result.space = space;
  result.vectors = vectors;
  result.values = values;

  return result;
}


} // namespace gsl
