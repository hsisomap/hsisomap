//
// Created by Can on 3/10/15.
//

#ifndef DIJKSTRACL_UWGRAPH_H
#define DIJKSTRACL_UWGRAPH_H

#include <stddef.h>
#include "../typedefs.h"

namespace GraphUtils {

class UndirectedWeightedGraph {
 public:
  virtual void Connect(Index a, Index b, Scalar weight) = 0;
//  virtual Scalar GetWeight(Index a, Index b) const = 0;
  virtual Index NumVertices() const = 0;
  virtual Index NumEdges() const = 0;
};

}

#endif //DIJKSTRACL_UWGRAPH_H
