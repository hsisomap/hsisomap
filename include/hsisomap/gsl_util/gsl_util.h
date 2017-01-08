//***************************************************************************************
//
//! \file gsl_util.h
//!  A collection of simple inline computation functions including some frequently used linear algebra computations.
//!  Header-only inline implementation and the resources are not managed. Note that the non-managed functions are all lower-case named and start with 'gsl_util_' if gsl_matrix is used.
//!
//! \author    Can
//! \version   1.0
//! \date      2015-07-27
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef GSL_UTIL_H
#define GSL_UTIL_H


#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_matrix_double.h>
#include <functional>
#include <iostream>


namespace gsl {

//! Pseudo-inversion using Jacobi SVD.
//! Calculates the pseudo-inversion of the input matrix using Jacobi SVD algorithm. The output matrix needs to be allocated outside of the function.
//! \param A_c pointer to input matrix.
//! \param pinv pointer to output matrix.
inline void gsl_util_pinv(const gsl_matrix *A_c, gsl_matrix *pinv) {

  gsl_matrix *A = gsl_matrix_calloc(A_c->size1, A_c->size2);
  gsl_matrix_memcpy(A, A_c);

  gsl_matrix *V = gsl_matrix_calloc(A->size1, A->size2);
  gsl_vector *S = gsl_vector_calloc(A->size1);

  gsl_linalg_SV_decomp_jacobi(A, V, S);

  for (size_t j = 0; j < A->size1; ++j) {
    double factor = gsl_vector_get(S, j);
    if (factor > 1e-10 || factor < -1e-10) factor = 1 / factor; else factor = 0;
    for (size_t i = 0; i < A->size2; ++i)
      gsl_matrix_set(V, i, j, gsl_matrix_get(V, i, j) * factor);
  }

  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, V, A, 0.0, pinv);
  gsl_matrix_free(A);
  gsl_matrix_free(V);
  gsl_vector_free(S);
}


//! Calculate mean value of a vector after applying a lambda function. Useful for mean and variance.
//! The data must be represented by a vector. Data1 and 2 must have the same size.
//!
//! There are two input vectors, and the actual value used for each element is determined by pre_operation function. For example, one could be actual data, and another is a reference to determine the modification of the data before it accumulates.
//!
//! If the unbiased == 1, the mean is the unbiased estimated sample mean (the denominator is the number of samples minus 1).
//! \param data1 first input vector
//! \param data2 second input vector
//! \param pre_operation lambda function that takes two value and return one value, to be applied on each elements of the two input vectors. The result will be accumulated before dividing the sample number to get the mean.
//! \param unbiased (Optional) it can only be 0 or 1 to be meaningful, as it will be subtracted from the number of elements of the input vectors. When it is 1, the function is calculating the unbiased mean. When it is 0, it is biased. By default it is 0 (biased).
//! \return the processed "mean" value.
inline double mean_of(const gsl_vector *data1,
               const gsl_vector *data2,
               std::function<double(double, double)> pre_operation,
               int unbiased = 0) {

  if (data1->size != data2->size) {
    std::cerr << "Vector size error" << std::endl;
    return 0;
  }
  double sum = 0;
  for (size_t ob = 0; ob < data1->size; ++ob) {
    sum += pre_operation(gsl_vector_get(data1, ob), gsl_vector_get(data2, ob));
  }
  sum /= data1->size - unbiased;
  return sum;
}


//! Calculate covariance matrix using loops by definition of covariance matrix.
//! Calculate covariance matrix using loops by definition of covariance matrix.
//! CAUTION: It is much slower than the matrix multiplication version (without '_loop' in the function name).
//!
//! The covariance matrix must be a square matrix with the length equal to the second dimension of the matrix to be calculated.
//! The unbiased is 1 by default to calculate the unbiased estimated sample covariances.
//! \param data input data matrix. The rows represent the samples while the columns represent the dimensions.
//! \param covariance output covariance matrix. It is a symmetric matrix with the size equal to the number of columns of the input matrix. The output matrix needs to be allocated outside of the function.
//! \param unbiased (Optional) it can only be 0 or 1 to be meaningful, as it will be subtracted from the number of samples. When it is 1, the function is calculating the unbiased mean. When it is 0, it is biased. By default it is 1 (unbiased).
inline void gsl_util_covariance_matrix_loop(const gsl_matrix *data, gsl_matrix *covariance, int unbiased = 1) {

  size_t dims = data->size2;
  if (covariance->size1 != covariance->size2 || covariance->size2 != dims) {
    std::cerr << "Dimension error" << std::endl;
    return;
  }

  double *means = new double[dims];

  for (size_t dim = 0; dim < dims; ++dim) {
    gsl_vector_const_view data1 = gsl_matrix_const_column(data, dim);
    means[dim] = mean_of(&data1.vector, &data1.vector, [](double el1, double el2) {
      return el1;
    });
  }

  for (size_t dim1 = 0; dim1 < dims; ++dim1) {
    gsl_vector_const_view data1 = gsl_matrix_const_column(data, dim1);
    double means_dim1 = means[dim1];
    for (size_t dim2 = dim1; dim2 < dims; ++dim2) {
      gsl_vector_const_view data2 = gsl_matrix_const_column(data, dim2);
      double means_dim2 = means[dim2];
      double cov = mean_of(&data1.vector, &data2.vector, [means_dim1, means_dim2](double el1, double el2) {
        return (el1 - means_dim1) * (el2 - means_dim2);
      }, unbiased);
      gsl_matrix_set(covariance, dim1, dim2, cov);
      gsl_matrix_set(covariance, dim2, dim1, cov);
    }
  }

  delete[] means;
}

