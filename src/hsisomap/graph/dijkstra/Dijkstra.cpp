//
// Created by Can on 9/18/15.
//

#include <hsisomap/graph/dijkstra/Dijkstra.h>
#include <hsisomap/graph/AdjacencyList.h>
#include <hsisomap/graph/BoostAdjacencyList.h>
#include <hsisomap/graph/dijkstra/DijkstraCL.h>
#include <hsisomap/graph/dijkstra/BoostDijkstra.h>

namespace Dijkstra {


std::shared_ptr<Dijkstra> DijkstraWithImplementation(DijkstraImplementations dijkstraImplementations,
                                                     std::shared_ptr<::GraphUtils::UndirectedWeightedGraph> graph) {
  switch (dijkstraImplementations) {
    case DIJKSTRA_IMPLEMENTATION_CL:
      if (auto p = std::dynamic_pointer_cast<::GraphUtils::AdjacencyList>(graph)) {
        return std::dynamic_pointer_cast<Dijkstra>(std::make_shared<DijkstraCL>(p));
      } else {
        throw std::invalid_argument("DIJKSTRA_IMPLEMENTATION_CL only accepts GraphUtils::AdjacencyList.");
      }
    case DIJKSTRA_IMPLEMENTATION_BOOST:
      if (auto p = std::dynamic_pointer_cast<::GraphUtils::BoostAdjacencyList>(graph)) {
        return std::dynamic_pointer_cast<Dijkstra>(std::make_shared<BoostDijkstra>(p));
      } else {
        throw std::invalid_argument("DIJKSTRA_IMPLEMENTATION_BOOST only accepts GraphUtils::BoostAdjacencyList.");
      }
  }
}

} // namespace Dijkstra