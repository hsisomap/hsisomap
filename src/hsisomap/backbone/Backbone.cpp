//
// Created by Can on 1/19/16.
//

#include <hsisomap/backbone/Backbone.h>
#include <hsisomap/gsl_util/matrix_util.h>
#include <hsisomap/Logger.h>
#include <hsisomap/util/VpTree.h>
#include <gsl/gsl_blas.h>
#include <hsisomap/gsl_util/gsl_util.h>

HSISOMAP_NAMESPACE_BEGIN

Backbone::Backbone(const std::shared_ptr<const gsl::Matrix> data, const std::vector<Index> &sampling_indices)
    :
    data_(data), sampling_indices_(sampling_indices) {

  sampled_data_ = gsl::GetRows(data_, sampling_indices_);

  // Make a copy of sampling indices as a set to facilitate finding.
  for (Index i = 0; i < sampling_indices_.size(); ++i) {
    sampling_indices_reverse_table_[sampling_indices_[i]] = i;
  }
  if (sampling_indices_reverse_table_.size() != sampling_indices.size()) throw std::invalid_argument("No repeated sampling allowed.");

}

void Backbone::PrepareNNCache(Index neighborhood_size, PropertyList optional_settings) {
  if (neighborhood_size > data_->rows()) neighborhood_size = data_->rows();
  Index PRIMARY_SEARCH_RANGE = static_cast<kIndex>(optional_settings[BACKBONE_NNCACHE_PRIMARY_SEARCH_RANGE]);
  Index SECONDARY_SEARCH_RANGE = static_cast<kIndex>(optional_settings[BACKBONE_NNCACHE_SECONDARY_SEARCH_RANGE]);
  if (PRIMARY_SEARCH_RANGE == 0) PRIMARY_SEARCH_RANGE = std::min(neighborhood_size * 10, data_->rows());
  if (SECONDARY_SEARCH_RANGE == 0) SECONDARY_SEARCH_RANGE = std::min(neighborhood_size * 20, data_->rows());

  VpTree<PixelView, SquaredDistance> vptree;
  LOGI("Create PixelView array.");
  auto pixel_views = CreatePixelViewsFromMatrix(*data_);
  LOGI("Creating VpTree for the whole image.")
  vptree.create(pixel_views);
  LOGI("VpTree created.")

  // The number of rows in NN cache matrix is the number of non-backbone pixels.
  // The number of cols in NN cache matrix is the number of (precalculated) neighborhood size plus one,
  // since the first column is the index of the non-backbone pixel in the whole image.
  nn_cache_ = std::make_shared<gsl::Matrix>(data_->rows() - sampling_indices_.size(), neighborhood_size + 1);

  Index recon_idx = 0;
  for (Index full_idx = 0; full_idx < data_->rows(); ++full_idx) {

    // Progress indicator
    if (full_idx % 10000 == 0) {
      LOGI("[NN] " << full_idx << " of " << data_->rows() << " finished ("
               << (int) ((float) full_idx * 100.0 / data_->rows()) << "%).")
    }

    // If it is found in the sampling indices set, ignore.
    if (sampling_indices_reverse_table_.find(full_idx) != sampling_indices_reverse_table_.end()) continue;

    // Otherwise, a non-backbone pixel is found.
    (*nn_cache_)(recon_idx, 0) = full_idx;

    Key LOG_PROMPT[4] = {"PRIMARY", "SECONDARY", "EXHAUSTIVE", "ERROR"};
    Index search_ranges[3] = {PRIMARY_SEARCH_RANGE, SECONDARY_SEARCH_RANGE, data_->rows()};
    for (Index iteration = 0; iteration < 3; ++iteration) {

      std::vector<PixelView> results;
      std::vector<Scalar> distance_squares;
      vptree.search(pixel_views[full_idx], search_ranges[iteration], &results, &distance_squares);

      Index encountered_backbone_count = 0;

      for (auto result : results) {
        if (sampling_indices_reverse_table_.find(result.index) != sampling_indices_reverse_table_.end()) {
          (*nn_cache_)(recon_idx, encountered_backbone_count + 1) = result.index;
          encountered_backbone_count++;
          if (encountered_backbone_count == neighborhood_size) break;
        }
      }

      if (encountered_backbone_count != neighborhood_size) {
        LOGI("Only " << encountered_backbone_count << " backbone pixels found in " << LOG_PROMPT[iteration]
                 << " search range. Use " << LOG_PROMPT[iteration + 1] << " search range instead.");
      } else {
        break;
      }

    }

    recon_idx++;

  }

  LOGI("[NN] NN Cache created.")
}

