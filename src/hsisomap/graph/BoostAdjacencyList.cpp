//
// Created by Can on 9/18/15.
//

#include <hsisomap/graph/BoostAdjacencyList.h>

namespace GraphUtils {

BoostAdjacencyList::BoostAdjacencyList(Index numVertices) : numVertices_(numVertices) { }

void BoostAdjacencyList::Connect(Index a, Index b, Scalar weight) {
  edges_.push_back(Edge(a, b));
  weights_.push_back(weight);
}

std::shared_ptr<BoostAdjacencyList::BoostGraph> BoostAdjacencyList::GetBoostGraph() {
  BoostGraphData graph(&edges_[0], &edges_[0] + edges_.size(), &weights_[0], numVertices_);
  IndexMap index = get(boost::vertex_index, graph);
  std::shared_ptr<BoostGraph> boostGraph(new BoostGraph());
  boostGraph->numVertices = numVertices_;
  boostGraph->graph = graph;
  boostGraph->index = index;
  return boostGraph;
}

Index BoostAdjacencyList::NumVertices() const {
  return numVertices_;
}

Index BoostAdjacencyList::NumEdges() const {
  return edges_.size();
}

}