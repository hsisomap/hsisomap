//
// Created by Can on 1/18/16.
//

#include <hsisomap/gsl_util/embedding.h>
#include <hsisomap/gsl_util/gsl_util.h>
#include <gsl/gsl_eigen.h>
#include <hsisomap/gsl_util/matrix_util.h>
#include <hsisomap/Logger.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

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

Embedding CMDS(const Matrix &distances, Index reduced_dimensions, bool solve_eigen_only, EMBEDDING_EIGENDECOMPOSITION_ALGORITHM eigen_algorithm) {
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


  result.vectors = std::make_shared<gsl::Matrix>(n, n);
  result.values = std::make_shared<gsl::Matrix>(1, n);
  gsl_vector * temp_values = gsl_vector_alloc(n);

  if (eigen_algorithm == EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_JACOBI) {
    LOGI("Start solving eigendecomposition using Jacobi SVD.")
    gsl_linalg_SV_decomp_jacobi(G.m_, result.vectors->m_, temp_values);
  } else if (eigen_algorithm == EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_QR) {
    LOGI("Start solving eigendecomposition using QR.")
    gsl_eigen_symmv_workspace *esw = gsl_eigen_symmv_alloc(n);
    gsl_eigen_symmv(G.m_, temp_values, result.vectors->m_, esw);
    gsl_eigen_symmv_free(esw);
  }

  gsl_matrix_set_row(result.values->m_, 0, temp_values);

  gsl_vector_free(temp_values);

  LOGI("Eigendecomposition finished.")

  if (!solve_eigen_only) throw std::invalid_argument("Solve all not supported yet.");

  return result;
}

Embedding ISOMAP(const Matrix &data, Index reduced_dimensions) {
  throw std::invalid_argument("To be implemented.");
}

Embedding MNFWithNearestNeighborNoiseEstimation(const Matrix &data, Index reduced_dimensions) {
  auto noise_estimation = NearestNeighborNoiseEstimation(data);
  return MNF(data, *noise_estimation, reduced_dimensions);
}

Embedding MNF(const Matrix &data, const Matrix &noise_covariance, Index reduced_dimensions) {

  if (noise_covariance.cols() != noise_covariance.rows() || noise_covariance.rows() != data.cols()) {
    throw std::invalid_argument("The rows and columns of noise covariance should both equal to the columns of the data.");
  }
  if (reduced_dimensions == 0) reduced_dimensions = data.cols();
  if (reduced_dimensions > data.cols()) throw std::invalid_argument("Reduced dimensions should be less than or equal to the total dimensions.");


  Matrix U1(noise_covariance);
  Matrix V1(data.cols(), data.cols());

  gsl_vector *S1v = gsl_vector_alloc(data.cols());

  gsl_linalg_SV_decomp_jacobi(U1.m_, V1.m_, S1v);

  Matrix wX(data.rows(), data.cols());
  Matrix wXintermediate(data.cols(), data.cols());
  Matrix invsqrtS1(data.cols(), data.cols());
  for (Index i = 0; i < data.cols(); ++i) {
    invsqrtS1(i, i) = 1.0 / sqrt(gsl_vector_get(S1v, i));
  }
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, U1.m_, invsqrtS1.m_, 0.0, wXintermediate.m_);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, data.m_, wXintermediate.m_, 0.0, wX.m_);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, wX.m_, wX.m_, 0.0, wXintermediate.m_);


  Matrix V2(data.cols(), data.cols());
  gsl_vector *S2v = gsl_vector_alloc(data.cols());
  gsl_linalg_SV_decomp_jacobi(wXintermediate.m_, V2.m_, S2v);


  Embedding result;
  result.space = std::make_shared<gsl::Matrix>(data.rows(), reduced_dimensions);
  result.vectors = std::make_shared<gsl::Matrix>(data.cols(), data.cols());
  result.values = std::make_shared<gsl::Matrix>(1, data.cols());

  Matrix intermediate2(data.cols(), data.cols());
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, invsqrtS1.m_, V2.m_, 0.0, intermediate2.m_);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, U1.m_, intermediate2.m_, 0.0, result.vectors->m_);

  gsl_matrix_const_view reduced_vectors = gsl_matrix_const_submatrix(result.vectors->m_, 0, 0, result.vectors->rows(), reduced_dimensions);

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, data.m_, &reduced_vectors.matrix, 0.0, result.space->m_);


  // TODO: assign eigenvalues and eigenvectors

  gsl_vector_free(S1v);
  gsl_vector_free(S2v);

  return result;
}

