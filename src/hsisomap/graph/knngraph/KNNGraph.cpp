//
// Created by Can on 1/22/16.
//

#include <hsisomap/graph/knngraph/KNNGraph.h>
#include <hsisomap/graph/knngraph/KNNGraph_FixedK_MST.h>

HSISOMAP_NAMESPACE_BEGIN

std::shared_ptr<KNNGraph> KNNGraphWithImplementation(KNNGraphImplementation knngraph_implementation,
                                                     std::shared_ptr<gsl::Matrix> data,
                                                     PropertyList property_list) {

  switch (knngraph_implementation) {
    case KNNGRAPH_IMPLEMENTATION_FIXED_K:
      throw std::invalid_argument("Subsetter Implementation RandomSkel not supported yet.");
    case KNNGRAPH_IMPLEMENTATION_FIXED_K_WITH_MST:
      return std::dynamic_pointer_cast<KNNGraph>(std::make_shared<KNNGraph_FixedK_MST>(data, property_list));
  }
}

HSISOMAP_NAMESPACE_END