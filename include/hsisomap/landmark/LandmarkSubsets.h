//
// Created by Can on 7/22/16.
//

#ifndef HSISOMAP_LANDMARKSUBSETS_H
#define HSISOMAP_LANDMARKSUBSETS_H

#include <hsisomap/gsl_util/matrix_util.h>
#include "Landmark.h"

HSISOMAP_NAMESPACE_BEGIN

class LandmarkSubsets: public Landmark {
 public:
  LandmarkSubsets(std::shared_ptr<gsl::Matrix> data, PropertyList property_list);
  const std::vector<Index> &landmarks() { return landmarks_; }
  std::shared_ptr<gsl::Matrix> landmark_data() { return gsl::GetRows(data_, landmarks_); }
 private:
  std::shared_ptr<gsl::Matrix> data_;
  std::vector<Index> landmarks_;
  PropertyList property_list_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_LANDMARKSUBSETS_H
