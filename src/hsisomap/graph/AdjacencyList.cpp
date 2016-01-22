//
// Created by Can on 3/10/15.
//

#include <hsisomap/graph/AdjacencyList.h>

namespace GraphUtils {

AdjacencyList::AdjacencyList(Index numVertices) : numVertices_(numVertices), numEdges_(0) {
  list_ = new ListNode * [numVertices];
  for (Index i = 0; i < numVertices; ++i) {
    list_[i] = NULL;
  }
}

AdjacencyList::~AdjacencyList() {
  for (Index i = 0; i < numVertices_; ++i) {
    RemoveNode(list_[i]);
  }
  delete [] list_;
}

void AdjacencyList::Connect(Index a, Index b, Scalar weight) {
  if (a == b) return;
  if (a > b) std::swap(a, b);
  list_[a] = ConnectNode(list_[a], b, weight);
  list_[b] = ConnectNode(list_[b], a, weight);
}

typename AdjacencyList::ListNode *AdjacencyList::ConnectNode(AdjacencyList::ListNode *node, Index b, Scalar weight) {
  if (node == NULL) { // insert as the last node in the list
    ListNode * newNode = new ListNode(b, weight);
    numEdges_++;
    return newNode;
  }
  if (node->idx == b) { // update the existing weight
    node->weight = weight;
    return node;
  }
  if (node->idx > b) { // insert in the list
    ListNode * newNode = new ListNode(b, weight, node);
    numEdges_++;
    return newNode;
  }
  node->next = ConnectNode(node->next, b, weight);
  return node;
}

Scalar AdjacencyList::GetWeight(Index a, Index b) const {
  throw std::invalid_argument("Not implemented");
  return 0;
}

Index AdjacencyList::NumVertices() const {
  return numVertices_;
}

Index AdjacencyList::NumEdges() const {
  return numEdges_;
}

template <typename T_Index, typename T_Scalar>
std::shared_ptr<GraphArray<T_Index, T_Scalar>> AdjacencyList::GetGraphArray() const {
  auto graph = std::make_shared<GraphArray<T_Index, T_Scalar>>();
  graph->vertices.resize(numVertices_);
  graph->edges.resize(numEdges_);
  graph->weights.resize(numEdges_, std::numeric_limits<T_Scalar>::max());
  T_Index base = 0;
  for (T_Index i = 0; i < numVertices_; ++i) {
    graph->vertices[i] = base;
    base += Traverse(list_[i], base, 0, *graph);
  }
  return std::make_shared<GraphArray<T_Index, T_Scalar>>(*graph);
}

template <typename T_Index, typename T_Scalar>
Index AdjacencyList::Traverse(AdjacencyList::ListNode *node, Index base, Index depth, GraphArray<T_Index, T_Scalar>& graph) const {
  if (!node) return depth;
  graph.edges[base + depth] = node->idx;
  graph.weights[base + depth] = node->weight;
  return Traverse(node->next, base, depth + 1, graph);
}

void AdjacencyList::RemoveNode(AdjacencyList::ListNode *node) {
  if (!node) return;
  RemoveNode(node->next);
  delete node;
}

void AdjacencyList::CopyFrom(const AdjacencyList &adjacencyList) {
  if (this == &adjacencyList) return;
  for (Index i = 0; i < numVertices_; ++i) {
    RemoveNode(list_[i]);
  }
  delete [] list_;
  numVertices_ = adjacencyList.numVertices_;
  numEdges_ = adjacencyList.numEdges_;
  list_ = new ListNode * [numVertices_];
  for (Index i = 0; i < numVertices_; ++i) {
    list_[i] = CopyNode(list_[i], adjacencyList.list_[i]);
  }
}

typename AdjacencyList::ListNode * AdjacencyList::CopyNode(AdjacencyList::ListNode *dest, AdjacencyList::ListNode *src) {
  if (src == NULL) return NULL;
  dest = new ListNode(src->idx, src->weight);
  dest->next = CopyNode(dest->next, src->next);
  return dest;
}

AdjacencyList::AdjacencyList(const AdjacencyList &other) {
  CopyFrom(other);
}

const AdjacencyList &AdjacencyList::operator=(const AdjacencyList &other) {
  CopyFrom(other);
  return *this;
}


// explicit instantiations
template struct GraphArray<size_t, float>;
template struct GraphArray<size_t, double>;
template struct GraphArray<int, float>;
template struct GraphArray<int, double>;
template std::shared_ptr<GraphArray<size_t, float>> AdjacencyList::GetGraphArray<size_t, float>() const;
template std::shared_ptr<GraphArray<size_t, double>> AdjacencyList::GetGraphArray<size_t, double>() const;
template std::shared_ptr<GraphArray<int, float>> AdjacencyList::GetGraphArray<int, float>() const;
template std::shared_ptr<GraphArray<int, double>> AdjacencyList::GetGraphArray<int, double>() const;

} // namespace GraphUtils
