//
// Created by Can on 1/17/16.
//

#ifndef HSISOMAP_HSIDATAMASK_H
#define HSISOMAP_HSIDATAMASK_H

#include <vector>
#include "typedefs.h"

HSISOMAP_NAMESPACE_BEGIN

struct HsiDataMask {
  std::shared_ptr<std::vector<std::vector<bool>>> spatial;
  std::shared_ptr<std::vector<bool>> spectral;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_HSIDATAMASK_H
