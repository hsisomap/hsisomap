//
// Created by Can on 1/22/16.
//

#ifndef HSISOMAP_KNNGRAPH_FIXEDK_MST_H
#define HSISOMAP_KNNGRAPH_FIXEDK_MST_H

#include "KNNGraph.h"

HSISOMAP_NAMESPACE_BEGIN


class KNNGraph_FixedK_MST: public KNNGraph {
 public:
  KNNGraph_FixedK_MST(
      std::shared_ptr<gsl::Matrix> data,
      PropertyList property_list);
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph() { return knngraph_; }
 private:
  std::shared_ptr<gsl::Matrix> data_;
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_KNNGRAPH_FIXEDK_MST_H