std::shared_ptr<gsl::Matrix> Backbone::Reconstruct(const gsl::Matrix &input,
                                                   PropertyList reconstruction_strategy,
                                                   std::shared_ptr<gsl::Matrix> optional_cache) {

  if (reconstruction_strategy[BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY]
      == BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_ADAPTIVE)
    throw std::invalid_argument("BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_ADAPTIVE not supported yet.");

  Index neighborhood_size = static_cast<Index>(reconstruction_strategy[BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER]);
  if (optional_cache) nn_cache_ = optional_cache;
  if (!nn_cache_) PrepareNNCache(neighborhood_size);

  auto result = std::make_shared<gsl::Matrix>(data_->rows(), input.cols());
  Index current_sampled_idx = 0;
  Index recon_idx = 0;
  for (Index full_idx = 0; full_idx < data_->rows(); ++full_idx) {
    // Progress indicator
    if (full_idx % 10000 == 0) {
      LOGI("[RECON] " << full_idx << " of " << data_->rows() << " finished ("
               << (int) ((float) full_idx * 100.0 / data_->rows()) << "%).")
    }

    if (full_idx == sampling_indices_[current_sampled_idx]) {
      // This is the backbone pixel; simply copy the value.
      for (Index b = 0; b < input.cols(); ++b) (*result)(full_idx, b) = input(current_sampled_idx, b);
      current_sampled_idx++;
      continue;
    }

    // Prepare dd matrix
    gsl::Matrix dd(neighborhood_size, data_->cols());
    std::vector<Index> encountered_backbone_samples(neighborhood_size, 0);

    for (Index n = 0; n < neighborhood_size; ++n) {
      encountered_backbone_samples[n] = sampling_indices_reverse_table_[(*nn_cache_)(recon_idx, n + 1)];
      for (Index b = 0; b < data_->cols(); ++b) dd(n, b) = (*data_)(full_idx, b) - (*data_)((*nn_cache_)(recon_idx, n + 1), b);
    }
    recon_idx++;

    // Prepare cc_inv
    gsl::Matrix cc(neighborhood_size, neighborhood_size);
    gsl::Matrix cc_inv(neighborhood_size, neighborhood_size);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, dd.m_, dd.m_, 0.0, cc.m_);
    gsl::gsl_util_pinv(cc.m_, cc_inv.m_);

    // Prepare ww (weights)
    gsl::Matrix ww(1, neighborhood_size);
    Scalar cc_inv_sum = 0;
    for (Index k = 0; k < neighborhood_size; ++k) {
      Scalar line_sum = 0;
      for (Index k2 = 0; k2 < neighborhood_size; ++k2) {
        line_sum += cc_inv(k, k2);
      }
      ww(0, k) = line_sum;
      cc_inv_sum += line_sum;
    }
//    for (Index k = 0; k < neighborhood_size; ++k) {
      gsl_matrix_scale(ww.m_, 1.0 / cc_inv_sum);
//    }

    // Apply ww to reconstructed matrix
    for (Index b = 0; b < input.cols(); ++b) {
      Scalar current_value = 0;
      for (Index k = 0; k < neighborhood_size; ++k) {
        current_value += ww(0, k) * input(encountered_backbone_samples[k], b);
      }
      (*result)(full_idx, b) = current_value;
    }

  }

  return result;

}

HSISOMAP_NAMESPACE_END


