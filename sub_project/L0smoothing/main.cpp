#include <vtkSphereSource.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXYPlotActor.h>
#include <auto_link_vtk.hpp>

#include <iostream>
#include <vector>
#include <auto_link_fftw.hpp>
#include <auto_link_opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fftw3.h>

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef __int64     int64;
typedef unsigned __int64    uint64;

typedef std::vector<bool>   bool_vector;
typedef std::vector<uchar>  uchar_vector;
typedef std::vector<ushort> ushort_vector;
typedef std::vector<uint>   uint_vector;
typedef std::vector<double> double_vector;
typedef std::vector<double_vector>  double_vector2d;
typedef std::vector<float>  float_vector;
typedef std::vector<float_vector>   float_vector2d;
typedef std::vector<int>    int_vector;
typedef std::vector<std::vector<int> >  int_vector2d;
typedef std::vector<int64>  int64_vector;
typedef std::vector<std::vector<int64> > int64_vector2d;

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
typedef UBLAS::vector<num>      Vector;
typedef UBLAS::mapped_vector<num>   MVector;    //M stand for mapped
typedef UBLAS::compressed_vector<num>   SVector;    //S stand for small and slow
// generic matrix
typedef UBLAS::zero_matrix  <num>   ZMatrix;
typedef UBLAS::identity_matrix  <num>   IMatrix;
// matrix
typedef UBLAS::basic_row_major<int64, int64>        row_major_int64;
typedef UBLAS::basic_column_major<int64, int64>     col_major_int64;
typedef UBLAS::map_std<int64, num> map_std_int64;
typedef UBLAS::matrix       <num, col_major_int64>  Matrix_col;
typedef UBLAS::matrix       <num, row_major_int64>  Matrix_row;
typedef UBLAS::mapped_matrix    <num, col_major_int64, map_std_int64>
MMatrix_col; //M stand for mapped
typedef UBLAS::mapped_matrix    <num, row_major_int64, map_std_int64>
MMatrix_row;
typedef UBLAS::coordinate_matrix<num, col_major_int64>
CMatrix_col;    //C stand for coordinate
typedef UBLAS::coordinate_matrix<num, row_major_int64>  CMatrix_row;
typedef UBLAS::compressed_matrix<num, col_major_int64>
SMatrix_col;    //S stand for small and slow
typedef UBLAS::compressed_matrix<num, row_major_int64>  SMatrix_row;
typedef UBLAS::diagonal_matrix  <num, col_major_int64>  DMatrix_col;
typedef UBLAS::diagonal_matrix  <num, row_major_int64>  DMatrix_row;


//#define COL_MAJOR
#if !defined(ROW_MAJOR) && !defined(COL_MAJOR)
#pragma message( "if no define any major, default use ROW_MAJOR" )
#define ROW_MAJOR
#endif

#if defined(ROW_MAJOR)
typedef Matrix_row  Matrix;
typedef MMatrix_row MMatrix;
typedef CMatrix_row CMatrix;
typedef SMatrix_row SMatrix;
typedef DMatrix_row DMatrix;
#elif defined(COL_MAJOR)
typedef Matrix_col  Matrix;
typedef MMatrix_col MMatrix;
typedef CMatrix_col CMatrix;
typedef SMatrix_col SMatrix;
typedef DMatrix_col DMatrix;
#endif

struct Matrix2
{
	Matrix real;
	Matrix imge;
	Matrix2() {}
	Matrix2(int s1, int s2): real(s1, s2), imge(s1, s2)
	{}
	Matrix2 operator+(const Matrix2& rhs) const
	{
		Matrix2 tmp;
		tmp.imge = imge + rhs.imge;
		tmp.real = real + rhs.real;
		return tmp;
	}
	Matrix2 operator*(double rhs) const
	{
		Matrix2 tmp;
		tmp.imge = imge * rhs;
		tmp.real = real * rhs;
		return tmp;
	}
	Matrix2 operator/(double rhs) const
	{
		Matrix2 tmp;
		tmp.imge = imge / rhs;
		tmp.real = real / rhs;
		return tmp;
	}
	Matrix2 operator/(const Matrix& rhs) const
	{
		Matrix2 tmp(rhs.size1(), rhs.size2());
		for (int i = 0; i < rhs.size1(); ++i)
		{
			for (int j = 0; j < rhs.size2(); ++j)
			{
				tmp.imge(i, j) = imge(i, j) / rhs(i, j);
				tmp.real(i, j) = real(i, j) / rhs(i, j);
			}
		}
		return tmp;
	}
};

