//
// Created by Can on 1/18/16.
//

#include <hsisomap/subsetter/SubsetterEmbedding.h>
#include <hsisomap/Logger.h>
#include <hsisomap/gsl_util/embedding.h>

HSISOMAP_NAMESPACE_BEGIN

SubsetterEmbedding::SubsetterEmbedding(std::shared_ptr<HsiData> hsi_data, PropertyList property_list, std::shared_ptr<gsl::Matrix> optional_embedding) : hsi_data_(hsi_data), property_list_(property_list), embedding_(optional_embedding) {

  std::function <gsl::Embedding(std::shared_ptr<gsl::Matrix>)> EmbeddingFunction;
  if (property_list[SUBSETTER_DEFAULT_EMBEDDING] == SUBSETTER_DEFAULT_EMBEDDING_PCA) {
    EmbeddingFunction = std::bind(gsl::PCA, std::placeholders::_1, 1);
  } else {
    throw std::invalid_argument("Embedding function not supported.");
  }

  if (embedding_.get() == nullptr) {
    embedding_ = EmbeddingFunction(hsi_data_->data()).space;
  }

}

HSISOMAP_NAMESPACE_END

