//
// Created by Can on 1/18/16.
//

#include <hsisomap/gsl_util/embedding.h>
#include <hsisomap/gsl_util/gsl_util.h>
#include <gsl/gsl_eigen.h>
#include <hsisomap/gsl_util/matrix_util.h>
#include <hsisomap/Logger.h>

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

Embedding CMDS(const Matrix &distances, Index reduced_dimensions, bool solve_eigen_only) {
  if (reduced_dimensions == 0) reduced_dimensions = distances.cols();
  if (distances.cols() != distances.rows()) throw std::invalid_argument("The distances matrix should be a square matrix and symmetry.");
  if (reduced_dimensions > distances.cols()) throw std::invalid_argument("Reduced dimensions should be less than or equal to the total dimensions.");
  Index n = distances.rows();
  Embedding result;

  LOGI("Start CMDS.")

  Matrix H(n, n, - 1.0 / static_cast<Scalar>(n));
  for (Index i = 0; i < n; ++i) H(i, i) += 1.0;

  Matrix Dsqr(distances);
  gsl_matrix_mul_elements(Dsqr.m_, distances.m_);

  Matrix G(n, n);
  Matrix intermediate(n, n);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, H.m_, Dsqr.m_, 0.0, intermediate.m_);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, intermediate.m_, H.m_, 0.0, G.m_);
  gsl_matrix_scale(G.m_, - 0.5);

  LOGI("Start solving eigendecomposition.")

  gsl_eigen_symmv_workspace *esw = gsl_eigen_symmv_alloc(n);
  result.vectors = std::make_shared<gsl::Matrix>(n, n);
  result.values = std::make_shared<gsl::Matrix>(1, n);
  gsl_vector * temp_values = gsl_vector_alloc(n);

  gsl_eigen_symmv(G.m_, temp_values, result.vectors->m_, esw);

  gsl_matrix_set_row(result.values->m_, 0, temp_values);

  gsl_eigen_symmv_free(esw);
  gsl_vector_free(temp_values);

  LOGI("Eigendecomposition finished.")

  if (!solve_eigen_only) throw std::invalid_argument("Solve all not supported yet.");

  return result;
}

} // namespace gsl
