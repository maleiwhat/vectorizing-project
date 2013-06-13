#pragma once
#include <opencv2/opencv.hpp>
#include <d3d11.h>
#include <DxErr.h>
#include "Line.h"
#include "math\BasicMath.h"

class vavImage
{
public:
	vavImage(const cv::Mat& im);
	vavImage(void);
	~vavImage(void);
	bool    ReadImage(std::string path);
	ID3D11ShaderResourceView* GetDx11Texture(ID3D11Device* dev,
	        ID3D11DeviceContext* devc);
	bool    Vaild();
	Vector2s    GetWhitePoints();
	int GetWidth() {return m_Image.cols;}
	int GetHeight() {return m_Image.rows;}
	const cv::Vec3b& GetColor(int x, int y) const;
	double  GetLight(int x, int y) const;
	void    GetFeatureEdge(Lines& lines);
	void    Threshold(int v);
	void    ShowEdgeLine(const Lines& lines);
	void    Resize(int x, int y, int method = cv::INTER_LINEAR);
	bool    CorrectPosition(int x, int y);
	double  GetBilinearLight(double x, double y);
	double_vector GetRingLight(double x, double y, double radius, int div);
	operator cv::Mat& ()
	{
		return m_Image;
	}
	vavImage& operator = (const cv::Mat& mat)
	{
		m_Image = mat.clone();
		return *this;
	}
private:
	cv::Mat m_Image;
};