Matrix padarray(Matrix& pad, Matrix& outSize)
{
	Matrix out = ZMatrix(pad.size1() + outSize(0, 0), pad.size2() + outSize(0, 1));

	for (int i = 0; i < pad.size1(); ++i)
	{
		for (int j = 0; j < pad.size2(); ++j)
		{
			out(i, j) = pad(i, j);
		}
	}
	return out;
}

Matrix Matfloor(const Matrix& mat)
{
	Matrix out(mat.size1(), mat.size2());
	for (int i = 0; i < mat.size1(); ++i)
	{
		for (int j = 0; j < mat.size2(); ++j)
		{
			out(i, j) = floor(mat(i, j));
		}
	}
	return out;
}

Matrix circshift(const Matrix& a, const Matrix& p)
{
	Matrix out = ZMatrix(a.size1(), a.size2());
	Matrix sizeA(1, 2);
	const int numDimsA = 2;
	sizeA(0, 0) = a.size1();
	sizeA(0, 1) = a.size2();
	int_vector idx0, idx1;
	for (int i = 0; i < a.size1(); ++i)
	{
		idx0.push_back((i + a.size1() - (int)p(0, 0)) % a.size1());
	}
	for (int i = 0; i < a.size2(); ++i)
	{
		idx1.push_back((i + a.size2() - (int)p(0, 1)) % a.size2());
	}
	for (int i = 0; i < a.size1(); ++i)
	{
		for (int j = 0; j < a.size2(); ++j)
		{
			out(i, j) = a(idx0[i], idx1[j]);
		}
	}
	return out;
}

Matrix Matdiffinv(const Matrix& mat, int dim)
{
	Matrix out = ZMatrix(mat.size1(), mat.size2());
//  if (dim == 1)
//  {
//      for (int i = 0; i < mat.size1(); ++i)
//      {
//          for (int j = 0; j < mat.size2(); ++j)
//          {
//              out(i, j) = mat(i, j) - mat((i + 1) % mat.size1(), j);
//          }
//      }
//  }
//  else if (dim == 2)
//  {
//      for (int i = 0; i < mat.size1(); ++i)
//      {
//          for (int j = 0; j < mat.size2(); ++j)
//          {
//              out(i, j) = mat(i, j) - mat(i, (j + 1) % mat.size2());
//          }
//      }
//  }
	if (dim == 1)
	{
		for (int i = 0; i < mat.size1(); ++i)
		{
			for (int j = 0; j < mat.size2(); ++j)
			{
				out(i, j) = mat((i - 1 + mat.size1()) % mat.size1(), j) - mat(i, j);
			}
		}
	}
	else if (dim == 2)
	{
		for (int i = 0; i < mat.size1(); ++i)
		{
			for (int j = 0; j < mat.size2(); ++j)
			{
				out(i, j) = mat(i, (j - 1 + mat.size2()) % mat.size2()) - mat(i, j);
			}
		}
	}
	return out;
}

Matrix Matdiff(const Matrix& mat, int dim)
{
	Matrix out = ZMatrix(mat.size1(), mat.size2());
	if (dim == 1)
	{
		for (int i = 0; i < mat.size1(); ++i)
		{
			for (int j = 0; j < mat.size2(); ++j)
			{
				out(i, j) = mat((i + 1) % mat.size1(), j) - mat(i, j);
			}
		}
	}
	else if (dim == 2)
	{
		for (int i = 0; i < mat.size1(); ++i)
		{
			for (int j = 0; j < mat.size2(); ++j)
			{
				out(i, j) = mat(i, (j + 1) % mat.size2()) - mat(i, j);
			}
		}
	}
	return out;
}