std::shared_ptr<Matrix> NearestNeighborNoiseEstimation(const Matrix &data) {

  // TODO: Outlier exclusion

  Index d = data.cols();
  Index n = data.rows();

  std::shared_ptr<Matrix> result = std::make_shared<Matrix>(d, d);

  Matrix noise_values(n, d);

  auto l2_dist = L2Distance(data, data, true);

  // Find the second smallest distance (first is 0, and should be forced, so it is smallest non-zero), so no need to fully sort the matrix
  for (Index i = 0; i < n; ++i) {
    Index smallest_non_zero_index = 0;
    Scalar smallest_non_zero_value = (*l2_dist)(i, 0);
    if (i == 0) {
      smallest_non_zero_value = std::numeric_limits<Scalar>::max();
    }
    for (Index j = 1; j < n; ++j) {
      if (j == i) continue;
      if ((*l2_dist)(i, j) < smallest_non_zero_value) {
        smallest_non_zero_index = j;
        smallest_non_zero_value = (*l2_dist)(i, j);
      }
    }

    for (Index k = 0; k < d; ++k) {
      noise_values(i, k) = data(i, k) - data(smallest_non_zero_index, k);
    }
  }

  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, noise_values.m_, noise_values.m_, 0.0, result->m_);

  return result;
}


std::shared_ptr<Matrix> L2Distance(const Matrix &data1, const Matrix &data2, bool force_zero_diagonal) {

  if (data1.cols() != data2.cols()) {
    throw std::invalid_argument("Data 1 and Data 2 shoud have same dimensionality (data matrices with same number of columns).");
  }

  // TODO: Consider vectorization and one dimension case
  Index d = data1.cols();
  Index n1 = data1.rows();
  Index n2 = data2.rows();


  std::shared_ptr<Matrix> result = std::make_shared<Matrix>(n1, n2);
//  for (Index i = 0; i < n1; ++i) {
//    for (Index j = 0; j < n2; ++j) {
//      // Force zero diagonal
//      if (force_zero_diagonal && i == j) {
//        (*result)(i, j) = 0;
//        continue;
//      }
//
//      Scalar sum = 0;
//      for (Index k = 0; k < d; ++k) {
//        sum += (data1(i, k) - data2(j, k)) * (data1(i, k) - data2(j, k));
//      }
//      (*result)(i, j) = sqrt(sum);
//    }
//  }


//  if (size(a,1) == 1)
//    a = [a; zeros(1,size(a,2))];
//  b = [b; zeros(1,size(b,2))];
//  end

//  gsl::Matrix aSquare(data1.rows(), data1.rows());
//  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, data1.m_, data1.m_, 0.0, aSquare.m_);
//
//  gsl::Matrix bSquare(data2.rows(), data2.rows());
//  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, data2.m_, data2.m_, 0.0, bSquare.m_);

  gsl_vector *aSum = gsl_vector_alloc(data1.rows());
  for (Index i = 0; i < data1.rows(); ++i) {
    gsl_vector_view v = gsl_matrix_row(data1.m_, i);
    double x = 0;
    gsl_blas_ddot(&v.vector, &v.vector, &x);
    gsl_vector_set(aSum, i, x);
  }

  gsl_vector *bSum = gsl_vector_alloc(data2.rows());
  for (Index i = 0; i < data2.rows(); ++i) {
    gsl_vector_view v = gsl_matrix_row(data2.m_, i);
    double x = 0;
    gsl_blas_ddot(&v.vector, &v.vector, &x);
    gsl_vector_set(bSum, i, x);
  }


  gsl_blas_dgemm(CblasNoTrans, CblasTrans, -2.0, data1.m_, data2.m_, 0.0, result->m_);
  for (Index i = 0; i < n1; ++i) {
//    gsl_matrix_get(data1.m_, )
    for (Index j = 0; j < n2; ++j) {
      gsl_matrix_set(result->m_, i, j, sqrt(gsl_vector_get(aSum, i) + gsl_vector_get(bSum, j) + gsl_matrix_get(result->m_, i, j)));
    }
  }

  gsl_vector_free(aSum);
  gsl_vector_free(bSum);

//      aa=sum(a.*a); bb=sum(b.*b); ab=a'*b;
//  d = sqrt(repmat(aa',[1 size(bb,2)]) + repmat(bb,[size(aa,2) 1]) - 2*ab);
//
//      % make sure result is all real
//  d = real(d);
//
//  % force 0 on the diagonal?
//  if (df==1)
//    d = d.*(1-eye(size(d)));
//  end



  return result;
}


} // namespace gsl
