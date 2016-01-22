//
// Created by Can on 1/19/16.
//

#ifndef HSISOMAP_BACKBONE_H
#define HSISOMAP_BACKBONE_H

#include <hsisomap/HsiData.h>
#include <unordered_set>
#include "../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN

Key BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY = "BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_STRATEGY";
kScalar BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED = 0;
kScalar BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_ADAPTIVE = 1;
Key BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER = "BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_FIXED_NUMBER";
Key BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_ADAPTIVE_NUMBER_LOWER_LIMIT =
    "BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_ADAPTIVE_NUMBER_LOWER_LIMIT";
Key BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_ADAPTIVE_NUMBER_UPPER_LIMIT =
    "BACKBONE_RECONSTRUCTION_NEIGHBORHOOD_ADAPTIVE_NUMBER_UPPER_LIMIT";

Key BACKBONE_NNCACHE_PRIMARY_SEARCH_RANGE = "BACKBONE_NNCACHE_PRIMARY_SEARCH_RANGE";
Key BACKBONE_NNCACHE_SECONDARY_SEARCH_RANGE = "BACKBONE_NNCACHE_SECONDARY_SEARCH_RANGE";


class Backbone {
 public:
  Backbone(const std::shared_ptr<const gsl::Matrix> data, const std::vector<Index> &sampling_indices);
  std::shared_ptr<gsl::Matrix> sampled_data() { return sampled_data_; }
  const std::vector<Index>& sampling_indices() const { return sampling_indices_; }
  void PrepareNNCache(Index neighborhood_size, PropertyList optional_settings = PropertyList());
  std::shared_ptr<gsl::Matrix> Reconstruct(const gsl::Matrix &input,
                                           PropertyList reconstruction_strategy,
                                           std::shared_ptr<gsl::Matrix> optional_cache = nullptr);
  std::shared_ptr<gsl::Matrix> nn_cache() { return nn_cache_; }
 private:
  std::shared_ptr<gsl::Matrix> nn_cache_;
  std::vector<Index> sampling_indices_;
  std::unordered_set<Index> sampling_indices_set_;
  std::unordered_map<Index, Index> sampling_indices_reverse_table_;
  const std::shared_ptr<const gsl::Matrix> data_;
  std::shared_ptr<gsl::Matrix> sampled_data_;
};


HSISOMAP_NAMESPACE_END


#endif //HSISOMAP_BACKBONE_H
