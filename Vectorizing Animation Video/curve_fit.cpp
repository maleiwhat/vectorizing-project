#include "curve_fit.h"

#include <iomanip>

bool LinearlyIndependent( MMatrix_row &mat )
{
	using namespace boost::numeric::ublas;
	std::vector<bool> isIndependentVector(mat.size1(),false);
	std::vector<bool>::iterator y_no_indep;
	int x_index = 0;
	while ((y_no_indep=std::find(isIndependentVector.begin(),isIndependentVector.end(),false))
		!= isIndependentVector.end())
	{
		int find_index = y_no_indep-isIndependentVector.begin();
		for (size_t y_index = find_index;
			y_index < mat.size1();y_index++) // 找到第一個非0項
		{
			if (mat(y_index,x_index)!=0 && isIndependentVector[y_index]==false)
			{
				Independent(mat, y_index, x_index);
				isIndependentVector[y_index] = true;
				break;
			}
		}
		x_index++;
		if (x_index == mat.size2())
			return false;
	}
	return true;
}

void CreateBSplineMatrix( MMatrix_row &mat, const values &vals )
{
	using namespace boost::numeric::ublas;
	const std::size_t size = vals.size();
	assert(size >= 4);
	mat = matrix<double>(size, size+1, 0);
	mat(0, 0) = -2;
	mat(0, 2) =  2;
	mat(size-1, size-3) = -2;
	mat(size-1, size-1) =  2;
	for (int i=1;i < size-1;i++)
	{
		mat(i, i-1) = 1;
		mat(i, i+0) = 4;
		mat(i, i+1) = 1;
	}
	for (int i=0;i < size;i++)
	{
		mat(i, size) = vals[i]*6;
	}
}

void Independent( MMatrix_row &mat, unsigned int y_index, int x_index )
{
	double data = mat(y_index,x_index);
	for (size_t j=0;j < mat.size2();j++)
	{
		mat(y_index, j) /= data;
	}
	for (size_t y=0;y<mat.size1();y++)
	{
		if (y != y_index)
		{
			double dot = mat(y, x_index) / mat(y_index, x_index);
			for (size_t x=0;x<mat.size2();x++)
			{
				mat(y, x) -= mat(y_index, x) * dot;
			}
		}
	}
}

bool InverseMatrix( MMatrix_row &dst, MMatrix_row &src )
{
	assert(src.size1() == src.size2());
	if (src.size1() != src.size2())
		return false;
	MMatrix_row tmp(src.size1(), src.size2()*2, 0);
	dst = MMatrix_row(src.size1(), src.size2());
	for (size_t i=0;i<src.size1();i++)
	{
		for (size_t j=0;j<src.size2();j++)
		{
			tmp(i, j) = src(i, j);
		}
		tmp(i, src.size1() + i) = 1;
	}
	if (!LinearlyIndependent(tmp))
		return false;
	for (size_t i=0;i<src.size1();i++)
	{
		for (size_t j=0;j<src.size2();j++)
		{
			dst(i, j) = tmp(i, src.size1() + j);
		}
	}
	return true;
}

void zero_smallvalue( Matrix_row &mat, double value /*= 0.0001*/ )
{
	for (int i=0;i<mat.size1();i++)
	{
		for (int j=0;j<mat.size2();j++)
		{
			if (mat(i, j) < value)
				mat(i, j) = 0;
		}
	}
}

void initV( double v )
{
	V3 = v*v*v;
	V2 = v*v;
	V1 = v;
}

double getAofX_value( int index )
{
	switch (index)
	{
	case 0:
		return -V3+3*V2-3*V1+1;
	case 1:
		return 3*V3-6*V2+4;
	case 2:
		return -3*V3+3*V2+3*V1+1;
	case 3:
		return V3;
	default:
		assert(0);
	}
}

Matrix_row MakebasicA0123( int NumOfSegPoints )
{
	assert(NumOfSegPoints>0);
	if (NumOfSegPoints == 1)
	{
		initV(0.5);
		Matrix_row mat(1, 4, 0);
		for (int j=0;j<4;j++)
		{
			mat(0, j) = getAofX_value(j);
		}
		return mat;
	}
	double slice = 1.0/(NumOfSegPoints-1);
	Matrix_row mat(NumOfSegPoints, 4, 0);
	for (int i=0;i<NumOfSegPoints;i++)
	{
		initV(slice*i);
		for (int j=0;j<4;j++)
		{
			mat(i, j) = getAofX_value(j);
		}
	}
	return mat;
}

Matrix_row MakeAMatrix( int s_size, int c_size )
{
	Matrix_row mat(s_size, c_size, 0);
	vectori seg_len;
	const double div = double(s_size) / (c_size-3.0);
	double afterpoints=0;
	int sum = s_size, i=1;
	for (double each_b_dis=div;
		each_b_dis <= s_size;
		i++, each_b_dis=i*div)
	{
		double dis = each_b_dis-(i-1)*div;
		afterpoints += dis - floor(dis);
		if (afterpoints>=1)
		{
			afterpoints-=1;
			seg_len.push_back(dis+1);
		}
		else
			seg_len.push_back(dis);
	}
	assert(c_size-3 == seg_len.size());
#ifdef _DEBUG
	int test_sum=0;
	for (int i=0;i<seg_len.size();i++)
		test_sum += seg_len[i];
	assert(s_size == test_sum);
#endif // _DEBUG
	int row=0;
	static int make_size;
	make_size=-1;
	Matrix_row m;
	for (size_t i=0;i<seg_len.size();i++)
	{
		if (make_size!=seg_len[i])
		{
			make_size = seg_len[i];
			m = MakebasicA0123(seg_len[i]);
		}
		for (int j=0;j<seg_len[i];j++)
		{
			for (int k=0;k<4;k++)
			{
				mat(row+j, k+i) = m(j, k);
			}
		}
		row += seg_len[i];
	}
	return mat;
}