Matrix MatPow2(Matrix& mat)
{
	Matrix out(mat.size1(), mat.size2());
	for (int i = 0; i < mat.size1(); ++i)
	{
		for (int j = 0; j < mat.size2(); ++j)
		{
			double tmp = mat(i, j);
			out(i, j) = tmp * tmp;
		}
	}
	return out;
}

void MatAdd(Matrix& mat, double val)
{
	for (int i = 0; i < mat.size1(); ++i)
	{
		for (int j = 0; j < mat.size2(); ++j)
		{
			mat(i, j) += val;
		}
	}
}

Matrix MatAbsPow2(Matrix2& mat)
{
	Matrix out = ZMatrix(mat.real.size1(), mat.real.size2());
	for (int i = 0; i < out.size1(); ++i)
	{
		for (int j = 0; j < out.size2(); ++j)
		{
			out(i, j) = mat.real(i, j) * mat.real(i, j) + mat.imge(i, j) * mat.imge(i, j);
		}
	}
	return out;
}

Matrix2 fft2(const Matrix& mat)
{
	fftw_complex*   data_in;
	fftw_complex*   fft;
	fftw_plan       plan_f;
	data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.size1() *
										 mat.size2());
	fft     = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.size1() *
										 mat.size2());
	plan_f = fftw_plan_dft_2d(mat.size1(), mat.size2(), data_in, fft,  FFTW_FORWARD,
							  FFTW_ESTIMATE);
	for (int i = 0, k = 0; i < mat.size1(); ++i)
	{
		for (int j = 0; j < mat.size2(); ++j)
		{
			data_in[k][0] = mat(i, j);
			data_in[k][1] = 0.0;
			k++;
		}
	}
	/* perform FFT */
	fftw_execute(plan_f);
	Matrix2 out(mat.size1(), mat.size2());
	for (int i = 0, k = 0; i < mat.size1(); ++i)
	{
		for (int j = 0; j < mat.size2(); ++j)
		{
			out.real(i, j) = fft[k][0];
			out.imge(i, j) = fft[k][1];
			k++;
		}
	}
	fftw_destroy_plan(plan_f);
	fftw_free(data_in);
	fftw_free(fft);
	return out;
}

Matrix2 fft2_1D(const Matrix& mat)
{
	fftw_complex*   data_in;
	fftw_complex*   fft;
	fftw_plan       plan_f;
	data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.size1() *
										 mat.size2());
	fft     = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.size1() *
										 mat.size2());
	plan_f = fftw_plan_dft_1d(mat.size1(), data_in, fft,  FFTW_FORWARD,
							  FFTW_ESTIMATE);
	for (int i = 0, k = 0; i < mat.size1(); ++i)
	{
		data_in[k][0] = mat(i, 0);
		data_in[k][1] = 0.0;
		k++;
	}
	/* perform FFT */
	fftw_execute(plan_f);
	Matrix2 out(mat.size1(), mat.size2());
	for (int i = 0, k = 0; i < mat.size1(); ++i)
	{
		out.real(i, 0) = fft[k][0];
		out.imge(i, 0) = fft[k][1];
		k++;
	}
	fftw_destroy_plan(plan_f);
	fftw_free(data_in);
	fftw_free(fft);
	return out;
}

Matrix ifft2(const Matrix2& mat)
{
	fftw_complex*   data_in;
	fftw_complex*   ifft;
	fftw_plan       plan_b;
	data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.imge.size1() *
										 mat.imge.size2());
	ifft    = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.imge.size1() *
										 mat.imge.size2());
	plan_b = fftw_plan_dft_2d(mat.imge.size1(), mat.imge.size2(), data_in, ifft,
							  FFTW_BACKWARD,  FFTW_ESTIMATE);
	for (int i = 0, k = 0; i < mat.imge.size1(); ++i)
	{
		for (int j = 0; j < mat.imge.size2(); ++j)
		{
			data_in[k][0] = mat.real(i, j);
			data_in[k][1] = mat.imge(i, j);
			k++;
		}
	}
	/* perform FFT */
	fftw_execute(plan_b);
	double normal_val = 1.0 / (mat.imge.size1() * mat.imge.size2());
	Matrix out(mat.imge.size1(), mat.imge.size2());
	for (int i = 0, k = 0; i < mat.imge.size1(); ++i)
	{
		for (int j = 0; j < mat.imge.size2(); ++j)
		{
			out(i, j) = ifft[k][0] * normal_val;
			k++;
		}
	}
	fftw_destroy_plan(plan_b);
	fftw_free(data_in);
	fftw_free(ifft);
	return out;
}

