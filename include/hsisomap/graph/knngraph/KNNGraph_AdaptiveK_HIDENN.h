//***************************************************************************************
//
//! \file KNNGraph_AdaptiveK_HIDENN.h
//!  kNN graph construction with adaptive k method based on HIDENN.
//!
//! The method is based on Hyperspectral Intrinsic Dimensionality Estimation with Nearest Neighbor Ratio (HIDENN) and Costa & Hero intrinsic dimensionality estimation.
//!
//! The result is randomized because of the random sampling part of the Costa & Hero method.
//!
//! The graph is also augmented by minimum spanning tree (MST) to ensure connectivity.
//!
//! Please see reference for details.
//!
//! \author    Can
//! \version   1.0
//! \date      2016-04-04
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef HSISOMAP_KNNGRAPH_ADAPTIVEK_HIDENN_H
#define HSISOMAP_KNNGRAPH_ADAPTIVEK_HIDENN_H

#include "KNNGraph.h"

HSISOMAP_NAMESPACE_BEGIN

Key KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER = "KNNGRAPH_ADAPTIVE_K_HIDENN_SUBSET_NUMBER"; //!< Property list key, specify the number of subsets that represents local spectral region that has the local adaptive k.
Key KNNGRAPH_ADAPTIVE_K_WITH_MST_EDGE_POOL_DEPTH = "KNNGRAPH_ADAPTIVE_K_WITH_MST_EDGE_POOL_DEPTH"; //!< (Optional) Property list key, specify the MST edge pool depth. It is a trade-off between space and time.

//! kNN graph construction with adaptive k method based on HIDENN.
//!
//! The method is based on Hyperspectral Intrinsic Dimensionality Estimation with Nearest Neighbor Ratio (HIDENN) and Costa & Hero intrinsic dimensionality estimation.
//!
//! The result is randomized because of the random sampling part of the Costa & Hero method.
//!
//! The graph is also augmented by minimum spanning tree (MST) to ensure connectivity.
//!
//! Please see reference for details.
class KNNGraph_AdaptiveK_HIDENN: public KNNGraph {
 public:

  //! Constructor.
  //! \param data the pairwise distance matrix. It needs to be symmetric.
  //! \param property_list the property list contains the options needed to construct the graph.
  KNNGraph_AdaptiveK_HIDENN(std::shared_ptr<gsl::Matrix> data, PropertyList property_list);

  //! Get the constructed kNN graph.
  //! \return the constructed kNN graph. It is a smart pointer to an GraphUtils::UndirectedWeightedGraph. The underlying implementation of the graph is specified in KNNGraphWithImplementation.
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph() { return knngraph_; }
 private:
  std::shared_ptr<gsl::Matrix> data_;
  std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph_;
  PropertyList property_list_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_KNNGRAPH_ADAPTIVEK_HIDENN_H
