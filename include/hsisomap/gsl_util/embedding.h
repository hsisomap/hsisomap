//***************************************************************************************
//
//! \file embedding.h
//!  A struct to represent the information of a data embedding, and accompanying utility functions to calculate an embedding space.
//!
//! \author    Can
//! \version   1.0
//! \date      2016-01-18
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef HSISOMAP_EMBEDDING_H
#define HSISOMAP_EMBEDDING_H

#include "../Matrix.h"

namespace gsl {

//! A struct to represent the information of a data embedding.
struct Embedding {
  std::shared_ptr<Matrix> space; //!< The input data that represented in the coordinates in the newly calculated embedding space. The rows are the samples. The columns are the dimensions.
  std::shared_ptr<Matrix> vectors; //!< The eigenvectors that can be used to convert the data into the embedding space.
  std::shared_ptr<Matrix> values; //!< The eigenvalues from the calculation of the eigenvectors.
};

//! Principal Component Analysis (PCA) embedding.
//! The function calculates the PCA embedding of the source data. The result is the embedding struct.
//! \param data input data as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \param reduced_dimensions (Optional) the number of reduced dimensions in the resulting embedding space. It can be omitted or set to zero and then the full dimensions would be retained by default.
//! \return PCA embedding of the input data.
Embedding PCA(const Matrix &data, Index reduced_dimensions = 0);

//! Const to help set the gsl::CMDS parameters. See gsl::CMDS for details.
const bool EMBEDDING_CMDS_DEFAULT_SOLVE_ALL = false;
//! Const to help set the gsl::CMDS parameters. See gsl::CMDS for details.
const bool EMBEDDING_CMDS_SOLVE_EIGEN_ONLY = true;

//! Enum type to help set the gsl::CMDS parameters. See gsl::CMDS for details.
enum EMBEDDING_EIGENDECOMPOSITION_ALGORITHM {
  EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_JACOBI = 0, //!< Eigendecomposition using Jacobi algorithm.
  EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_QR = 1 //!< Eigendecomposition using QR decomposition algorithm.
};

//! Classical Multi-Dimensional Scaling (CMDS) embedding.
//! The function calculates the CMDS embedding of the source data. The result is the embedding struct.
//! \param data input data as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \param reduced_dimensions (Optional) the number of reduced dimensions in the resulting embedding space. It can be omitted or set to zero and then the full dimensions would be retained by default.
//! \param solve_eigen_only (Optional) if it is true (EMBEDDING_CMDS_SOLVE_EIGEN_ONLY), only solves eigenvectors and eigenvalues, leaving space (representation of the data in new embedding space) null. The option is for when the last step of coordinate calculation is not needed. By default it is false (EMBEDDING_CMDS_DEFAULT_SOLVE_ALL).
//! \param eigen_algorithm (Optional) the enum variable of EMBEDDING_EIGENDECOMPOSITION_ALGORITHM, specifying the algorithm for eigendecomposition. Currently it supports Jacobi (EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_JACOBI), which is slower but more accurate, or QR decomposition (EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_QR), which is faster but less accurate. By default it is EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_JACOBI.
//! \return CMDS embedding of the input data.
Embedding CMDS(const Matrix &distances, Index reduced_dimensions = 0, bool solve_eigen_only = false, EMBEDDING_EIGENDECOMPOSITION_ALGORITHM eigen_algorithm = EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_JACOBI);

//! Isometric Mapping (ISOMAP) embedding.
//! The function calculates the ISOMAP embedding of the source data. Note that unlike the landmark ISOMAP or other advanced workflow, this calculates the plain ISOMAP with full data points, and it may become impractical when the data is large-scale. The result is the embedding struct.
//! \param data input data as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \param reduced_dimensions (Optional) the number of reduced dimensions in the resulting embedding space. It can be omitted or set to zero and then the full dimensions would be retained by default.
//! \return ISOMAP embedding of the input data.
Embedding ISOMAP(const Matrix &data, Index reduced_dimensions = 0);

//! MNF embedding with Nearest Neighbor Noise Estimation.
//! The function calls gsl::MNF to calculate the MNF embedding, with the noise covariance matrix automatically calculated by the nearest neighbor estimation method.
//! \param data input data as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \param reduced_dimensions (Optional) the number of reduced dimensions in the resulting embedding space. It can be omitted or set to zero and then the full dimensions would be retained by default.
//! \return MNF embedding of the input data using nearest neighbor noise estimation.
Embedding MNFWithNearestNeighborNoiseEstimation(const Matrix &data, Index reduced_dimensions = 0);

//! Minimum Noise Fraction (MNF) embedding.
//! The function calculates the MNF embedding of the source data with the specified noise covariance matrix. The result is the embedding struct.
//! \param data input data as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \param noise_covariance noise covariance matrix as gsl::Matrix. The numbers of rows and columns are both equal to the number of dimensions.
//! \param reduced_dimensions (Optional) the number of reduced dimensions in the resulting embedding space. It can be omitted or set to zero and then the full dimensions would be retained by default.
//! \return MNF embedding of the input data using specified noise covariance matrix.
Embedding MNF(const Matrix &data, const Matrix &noise_covariance, Index reduced_dimensions = 0);

//! Estimate the noise covariance matrix from the data using nearest neighbor method.
//! This function estimates the noise covariance matrix of the input data using nearest neighbor method, which is calculate the nearest neighbor distances in spectral space as the noise covariance matrix. The output is a std::shared_ptr of a gsl::Matrix.
//! \param data input data as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \return smart pointer to the noise covariance matrix of the input data using nearest neighbor method.
std::shared_ptr<Matrix> NearestNeighborNoiseEstimation(const Matrix &data);

//! Calculates L2 distance matrix.
//! The function calculates the L2 distance pairwisely between all vectors from each of the two input data matrices. The dimensions (number of columns) of the two input data matrices must be the same. The output matrix has the number of the rows and columns equal to the number of rows of data1 and data2 matrices respectively.
//! \param data1 first input data matrix as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \param data2 second input data matrix as gsl::Matrix. The rows are the samples. The columns are the dimensions.
//! \param force_zero_diagonal [TO BE SUPPORTED] (Optional) the option to set whether to force set the diagonal of the output matrix to zero. By default it is not to force set to zero. It could and only could be useful when data1 and data2 are the same matrix (in which case the pairwise distance of all same group of vectors are calculated), and the output matrix should be symmetric and with its diagonal elements equal to zeros. Use this option to optionally force set the diagonal to zero to prevent the possible non-zero small number resulted at the diagonal elements due to imprecision of the floating number calculation.
//! \return smart pointer to the L2 distance matrix of the input data.
std::shared_ptr<Matrix> L2Distance(const Matrix &data1, const Matrix &data2, bool force_zero_diagonal = false);

} // namespace gsl

#endif //HSISOMAP_EMBEDDING_H
