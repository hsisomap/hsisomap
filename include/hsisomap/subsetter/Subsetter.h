//
// Created by Can on 1/18/16.
//

#ifndef HSISOMAP_SUBSETTER_H
#define HSISOMAP_SUBSETTER_H

#include <vector>
#include <hsisomap/HsiData.h>
#include "../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN

enum SubsetterImplementation {
  SUBSETTER_IMPLEMENTATION_RANDOMSKEL = 0,
  SUBSETTER_IMPLEMENTATION_EMBEDDING = 1
};

Key SUBSETTER_SUBSETS = "SUBSETTER_SUBSETS";
Key SUBSETTER_DEFAULT_EMBEDDING = "SUBSETTER_DEFAULT_EMBEDDING";
kScalar SUBSETTER_DEFAULT_EMBEDDING_PCA = 0.0;
Key SUBSETTER_EMBEDDING_SLICING_MODE = "SUBSETTER_DEFAULT_EMBEDDING_SLICING_MODE";
kScalar SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEDIAN = 0.0;
kScalar SUBSETTER_EMBEDDING_SLICING_MODE_FIRST_MEAN = 1.0;

class Subsetter {
 public:
  virtual std::vector<std::vector<Index>> subsets() = 0;
  virtual std::shared_ptr<gsl::Matrix> data() = 0;
};

std::shared_ptr<Subsetter> SubsetterWithImplementation(SubsetterImplementation subsetter_implementation,
                                                       std::shared_ptr<gsl::Matrix> data,
                                                       PropertyList property_list,
                                                       std::shared_ptr<gsl::Matrix> optional_embedding = nullptr);

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_SUBSETTER_H
