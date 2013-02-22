#ifndef _HKCV_UTILS_H_
#define _HKCV_UTILS_H_
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

namespace HKCV {

	/*---------------------------------------------------------------------*/		
	/*! \name                     Typedef                                  */
	/*! \{                                                                 */

	typedef std::vector<cv::Point>	VecPnt2i;
	typedef std::vector<VecPnt2i>	VecPnt2is;
	typedef std::vector<cv::Point2d>	VecPnt2d;
	typedef std::vector<cv::Vec3d>	VecVec3d;
	typedef std::vector<cv::Vec4i>   VecVec4i;
	typedef std::vector<cv::Mat>		VecMat;

	/*! \}                                                                 */
	/*---------------------------------------------------------------------*/		
	/*! \name                 Utility Function                             */
	/*! \{                                                                 */
	template<typename T> inline T sqr(T x) { return x * x;}

	/// OpenCV Point utilities
	template<class T> inline T CvPntDist(const cv::Point_<T> &p1, const cv::Point_<T> &p2) {return sqrt(sqr(p1.x - p2.x) + sqr(p1.y - p2.y));}
	template<class T> inline T CvPntSqrDist(const cv::Point_<T> &p1, const cv::Point_<T> &p2) {return sqr(p1.x - p2.x) + sqr(p1.y - p2.y);}

	/// OpenCV Vec utilities
	template<class T> inline T CvVec3Dist(const cv::Vec<T, 3> &v1, const cv::Vec<T, 3> &v2) {return sqrt(sqr(v1[0] - v2[0])+sqr(v1[1] - v2[1])+sqr(v1[2] - v2[2]));}
	template<class T> inline T CvVec3SqrDist(const cv::Vec<T, 3> &v1, const cv::Vec<T, 3> &v2) {return sqr(v1[0] - v2[0])+sqr(v1[1] - v2[1])+sqr(v1[2] - v2[2]);}
	//template<class T1, class T2> inline void operator /= (cv::Vec<T1, 3> &v1, const T2 v2) { v1[0] /= v2; v1[1] /= v2; v1[2] /= v2; }
	template<class T> inline void operator /= (cv::Vec<T, 3> &v1, const cv::Scalar s) { v1[0] /= s[0]; v1[1] /= s[1]; v1[2] /= s[2]; }
	template<class T> inline void operator *= (cv::Vec<T, 3> &v1, const cv::Scalar s) { v1[0] *= s[0]; v1[1] *= s[1]; v1[2] *= s[2]; }
	template<class T> inline T CvVec2Norm(const cv::Vec<T, 2> &_v) {return sqrt(sqr(_v[0])+sqr(_v[1]));}

	/// OpenCV Size utilities
	inline bool operator == (const cv::Size _lhs, const cv::Size _rhs) { return (_lhs.width == _rhs.width && _lhs.height == _rhs.height); }
	inline double CvDiagLength(const cv::Size& _size) { return sqrt(sqr((double)_size.width)+sqr((double)_size.height)); }	
	inline double CvDiagLength(const cv::Rect& _rect) { return CvDiagLength(_rect.size()); }	

	/// OpenCV Rect utilities
#define CHECK_RECT(r) (r.x >=0 && r.y >= 0 && r.width > 0 && r.height > 0)		

	/// Pad _size.width and _size.height pixels to _rect's width and height
	/// Note: size(1,1) indicates _rect's width and height will increase by 2
	inline cv::Rect CvRectPadding(const cv::Rect& _rect, cv::Size _size)
	{
		cv::Rect padRect = _rect;
		padRect += cv::Size(_size.width*2, _size.height*2);
		padRect -= cv::Point(_size.width, _size.height);
		return padRect;
	}

	inline std::ostream& operator<<(std::ostream &_os, const cv::Rect& _rect)
	{		
		_os << "Rect: [" << _rect.x << "," << _rect.y << "," << _rect.width << "," << _rect.height << "]";
		return _os;
	}

	/// overload operator
	template< class T, int D>
	std::ostream& operator<<(std::ostream &_os, const cv::Vec<T, D> &_vec)
	{		
		int size = _vec.rows;
		_os << "[" << (unsigned int)size << "](";
		if (size > 0)
			_os << _vec[0];
		for(int i = 1; i < size; ++i)
			_os << "," << _vec[i];
		_os << ")";

		return _os;
	}	
	
	/// OpenCV template help utilities
	template<typename T>
	inline void cvMat2Array(const cv::Mat& _mat, double* _array)
	{
		/// The input should be channel 1 OR channel 3 with type uchar (8U)
		if(!_mat.data || _mat.channels() != 1)
		{
			CV_Error(CV_StsBadArg, "_mat has incorrect data or channel (only support single channel for now...)");	
			return;
		}			

		/// copy data to mxArray
		for(int y = 0; y < _mat.rows; ++y)
		{
			const T* val_ptr = _mat.ptr<T>(y);
			for(int x = 0; x < _mat.cols; ++x, val_ptr++)
				_array[x*_mat.rows+y] = double(*val_ptr);
		}
	}

	/*! \}                                                                 */
}

#endif