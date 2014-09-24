#include "SavepointImage.h"


SavepointImage::SavepointImage(const std::string& path)
{
	m_hasimg = false;
	m_path = path;
	FILE* p_File;
	p_File = fopen(path.c_str(), "rb");
	if(p_File == NULL)
	{
		return;
	}
	else
	{
		fseek(p_File, 0, SEEK_END);
		int size = ftell(p_File);
		if(size > 100)
		{ m_hasimg = true; }
	}
	fclose(p_File);
}

SavepointImage::~SavepointImage(void)
{
}

void SavepointImage::SaveImage(cv::Mat img)
{
	cv::imwrite(m_path, img);
	cv::waitKey(10);
}

bool SavepointImage::hasImage()
{
	return m_hasimg;
}

cv::Mat SavepointImage::ReadImage()
{
	m_Image = cv::imread(m_path);
	return m_Image;
}
