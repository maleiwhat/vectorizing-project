#include "stdafx.h"
#include "imath.h"
#include <vector>

void Transpose( MMatrix_col& m_out, const MMatrix_row& m_in )
{
	m_out.clear();
	m_out.resize( m_in.size2(), m_in.size1() );
	MMatrix_col& TA = m_out;

	for ( MMatrix_row::const_iterator1 it1 = m_in.begin1(); it1 != m_in.end1(); ++it1 )
	{
		for ( MMatrix_row::const_iterator2 it2 = it1.begin(); it2 != it1.end(); ++it2 )
		{
			TA.insert_element( it2.index2(), it2.index1(), *it2 );
		}
	}
}

void Transpose( MMatrix_row& m_out, const MMatrix_col& m_in )
{
	m_out.clear();
	m_out.resize( m_in.size2(), m_in.size1() );
	MMatrix_row& TA = m_out;

	for ( MMatrix_col::const_iterator2 it1 = m_in.begin2(); it1 != m_in.end2(); ++it1 )
	{
		for ( MMatrix_col::const_iterator1 it2 = it1.begin(); it2 != it1.end(); ++it2 )
		{
			TA.insert_element( it2.index2(), it2.index1(), *it2 );
		}
	}
}

template <class MATRIX_TYPE>
void Sparse_Multiply( MATRIX_TYPE& out, const MMatrix_row& A, const MMatrix_col& B )
{
	out.clear();
	out.resize( A.size1(), B.size2() );
	MMatrix_row::const_iterator1 it1 = A.begin1();
	MMatrix_col::const_iterator2 it2 = B.begin2();

	for ( ; it1 != A.end1() && it2 != B.end2(); )
	{
		if ( it1.index1() == it2.index2() )
		{
			MMatrix_row::const_iterator2 itx1 = it1.begin();
			MMatrix_col::const_iterator1 itx2 = it2.begin();
			double sum = 0;

			for ( ; itx1 != it1.end() && itx2 != it2.end(); )
			{
				if ( itx1.index1() == itx2.index2() )
				{
					sum += *itx1 * *itx2;
					++itx1; ++itx2;
				}
				else if ( itx1.index1() < itx2.index2() )
				{
					++itx1;
				}
				else
				{
					++itx2;
				}
			}

			if ( sum != 0 )
			{
				out.insert_element( it1.index1(), it2.index2(), sum );
			}

			++it1; ++it2;
		}
		else if ( it1.index1() < it2.index2() )
		{
			++it1;
		}
		else
		{
			++it2;
		}
	}
}

MMatrix_row MultiplyATA( const MMatrix_row& A )
{
	MMatrix_col TA;
	Transpose( TA, A );
	MMatrix_row M;
	Sparse_Multiply( M, A, TA );
	return M;
}

MMatrix_col MultiplyATA( const MMatrix_col& A )
{
	MMatrix_row tA( A.size1(), A.size2() );

	for ( MMatrix_col::const_iterator2 it1 = A.begin2(); it1 != A.end2(); ++it1 )
	{
		for ( MMatrix_col::const_iterator1 it2 = it1.begin(); it2 != it1.end(); ++it2 )
		{
			tA.insert_element( it2.index1(), it2.index2(), *it2 );
		}
	}

	MMatrix_row tM = MultiplyATA( tA );
	MMatrix_col M( tM.size1(), tM.size2() );

	for ( MMatrix_row::const_iterator1 it1 = tM.begin1(); it1 != tM.end1(); ++it1 )
	{
		for ( MMatrix_row::const_iterator2 it2 = it1.begin(); it2 != it1.end(); ++it2 )
		{
			M.insert_element( it2.index1(), it2.index2(), *it2 );
		}
	}

	return M;
}

#ifdef USE_CLAPACK
void svd( const Matrix& A, Matrix& U, DMatrix& D, Matrix& VT )
{
	namespace ublas = boost::numeric::ublas;
	double_vector	s( ( std::min )( A.size1(), A.size2() ) );
	ublas::matrix<double, ublas::column_major> CA( A ), CU( A.size1(), A.size1() ), CVT( A.size2(), A.size2() );
	int	info;
	info = boost::numeric::bindings::lapack::gesdd( 'A', CA, s, CU, CVT );
	BOOST_UBLAS_CHECK( info == 0, ublas::internal_logic() );
	Matrix CCU( CU ), CCVT( CVT );
	DMatrix CD( A.size1(), A.size2() );

	for ( size_t i = 0; i < s.size(); ++i )
	{
		CD( i, i ) = s[i];
	}

#if BOOST_UBLAS_TYPE_CHECK
	BOOST_UBLAS_CHECK(
	        ublas::detail::expression_type_check( ublas::prod( Matrix( ublas::prod( CCU, CD ) ), CCVT ), A ),
	        ublas::internal_logic()
	);
#endif
	U.assign_temporary( CCU );
	D.assign_temporary( CD );
	VT.assign_temporary( CCVT );
}

void GetInverseBySVD( Matrix& dst, Matrix& src )
{
	namespace ublas = boost::numeric::ublas;
	Matrix U, VT;
	DMatrix D;
	svd( src, U, D, VT ); // m = U x D x VT
	ublas::matrix<double> m1, m2, m3;
	m2 = trans( D );

	for ( size_t i = 0; i < m2.size1(); i++ )
	{
		for ( size_t j = 0; j < m2.size2(); j++ )
		{
			if ( m2( i, j ) != 0 )
			{
				m2( i, j ) = 1.0 / m2( i, j );
			}
		}
	}

	m3 = trans( U );
	m1 = trans( VT );
	dst = prod( m2, m3 );
	dst = prod( m1, dst );
}

#endif

/* no use
class CCMMatrix
{
private:
	int m;
	int n;
	std::vector <int>  rowIndex;
	std::vector <int> colIndex;
	std::vector <double>  values;

public:
	int rowSize()
	{
		return m;
	}
	int colSize()
	{
		return n;
	}
};
*/
