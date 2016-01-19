//
// Created by Can on 1/18/16.
//

#ifndef HSISOMAP_SUBSETTEREMBEDDING_H
#define HSISOMAP_SUBSETTEREMBEDDING_H

#include "Subsetter.h"

HSISOMAP_NAMESPACE_BEGIN

class SubsetterEmbedding : public Subsetter {
 public:
  SubsetterEmbedding(std::shared_ptr<HsiData> hsi_data, PropertyList property_list, std::shared_ptr<gsl::Matrix> optional_embedding);
  std::vector<std::vector<Index>> subsets() { return subsets_; }
  std::shared_ptr<HsiData> hsi_data() { return hsi_data_; }
 private:
  std::shared_ptr<HsiData> hsi_data_;
  PropertyList property_list_;
  std::shared_ptr<gsl::Matrix> embedding_;
  std::vector<std::vector<Index>> subsets_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_SUBSETTEREMBEDDING_H
