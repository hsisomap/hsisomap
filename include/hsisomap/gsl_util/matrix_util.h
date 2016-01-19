//
// Created by Can on 1/18/16.
//

#ifndef HSISOMAP_MATRIX_UTIL_H
#define HSISOMAP_MATRIX_UTIL_H

#include <hsisomap/Matrix.h>

namespace gsl {

void MakeBasesSameDirectionAs(std::shared_ptr<Matrix> bases, std::shared_ptr<Matrix> vector = nullptr);

} // namespace gsl

#endif //HSISOMAP_MATRIX_UTIL_H
