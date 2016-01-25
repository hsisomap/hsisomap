//
// Created by Can on 1/22/16.
//

#ifndef HSISOMAP_KNNGRAPH_H
#define HSISOMAP_KNNGRAPH_H

#include <vector>
#include <hsisomap/Matrix.h>
#include <hsisomap/graph/UndirectedWeightedGraph.h>
#include "../../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN

enum KNNGraphImplementation {
  KNNGRAPH_IMPLEMENTATION_FIXED_K = 0,
  KNNGRAPH_IMPLEMENTATION_FIXED_K_WITH_MST = 1
};


Key KNNGRAPH_GRAPH_BACKEND = "KNNGRAPH_GRAPH_BACKEND";
kScalar KNNGRAPH_GRAPH_BACKEND_ADJACENCYLIST = 0.0;
kScalar KNNGRAPH_GRAPH_BACKEND_BOOST = 1.0;


class KNNGraph {
 public:
  virtual std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph() = 0;
};

std::shared_ptr<KNNGraph> KNNGraphWithImplementation(KNNGraphImplementation knngraph_implementation,
                                                       std::shared_ptr<gsl::Matrix> data,
                                                       PropertyList property_list);


HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_KNNGRAPH_H
