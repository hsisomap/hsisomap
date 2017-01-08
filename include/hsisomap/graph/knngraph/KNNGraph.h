//***************************************************************************************
//
//! \file KNNGraph.h
//!  A collections of implementations of Dijkstra multi-source all-path shortest path on graph algorithm.
//!
//! \author    Can
//! \version   1.0
//! \date      2016-01-22
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef HSISOMAP_KNNGRAPH_H
#define HSISOMAP_KNNGRAPH_H

#include <vector>
#include <hsisomap/Matrix.h>
#include <hsisomap/graph/UndirectedWeightedGraph.h>
#include "../../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN

//! Enum of different kNN graph implementation.
enum KNNGraphImplementation {
  KNNGRAPH_IMPLEMENTATION_FIXED_K = 0, //!< kNN graph with fixed k.
  KNNGRAPH_IMPLEMENTATION_FIXED_K_WITH_MST = 1, //!< kNN graph with fixed k augmented with minimum spanning tree (MST) to ensure connectivity.
  KNNGRAPH_IMPLEMENTATION_ADAPTIVE_K_HIDENN = 2 //!< kNN graph with adaptive k from HIDENN method, also MST augmented.
};


Key KNNGRAPH_GRAPH_BACKEND = "KNNGRAPH_GRAPH_BACKEND"; //!< kNN graph backend key for the property list.
kScalar KNNGRAPH_GRAPH_BACKEND_ADJACENCYLIST = 0.0; //!< kNN graph backend value for the property list, to generate adjacency list graph representation.
kScalar KNNGRAPH_GRAPH_BACKEND_BOOST = 1.0; //!< kNN graph backend value for the property list, to Boost Graph Library based graph representation.

//! Abstract class to manage different implementations of kNN graph.
class KNNGraph {
 public:

  //! Get the constructed kNN graph.
  //! \return the constructed kNN graph. It is a smart pointer to an GraphUtils::UndirectedWeightedGraph. The underlying implementation of the graph is specified in KNNGraphWithImplementation.
  virtual std::shared_ptr<GraphUtils::UndirectedWeightedGraph> knngraph() = 0;
};

//! Return the implementation class and construct the kNN graph with the specified implementation type.
//!
//! Note that the graph construction usually happens in the constructor. Therefore this function also essentially performs the graph construction.
//! \param knngraph_implementation the kNN graph construction method with one of KNNGraphImplementation.
//! \param data the data matrix (the pairwise distance matrix).
//! \param property_list specify the property list used by a certain method of kNN graph. The key and value pairs could be different for different method.
//! \return smart pointer to the implementation class of the kNN graph with the specified implementation type.
std::shared_ptr<KNNGraph> KNNGraphWithImplementation(KNNGraphImplementation knngraph_implementation,
                                                       std::shared_ptr<gsl::Matrix> data,
                                                       PropertyList property_list);


HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_KNNGRAPH_H
