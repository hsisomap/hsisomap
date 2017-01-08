//***************************************************************************************
//
//! \file AdjacencyList.h
//!  A simple implementation of adjacency list for undirected weighted graph with
//! OpenCL C graph array representation supported.
//!
//! \author    Can
//! \version   1.0
//! \date      2015-03-23
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef DIJKSTRACL_UWGADJACENCYLIST_H
#define DIJKSTRACL_UWGADJACENCYLIST_H

#include <vector>
#include "UndirectedWeightedGraph.h"

namespace GraphUtils {

//! A compact array based graph representation for OpenCL parallel implementation.
//!
//! The graph array struct is a compact representation of the graph.
//! It can be efficiently transferred between OpenCL devices and accessed by C code, and takes less space.
//! It is however more difficult to construct. Therefore, the adjacency list is usually used to efficiently construct a large scale graph, and then
//! convert to graph array representation after the construction is finished. This customized adjacency list representation is more efficient than the Boost Graph Library representation.
//! \tparam T_Index the index type of the graph array. For example, it is cl_Index for OpenCL usages.
//! \tparam T_Scalar the scalar type of the graph array. For example, it is cl_Scalar for OpenCL usages.
template <typename T_Index, typename T_Scalar>
struct GraphArray {
  std::vector<T_Index> vertices; //!< Compact vertex array.
  std::vector<T_Index> edges; //!< Compact edge array.
  std::vector<T_Scalar> weights; //!< Weights of the edges corresponding to the edge array.

  //! Constructor.
  //! Construct an empty graph.
  GraphArray() { }

  //! Copy constructor.
  //! \param g the graph array struct that needs to be copied.
  GraphArray(const GraphArray& g) : vertices(g.vertices), edges(g.edges), weights(g.weights) { }
};

//! Customized implemented essential Adjancency List based graph representation.
class AdjacencyList : public UndirectedWeightedGraph {
 public:

  //! Constructor.
  //! \param numVertices the number of vertices in the graph.
  AdjacencyList(Index numVertices);

  //! Copy constructor.
  //! \param other the adjacency list to be copied.
  AdjacencyList(const AdjacencyList& other);

  //! Destructor.
  ~AdjacencyList();

  //! Connect two vertices in the graph using a weighted edge.
  //! The graph is not connected at the beginning, and each edge needs to be connected by this method.
  //! \param a the index of the source vertex of the edge.
  //! \param b the index of the target vertex of the edge. For undirected graph, a and b can be exchanged.
  //! \param weight the weight of the edge.
  void Connect(Index a, Index b, Scalar weight);

  //! [TO BE SUPPORTED] Get the weight between two vertices.
  //! \param a the source vertex of the edge.
  //! \param b the target vertex of the edge. For undirected graph, a and b can be exchanged.
  //! \return the weight of the edge.
  Scalar GetWeight(Index a, Index b) const;

  //! Get the number of vertices of the graph.
  //! \return the number of vertices of the graph.
  Index NumVertices() const;

  //! Get the number of edges of the graph.
  //! \return the number of edges of the graph.
  Index NumEdges() const;

  //! Construct and obtained graph array struct representation of the graph from the adjacency list representation.
  //! \tparam T_Index the index type of the graph array. For example, it is cl_Index for OpenCL usages.
  //! \tparam T_Scalar the scalar type of the graph array. For example, it is cl_Scalar for OpenCL usages.
  //! \return the pointer to the graph array struct.
  template <typename T_Index, typename T_Scalar>
  std::shared_ptr<GraphArray<T_Index, T_Scalar>> GetGraphArray() const;

  //! Equal operator.
  //! \param other the other adjacency list to be compared.
  //! \return whether they are the same.
  const AdjacencyList& operator=(const AdjacencyList& other);
 private:
  struct ListNode
  {
    Index idx;
    Scalar weight;
    ListNode * next;
    ListNode(Index idx, Scalar weight, ListNode * next = NULL) : idx(idx), weight(weight), next(next) { }
  };
  ListNode ** list_;
  Index numVertices_;
  Index numEdges_;
  ListNode * ConnectNode(ListNode *node, Index d, Scalar weight);
  template <typename T_Index, typename T_Scalar>
  Index Traverse(ListNode * node, Index base, Index depth, GraphArray<T_Index, T_Scalar>& graph) const;
  void RemoveNode(ListNode * node);
  void CopyFrom(const AdjacencyList&);
  ListNode * CopyNode(ListNode *dest, ListNode *src);
};

} // namespace GraphUtils

#endif //DIJKSTRACL_UWGADJACENCYLIST_H
