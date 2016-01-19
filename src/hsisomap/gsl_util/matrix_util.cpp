//
// Created by Can on 1/18/16.
//

#include <hsisomap/gsl_util/matrix_util.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector_double.h>

namespace gsl {

void MakeBasesSameDirectionAs(std::shared_ptr<Matrix> bases, std::shared_ptr<Matrix> datum) {
  if (datum == nullptr) {
    datum = std::make_shared<Matrix>(1, bases->rows(), 0.0);
    (*datum)(0, 0) = 1.0;
  }
  gsl_vector_const_view datum_view = gsl_matrix_const_row(datum->m_, 0);
  for (Index col = 0; col < bases->cols(); col++) {
    gsl_vector_view column_view = gsl_matrix_column(bases->m_, col);
    double inner_product = 0;
    gsl_blas_ddot(&column_view.vector, &datum_view.vector, &inner_product);
    if (inner_product == 0) {
      std::cerr << "Basis orthogonal to the datum vector; try another datum vector." << std::endl;
    }
    if (inner_product < 0) {
      gsl_vector_scale(&column_view.vector, - 1.0);
    }
  }
};

} // namespace gsl
