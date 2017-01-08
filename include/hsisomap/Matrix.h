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
#include <vector>
#include "typedefs.h"

//! \brief A collection of matrix and vector related computation functions based on GSL (GNU Scientific Library).
//!
//! This is a collection of matrix and vector related computation interfaces based on GSL.
//!
//!   - The foundation of the collection is gsl::Matrix (Matrix.h), a wrapper class to facilitate matrix operations and resource management for the matrix data type of GSL. Note that in the commonly usages in the library, the vectors are not represented as a separate wrapper class, but a special case of gsl::Matrix.
//!
//!   - A collection of helper functions for simple matrix manipulations are in matrix_util.h file.
//!
//!   - A collection of simple inline computation functions (for GSL's gsl_matrix, not gsl::Matrix, resource not managed) including some frequently used linear algebra computations is in gsl_util.h file.
//!
//!   - A collection of advanced embedding space computation functions, and a struct to represent information of a data embedding, is in embedding.h file.
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

  //! Constructor with Initialization using STL vector of vectors.
  //! Construct a matrix with the std::vector<std::vector<T>> type. The width of the matrix is the size of the first vector of Scalar. T will be casted to Scalar.
  //! \param vector_of_vectors the row-major vector of vectors representing a matrix.
  template<typename T>
  explicit Matrix(const std::vector<std::vector<T>> vector_of_vectors);

  //! Copy Constructor.
  //! Construct a matrix with an existing matrix. The old matrix will be copied.
  //! \param other matrix to be copied.
  Matrix(const Matrix& other);

  //! Destructor.
  //! The destructor will release the memory of the matrix.
  ~Matrix();

  //! Const Element Accessor.
  //! Access one element of the const matrix.
  //! \param row the row index of the element.
  //! \param col the column index of the element.
  //! \return const reference to the specified element.
  const Scalar & operator()(Index row, Index col) const;

  //! Element Accessor.
  //! Access one element reference of the matrix. This can be the lvalue reference.
  //! \param row the row index of the element.
  //! \param col the column index of the element.
  //! \return reference to the specified element.
  Scalar & operator()(Index row, Index col);

  //! Const Element Getter.
  //! This is the named accessor which has the same effect of the const version of operator().
  //! \param row the row index of the element.
  //! \param col the column index of the element.
  //! \return a copy of the specified element.
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
  //! \return the number of rows of the matrix.
  Index rows() const;

  //! Get the number of columns of the matrix.
  //! \return the number of columns of the matrix.
  Index cols() const;

  //! Copy the new matrix to the current matrix.
  //! The old matrix will be released.
  //! This function will be called by the copy constructor and operator=.
  //! \param other the new matrix to be copied.
  void Copy(const Matrix& other);

  //! Assignment operator.
  //! Copy the other matrix to this matrix.
  //! \param other the new matrix to be assigned.
  //! \return const reference of the new matrix to be chained in an expression.
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

  //! Transpose the matrix.
  //! Transpose the matrix.
  void Transpose();

  //! Equality operator.
  //! Check the equality of the matrix by checking all corresponding elements.
  //! equality_limit will be used as the precision of the equality.
  //! \param other matrix to be compared.
  //! \return whether they are equal.
  bool operator==(const Matrix& other) const;

  //! Reset the gsl_matrix pointer.
  //! Reset the internal gsl_matrix raw pointer to a new one. Release the old memory.
  void Reset(gsl_matrix * new_m);

  //! Get a copy of the rows with the indices.
  //! \param rows the indices of the rows to be extracted.
  //! \return a new matrix consisting the specified rows.
  Matrix GetRows(std::vector<Index> rows) const;

  //! Get a copy of the row with the index.
  //! \param row the indices of the row to be extracted.
  //! \return a new one-row matrix consisting the specified row.
  Matrix GetRow(Index row) const;

  //! Get a copy of the columns with the indices.
  //! \param cols the indices of the columns to be extracted.
  //! \return a new matrix consisting the specified columns.
  Matrix GetCols(std::vector<Index> cols) const;

  //! Get a copy of the column with the index.
  //! \param col the indices of the column to be extracted.
  //! \return a new one-column matrix consisting the specified row.
  Matrix GetCol(Index col) const;

};

//! Output stream operator.
//! The output stream operator will output the matrix values in normal matrix representation, with spaces to separate elements, and new line to separate rows.
//!
//! If the value is overflowed (or to be exactly, the maximum possible value of the representation), it will be printed as '-' (dash).
//! \param os ostream object.
//! \param matrix matrix to be streamed out.
//! \return same ostream object to be chained in an expression.
std::ostream& operator<<(std::ostream& os, const Matrix& matrix);

//! Input streaming operator.
//! It supports standard istream separators, and the value is inputed in a row-major order.
//! \param is istream object.
//! \param matrix matrix to be streamed in.
//! \return same istream object to be chained in an expression.
std::istream& operator>>(std::istream& is, Matrix& matrix);

} // namespace gsl

#endif //DIJKSTRACL_MATRIX_H
