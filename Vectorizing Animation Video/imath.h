#pragma once
/************************************************************************/
/* design by liang-syuan huang                                          */
/* This file imath define all Matrix and Vector that we need            */
/* The first thing : you need to decide row major/column major          */
/* you can use #define ROW_MAJOR or COL_MAJOR to decide matrix major    */
/*                                                                      */
/* use ogre math for compute dot product and cross producct             */
/*                                                                      */
/*                                                                      */
/************************************************************************/

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef __int64		int64;
typedef unsigned __int64	uint64;

typedef std::vector<bool>	bool_vector;
typedef std::vector<uchar>	uchar_vector;
typedef std::vector<ushort>	ushort_vector;
typedef std::vector<uint>	uint_vector;
typedef std::vector<double>	double_vector;
typedef std::vector<double_vector>	double_vector2d;
typedef std::vector<float>	float_vector;
typedef std::vector<float_vector>	float_vector2d;
typedef std::vector<int>	int_vector;
typedef std::vector<std::vector<int> >	int_vector2d;
typedef std::vector<int64>	int64_vector;
typedef std::vector<std::vector<int64> > int64_vector2d;

#pragma warning( push, 0 )
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix3.h"
#include "math/Matrix4.h"
#pragma warning( pop )

#pragma warning( push, 0 )
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#pragma warning( pop )

namespace UBLAS = boost::numeric::ublas;
typedef double num;
// vector
typedef UBLAS::vector<num>		Vector;
typedef UBLAS::mapped_vector<num>	MVector;	//M stand for mapped
typedef UBLAS::compressed_vector<num>	SVector;	//S stand for small and slow
// generic matrix
typedef UBLAS::zero_matrix	<num>	ZMatrix;
typedef UBLAS::identity_matrix	<num>	IMatrix;
// matrix
typedef UBLAS::basic_row_major<int64, int64>		row_major_int64;
typedef UBLAS::basic_column_major<int64, int64>		col_major_int64;
typedef UBLAS::map_std<int64, num> map_std_int64;
typedef UBLAS::matrix		<num, col_major_int64>	Matrix_col;
typedef UBLAS::matrix		<num, row_major_int64>	Matrix_row;
typedef UBLAS::mapped_matrix	<num, col_major_int64, map_std_int64> MMatrix_col; //M stand for mapped
typedef UBLAS::mapped_matrix	<num, row_major_int64, map_std_int64> MMatrix_row;
typedef UBLAS::coordinate_matrix<num, col_major_int64>	CMatrix_col;	//C stand for coordinate
typedef UBLAS::coordinate_matrix<num, row_major_int64>	CMatrix_row;
typedef UBLAS::compressed_matrix<num, col_major_int64>	SMatrix_col;	//S stand for small and slow
typedef UBLAS::compressed_matrix<num, row_major_int64>	SMatrix_row;
typedef UBLAS::diagonal_matrix	<num, col_major_int64>	DMatrix_col;
typedef UBLAS::diagonal_matrix	<num, row_major_int64>	DMatrix_row;

//#define COL_MAJOR
#if !defined(ROW_MAJOR) && !defined(COL_MAJOR)
#pragma message( "if no define any major, default use ROW_MAJOR" )
#define ROW_MAJOR
#endif

#if defined(ROW_MAJOR)
typedef Matrix_row	Matrix;
typedef MMatrix_row	MMatrix;
typedef CMatrix_row	CMatrix;
typedef SMatrix_row	SMatrix;
typedef DMatrix_row	DMatrix;
#elif defined(COL_MAJOR)
typedef Matrix_col	Matrix;
typedef MMatrix_col	MMatrix;
typedef CMatrix_col	CMatrix;
typedef SMatrix_col	SMatrix;
typedef DMatrix_col	DMatrix;
#endif

//#define USE_CLAPACK
#ifdef USE_CLAPACK
#define BOOST_NUMERIC_BINDINGS_USE_CLAPACK
#pragma warning( push, 0 )
#include <boost/numeric/bindings/lapack/gesvd.hpp>
#include <boost/numeric/bindings/lapack/gesdd.hpp>
#include <boost/numeric/bindings/traits/std_vector.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#pragma warning( pop )
// # pragma comment(lib, "libf2cd.lib")
// # pragma comment(lib, "BLASd.lib")
// # pragma comment(lib, "lapackd.lib")
#undef  BOOST_NUMERIC_BINDINGS_USE_CLAPACK
void svd( const Matrix& A, Matrix& U, DMatrix& D, Matrix& VT );
void GetInverseBySVD( Matrix& dst, Matrix& src ); //ºâmatrixµêÀÀªº¤Ï¯x°}
#endif

// function declaration
void Transpose( MMatrix_col& m_out, const MMatrix_row& m_in );
void Transpose( MMatrix_row& m_out, const MMatrix_col& m_in );
MMatrix_row MultiplyATA( const MMatrix_row& A );
MMatrix_col MultiplyATA( const MMatrix_col& A );
template <class MATRIX_TYPE>
void Sparse_Multiply( MATRIX_TYPE& out, const MMatrix_row& A, const MMatrix_col& B );