//! Calculate covariance matrix using matrix multiplication and intrusively changes input data to save space and time.
//! Calculate covariance matrix using matrix multiplication, which is much faster than using the definition. It also intrusively changes input data to save space and time.
//!
//! It is useful when you don't need the input data matrix anymore, or you already have a copy, and / or you need the centered data. It is also faster than non-intrusive version.
//! The input data will become the centered data (data matrix minus mean vector).
//!
//! The covariance matrix must be a square matrix with the length equal to the second dimension of the matrix to be calculated.
//! The unbiased is 1 by default to calculate the unbiased estimated sample covariances.
//! \param data input data matrix. The rows represent the samples while the columns represent the dimensions.
//! \param covariance output covariance matrix. It is a symmetric matrix with the size equal to the number of columns of the input matrix. The output matrix needs to be allocated outside of the function.
//! \param unbiased (Optional) it can only be 0 or 1 to be meaningful, as it will be subtracted from the number of samples. When it is 1, the function is calculating the unbiased mean. When it is 0, it is biased. By default it is 1 (unbiased).
inline void gsl_util_covariance_matrix_intrusive(gsl_matrix *data, gsl_matrix *covariance, int unbiased = 1) {

  size_t dims = data->size2;
  size_t obs = data->size1;
  if (covariance->size1 != covariance->size2 || covariance->size2 != dims) {
    std::cerr << "Dimension error" << std::endl;
    return;
  }

  gsl_vector *means = gsl_vector_alloc(dims);
  for (size_t c = 0; c < dims; ++c) {
    gsl_vector_set(means, c, gsl_stats_mean(data->data + c, dims, obs));
  }

  for (size_t r = 0; r < obs; ++r) {
    gsl_vector_view row = gsl_matrix_row(data, r);
    gsl_vector_sub(&row.vector, means);
  }

  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, data, data, 0.0, covariance);
  gsl_matrix_scale(covariance, 1.0 / (obs - unbiased));
  gsl_vector_free(means);
}

//! Calculate covariance matrix using matrix multiplication non-intrusively (the original data is not modified).
//! Calculate covariance matrix using matrix multiplication, which is much faster than using the definition. It will not modify the original data matrix so that you can use const for input data.
//!
//! The covariance matrix must be a square matrix with the length equal to the second dimension of the matrix to be calculated.
//! The unbiased is 1 by default to calculate the unbiased estimated sample covariances.
//!
//! The function optionally takes a pre-allocated matrix and a pre-allocated vector to be used and return the centered data matrix and the mean vector of all row vectors.
//! \param data input data matrix. The rows represent the samples while the columns represent the dimensions.
//! \param covariance output covariance matrix. It is a symmetric matrix with the size equal to the number of columns of the input matrix. The output matrix needs to be allocated outside of the function.
//! \param unbiased (Optional) it can only be 0 or 1 to be meaningful, as it will be subtracted from the number of samples. When it is 1, the function is calculating the unbiased mean. When it is 0, it is biased. By default it is 1 (unbiased).
//! \param centered_data_return (Optional) return the centered data matrix. It needs to be pre-allocated before calling the function and the memory will be used in the calculation. If not needed, set it to NULL. By default it is NULL.
//! \param means_return (Optional) return the means vector. It needs to be pre-allocated before calling the function and the memory will be used in the calculation. If not needed, set it to NULL. By default it is NULL.
inline void gsl_util_covariance_matrix(const gsl_matrix *data, gsl_matrix *covariance, int unbiased = 1, gsl_matrix *centered_data_return = NULL, gsl_vector *means_return = NULL) {

  size_t dims = data->size2;
  size_t obs = data->size1;
  if (covariance->size1 != covariance->size2 || covariance->size2 != dims) {
    std::cerr << "Dimension error" << std::endl;
    return;
  }

  gsl_vector *means;
  if (means_return) {
    means = means_return;
  } else {
    means = gsl_vector_alloc(dims);
  }

  for (size_t c = 0; c < dims; ++c) {
    gsl_vector_set(means, c, gsl_stats_mean(data->data + c, dims, obs));
  }

  gsl_matrix *centered_data;
  if (centered_data_return) {
    centered_data = centered_data_return;
  } else {
    centered_data = gsl_matrix_alloc(obs, dims);
  }

  for (size_t r = 0; r < obs; ++r) {
    for (size_t c = 0; c < dims; ++c) {
      gsl_matrix_set(centered_data, r, c,
                     gsl_matrix_get(data, r, c) - gsl_vector_get(means, c));
    }
  }

  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, centered_data, centered_data, 0.0, covariance);
  gsl_matrix_scale(covariance, 1.0 / (obs - unbiased));
  if (!means_return) {
    gsl_vector_free(means);
  }
  if (!centered_data_return) {
    gsl_matrix_free(centered_data);
  }

}


const int GSL_UTIL_COVARIANCE_MATRIX_BIASED = 0; //!< Calculate biased mean / covariance when asked for unbiased option in gsl::mean_of, gsl::gsl_util_covariance_matrix_loop, gsl::gsl_util_covariance_matrix_intrusive, gsl::gsl_util_covariance_matrix.
const int GSL_UTIL_COVARIANCE_MATRIX_UNBIASED = 1; //!< Calculate unbiased mean / covariance when asked for unbiased option  in gsl::mean_of, gsl::gsl_util_covariance_matrix_loop, gsl::gsl_util_covariance_matrix_intrusive, gsl::gsl_util_covariance_matrix.

} // namespace gsl

#endif //GSL_UTIL_H
