//
// Created by Can on 9/18/15.
//

#include <numeric>
#include <gsl/gsl_matrix_double.h>
#include <hsisomap/graph/dijkstra/BoostDijkstra.h>


namespace Dijkstra {

typedef GraphUtils::BoostAdjacencyList::BoostGraphData BoostGraphData;
typedef GraphUtils::BoostAdjacencyList::Vertex Vertex;

BoostDijkstra::BoostDijkstra(std::shared_ptr<GraphUtils::BoostAdjacencyList> adjList)
    : graph_(adjList->GetBoostGraph()),
      numVertices_(graph_->numVertices),
      sourceVertices_(graph_->numVertices, 0) {
  std::iota(sourceVertices_.begin(), sourceVertices_.end(), 0);
}

void BoostDijkstra::SetSourceVertices(std::vector<Index> sourceVertices) {
  sourceVertices_.resize(sourceVertices.size());
  std::copy(sourceVertices.begin(), sourceVertices.end(), sourceVertices_.begin());
}

int BoostDijkstra::Run() {
  distanceMatrix_ = std::make_shared<gsl::Matrix>(sourceVertices_.size(), graph_->numVertices);

  boost::graph_traits<BoostGraphData>::vertex_iterator vi;
  for (Index i = 0; i < sourceVertices_.size(); ++i) {
    Vertex source = boost::vertex(sourceVertices_[i], graph_->graph);
    boost::dijkstra_shortest_paths(graph_->graph, source, boost::distance_map(&distanceMatrix_->m_->data[i * numVertices_]));
  }
  return 0;
}

std::shared_ptr<gsl::Matrix> BoostDijkstra::GetDistanceMatrix() {
  return distanceMatrix_;
}

}
