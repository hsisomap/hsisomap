//
// Created by Can on 1/11/16.
//

#include <hsisomap/HsiData.h>
#include <fstream>
#include <sstream>
#include <hsisomap/Logger.h>

HSISOMAP_NAMESPACE_BEGIN

StringPropertyList ParseHeader(const std::string header_file) {
  StringPropertyList property_list;

  std::ifstream ifs(header_file);
  if (!ifs.is_open())
    throw std::invalid_argument(std::string("Header file \"").append(header_file).append("\" does not exist."));

  std::string strline;
  auto trim_spaces = [](std::string const str) -> std::string {
    if (str.empty()) return str;
    size_t first = str.find_first_not_of(" \n\r\t"); // need to include \r etc. for cross-platform newline char
    size_t last = str.find_last_not_of(" \n\r\t");
    return str.substr(first, last - first + 1);
  };
  while (!ifs.eof()) {
    getline(ifs, strline);
    size_t delim_pos = strline.find("=");
    if (delim_pos != std::string::npos) {
      std::string token = trim_spaces(strline.substr(0, delim_pos));
      std::transform(token.begin(), token.end(), token.begin(), ::tolower);
      std::string value_str = trim_spaces(strline.substr(delim_pos + 1, strline.size() - delim_pos));
      property_list[token] = value_str;
    }
  }
  ifs.close();
  return property_list;
}

void WriteHeader(const std::string header_file, StringPropertyList property_list) {
  std::ofstream ofs(header_file);

  ofs << "ENVI" << std::endl;
  Key delim = " = ";
  for (auto p : property_list) {
    ofs << p.first << delim << p.second << std::endl;
  }
}

template<typename T>
void HsiData::ReadBinaryFile(const std::string image_file) {

  std::ifstream input_file(image_file);

  for (size_t i = 0; i < samples_ * lines_ * bands_; i++) {
    unsigned char byte[sizeof(T)];
    input_file.read(reinterpret_cast<char *>(byte), sizeof(T));

    Scalar value = static_cast<Scalar>(reinterpret_cast<T &>(byte));
    size_t line = 0, sample = 0, band = 0, intermediate = 0;
    if (property_list_[HSIDATA_PROPERTY_INTERLEAVE] == HSIDATA_PROPERTY_INTERLEAVE_BSQ) {
      band = i / (samples_ * lines_);
      intermediate = i % (samples_ * lines_);
      line = intermediate / samples_;
      sample = intermediate % samples_;
    } else if (property_list_[HSIDATA_PROPERTY_INTERLEAVE] == HSIDATA_PROPERTY_INTERLEAVE_BIP) {
      line = i / (samples_ * bands_);
      intermediate = i % (samples_ * bands_);
      sample = intermediate / bands_;
      band = intermediate % bands_;
    } else if (property_list_[HSIDATA_PROPERTY_INTERLEAVE] == HSIDATA_PROPERTY_INTERLEAVE_BIL) {
      line = i / (samples_ * bands_);
      intermediate = i % (samples_ * bands_);
      band = intermediate / samples_;
      sample = intermediate % samples_;
    } else {
      throw std::invalid_argument("Invalid interleave value.");
    }
    (*data_block_)(line * samples_ + sample, band) = value;
  }

}

template<typename T>
void HsiData::WriteBinaryFile(const std::string image_file) {

  std::ofstream output_file(image_file, std::ios::binary);

  for (size_t i = 0; i < samples_ * lines_ * bands_; i++) {

    size_t line = 0, sample = 0, band = 0, intermediate = 0;
    if (property_list_[HSIDATA_PROPERTY_INTERLEAVE] == HSIDATA_PROPERTY_INTERLEAVE_BSQ) {
      band = i / (samples_ * lines_);
      intermediate = i % (samples_ * lines_);
      line = intermediate / samples_;
      sample = intermediate % samples_;
    } else if (property_list_[HSIDATA_PROPERTY_INTERLEAVE] == HSIDATA_PROPERTY_INTERLEAVE_BIP) {
      line = i / (samples_ * bands_);
      intermediate = i % (samples_ * bands_);
      sample = intermediate / bands_;
      band = intermediate % bands_;
    } else if (property_list_[HSIDATA_PROPERTY_INTERLEAVE] == HSIDATA_PROPERTY_INTERLEAVE_BIL) {
      line = i / (samples_ * bands_);
      intermediate = i % (samples_ * bands_);
      band = intermediate / samples_;
      sample = intermediate % samples_;
    } else {
      throw std::invalid_argument("Invalid interleave value.");
    }

    T value = static_cast<T>((*data_block_)(line * samples_ + sample, band));
    output_file.write(reinterpret_cast<char *>(&value), sizeof(T));
  }

}