Matrix ifft2_1D(const Matrix2& mat)
{
	fftw_complex*   data_in;
	fftw_complex*   ifft;
	fftw_plan       plan_b;
	data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.imge.size1());
	ifft    = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mat.imge.size1());
	plan_b = fftw_plan_dft_1d(mat.imge.size1(), data_in, ifft,
							  FFTW_BACKWARD,  FFTW_ESTIMATE);
	for (int i = 0, k = 0; i < mat.imge.size1(); ++i)
	{
		data_in[k][0] = mat.real(i, 0);
		data_in[k][1] = mat.imge(i, 0);
		k++;
	}
	/* perform FFT */
	fftw_execute(plan_b);
	double normal_val = 1.0 / (mat.imge.size1() * mat.imge.size2());
	Matrix out(mat.imge.size1(), mat.imge.size2());
	for (int i = 0, k = 0; i < mat.imge.size1(); ++i)
	{
		for (int j = 0; j < mat.imge.size2(); ++j)
		{
			out(i, j) = ifft[k][0] * normal_val;
			k++;
		}
	}
	fftw_destroy_plan(plan_b);
	fftw_free(data_in);
	fftw_free(ifft);
	return out;
}

Matrix2 psf2otf(Matrix& psf, Matrix& outSize)
{
	Matrix psfSize(1, 2);
	psfSize(0, 0) = psf.size1();
	psfSize(0, 1) = psf.size2();
	Matrix padSize(1, 2);
	padSize = outSize - psfSize;
	psf = padarray(psf, padSize);
	psf = circshift(psf, -Matfloor(psfSize / 2));
	Matrix2 otf = fft2(psf);
	return otf;
}

