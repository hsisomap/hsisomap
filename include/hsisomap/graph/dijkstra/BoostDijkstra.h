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

//! Boost Graph Library implementation of Dijkstra algorithm.
//! The implementation calculates the distances from the specified source vertices (in sequence) to all vertices on the graph using Dijkstra algorithm.
class BoostDijkstra : public Dijkstra {
 public:

  //! Constructor.
  //! \param graph the graph to be calculated represented by GraphUtils::BoostAdjacencyList.
  BoostDijkstra(std::shared_ptr<GraphUtils::BoostAdjacencyList> graph);

  //! Set the source vertices list.
  //! \param sourceVertices the vector of source indices.
  void SetSourceVertices(std::vector<Index> sourceVertices);

  //! Run the Dijkstra algorithm.
  //! \return error code. 0 if succeeded.
  int Run();

  //! Get the distance matrix from the Dijkstra algorithm.
  //! \return the calculated distance matrix as gsl::Matrix. The rows represent the source vertices, and the columns represent destination vertices.
  std::shared_ptr<gsl::Matrix> GetDistanceMatrix();
 private:
  std::shared_ptr<GraphUtils::BoostAdjacencyList::BoostGraph> graph_; //!< the pointer to the input graph.
  Index numVertices_; //!< number of vertices of the graph, calculated from graph_.
  std::vector<Index> sourceVertices_; //!< the source vertices list from which the shortest distances to all vertices are calculated.
  std::shared_ptr<gsl::Matrix> distanceMatrix_; //!< the pointer to the resulted distance matrix.
};

}

#endif //DIJKSTRACL_BOOSTDIJKSTRA_H
