/************************************************************************/
/*  This software is developed by Ming-Ming Cheng.				        */
/*       Url: http://cg.cs.tsinghua.edu.cn/people/~cmm/                 */
/*  This software is free fro non-commercial use. In order to use this	*/
/*  software for academic use, you must cite the corresponding paper:	*/
/*      Ming-Ming Cheng, Curve Structure Extraction for Cartoon Images, */
/*      in The 5th Joint Conference on Harmonious Human Machine			*/
/*      Environment (HHME), 2009, pp. 13-20.							*/
/************************************************************************/

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif


#include <stdio.h>
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

#pragma warning(disable:4996)
//// OpenCV 2.1
//#include <cvaux.h>
//#include <highgui.h>
//#ifdef _DEBUG
//#pragma comment(lib, "cxcore210d.lib")
//#pragma comment(lib, "cv210d.lib")
//#pragma comment(lib, "highgui210d.lib")
//#else
//#pragma comment(lib, "cxcore210.lib")
//#pragma comment(lib, "cv210.lib")
//#pragma comment(lib, "highgui210.lib")
//#endif // _DEBUG

// OpenCV
#include <opencv2/opencv.hpp> //OpenCV 2.2
// #ifdef _DEBUG
// #pragma comment(lib, "opencv_core220d.lib")
// #pragma comment(lib, "opencv_highgui220d.lib")
// #pragma comment(lib, "opencv_imgproc220d.lib")
// #else
// #pragma comment(lib, "opencv_core220.lib")
// #pragma comment(lib, "opencv_highgui220.lib")
// #pragma comment(lib, "opencv_imgproc220.lib")
// #endif // _DEBUG
#include <auto_link_opencv.hpp>

extern cv::Point const DIRECTION4[4];
extern cv::Point const DIRECTION8[8];
extern cv::Point const DIRECTION16[16];
extern float const DRT_ANGLE[8];
extern float const PI_FLOAT;
extern float const PI2;
extern float const PI_HALF;


const double EPS = 1e-8;		// Epsilon (zero value)
#define CHK_IND(p) ((p).x >= 0 && (p).x < m_w && (p).y >= 0 && (p).y < m_h)

template<typename T> inline int CmSgn(T number) {if(abs(number) < EPS) return 0; return number > 0 ? 1 : -1; }
