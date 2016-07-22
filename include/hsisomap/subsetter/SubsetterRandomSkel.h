//
// Created by Can on 4/28/16.
//

#ifndef HSISOMAP_SUBSETTERRANDOMSKEL_H
#define HSISOMAP_SUBSETTERRANDOMSKEL_H

#include "Subsetter.h"

HSISOMAP_NAMESPACE_BEGIN

class SubsetterRandomSkel : public Subsetter {
 public:
  SubsetterRandomSkel(std::shared_ptr<gsl::Matrix> data, PropertyList property_list);
  std::vector<std::vector<Index>> subsets() { return subsets_; }
  std::shared_ptr<gsl::Matrix> data() { return data_; }
 private:
  std::shared_ptr<gsl::Matrix> data_;
  PropertyList property_list_;
  std::shared_ptr<gsl::Matrix> embedding_;
  std::vector<std::vector<Index>> subsets_;
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_SUBSETTERRANDOMSKEL_H
