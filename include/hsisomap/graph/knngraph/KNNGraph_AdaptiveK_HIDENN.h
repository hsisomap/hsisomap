//
// Created by Can on 4/4/16.
//

#ifndef HSISOMAP_KNNGRAPH_ADAPTIVEK_HIDENN_H
#define HSISOMAP_KNNGRAPH_ADAPTIVEK_HIDENN_H

#include "KNNGraph.h"

HSISOMAP_NAMESPACE_BEGIN

Key KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER = "KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER";
Key KNNGRAPH_ADAPTIVE_K_WITH_MST_EDGE_POOL_DEPTH = "KNNGRAPH_ADAPTIVE_K_WITH_MST_EDGE_POOL_DEPTH";

class KNNGraph_AdaptiveK_HIDENN: public KNNGraph {
 public:
  KNNGraph_AdaptiveK_HIDENN(std::shared_ptr<gsl::Matrix> data, PropertyList property_list);
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph() { return knngraph_; }
 private:
  std::shared_ptr<gsl::Matrix> data_;
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph_;
  PropertyList property_list_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_KNNGRAPH_ADAPTIVEK_HIDENN_H
