#pragma once
#include <opencv2/opencv.hpp>
#include <d3d11.h>
#include <DxErr.h>
#include "Edge.h"

class vavImage
{
public:
	static void SetDx11Device(ID3D11Device* dev, ID3D11DeviceContext* devc);
	vavImage(const cv::Mat& im);
	vavImage(void);
	~vavImage(void);
	bool    ReadImage(std::string path);
	Lines   AnimaEdge(int kSize, float linkEndBound, float linkStartBound);
	Lines   SobelEdge(int kSize, float linkEndBound, float linkStartBound);
	ID3D11ShaderResourceView* GetDx11Texture();
	bool    Vaild();
	Vector2s    GetWhitePoints();
	int GetWidth()
	{
		return m_Image.cols;
	}
	int GetHeight()
	{
		return m_Image.rows;
	}
	const cv::Vec3b& GetColor(int x, int y) const;
	void    GetFeatureEdge(Lines& lines);
	void    TPSFromFeatureEdge(const Lines& lines);
	void    Threshold(int v);
	void    ShowEdgeLine(const Lines& lines);
	void    Resize(int x, int y, int method = cv::INTER_LINEAR);
	bool    CorrectPosition(int x, int y);
	double  GetLight(int x, int y) const;

	double  GetBilinearLight(double x, double y);
	double  GetBilinearR(double x, double y);
	double  GetBilinearG(double x, double y);
	double  GetBilinearB(double x, double y);

	// if any pixel is 0 return 0
	double  GetBilinearR_if0(double x, double y);
	double  GetBilinearG_if0(double x, double y);
	double  GetBilinearB_if0(double x, double y);

	double_vector GetRingLight(double x, double y, double radius, int div);
	double_vector GetRingR(double x, double y, double radius, int div);
	double_vector GetRingG(double x, double y, double radius, int div);
	double_vector GetRingB(double x, double y, double radius, int div);

	double_vector GetVerticalLight(double x, double y, double radius, int div);
	double_vector GetVerticalR(double x, double y, double radius, int div);
	double_vector GetVerticalG(double x, double y, double radius, int div);
	double_vector GetVerticalB(double x, double y, double radius, int div);

	double_vector GetHorizontalLight(double x, double y, double radius, int div);
	double_vector GetHorizontalR(double x, double y, double radius, int div);
	double_vector GetHorizontalG(double x, double y, double radius, int div);
	double_vector GetHorizontalB(double x, double y, double radius, int div);

	double_vector GetLineLight(double x1, double y1, double x2, double y2, int div);
	Vector3s GetLineColor(double x1, double y1, double x2, double y2, int div);
	Vector3 GetBilinearColor(double x, double y);

	bool	IsBlackLine(double x, double y, double radius);
	void	ToExpImage();
	cv::Mat GetCvMat()
	{
		return m_Image;
	}
	cv::Mat Clone()
	{
		return m_Image.clone();
	}
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
	static ID3D11Device* m_Device;
	static ID3D11DeviceContext* m_DeviceContext;
	cv::Mat m_Image;
};

