//
// Created by Can on 1/25/16.
//

#include <hsisomap/landmark/Landmark.h>
#include <hsisomap/landmark/LandmarkList.h>
#include <hsisomap/landmark/LandmarkSubsets.h>

HSISOMAP_NAMESPACE_BEGIN

std::shared_ptr<Landmark> LandmarkWithImplementation(LandmarkImplementation landmark_implementation,
                                                     std::shared_ptr<gsl::Matrix> data,
                                                     PropertyList property_list, std::shared_ptr<std::vector<Index>> optional_landmark_list) {

  switch (landmark_implementation) {
    case LANDMARK_IMPLEMENTATION_LIST:
      if (optional_landmark_list == nullptr) throw std::invalid_argument("LANDMARK_IMPLEMENTATION_LIST requrires optional_landmark_list argument.");
      return std::dynamic_pointer_cast<Landmark>(std::make_shared<LandmarkList>(data, property_list, *optional_landmark_list));
    case LANDMARK_IMPLEMENTATION_SUBSETS:
      return std::dynamic_pointer_cast<Landmark>(std::make_shared<LandmarkSubsets>(data, property_list));
    case LANDMARK_IMPLEMENTATION_GLOBAL_RANDOMSKEL:
      throw std::invalid_argument("Landmark Implementation Global RandomSkel not supported yet.");
  }
}

HSISOMAP_NAMESPACE_END

