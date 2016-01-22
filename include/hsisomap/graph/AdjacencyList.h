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

template <typename T_Index, typename T_Scalar>
struct GraphArray {
  std::vector<T_Index> vertices;
  std::vector<T_Index> edges;
  std::vector<T_Scalar> weights;
  GraphArray() { }
  GraphArray(const GraphArray& g) : vertices(g.vertices), edges(g.edges), weights(g.weights) { }
};

class AdjacencyList : public UndirectedWeightedGraph {
 public:
  AdjacencyList(Index numVertices);
  AdjacencyList(const AdjacencyList& other);
  ~AdjacencyList();
  void Connect(Index a, Index b, Scalar weight);
  Scalar GetWeight(Index a, Index b) const;
  Index NumVertices() const;
  Index NumEdges() const;
  template <typename T_Index, typename T_Scalar>
  std::shared_ptr<GraphArray<T_Index, T_Scalar>> GetGraphArray() const;
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
