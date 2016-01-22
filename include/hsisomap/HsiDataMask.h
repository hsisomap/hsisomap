//
// Created by Can on 1/17/16.
//

#ifndef HSISOMAP_HSIDATAMASK_H
#define HSISOMAP_HSIDATAMASK_H

#include <vector>
#include "typedefs.h"

HSISOMAP_NAMESPACE_BEGIN

const bool HSIDATAMASK_USE_MASK = true;
const bool HSIDATAMASK_NO_MASK = false;

struct HsiDataMask {
  bool use_mask;
  Index lines;
  Index samples;
  Index bands;
  std::shared_ptr<std::vector<std::vector<bool>>> spatial;
  std::shared_ptr<std::vector<bool>> spectral;
  HsiDataMask(bool use_mask, Index lines, Index samples, Index bands) : lines(lines), samples(samples), bands(bands), spatial(nullptr), spectral(nullptr) {
    if (use_mask) {
      spatial = std::make_shared<std::vector<std::vector<bool>>>(lines, std::vector<bool>(samples, true));
      spectral = std::make_shared<std::vector<bool>>(bands, true);
    }
  }
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_HSIDATAMASK_H