values InverseCubicBSpline( const values &sam, int c_size )
{
	const std::size_t s_size = sam.size();
	assert(s_size >= c_size);
	assert(c_size > 4);
	MMatrix_row mat = MakeAMatrix(s_size, c_size);
	//std::cout << mat << std::endl;
	MMatrix_row invmat;
	MMatrix_col tranmat;
	Transpose(tranmat, mat);
	MMatrix_row tmp = prod(tranmat, mat);
#ifdef USE_CLAPACK
	GetInverseBySVD(invmat, mat);
	return prod(invmat, 6.0*sam);
#else
	// test inverse	
	if (InverseMatrix(invmat, tmp))
	{
		//Sparse_Multiply(tmp, invmat, tranmat);
		tmp = prod(invmat, tranmat);
		return prod(tmp, 6.0*sam);
	}
	else
		return values();
#endif // USE_CLAPACK
}

values GetCubicBSpline( const values &control, int s_size )
{
	const std::size_t c_size = control.size();
	assert(s_size >= c_size);
	assert(c_size > 4);
	Matrix_row mat = MakeAMatrix(s_size, c_size);
	//std::cout << mat << std::endl;
	return prod(mat, control)/6.0;
}

Vector2s Get_C0_to_Cn_From_Values( const Vector2s &vals, int n )
{
	const std::size_t size = vals.size();
	if (n < 5)
		n = 5;
	Vector2s ans(n);
	for (int dim=0;dim<2;dim++)
	{
		values ublasvals(size);
		for (size_t row=0;row<size;row++)
			ublasvals[row] = vals[row][dim];
		values tmp = InverseCubicBSpline(ublasvals, n);
		for (size_t row=0;row<n;row++)
			ans[row][dim] = tmp[row];
	}
	return ans;
}

Vector2s Get_C0_to_Cn_From_Values( const Vector2s &vals )
{
	MMatrix_row matrix;
	const std::size_t size = vals.size();
	Vector2s ans(vals.size());
	for (int dim=0;dim<2;dim++)
	{
		values datas(size);
		for (size_t row=0;row<size;row++)
			datas[row] = vals[row][dim];
		CreateBSplineMatrix(matrix, datas);
		LinearlyIndependent(matrix);
		for (size_t row=0;row<size;row++)
			ans[row][dim] = matrix(row, size);
	}
	return ans;
}

Vector2s Get_Values_From_C0_to_Cn( const Vector2s &vals, int s_size )
{
	const std::size_t size = vals.size();
	Vector2s ans(s_size);
	for (size_t dim=0;dim<2;dim++)
	{
		values ublasvals(size);
		for (size_t row=0;row<size;row++)
			ublasvals[row] = vals[row][dim];
		values tmp = GetCubicBSpline(ublasvals, s_size);
		for (size_t row=0;row<s_size;row++)
			ans[row][dim] = tmp[row];
	}
	return ans;
}
#ifdef USE_CLAPACK
void svd( const boost::numeric::ublas::matrix<double>& A, boost::numeric::ublas::matrix<double>& U, boost::numeric::ublas::diagonal_matrix<double>& D, boost::numeric::ublas::matrix<double>& VT )
{
	namespace ublas = boost::numeric::ublas;

	std::vector<double>                        s((std::min)(A.size1(), A.size2()));
	ublas::matrix<double, ublas::column_major> CA(A), CU(A.size1(), A.size1()), CVT(A.size2(), A.size2());
	int                                        info;

	info = boost::numeric::bindings::lapack::gesdd('A', CA, s, CU, CVT);
	BOOST_UBLAS_CHECK(info == 0, ublas::internal_logic());

	ublas::matrix<double>          CCU(CU), CCVT(CVT);
	ublas::diagonal_matrix<double> CD(A.size1(), A.size2());

	for (size_t i = 0; i < s.size(); ++i) {
		CD(i, i) = s[i];
	}

#if BOOST_UBLAS_TYPE_CHECK
	BOOST_UBLAS_CHECK(
		ublas::detail::expression_type_check(ublas::prod(ublas::matrix<double>(ublas::prod(CCU, CD)), CCVT), A),
		ublas::internal_logic()
		);
#endif

	U.assign_temporary(CCU);
	D.assign_temporary(CD);
	VT.assign_temporary(CCVT);
}

void GetInverseBySVD( Matrix_row &dst, Matrix_row &src )
{
	namespace ublas = boost::numeric::ublas;
	ublas::matrix<double> U, VT;
	ublas::diagonal_matrix<double> D;
	svd(src, U, D, VT); // m = U x D x VT
	ublas::matrix<double> m1,m2,m3;
	m2 = trans(D);
	for (size_t i = 0; i < m2.size1(); i++) {
		for (size_t j = 0; j < m2.size2(); j++) {
			if (m2(i, j)!=0)
				m2(i, j) = 1.0/m2(i, j);
		}
	}
	m3 = trans(U);
	m1 = trans(VT);
	dst = prod(m2, m3);
	dst = prod(m1, dst);
}
#endif
