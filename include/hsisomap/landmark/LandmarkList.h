//
// Created by Can on 1/25/16.
//

#ifndef HSISOMAP_LANDMARKLIST_H
#define HSISOMAP_LANDMARKLIST_H

#include <hsisomap/gsl_util/matrix_util.h>
#include "Landmark.h"

HSISOMAP_NAMESPACE_BEGIN

class LandmarkList: public Landmark {
 public:
  LandmarkList(std::shared_ptr<gsl::Matrix> data, PropertyList property_list, std::vector<Index> landmark_list)
      : data_(data), property_list_(property_list), landmarks_(landmark_list) { }
  std::vector<Index> landmarks() { return landmarks_; }
  std::shared_ptr<gsl::Matrix> landmark_data() { return gsl::GetRows(data_, landmarks_); }
 private:
  std::shared_ptr<gsl::Matrix> data_;
  std::vector<Index> landmarks_;
  PropertyList property_list_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_LANDMARKLIST_H