HsiData::HsiData(std::string image_file, std::string header_file) : data_mask_(HSIDATAMASK_NO_MASK, 0, 0, 0) {
  if (header_file == "") {
    header_file = image_file;
    header_file.append(".hdr");
  }

  property_list_ = ParseHeader(header_file);
  std::stringstream sstr[] = {std::stringstream(property_list_[HSIDATA_PROPERTY_LINES]),
                              std::stringstream(property_list_[HSIDATA_PROPERTY_SAMPLES]),
                              std::stringstream(property_list_[HSIDATA_PROPERTY_BANDS]),
                              std::stringstream(property_list_[HSIDATA_PROPERTY_DATA_TYPE]),
                              std::stringstream(property_list_[HSIDATA_PROPERTY_BYTE_ORDER])};
  sstr[0] >> lines_;
  sstr[1] >> samples_;
  sstr[2] >> bands_;
  short data_type = 0, byte_order = 0;
  sstr[3] >> data_type;
  sstr[4] >> byte_order;

  if (byte_order == 1) throw std::invalid_argument("Network byte order not supported.");
  auto interleave = property_list_[HSIDATA_PROPERTY_INTERLEAVE];
  std::transform(interleave.begin(), interleave.end(), interleave.begin(), ::tolower);
  property_list_[HSIDATA_PROPERTY_INTERLEAVE] = interleave;

  data_block_ = std::make_shared<gsl::Matrix>(lines_ * samples_, bands_);

  switch (data_type) {
    case 4:
      ReadBinaryFile<float>(image_file);
      break;
    case 5:
      ReadBinaryFile<double>(image_file);
      break;
    default:
      throw std::invalid_argument("Invalid data type or type not supported yet.");
  }

  data_mask_.lines = lines_;
  data_mask_.samples = samples_;
  data_mask_.bands = bands_;

}

HsiData::HsiData(std::shared_ptr<gsl::Matrix> data_block, HsiDataMask data_mask, StringPropertyList property_list)
    : data_block_(data_block), data_mask_(data_mask), property_list_(property_list), lines_(data_mask.lines),
      samples_(data_mask.samples), bands_(data_mask.bands) { }

HsiData::HsiData(std::shared_ptr<gsl::Matrix> data_block,
                 Index lines,
                 Index samples,
                 Index bands,
                 StringPropertyList property_list)
    : data_block_(data_block), data_mask_(HSIDATAMASK_NO_MASK, lines, samples, bands), property_list_(property_list),
      lines_(lines), samples_(samples), bands_(bands) { }


void HsiData::WriteImageFile(std::string image_file, std::string header_file) {
  if (header_file == "") {
    header_file = image_file;
    header_file.append(".hdr");
  }

  // Add missing entries for the header file. Note that the data cube sizes specified in the property list are ignored.
  // The data cube sizes should be specified from the constructors or HsiDataMask only.
  if (property_list_[HSIDATA_PROPERTY_INTERLEAVE] == "")
    property_list_[HSIDATA_PROPERTY_INTERLEAVE] = HSIDATA_PROPERTY_INTERLEAVE_BIP;
  if (property_list_[HSIDATA_PROPERTY_DATA_TYPE] == "") property_list_[HSIDATA_PROPERTY_DATA_TYPE] = std::to_string(4);
  if (property_list_[HSIDATA_PROPERTY_BYTE_ORDER] == "")
    property_list_[HSIDATA_PROPERTY_BYTE_ORDER] = std::to_string(0);
  if (property_list_[HSIDATA_PROPERTY_HEADER_OFFSET] == "")
    property_list_[HSIDATA_PROPERTY_HEADER_OFFSET] = HSIDATA_PROPERTY_HEADER_OFFSET_ZERO;
  if (property_list_[HSIDATA_PROPERTY_FILE_TYPE] == "")
    property_list_[HSIDATA_PROPERTY_FILE_TYPE] = HSIDATA_PROPERTY_FILE_TYPE_STANDARD;
  property_list_[HSIDATA_PROPERTY_LINES] = std::to_string(lines_);
  property_list_[HSIDATA_PROPERTY_SAMPLES] = std::to_string(samples_);
  property_list_[HSIDATA_PROPERTY_BANDS] = std::to_string(bands_);

  std::stringstream sstr[] = {std::stringstream(property_list_[HSIDATA_PROPERTY_DATA_TYPE]),
                              std::stringstream(property_list_[HSIDATA_PROPERTY_BYTE_ORDER]),
                              std::stringstream(property_list_[HSIDATA_PROPERTY_HEADER_OFFSET])};
  short data_type = 0, byte_order = 0, header_offset = 0;
  sstr[0] >> data_type;
  sstr[1] >> byte_order;
  sstr[2] >> header_offset;

  if (header_offset != 0) throw std::invalid_argument("Non-zero header offset not supported yet.");

  switch (data_type) {
    case 4:
      WriteBinaryFile<float>(image_file);
      break;
    default:
      throw std::invalid_argument("Invalid data type or type not supported yet.");
  }
  WriteHeader(header_file, property_list_);

}

HSISOMAP_NAMESPACE_END
