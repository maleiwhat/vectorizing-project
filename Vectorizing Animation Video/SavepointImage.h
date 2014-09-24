#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class SavepointImage
{
public:
	SavepointImage(const std::string& path);
	~SavepointImage(void);
	bool hasImage();
	cv::Mat ReadImage();
	void SaveImage(cv::Mat img);
	cv::Mat m_Image;
private:
	std::string m_path;
	bool m_hasimg;
};

