//
// Created by Can on 1/25/16.
//

#ifndef HSISOMAP_LANDMARK_H
#define HSISOMAP_LANDMARK_H

#include <hsisomap/Matrix.h>
#include "../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN

enum LandmarkImplementation {
  LANDMARK_IMPLEMENTATION_LIST = 0,
  LANDMARK_IMPLEMENTATION_SUBSETS = 1,
  LANDMARK_IMPLEMENTATION_GLOBAL_RANDOMSKEL = 2
};


Key LANDMARK_COUNT = "LANDMARK_COUNT";

class Landmark {
 public:
  virtual std::vector<Index> landmarks() = 0;
  virtual std::shared_ptr<gsl::Matrix> landmark_data() = 0;
};

std::shared_ptr<Landmark> LandmarkWithImplementation(LandmarkImplementation landmark_implementation,
                                                     std::shared_ptr<gsl::Matrix> data,
                                                     PropertyList property_list,
                                                     std::shared_ptr<std::vector<Index>>
                                                     optional_landmark_list = nullptr);

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_LANDMARK_H
