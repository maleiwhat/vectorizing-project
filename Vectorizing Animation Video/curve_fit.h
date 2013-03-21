#pragma once
#include "imath.h"
#include "math/BasicMath.h"
//#define USE_CLAPACK
#ifdef USE_CLAPACK
#define BOOST_NUMERIC_BINDINGS_USE_CLAPACK
#include <boost/numeric/bindings/lapack/gesvd.hpp>
#include <boost/numeric/bindings/lapack/gesdd.hpp>
#include <boost/numeric/bindings/traits/std_vector.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
# pragma comment(lib, "libf2cd.lib")
# pragma comment(lib, "BLASd.lib")
# pragma comment(lib, "lapackd.lib")
#undef  BOOST_NUMERIC_BINDINGS_USE_CLAPACK
#endif

typedef boost::numeric::ublas::zero_matrix<double> ZMatrix;
typedef boost::numeric::ublas::vector<double> values;
typedef std::vector<int> vectori;
bool LinearlyIndependent(MMatrix_row &mat);
void Independent(MMatrix_row &mat, unsigned int y_index, int x_index );
void CreateBSplineMatrix(MMatrix_row &mat, const values &vals);
bool InverseMatrix(MMatrix_row &dst, MMatrix_row &src);
void zero_smallvalue(Matrix_row &mat, double value = 0.0001);
static double V3, V2, V1;
void initV(double v);
double getAofX_value(int index);
Matrix_row MakebasicA0123(int NumOfSegPoints);
Matrix_row MakeAMatrix(int s_size, int c_size);
/*sam = sample points, c_size = control size*/
values InverseCubicBSpline(const values &sam, int c_size);
values GetCubicBSpline(const values &control, int s_size);
Vector2s Get_C0_to_Cn_From_Values(const Vector2s &vals, int n);
Vector2s Get_C0_to_Cn_From_Values(const Vector2s &vals);
Vector2s Get_Values_From_C0_to_Cn(const Vector2s &vals, int s_size);
#ifdef USE_CLAPACK
void svd(const boost::numeric::ublas::matrix<double>& A, boost::numeric::ublas::matrix<double>& U, boost::numeric::ublas::diagonal_matrix<double>& D, boost::numeric::ublas::matrix<double>& VT);
void GetInverseBySVD(Matrix_row &dst, Matrix_row &src);
#endif
