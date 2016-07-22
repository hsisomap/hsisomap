//
// Created by Can on 1/18/16.
//

#ifndef HSISOMAP_EMBEDDING_H
#define HSISOMAP_EMBEDDING_H

#include "../Matrix.h"

namespace gsl {

struct Embedding {
  std::shared_ptr<Matrix> space;
  std::shared_ptr<Matrix> vectors;
  std::shared_ptr<Matrix> values;
};

Embedding PCA(const Matrix &data, Index reduced_dimensions = 0);

const bool EMBEDDING_CMDS_DEFAULT_SOLVE_ALL = false;
const bool EMBEDDING_CMDS_SOLVE_EIGEN_ONLY = true;

enum EMBEDDING_EIGENDECOMPOSITION_ALGORITHM {
  EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_JACOBI = 0,
  EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_QR = 1
};

Embedding CMDS(const Matrix &distances, Index reduced_dimensions = 0, bool solve_eigen_only = false, EMBEDDING_EIGENDECOMPOSITION_ALGORITHM eigen_algorithm = EMBEDDING_EIGENDECOMPOSITION_ALGORITHM_JACOBI);

Embedding ISOMAP(const Matrix &data, Index reduced_dimensions = 0);

Embedding MNF(const Matrix &data, const Matrix &noise_covariance, Index reduced_dimensions = 0);

std::shared_ptr<Matrix> NearestNeighborNoiseEstimation(const Matrix &data);

std::shared_ptr<Matrix> L2Distance(const Matrix &data1, const Matrix &data2, bool force_zero_diagonal = false);

} // namespace gsl

#endif //HSISOMAP_EMBEDDING_H
