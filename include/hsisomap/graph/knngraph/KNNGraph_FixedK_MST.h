//***************************************************************************************
//
//! \file KNNGraph_FixedK_MST.h
//!  kNN graph construction with fixed k and minimum spanning tree (MST) augmented to ensure graph connectivity.
//!
//! \author    Can
//! \version   1.0
//! \date      2016-01-22
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef HSISOMAP_KNNGRAPH_FIXEDK_MST_H
#define HSISOMAP_KNNGRAPH_FIXEDK_MST_H

#include "KNNGraph.h"

HSISOMAP_NAMESPACE_BEGIN

Key KNNGRAPH_FIXED_K_NUMBER = "KNNGRAPH_FIXED_K_NUMBER"; //!< Property list key, specify k value (neighborhood size).
Key KNNGRAPH_FIXED_K_WITH_MST_EDGE_POOL_DEPTH = "KNNGRAPH_FIXED_K_WITH_MST_EDGE_POOL_DEPTH"; //!< (Optional) Property list key, specify the MST edge pool depth. It is a trade-off between space and time.

//! kNN graph construction with fixed k and minimum spanning tree (MST) augmented to ensure graph connectivity.
class KNNGraph_FixedK_MST: public KNNGraph {
 public:

  //! Constructor.
  //! \param data the pairwise distance matrix. It needs to be symmetric.
  //! \param property_list the property list contains the options needed to construct the graph.
  KNNGraph_FixedK_MST(
      std::shared_ptr<gsl::Matrix> data,
      PropertyList property_list);

  //! Get the constructed kNN graph.
  //! \return the constructed kNN graph. It is a smart pointer to an GraphUtils::UndirectedWeightedGraph. The underlying implementation of the graph is specified in KNNGraphWithImplementation.
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph() { return knngraph_; }
 private:
  std::shared_ptr<gsl::Matrix> data_;
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph_;
  PropertyList property_list_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_KNNGRAPH_FIXEDK_MST_H
