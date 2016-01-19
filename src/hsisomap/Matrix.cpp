
#include <algorithm>
#include <iostream>
#include <hsisomap/Matrix.h>

namespace gsl {

Matrix::Matrix() : m_(NULL) { }

Matrix::Matrix(Index rows, Index cols) {
  m_ = gsl_matrix_alloc(rows, cols);
}

Matrix::Matrix(Index rows, Index cols, Scalar initialValue) {
  if (initialValue == 0) {
    m_ = gsl_matrix_calloc(rows, cols);
  } else {
    m_ = gsl_matrix_alloc(rows, cols);
    SetAll(initialValue);
  }
}

Matrix::Matrix(std::initializer_list<std::initializer_list<Scalar>> initializerList) {
  if (initializerList.size() == 0) throw std::invalid_argument("Empty initializer list");
  m_ = gsl_matrix_calloc(initializerList.size(), initializerList.begin()->size());
  Index row = 0, col = 0;
  for (auto initializer_list_row : initializerList) {
    for (Scalar element : initializer_list_row) {
      gsl_matrix_set(m_, row, col, element);
      col++;
      if (col == m_->size2) break;
    }
    row++;
    col = 0;
  }
}

Matrix::~Matrix() {
  if (m_) gsl_matrix_free(m_);
}

const Scalar &Matrix::operator()(Index row, Index col) const {
  return *gsl_matrix_ptr(m_, row, col);
}

Scalar &Matrix::operator()(Index row, Index col) {
  return *gsl_matrix_ptr(m_, row, col);
}

void Matrix::SetAll(Scalar scalar) {
  gsl_matrix_set_all(m_, scalar);
}

void Matrix::SetZero() {
  gsl_matrix_set_zero(m_);
}

Index Matrix::rows() const {
  return m_->size1;
}

Index Matrix::cols() const {
  return m_->size2;
}

void Matrix::SetIdentity() {
  gsl_matrix_set_identity(m_);
}

void Matrix::Copy(const Matrix &other) {
  if (this == &other) return;
  Redimension(other.rows(), other.cols());
  for (Index r = 0; r < m_->size1; ++r) {
    for (Index c = 0; c < m_->size2; ++c) {
      gsl_matrix_set(m_, r, c, other(r, c));
    }
  }
}

const Matrix &Matrix::operator=(const Matrix &other) {
  Copy(other);
  return *this;
}

Matrix::Matrix(const Matrix &other) {
  Copy(other);
}

void Matrix::Resize(Index rows, Index cols) {
  if (!m_) {
    m_ = gsl_matrix_calloc(rows, cols);
    return;
  }
  if (m_->size1 == rows && m_->size2 == cols) return;

  gsl_matrix * newMatrix = gsl_matrix_calloc(rows, cols);
  for (Index r = 0; r < std::min(m_->size1, rows); ++r) {
    for (Index c = 0; c < std::min(m_->size2, cols); ++c) {
      gsl_matrix_set(newMatrix, r, c, gsl_matrix_get(m_, r, c));
    }
  }
  gsl_matrix_free(m_);
  m_ = newMatrix;
}

void Matrix::Redimension(Index rows, Index cols) {
  if (!m_) {
    m_ = gsl_matrix_alloc(rows, cols);
  } else if (m_->size1 != rows || m_->size2 != cols) {
    gsl_matrix_free(m_);
    m_ = gsl_matrix_alloc(rows, cols);
  }
}

Scalar Matrix::Get(Index row, Index col) const {
  return gsl_matrix_get(m_, row, col);
}

void Matrix::Set(Index row, Index col, Scalar val) {
  gsl_matrix_set(m_, row, col, val);
}

void Matrix::Reset(gsl_matrix * new_m) {
  if (new_m == m_) return;
  if (new_m == nullptr) return;
  if (m_) gsl_matrix_free(m_);
  m_ = new_m;
}

bool Matrix::operator==(const Matrix &other) const {
  if (this == &other) return true;
  if (m_->size1 != other.rows() || m_->size2 != other.cols()) return false;
  for (Index r = 0; r < m_->size1; ++r) {
    for (Index c = 0; c < m_->size2; ++c) {
      Scalar diff = other.Get(r, c) - gsl_matrix_get(m_, r, c);
      if (diff < 0) diff = -diff;
      if (diff > equality_limit_) return false;
    }
  }
  return true;
}

std::ostream &operator<<(std::ostream &os, const Matrix &matrix) {
  for (Index r = 0; r < matrix.m_->size1; ++r) {
    for (Index c = 0; c < matrix.m_->size2; ++c) {
      double val = gsl_matrix_get(matrix.m_, r, c);
      if (val == std::numeric_limits<Scalar>::max()) {
        os << "- ";
      } else {
        os << val << " ";
      }
    }
    os << std::endl;
  }
  return os;
}

} // namespace gsl
