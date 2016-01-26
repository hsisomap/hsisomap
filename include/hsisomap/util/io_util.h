//
// Created by Can on 1/20/16.
//

#ifndef HSISOMAP_IO_UTIL_H
#define HSISOMAP_IO_UTIL_H

#include <vector>
#include <string>
#include <fstream>
#include "../typedefs.h"

HSISOMAP_NAMESPACE_BEGIN


template <typename T>
inline std::vector<T> Read1DVectorFromTextFile(std::string file_name) {
  std::ifstream ifs(file_name);
  if (!ifs.is_open()) throw std::invalid_argument(std::string("Text file \"").append(file_name).append("\" does not exist."));

  std::vector<T> result;
  T t;
  while (ifs >> t) {
    result.push_back(t);
  }

  return result;
}


HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_IO_UTIL_H