cv::Mat L0Smoothing(cv::Mat Im, double lambda = 0.02, double kappa = 2.0)
{
	cv::Mat out(Im.rows, Im.cols, CV_32FC3);
	Matrix SR(Im.rows, Im.cols);
	Matrix SG(Im.rows, Im.cols);
	Matrix SB(Im.rows, Im.cols);
	for (int j = 0; j < Im.cols; ++j)
	{
		for (int i = 0; i < Im.rows; ++i)
		{
			cv::Vec3f& v1 = Im.at<cv::Vec3f>(i, j);
			SR(i, j) = v1[0];
			SG(i, j) = v1[1];
			SB(i, j) = v1[2];
		}
	}
	double betamax = 1e2;
	Matrix fx(1, 2);
	fx(0, 0) = 1;
	fx(0, 1) = -1;
	Matrix fy(2, 1);
	fy(0, 0) = 1;
	fy(1, 0) = -1;
	Matrix sizeI2D(1, 2);
	sizeI2D(0, 0) = Im.rows;
	sizeI2D(0, 1) = Im.cols;
	Matrix2 otfFx = psf2otf(fx, sizeI2D);
	Matrix2 otfFy = psf2otf(fy, sizeI2D);
	Matrix otfFx2 = MatAbsPow2(otfFx);
	Matrix otfFy2 = MatAbsPow2(otfFy);
	Matrix2 Normin1R = fft2(SR);
	Matrix2 Normin1G = fft2(SG);
	Matrix2 Normin1B = fft2(SB);
	Matrix Denormin2 = otfFx2 + otfFy2;
	float beta = 2 * lambda;
	while (beta < betamax)
	{
		float lb = lambda / beta;
		Matrix Denormin = beta * Denormin2;
		MatAdd(Denormin, 1);
		Matrix hR = Matdiff(SR, 2);
		Matrix vR = Matdiff(SR, 1);
		Matrix hG = Matdiff(SG, 2);
		Matrix vG = Matdiff(SG, 1);
		Matrix hB = Matdiff(SB, 2);
		Matrix vB = Matdiff(SB, 1);
		Matrix Pos2Sum = MatPow2(hR) + MatPow2(vR) +
						 MatPow2(hG) + MatPow2(vG) +
						 MatPow2(hB) + MatPow2(vB);
		for (int j = 0; j < Im.cols; ++j)
		{
			for (int i = 0; i < Im.rows; ++i)
			{
				if (Pos2Sum(i, j) < lb)
				{
					hR(i, j) = 0;
					vR(i, j) = 0;
					hG(i, j) = 0;
					vG(i, j) = 0;
					hB(i, j) = 0;
					vB(i, j) = 0;
				}
			}
		}
		Matrix Normin2R = Matdiffinv(hR, 2) + Matdiffinv(vR, 1);
		Matrix Normin2G = Matdiffinv(hG, 2) + Matdiffinv(vG, 1);
		Matrix Normin2B = Matdiffinv(hB, 2) + Matdiffinv(vB, 1);
		Matrix2 FSR = (Normin1R + fft2(Normin2R) * beta) / Denormin;
		Matrix2 FSG = (Normin1G + fft2(Normin2G) * beta) / Denormin;
		Matrix2 FSB = (Normin1B + fft2(Normin2B) * beta) / Denormin;
		SR = ifft2(FSR);
		SG = ifft2(FSG);
		SB = ifft2(FSB);
		beta = beta * kappa;
		printf(".");
		for (uint i = 0; i < out.rows; ++i)
		{
			for (uint j = 0; j < out.cols; ++j)
			{
				cv::Vec3f& v1 = out.at<cv::Vec3f>(i, j);
				v1[0] = SR(i, j);
				v1[1] = SG(i, j);
				v1[2] = SB(i, j);
			}
		}
		cv::imshow("out", out);
		cv::waitKey(1);
	}
	for (uint j = 0; j < out.cols; ++j)
	{
		for (uint i = 0; i < out.rows; ++i)
		{
			cv::Vec3f& v1 = out.at<cv::Vec3f>(i, j);
			v1[0] = SR(i, j);
			v1[1] = SG(i, j);
			v1[2] = SB(i, j);
		}
	}
	return out;
}

double_vector L0Smoothing_1D(double_vector data, int jump)
{
	double lambda = 0.01, kappa = 2.0;
	double_vector out(data.size());
	Matrix SR(data.size(), 1);
	for (int j = 0; j < 1; ++j)
	{
		for (int i = 0; i < data.size(); ++i)
		{
			SR(i, j) = data[i];
		}
	}
	double betamax = 1;
	Matrix fx(1, 2);
	fx(0, 0) = 1;
	fx(0, 1) = -1;
	Matrix fy(2, 1);
	fy(0, 0) = 1;
	fy(1, 0) = -1;
	Matrix sizeI2D(1, 2);
	sizeI2D(0, 0) = data.size();
	sizeI2D(0, 1) = 1;
	Matrix2 otfFx = psf2otf(fx, sizeI2D);
	Matrix2 otfFy = psf2otf(fy, sizeI2D);
	Matrix otfFx2 = MatAbsPow2(otfFx);
	Matrix otfFy2 = MatAbsPow2(otfFy);
	Matrix2 Normin1R = fft2_1D(SR);
	Matrix Denormin2 = otfFx2;
	float beta = 2 * lambda;
	while (beta < betamax)
	{
		float lb = lambda / beta;
		Matrix Denormin = ZMatrix(data.size(), 1);
		Denormin = beta * Denormin2;
		MatAdd(Denormin, 1);
		Matrix vR = Matdiff(SR, 1);
		Matrix Pos2Sum = MatPow2(vR);
		for (int j = 0; j < 1; ++j)
		{
			for (int i = 0; i < data.size(); ++i)
			{
				if (Pos2Sum(i, j) < lb)
				{
					vR(i, j) = 0;
				}
			}
		}
		Matrix Normin2R = Matdiffinv(vR, 1);
		Matrix2 FSR = (Normin1R + fft2_1D(vR) * beta)/Denormin;
		SR = ifft2_1D(FSR);
		beta = beta * kappa;
		printf(".");
		for (uint i = 0; i < data.size(); ++i)
		{
			for (uint j = 0; j < 1; ++j)
			{
				out[i] = SR(i, j);
			}
		}
	}
	return out;
}

