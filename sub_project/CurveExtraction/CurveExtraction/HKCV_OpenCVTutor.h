// OpenCV Datatype
CV_<bit-depth>{U|S|F}C(<number_of_channels>)
CV_8U - unsigned 8-bit integers
CV_8S - signed 8-bit integers
CV_16U - unsigned 16-bit integers
CV_16S - signed 16-bit integers
CV_32S - signed 32-bit integers
CV_32F - single precision floating-point numbers
CV_64F - double precision floating-point numbers

/// How to scan images?
/// 1. The efficient way
void Efficient_Samples(void)
{
	int i,j, nRows, nCols;
	
	Mat I(nRows, nCols, CV_8UC1);
	uchar* p;
	for( i = 0; i < I.rows; ++i)
	{
		p = I.ptr<uchar>(i);
		for ( j = 0; j < I.cols; ++j)
		{
			p[j] = 0;
		}
	}

	Mat I(nRows, nCols, CV_8UC3);
	uchar* p;
	for( i = 0; i < I.rows; ++i)
	{
		p = I.ptr<uchar>(i);
		for ( j = 0; j < I.cols; ++j, p+=3)
		{
			p[0] = 0;
			p[1] = 0;
			p[2] = 0;
		}
	}

	Mat I(nRows, nCols, CV_32FC1);
	float* p;
	for( i = 0; i < I.rows; ++i)
	{
		p = I.ptr<float>(i);
		for ( j = 0; j < I.cols; ++j)
		{
			p[j] = 0;
		}
	}

	/// The "data" data member of a Mat object returns the pointer to the first row, first column.
	/// Checking this is the simplest method to check if your image loading was a success.
	/// In case the storage is continues we can use this to go through the whole data pointer. 
	Mat I(nRows, nCols, CV_8UC1);
	uchar* p = I.data;
	for( unsigned int i =0; i < I.cols*I.rows; ++i )
	{
		p[i] = 0;
	}

	Mat I(nRows, nCols, CV_8UC1);
	uchar* p = I.data;
	for( unsigned int i =0; i < I.cols*I.rows; ++i, p++ )
	{
		*p = 0;
	}		
}

/// 2. The iterator (safe) method
void Iterator_Samples(void)
{
	int i,j, nRows, nCols;

	Mat I(nRows, nCols, CV_8UC1);
	MatIterator_<uchar> it, end;
	for( it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
	{
		*it = 0;
	}

	///  It¡¦s important to remember that OpenCV iterators go through the columns 
	///  and automatically skip to the next row. Therefore in case of color images 
	///  if you use a simple uchar iterator you¡¦ll be able to access only the blue channel values.
	Mat I(nRows, nCols, CV_8UC3);
	MatIterator_<Vec3b> it, end;
	for( it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
	{
		(*it)[0] = 0;
		(*it)[1] = 0;
		(*it)[2] = 0;
	}
}

/// 3. On-the-fly address calculation with reference returning
/// This method isn¡¦t recommended for scanning. It was made to acquire or modify somehow 
/// random elements in the image. 
void RandomAccess_Samples(void)
{
	int i,j, nRows, nCols;

	Mat I(nRows, nCols, CV_8UC1);
	for( int i = 0; i < I.rows; ++i)
	{
		for( int j = 0; j < I.cols; ++j )
		{
			I.at<uchar>(i,j) = 0;
		}
	}

	/// Note that Mat::at<_Tp>(int y, int x) and Mat_<_Tp>::operator ()(int y, int x) do 
	/// absolutely the same and run at the same speed, but the latter is certainly shorter.
	Mat I(nRows, nCols, CV_8UC3);
	Mat_<Vec3b> _I = I;
	for( int i = 0; i < I.rows; ++i)
	{
		for( int j = 0; j < I.cols; ++j )
		{
			_I(i,j)[0] = 0;
			_I(i,j)[1] = 0;
			_I(i,j)[2] = 0;
		}
	}
	I = _I;
}