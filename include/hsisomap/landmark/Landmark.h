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
Key LANDMARK_SUBSETS_NOISE_MODEL = "LANDMARK_SUBSETS_NOISE_MODEL";
kScalar LANDMARK_SUBSETS_NOISE_MODEL_MNF = 1.0;
kScalar LANDMARK_SUBSETS_NOISE_MODEL_ISOMAP = 2.0;
Key LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE = "LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_PERCENTAGE";
Key LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSIONS = "LANDMARK_SUBSETS_PRESELECTION_NOISE_EXCLUSION_NOISE_DIMENSIONS";
Key LANDMARK_SUBSETS_PRESELECTION_SUBSET_RANDOM_SKELETONIZATION = "LANDMARK_SUBSETS_PRESELECTION_SUBSET_RANDOM_SKELETONIZATION";
Key LANDMARK_SUBSETS_PRESELECTION_SUBSET_RANDOM_SKELETONIZATION_LIMIT_K = "LANDMARK_SUBSETS_PRESELECTION_SUBSET_RANDOM_SKELETONIZATION_LIMIT_K";

//Key LANDMARK_SUBSETS_COUNT = "LANDMARK_SUBSETS_COUNT"; // Discard - current implementation calculates this value automatically


class Landmark {
 public:
  virtual const std::vector<Index> &landmarks() = 0;
  virtual std::shared_ptr<gsl::Matrix> landmark_data() = 0;
};

std::shared_ptr<Landmark> LandmarkWithImplementation(LandmarkImplementation landmark_implementation,
                                                     std::shared_ptr<gsl::Matrix> data,
                                                     PropertyList property_list,
                                                     std::shared_ptr<std::vector<Index>>
                                                     optional_landmark_list = nullptr);

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_LANDMARK_H
