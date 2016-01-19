//
// Created by Can on 1/18/16.
//

#ifndef HSISOMAP_EMBEDDING_H
#define HSISOMAP_EMBEDDING_H

#include "../Matrix.h"

namespace gsl {

struct Embedding {
  std::shared_ptr<Matrix> space;
  std::shared_ptr<Matrix> vectors;
  std::shared_ptr<Matrix> values;
};

Embedding PCA(std::shared_ptr<Matrix> data, Index reduced_dimensions = -1);


} // namespace gsl

#endif //HSISOMAP_EMBEDDING_H