int main(int argc, char** argv)
{
	cv::Mat     img1, img2, img3;

	/* check for supplied argument */
	if (argc < 3)
	{
		fprintf(stderr, "Usage: fft_image <filename> lambda\n");
		return 1;
	}

	/* load original image */
	img1 = cv::imread(argv[1]);
	img1.convertTo(img1, CV_32FC3, 1 / 255.0f);
	cv::imshow("input", img1);
	img2 = L0Smoothing(img1, atof(argv[2]));
	cv::imshow("result", img2);
	img2.convertTo(img2, CV_8UC3, 255.0f);
	cv::imwrite("result.png", img2);
	cv::waitKey();
	return 0;
}


/*
int main(int, char *[])
{
	vtkSmartPointer<vtkXYPlotActor> plot =
		vtkSmartPointer<vtkXYPlotActor>::New();
	plot->ExchangeAxesOff();
	plot->SetLabelFormat("%g");
	plot->SetXValuesToIndex();
	double_vector2d data;
	double_vector data1;
	// up flat
	double now = 190.0;
	for (int b = 0; b < 80; b++)
	{
		now += vtkMath::Random(-1, 1);
		data1.push_back(now);
	}
	// down
	for (int b = 0; b < 80; b += 4)
	{
		now -= 4;
		data1.push_back(now);
	}
	// down flat
	for (int b = 0; b < 10; b++)
	{
		now += vtkMath::Random(-1, 1);
		data1.push_back(now);
	}
	// up
	for (int b = 0; b < 80; b += 4)
	{
		now += 4;
		data1.push_back(now);
	}
	// up flat
	for (int b = 0; b < 80; b++)
	{
		now += vtkMath::Random(-1, 1);
		data1.push_back(now);
	}
	// down
	for (int b = 0; b < 80; b += 4)
	{
		now -= 4;
		data1.push_back(now);
	}
	// down flat
	for (int b = 0; b < 10; b++)
	{
		now += vtkMath::Random(-1, 1);
		data1.push_back(now);
	}
	// up
	for (int b = 0; b < 80; b += 4)
	{
		now += 4;
		data1.push_back(now);
	}
	// up flat
	for (int b = 0; b < 80; b++)
	{
		now += vtkMath::Random(-1, 1);
		data1.push_back(now);
	}
	data.push_back(data1);
	data.push_back(L0Smoothing_1D(data1, 4));
	for (unsigned int i = 0 ; i < 2 ; i++)
	{
		vtkSmartPointer<vtkDoubleArray> array_s =
			vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkFieldData> field =
			vtkSmartPointer<vtkFieldData>::New();
		vtkSmartPointer<vtkDataObject> dd =
			vtkSmartPointer<vtkDataObject>::New();
		for (int b = 0; b < data[i].size(); b++)
		{
			array_s->InsertValue(b, data[i][b]);
		}
		field->AddArray(array_s);
		dd->SetFieldData(field);
		plot->AddDataObjectInput(dd);
	}
	plot->SetPlotColor(0, 1, 0, 0);
	plot->SetPlotColor(1, 0, 1, 0);
	plot->SetWidth(1);
	plot->SetHeight(1);
	plot->SetPosition(0, 0);
	plot->SetPlotRange(0, 0, 360, 256);
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(plot);
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(500, 500);
	vtkSmartPointer<vtkRenderWindowInteractor> interactor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);
	// Initialize the event loop and then start it
	interactor->Initialize();
	interactor->Start();
	return EXIT_SUCCESS;
}
*/