//
// Created by Can on 9/18/15.
//

#ifndef DIJKSTRACL_BOOSTDIJKSTRA_H
#define DIJKSTRACL_BOOSTDIJKSTRA_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "../BoostAdjacencyList.h"
#include "../../Matrix.h"
#include "Dijkstra.h"

namespace Dijkstra {

//class Dijkstra;
class BoostDijkstra : public Dijkstra {
 public:
  BoostDijkstra(std::shared_ptr<GraphUtils::BoostAdjacencyList> graph);
  int Run();
  std::shared_ptr<gsl::Matrix> GetDistanceMatrix();
 private:
  std::shared_ptr<GraphUtils::BoostAdjacencyList::BoostGraph> graph_;
  Index numVertices_; //!< number of vertices of the graph, calculated from graph_.
  std::vector<Index> sourceVertices_; //!< the source vertices list from which the shortest distances to all vertices are calculated.
  std::shared_ptr<gsl::Matrix> distanceMatrix_;
};

}

#endif //DIJKSTRACL_BOOSTDIJKSTRA_H
