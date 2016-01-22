//
// Created by Can on 9/18/15.
//

#ifndef DIJKSTRACL_DIJKSTRA_H
#define DIJKSTRACL_DIJKSTRA_H

#include <vector>
#include "../../typedefs.h"
#include "../../Matrix.h"
#include "../UndirectedWeightedGraph.h"


namespace Dijkstra {

enum DijkstraImplementations {
  DIJKSTRA_IMPLEMENTATION_CL = 0,
  DIJKSTRA_IMPLEMENTATION_BOOST = 1
};

class Dijkstra {
 public:
//  virtual void SetSourceVertices(std::vector<Index> sourceVertices) = 0;
  virtual int Run() = 0;
  virtual std::shared_ptr<gsl::Matrix> GetDistanceMatrix() = 0;
};

std::shared_ptr<Dijkstra> DijkstraWithImplementation(DijkstraImplementations dijkstraImplementations, std::shared_ptr<::GraphUtils::UndirectedWeightedGraph> graph);

} // namespace Dijkstra

#endif //DIJKSTRACL_DIJKSTRA_H
