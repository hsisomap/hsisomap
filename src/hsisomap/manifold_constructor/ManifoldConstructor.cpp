//
// Created by Can on 1/22/16.
//

#include <hsisomap/manifold_constructor/ManifoldConstructor.h>
#include <hsisomap/Logger.h>
#include <gsl/gsl_blas.h>

HSISOMAP_NAMESPACE_BEGIN


std::shared_ptr<gsl::Matrix> ConstructManifold(const gsl::Matrix &landmark_to_all_distances, gsl::Matrix &landmark_distances, gsl::Embedding &landmark_cmds_embedding, Index reduced_dimensions) {

  Index L = landmark_to_all_distances.rows();
  Index N = landmark_to_all_distances.cols();

  gsl::Matrix mean_sqrdist_lm(L, 1);
  for (Index l = 0; l < L; ++l) {
    Scalar mean = 0;
    for (Index l2 = 0; l2 < L; ++l2) mean += landmark_distances(l, l2) * landmark_distances(l, l2); // get squared distance
    mean /= static_cast<Scalar>(L);
    mean_sqrdist_lm(l, 0) = mean;
//    for (Index n = 0; n < N; ++n)
  }

  gsl::Matrix PLt(L, reduced_dimensions);
  for (Index l = 0; l < L; ++l) {
    for (Index d = 0; d < reduced_dimensions; ++d) {
      PLt(l, d) = (*landmark_cmds_embedding.vectors)(l, d) / std::sqrt((*landmark_cmds_embedding.values)(0, d));
    }
  }

  gsl::Matrix Delta(landmark_to_all_distances);
  for (Index l = 0; l < L; ++l) {
    for (Index n = 0; n < N; ++n) {
      Delta(l, n) = mean_sqrdist_lm(l, 0) - Delta(l, n);
    }
  }

  LOGI("Calculating manifold coordinates: PL * Delta.")
  auto manifold = std::make_shared<gsl::Matrix>(N, reduced_dimensions);
  gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, Delta.m_, PLt.m_, 0.0, manifold->m_);

  LOGI("Manifold coordinates constructed.")
  return manifold;

}



HSISOMAP_NAMESPACE_END
