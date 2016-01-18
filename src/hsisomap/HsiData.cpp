//
// Created by Can on 1/11/16.
//

#include <hsisomap/HsiData.h>
#include <fstream>
#include <sstream>
#include <hsisomap/Logger.h>

HSISOMAP_NAMESPACE_BEGIN

StringPropertyList ParseHeader(std::string header_file) {
  StringPropertyList property_list;

  std::ifstream ifs(header_file);
  if (!ifs.is_open()) throw std::invalid_argument(std::string("Header file \"").append(header_file).append("\" does not exist."));

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

template <typename T>
void HsiData::ReadBinaryFile(std::string image_file, Interleave interleave) {

  std::ifstream input_file(image_file);

  for(size_t i = 0; i < samples_ * lines_ * bands_; i++)
  {
    unsigned char byte[sizeof(T)];
    input_file.read(reinterpret_cast<char*>(byte), sizeof(T));

    double value = static_cast<double>(reinterpret_cast<T&>(byte));
    size_t line = 0, sample = 0, band = 0, intermediate = 0;
    switch (interleave) {
      case BSQ:
        band = i / (samples_ * lines_);
        intermediate = i % (samples_ * lines_);
        line = intermediate / samples_;
        sample = intermediate % samples_;
        break;
      case BIP:
        line = i / (samples_ * bands_);
        intermediate = i % (samples_ * bands_);
        sample = intermediate / bands_;
        band = intermediate % bands_;
        break;
      case BIL:
        line = i / (samples_ * bands_);
        intermediate = i % (samples_ * bands_);
        band = intermediate / samples_;
        sample = intermediate % samples_;
    }
    (*data_block_)(line * samples_ + sample, band) = value;
  }

}

HsiData::HsiData(std::string image_file, std::string header_file) {
  if (header_file == "") {
    header_file = image_file;
    header_file.append(".hdr");
  }

  property_list_ = ParseHeader(header_file);
  std::stringstream sstr[] = {std::stringstream(property_list_["lines"]),
                              std::stringstream(property_list_["samples"]),
                              std::stringstream(property_list_["bands"]),
                              std::stringstream(property_list_["data type"]),
                              std::stringstream(property_list_["byte order"])};
  sstr[0] >> lines_;
  sstr[1] >> samples_;
  sstr[2] >> bands_;
  short data_type = 0, byte_order = 0;
  sstr[3] >> data_type;
  sstr[4] >> byte_order;

  if (byte_order == 1) throw std::invalid_argument("Network byte order not supported.");
  auto interleave_string = property_list_["interleave"];
  std::transform(interleave_string.begin(), interleave_string.end(), interleave_string.begin(), ::tolower);
  Interleave interleave = BSQ;
  if (interleave_string == "bsq") {
    interleave = BSQ;
  } else if (interleave_string == "bip") {
    interleave = BIP;
  } else if (interleave_string == "bil") {
    interleave = BIL;
  } else {
    throw std::invalid_argument("Interleave not specified or not valid in the header file.");
  }

  data_block_ = std::make_shared<gsl::Matrix>(lines_ * samples_, bands_);

  switch (data_type) {
    case 4:
      ReadBinaryFile<float>(image_file, interleave);
      break;
    default:
      throw std::invalid_argument("Invalid data type or type not supported yet.");
  }

}

HSISOMAP_NAMESPACE_END
