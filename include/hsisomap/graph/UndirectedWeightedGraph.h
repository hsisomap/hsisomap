//***************************************************************************************
//
//! \file UndirectedWeightedGraph.h
//!  An abstract class for a collection of undirected weighted graph graph representation classes for different needs.
//!
//! \author    Can
//! \version   1.0
//! \date      2015-03-10
//! \copyright GNU Public License V3.0
//
//***************************************************************************************


#ifndef DIJKSTRACL_UWGRAPH_H
#define DIJKSTRACL_UWGRAPH_H

#include <stddef.h>
#include "../typedefs.h"

//! A collection of graph representation classes for different needs.
namespace GraphUtils {

//! Abstract class for undirected weighted graph classes with different representations.
class UndirectedWeightedGraph {
 public:

  //! Connect two vertices in the graph using a weighted edge.
  //! The graph is not connected at the beginning, and each edge needs to be connected by this method.
  //! \param a the index of the source vertex of the edge.
  //! \param b the index of the target vertex of the edge. For undirected graph, a and b can be exchanged.
  //! \param weight the weight of the edge.
  virtual void Connect(Index a, Index b, Scalar weight) = 0;

  //! [TO BE SUPPORTED] Get the weight between two vertices.
  //! \param a the source vertex of the edge.
  //! \param b the target vertex of the edge. For undirected graph, a and b can be exchanged.
  //! \return the weight of the edge.
//  virtual Scalar GetWeight(Index a, Index b) const = 0;

  //! Get the number of vertices of the graph.
  //! \return the number of vertices of the graph.
  virtual Index NumVertices() const = 0;

  //! Get the number of edges of the graph.
  //! \return the number of edges of the graph.
  virtual Index NumEdges() const = 0;
};

}

#endif //DIJKSTRACL_UWGRAPH_H
