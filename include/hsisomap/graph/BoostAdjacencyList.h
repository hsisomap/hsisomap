//***************************************************************************************
//
//! \file BoostAdjacencyList.h
//!  Boost Graph Library Adjancency List based graph representation wrapper class.
//!
//! \author    Can
//! \version   1.0
//! \date      2015-09-18
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef DIJKSTRACL_BOOSTADJACENCYLIST_H
#define DIJKSTRACL_BOOSTADJACENCYLIST_H

#include "UndirectedWeightedGraph.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace GraphUtils {

//! Boost Graph Library Adjancency List based graph representation wrapper class.
class BoostAdjacencyList : public UndirectedWeightedGraph {
 public:

  //! Data type to store graph information.
  typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, Scalar>>
      BoostGraphData;

  //! Data type to represent a vertex.
  typedef boost::graph_traits<BoostGraphData>::vertex_descriptor Vertex;

  //! Data type to represent an edge.
  typedef std::pair<int, int> Edge;

  //! Data type to represent an index map.
  typedef boost::property_map<BoostGraphData, boost::vertex_index_t>::type IndexMap;

  //! Struct to represent all the information of a graph.
  struct BoostGraph {
    BoostGraphData graph;
    size_t numVertices;
    IndexMap index;
  };

  //! Constructor.
  //! \param numVertices the number of vertices.
  BoostAdjacencyList(Index numVertices);

  //! Obtain the pointer to the boost graph struct.
  //! \return the pointer to the boost graph struct.
  std::shared_ptr<BoostGraph> GetBoostGraph();

  //! Connect two vertices in the graph using a weighted edge.
  //! The graph is not connected at the beginning, and each edge needs to be connected by this method.
  //! \param a the index of the source vertex of the edge.
  //! \param b the index of the target vertex of the edge. For undirected graph, a and b can be exchanged.
  //! \param weight the weight of the edge.
  void Connect(Index a, Index b, Scalar weight);

  //! Get the number of vertices of the graph.
  //! \return the number of vertices of the graph.
  Index NumVertices() const;

  //! Get the number of edges of the graph.
  //! \return the number of edges of the graph.
  Index NumEdges() const;
 private:
  Index numVertices_;
  std::vector<Edge> edges_;
  std::vector<Scalar> weights_;

};

} // namespace GraphUtils

#endif //DIJKSTRACL_BOOSTADJACENCYLIST_H
