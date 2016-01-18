//
// Created by Can on 1/11/16.
//

#ifndef HSISOMAP_HSIDATA_H
#define HSISOMAP_HSIDATA_H

#include "typedefs.h"
#include "Matrix.h"
#include "HsiDataMask.h"

HSISOMAP_NAMESPACE_BEGIN

class HsiData {
 public:
  enum Interleave { BSQ, BIP, BIL };
  HsiData(gsl::Matrix data_block, HsiDataMask data_mask, StringPropertyList property_list);
  HsiData(gsl::Matrix data_block, Index lines, Index samples, Index bands, StringPropertyList property_list);
  HsiData(std::string image_file, std::string header_file = "");
  void WriteImageFile(std::string image_file, std::string header_file = "");
  Index lines() { return lines_; }
  Index samples() { return samples_; }
  Index bands() { return bands_; }
  StringPropertyList get_property_list() { return property_list_; }
  std::string get_property(std::string key) { return property_list_[key]; }
  std::shared_ptr<gsl::Matrix> data() { return data_block_; }
 private:
  Index lines_;
  Index samples_;
  Index bands_;
  std::shared_ptr<gsl::Matrix> data_block_;
  StringPropertyList property_list_;
  template <typename T>
  void ReadBinaryFile(std::string image_file, Interleave interleave);
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_HSIDATA_H
