//
// Created by Can on 1/11/16.
//

#ifndef HSISOMAP_HSIDATA_H
#define HSISOMAP_HSIDATA_H

#include "typedefs.h"
#include "Matrix.h"
#include "HsiDataMask.h"

HSISOMAP_NAMESPACE_BEGIN

// List the common constant strings and values, so they can be prompted and checked by IDE and compilers.
Key HSIDATA_PROPERTY_LINES = "lines";
Key HSIDATA_PROPERTY_SAMPLES = "samples";
Key HSIDATA_PROPERTY_BANDS = "bands";
Key HSIDATA_PROPERTY_INTERLEAVE = "interleave";
Key HSIDATA_PROPERTY_INTERLEAVE_BSQ = "bsq";
Key HSIDATA_PROPERTY_INTERLEAVE_BIP = "bip";
Key HSIDATA_PROPERTY_INTERLEAVE_BIL = "bil";
Key HSIDATA_PROPERTY_DATA_TYPE = "data type";
Key HSIDATA_PROPERTY_DATA_TYPE_FLOAT = "4";
Key HSIDATA_PROPERTY_BYTE_ORDER = "byte order";
Key HSIDATA_PROPERTY_FILE_TYPE = "file type";
Key HSIDATA_PROPERTY_FILE_TYPE_STANDARD = "ENVI Standard";
Key HSIDATA_PROPERTY_HEADER_OFFSET = "header offset";
Key HSIDATA_PROPERTY_HEADER_OFFSET_ZERO = "0";

class HsiData {
 public:
  HsiData(std::shared_ptr<gsl::Matrix> data_block, HsiDataMask data_mask, StringPropertyList property_list = StringPropertyList());
  HsiData(std::shared_ptr<gsl::Matrix> data_block, Index lines, Index samples, Index bands, StringPropertyList property_list = StringPropertyList());
  HsiData(std::string image_file, std::string header_file = "");
  void WriteImageFile(std::string image_file, std::string header_file = "");
  Index lines() { return lines_; }
  Index samples() { return samples_; }
  Index bands() { return bands_; }
  StringPropertyList &property_list() { return property_list_; }
  std::string get_property(std::string key) { return property_list_[key]; }
  HsiDataMask data_mask() { return data_mask_; }
  std::shared_ptr<gsl::Matrix> data() { return data_block_; }
 private:
  Index lines_; // TODO: should change to be completely backened by data mask
  Index samples_;
  Index bands_;
  std::shared_ptr<gsl::Matrix> data_block_;
  HsiDataMask data_mask_;
  StringPropertyList property_list_;
  template <typename T>
  void ReadBinaryFile(std::string image_file);
  template <typename T>
  void WriteBinaryFile(std::string image_file);
};

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_HSIDATA_H
