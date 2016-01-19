//***************************************************************************************
//
//! \file Matrix.h
//!  A simple wrapper class for matrix operations of GNU Scientific Library.
//!
//! \author    Can
//! \version   1.0
//! \date      2015-03-23
//! \copyright GNU Public License V3.0
//
//***************************************************************************************

#ifndef DIJKSTRACL_MATRIX_H
#define DIJKSTRACL_MATRIX_H

#include <gsl/gsl_matrix.h>
#include <iostream>
#include "typedefs.h"

namespace gsl {

//! A simple wrapper class for common matrix operations and resource management for GNU Scientific Library
class Matrix {
 public:
  gsl_matrix * m_; //!< Plain pointer for actual GSL matrix type; use with caution.
  Scalar equality_limit_ = 1e-12; //!< Floating point equality precision; difference less than this value is considered no difference.

  //! Default constructor.
  //! No GSL matrix will be allocated; the plain pointer to the matrix is set to NULL.
  Matrix();

  //! Uninitialized Constructor.
  //! Construct a matrix with specified size and uninitialized elements.
  //! \param rows number of rows of the matrix.
  //! \param cols number of columns of the matrix.
  Matrix(Index rows, Index cols);

  //! Constructor with Initialization.
  //! Construct a matrix with specified size and initialize elements with the specified value.
  //! \param rows number of rows of the matrix.
  //! \param cols number of columns of the matrix.
  //! \param initialValue the value to be initialized with.
  Matrix(Index rows, Index cols, Scalar initialValue);

  //! Constructor with Initialization using Initializer List.
  //! Construct a matrix with the initializer list. The width of the matrix is the size of the first Scalar initializer list.
  //! \param initializerList the initializer list of initializer lists of Scalar type.
  Matrix(std::initializer_list<std::initializer_list<Scalar>> initializerList);

  //! Copy Constructor.
  //! Construct a matrix with an existing matrix. The old matrix will be copied.
  Matrix(const Matrix& other);

  //! Destructor.
  //! The destructor will release the memory of the matrix.
  ~Matrix();

  //! Const Element Accessor.
  //! Access one element of the const matrix.
  //! \param row the row index of the element.
  //! \param col the column index of the element.
  const Scalar & operator()(Index row, Index col) const;

  //! Element Accessor.
  //! Access one element reference of the matrix. This can be the lvalue reference.
  //! \param row the row index of the element.
  //! \param col the column index of the element.
  Scalar & operator()(Index row, Index col);

  //! Const Element Getter.
  //! This is the named accessor which has the same effect of the const version of operator().
  //! \param row the row index of the element.
  //! \param col the column index of the element.
  Scalar Get(Index row, Index col) const;

  //! Element Setter.
  //! Set one element reference of the matrix using the specified value.
  //! \param row the row index of the element.
  //! \param col the column index of the element.
  //! \param val the new value of the element to be set.
  void Set(Index row, Index col, Scalar val);

  //! Element Setter.
  //! Set all the elements of the matrix to the new value.
  //! \param scalar the new value of the elements of the matrix.
  void SetAll(Scalar scalar);

  //! Element Setter.
  //! Set all the elements of the matrix to zero.
  void SetZero();

  //! Element Setter.
  //! Set the matrix to be an identity matrix.
  //! It also works with non-square matrix in which case the diagonal
  //! elements are set to one and all other elements are set to zero.
  void SetIdentity();

  //! Get the number of rows of the matrix.
  Index rows() const;

  //! Get the number of columns of the matrix.
  Index cols() const;

  //! Copy the new matrix to the current matrix.
  //! The old matrix will be released.
  //! This function will be called by the copy constructor and operator=.
  //! \param other the new matrix to be copied.
  void Copy(const Matrix& other);

  //! Assignment operator.
  //! Copy the other matrix to this matrix.
  const Matrix& operator=(const Matrix& other);

  //! Resize the matrix.
  //! The matrix will be resized and the element values are preserved as much as possible.
  //! A new matrix will be allocated and elements will be copied. The old matrix will be released.
  void Resize(Index rows, Index cols);

  //! Redimension the matrix.
  //! This function has the same effect of Resize except for that the old elements of the matrix are
  //! not preserved. The new matrix will be uninitialized.
  //! \param rows number of rows of the new matrix.
  //! \param cols number of columns of the new matrix.
  void Redimension(Index rows, Index cols);

  //! Equality operator.
  //! Check the equality of the matrix by checking all corresponding elements.
  //! equality_limit will be used as the precision of the equality.
  bool operator==(const Matrix& other) const;

  //! Reset the gsl_matrix pointer.
  //! Reset the internal gsl_matrix raw pointer to a new one. Release the old memory.
  void Reset(gsl_matrix * new_m);
};

std::ostream& operator<<(std::ostream& os, const Matrix& matrix);

} // namespace gsl

#endif //DIJKSTRACL_MATRIX_H
