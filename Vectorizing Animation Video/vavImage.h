#pragma once
#include <opencv2/opencv.hpp>
#include <d3d11.h>
#include <DxErr.h>
#include "Edge.h"

class vavImage
{
public:
	static void SetDx11Device(ID3D11Device* dev);
	vavImage(const cv::Mat& im);
	vavImage(void);
	~vavImage(void);
	bool	ReadImage(std::string path);
	Lines	AnimaEdge(int kSize, float linkEndBound, float linkStartBound);
	Lines	SobelEdge(int kSize, float linkEndBound, float linkStartBound);
	ID3D11ShaderResourceView* GetDx11Texture();
	bool	Vaild();
	Vector2s	GetWhitePoints();
	int	GetWidth() {return m_Image.cols;}
	int	GetHeight() {return m_Image.rows;}
	const cv::Vec3b& GetColor(int x, int y) const;
	void	GetFeatureEdge(Lines& lines);
	void	TPSFromFeatureEdge(const Lines& lines);
	void	Threshold(int v);
	void	ShowEdgeLine(const Lines& lines);
	void	Resize(int x, int y, int method = cv::INTER_LINEAR);
	bool	CorrectPosition(int x, int y);
	operator cv::Mat&()
	{
		return m_Image;
	}
	vavImage& operator = (const cv::Mat& mat)
	{
		m_Image = mat.clone();
		return *this;
	}
private:
	static ID3D11Device* m_Device;
	cv::Mat	m_Image;
};

