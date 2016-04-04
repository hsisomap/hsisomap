//
// Created by Can on 1/25/16.
//

#ifndef HSISOMAP_ROI_H
#define HSISOMAP_ROI_H

#include "typedefs.h"
#include "Matrix.h"

HSISOMAP_NAMESPACE_BEGIN

class Roi {
 public:
  Roi(const gsl::Matrix &roi_image);

};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_ROI_H
