//
// Created by Can on 9/18/15.
//

#ifndef DIJKSTRACL_DIJKSTRA_H
#define DIJKSTRACL_DIJKSTRA_H

#include <vector>
#include "../../typedefs.h"
#include "../../Matrix.h"
#include "../UndirectedWeightedGraph.h"

//! \brief A collections of implementations of Dijkstra multi-source all-path shortest path on graph algorithm.
//!
//! A collections of various implementations of Dijkstra multi-source all-path shortest path on graph algorithm.
//!
//! Currently the implementations include an OpenCL parallel accelerated implementation, and a standard implementation in Boost Graph Library.
namespace Dijkstra {

//! Enum to specify implementations of Dijkstra algorithm.
enum DijkstraImplementations {
  DIJKSTRA_IMPLEMENTATION_CL = 0, //!< OpenCL parallel accelerated implementation.
  DIJKSTRA_IMPLEMENTATION_BOOST = 1 //!< Boost Graph Library implementation.
};

//! Abstract class for various implementations of Dijkstra algorithm.
class Dijkstra {
 public:

  //! Set the indices of the source vertices.
  //! \param sourceVertices the vector of the indices of the source vertices.
  virtual void SetSourceVertices(std::vector<Index> sourceVertices) = 0;

  //! Run the Dijkstra algorithm.
  //! \return error code. 0 if succeeded.
  virtual int Run() = 0;

  //! Get the distance matrix from the Dijkstra algorithm.
  //! \return the calculated distance matrix as gsl::Matrix. The rows represent the source vertices, and the columns represent destination vertices.
  virtual std::shared_ptr<gsl::Matrix> GetDistanceMatrix() = 0;
};

//! Get an implementation object of Dijkstra algorithm with specified implementation.
//! \param dijkstraImplementations the implementation method of Dijkstra algorithm. It is one of the enum values of DijkstraImplementations.
//! \param graph graph representation to be calculated.
//! \return the implementation object that can be called to run the algorithm.
std::shared_ptr<Dijkstra> DijkstraWithImplementation(DijkstraImplementations dijkstraImplementations, std::shared_ptr<::GraphUtils::UndirectedWeightedGraph> graph);

} // namespace Dijkstra

#endif //DIJKSTRACL_DIJKSTRA_H
