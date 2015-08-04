#include "stdafx.h"
#include <auto_link_opencv.hpp>
#include <algorithm>
#include "vavImage.h"
#include "TPSInterpolate.hpp"
#include "math\Quaternion.h"
#include "CmCurveEx.h"
#include "CvExtenstion.h"
#include "IFExtenstion.h"


ID3D11Device* vavImage::m_Device;
ID3D11DeviceContext* vavImage::m_DeviceContext;


vavImage::vavImage(void)
{
	m_texture = NULL;
	m_pTextureRead = NULL;
	m_pTextureDraw = NULL;
}

vavImage::vavImage(const cv::Mat& im)
{
	m_Image = im.clone();
	m_texture = NULL;
	m_pTextureRead = NULL;
	m_pTextureDraw = NULL;
}

ID3D11ShaderResourceView* vavImage::GetDx11Texture()
{
	if (m_texture)
	{
		return m_texture;
	}
	if (m_Image.rows == 0 || m_Image.cols == 0)
	{
		return NULL;
	}
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width     = m_Image.rows;
	texDesc.Height    = m_Image.cols;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = texDesc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;
	float* characterImages = new float[m_Image.rows * m_Image.cols * 4];
	for (int j = 0; j < m_Image.cols; ++j)
	{
		for (int i = 0; i < m_Image.rows; ++i)
		{
			int offset = (j * m_Image.rows + i) * 4;
			cv::Vec3b intensity = m_Image.at<cv::Vec3b>(i, j);
			characterImages[offset  ] = intensity[2] / 255.0f;
			characterImages[offset + 1] = intensity[1] / 255.0f;
			characterImages[offset + 2] = intensity[0] / 255.0f;
			characterImages[offset + 3] = 1.0f;
		}
	}
	texDesc.Width = m_Image.rows;
	texDesc.Height = m_Image.cols;
	D3D11_SUBRESOURCE_DATA sSubData;
	sSubData.SysMemPitch = (UINT)(m_Image.rows * 4 * 4);
	sSubData.SysMemSlicePitch = (UINT)(m_Image.rows * m_Image.cols * 4 * 4);
	sSubData.pSysMem = characterImages;
	
	HRESULT d3dResult = m_Device->CreateTexture2D(&texDesc, &sSubData,
						&m_pTextureDraw);
	ID3D11ShaderResourceView* pShaderResView;
	int x = 0;
	delete[] characterImages;
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG(L"vavImage: Failed to create texture2D!");
		return 0;
	}
	d3dResult = m_Device->CreateShaderResourceView(m_pTextureDraw, &srDesc,
				&pShaderResView);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG(L"vavImage: Failed to create ShaderResourceView!");
		return 0;
	}
	if (0)
	{
		// take draw texture
		
		D3D11_TEXTURE2D_DESC texDescCV;
		ZeroMemory(&texDescCV, sizeof(texDescCV));
		texDescCV.Width     = m_Image.rows;
		texDescCV.Height    = m_Image.cols;
		texDescCV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDescCV.MipLevels = 1;
		texDescCV.ArraySize = 1;
		texDescCV.SampleDesc.Quality = 0;
		texDescCV.SampleDesc.Count = 1;
		texDescCV.MiscFlags = 0;
		texDescCV.Usage = D3D11_USAGE_STAGING;
		texDescCV.BindFlags = 0;
		texDescCV.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		HR(m_Device->CreateTexture2D(&texDescCV, 0, &m_pTextureRead));
		m_DeviceContext->CopyResource(m_pTextureRead, m_pTextureDraw);
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		float* pimg;
		//HR(m_DeviceContext->Map(pTextureRead, 0, D3D11_MAP_READ, 0, &MappedResource));
		unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
		HR(m_DeviceContext->Map(m_pTextureRead, subresource, D3D11_MAP_READ, 0,
								&MappedResource));
		pimg = (float*)MappedResource.pData;
		cv::Mat simg = m_Image.clone();
		simg = cv::Scalar(0);
		int addoffset = (8 - (simg.rows - (simg.rows / 8 * 8))) % 8;
		printf("addoffset: %d \n", addoffset);
		for (int j = 0; j < simg.cols; ++j)
		{
			for (int i = 0; i < simg.rows; ++i)
			{
				int offset = (j * (simg.rows + addoffset) + i) * 4;
				cv::Vec3b& intensity = simg.at<cv::Vec3b>(i, j);
				intensity[2] = pimg[offset  ] * 255.0f;
				intensity[1] = pimg[offset + 1] * 255.0f;
				intensity[0] = pimg[offset + 2] * 255.0f;
			}
		}
		//cv::imshow("simg", simg);
	}
	m_texture = pShaderResView;
	return pShaderResView;
}
vavImage::~vavImage(void)
{
	if (m_texture)
	{
		m_texture->Release();
		m_pTextureRead->Release();
		m_pTextureDraw->Release();
	}
}
bool vavImage::ReadImage(std::string path)
{
	m_texture = NULL;
	m_Image = cv::imread(path.c_str());
	if (m_Image.cols > 0 && m_Image.rows > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void vavImage::SetDx11Device(ID3D11Device* dev, ID3D11DeviceContext* devc)
{
	m_Device = dev;
	m_DeviceContext = devc;
}
bool vavImage::Vaild()
{
	if (m_Image.cols > 0 && m_Image.rows > 0)
	{
		return true;
	}
	return false;
}
Vector2s vavImage::GetWhitePoints()
{
	Vector2s out;
	for (int j = 0; j < m_Image.cols; ++j)
	{
		for (int i = 0; i < m_Image.rows; ++i)
		{
			cv::Vec3b intensity = m_Image.at<cv::Vec3b>(i, j);
			if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			{
				out.push_back(Vector2(i, j));
				//std::cout << i << " " << j << std::endl;
			}
		}
	}
	return out;
}
const cv::Vec3b& vavImage::GetColor(int x, int y) const
{
	if (y < 0)
	{
		x = 0;
	}
	if (y >= m_Image.rows)
	{
		y = m_Image.rows - 1;
	}
	if (x < 0)
	{
		y = 0;
	}
	if (x >= m_Image.cols)
	{
		x = m_Image.cols - 1;
	}
	return m_Image.at<cv::Vec3b>(y, x);
}
void vavImage::GetFeatureEdge(Lines& lines)
{
	return;
	for (Lines::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		Line& li = *it;
		if (li.size() == 3)
		{
			li.erase(li.begin() + 1);
		}
		else if (li.size() > 3)
		{
			Vector2 lastmove = li[1] - li[0];
			for (int i = 2; i < li.size() - 1; ++i)
			{
				Vector2 move = li[i + 1] - li[i];
				if (move == lastmove)
				{
					li[i - 1].x = -999;
				}
				lastmove = move;
			}
			for (int i = 1; i < li.size(); ++i)
			{
				if (li[i].x == -999)
				{
					li.erase(li.begin() + i);
					i--;
				}
			}
		}
	}
}
typedef std::array<double, 2> double2;
typedef std::array<double, 3> double3;
void vavImage::TPSFromFeatureEdge(const Lines& lines)
{
	std::vector< double2 >  para_color;
	std::vector< double3 >  values_color;
	for (Lines::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		for (Line::const_iterator it2 = it->begin(); it2 != it->end(); ++it2)
		{
			double2 v2;
			v2[0] = it2->x;
			v2[1] = it2->y;
			para_color.push_back(v2);
			const cv::Vec3b color = GetColor(it2->x, it2->y);
			double3 v3;
			v3[0] = color[0];
			v3[1] = color[1];
			v3[2] = color[2];
			values_color.push_back(v3);
		}
	}
	std::cout << "para_color: " << para_color.size()  << "values_color: " <<
			  values_color.size() << std::endl;
	ThinPlateSpline<2, 3> TPS_color(para_color, values_color);
	std::array<double, 2> curPos;
	std::array<double, 3> cp;
	for (int j = 0; j < m_Image.cols; ++j)
	{
		for (int i = 0; i < m_Image.rows; ++i)
		{
			cv::Vec3b& intensity = m_Image.at<cv::Vec3b>(i, j);
			curPos[0] = j;
			curPos[1] = i;
			cp = TPS_color.interpolate(curPos);
			intensity[0] = cp[0];
			intensity[1] = cp[1];
			intensity[2] = cp[2];
		}
	}
}
void vavImage::Threshold(int v)
{
	v *= 3;
	for (int j = 1; j < m_Image.cols - 1; ++j)
	{
		for (int i = 1; i < m_Image.rows - 1; ++i)
		{
			cv::Vec3b& intensity = m_Image.at<cv::Vec3b>(i, j);
			if ((intensity[0] + intensity[1] + intensity[2]) >= v)
			{
				intensity[0] = 255;
				intensity[1] = 255;
				intensity[2] = 255;
			}
			else
			{
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
			}
		}
	}
}
void vavImage::ShowEdgeLine(const Lines& li)
{
	for (Lines::const_iterator it = li.begin(); it != li.end(); ++it)
	{
		int R = 255, B = 255, G = 255;
		for (Line::const_iterator it2 = it->begin(); it2 != it->end(); ++it2)
		{
			cv::Vec3b& intensity = m_Image.at<cv::Vec3b>(it2->y, it2->x);
			intensity[0] = R;
			intensity[1] = G;
			intensity[2] = B;
		}
	}
}
void vavImage::Resize(int x, int y, int method)
{
	cv::Mat tmp = m_Image;
	resize(tmp, m_Image, cv::Size(x, y), 0, 0, method);
}
bool vavImage::CorrectPosition(int x, int y)
{
	if (x >= 0 && y >= 0 && x < m_Image.cols && y < m_Image.rows)
	{
		return true;
	}
	return false;
}
Lines vavImage::AnimaEdge(int kSize, float linkEndBound, float linkStartBound)
{
	cv::Mat srcImg1f, srcImg2f, show3u = cv::Mat::zeros(m_Image.size(), CV_8UC3);
	cvtColor(m_Image, srcImg1f, CV_BGR2HSV);
	for (int x = 0; x < srcImg1f.cols; ++x)
	{
		for (int y = 0; y < srcImg1f.rows; ++y)
		{
			cv::Vec3b& c = srcImg1f.at<cv::Vec3b>(y, x);
			c[0] = c[2];
			c[1] = c[2];
		}
	}
	cvtColor(srcImg1f, srcImg1f, CV_BGR2GRAY);
	srcImg1f.convertTo(srcImg2f, CV_32FC1, 1.0 / 255);
	CmCurveEx dEdge(srcImg2f);
	dEdge.CalSecDer(kSize, linkEndBound, linkStartBound);
	dEdge.Link();
	return GetLines(dEdge.GetEdges());
}
Lines vavImage::SobelEdge(int kSize, float linkEndBound, float linkStartBound)
{
	cv::Mat srcImg1f, srcImg2f, show3u = cv::Mat::zeros(m_Image.size(), CV_8UC3);
	cvtColor(m_Image, srcImg1f, CV_BGR2HSV);
	for (int x = 0; x < srcImg1f.cols; ++x)
	{
		for (int y = 0; y < srcImg1f.rows; ++y)
		{
			cv::Vec3b& c = srcImg1f.at<cv::Vec3b>(y, x);
			c[0] = c[2];
			c[1] = c[2];
		}
	}
	cvtColor(srcImg1f, srcImg1f, CV_BGR2GRAY);
	srcImg1f.convertTo(srcImg2f, CV_32FC1, 1.0 / 255);
	CmCurveEx dEdge(srcImg2f);
	dEdge.CalFirDer(kSize, linkEndBound, linkStartBound);
	dEdge.Link();
	return GetLines(dEdge.GetEdges());
}

double vavImage::GetBilinearLight(double x, double y)
{
	if (y < 0)
	{
		return 0;
	}
	if (y + 1 >= m_Image.rows)
	{
		return 0;
	}
	if (x < 0)
	{
		return 0;
	}
	if (x + 1 >= m_Image.cols)
	{
		return 0;
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	v[0] = GetLight(left_up.x, left_up.y);
	v[1] = GetLight(left_down.x, left_down.y);
	v[2] = GetLight(right_up.x, right_up.y);
	v[3] = GetLight(right_down.x, right_down.y);
	// bilinear interpolation
	double up = v[0] * abs(x - right_down.x);
	up += v[2] * abs(x - left_down.x);
	double down = v[1] * abs(x - right_up.x);
	down += v[3] * abs(x - left_up.x);
	double ans = up * abs(y - left_down.y) + down * abs(y - left_up.y);
	return ans;
}

double vavImage::GetLight(int x, int y) const
{
	const cv::Vec3b& v = m_Image.at<cv::Vec3b>(y, x);
	return 0.299 * v[2] + 0.587 * v[1] + 0.114 * v[0];
}

double vavImage::GetBilinearR(double x, double y)
{
	if (y < 0 || (y + 1 >= m_Image.rows) || x < 0 || (x + 1 >= m_Image.cols))
	{
		if (y < 0)
		{
			y = 0;
		}
		if (x < 0)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 1;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 1;
		}
		return m_Image.at<cv::Vec3b>((int)y, (int)x)[2];
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[2];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[2];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[2];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[2];
	// bilinear interpolation
	double up = v[0] * abs(x - right_down.x);
	up += v[2] * abs(x - left_down.x);
	double down = v[1] * abs(x - right_up.x);
	down += v[3] * abs(x - left_up.x);
	double ans = up * abs(y - left_down.y) + down * abs(y - left_up.y);
	return ans;
}

double vavImage::GetBilinearG(double x, double y)
{
	if (y < 0 || (y + 1 >= m_Image.rows) || x < 0 || (x + 1 >= m_Image.cols))
	{
		if (y < 0)
		{
			y = 0;
		}
		if (x < 0)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 1;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 1;
		}
		return m_Image.at<cv::Vec3b>((int)y, (int)x)[1];
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[1];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[1];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[1];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[1];
	// bilinear interpolation
	double up = v[0] * abs(x - right_down.x);
	up += v[2] * abs(x - left_down.x);
	double down = v[1] * abs(x - right_up.x);
	down += v[3] * abs(x - left_up.x);
	double ans = up * abs(y - left_down.y) + down * abs(y - left_up.y);
	return ans;
}

double vavImage::GetBilinearB(double x, double y)
{
	if (y < 0 || (y + 1 >= m_Image.rows) || x < 0 || (x + 1 >= m_Image.cols))
	{
		if (y < 0)
		{
			y = 0;
		}
		if (x < 0)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 1;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 1;
		}
		return m_Image.at<cv::Vec3b>((int)y, (int)x)[0];
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[0];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[0];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[0];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[0];
	// bilinear interpolation
	double up = v[0] * abs(x - right_down.x);
	up += v[2] * abs(x - left_down.x);
	double down = v[1] * abs(x - right_up.x);
	down += v[3] * abs(x - left_up.x);
	double ans = up * abs(y - left_down.y) + down * abs(y - left_up.y);
	return ans;
}


double vavImage::GetBilinearR_if0(double x, double y)
{
	if (y < 1 || (y + 1 >= m_Image.rows) || x < 1 || (x + 1 >= m_Image.cols))
	{
		if (y < 1)
		{
			y = 0;
		}
		if (x < 1)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 1;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 1;
		}
		return m_Image.at<cv::Vec3b>((int)y, (int)x)[2];
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[2];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[2];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[2];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[2];
	if (v[0]*v[1]*v[2]*v[3] < 1)
	{
		return 0;
	}
	// bilinear interpolation
	double up = v[0] * abs(x - right_down.x);
	up += v[2] * abs(x - left_down.x);
	double down = v[1] * abs(x - right_up.x);
	down += v[3] * abs(x - left_up.x);
	double ans = up * abs(y - left_down.y) + down * abs(y - left_up.y);
	return ans;
}

double vavImage::GetBilinearG_if0(double x, double y)
{
	if (y < 1 || (y + 1 >= m_Image.rows) || x < 1 || (x + 1 >= m_Image.cols))
	{
		if (y < 1)
		{
			y = 0;
		}
		if (x < 1)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 1;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 1;
		}
		return m_Image.at<cv::Vec3b>((int)y, (int)x)[1];
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[1];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[1];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[1];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[1];
	if (v[0]*v[1]*v[2]*v[3] < 1)
	{
		return 0;
	}
	// bilinear interpolation
	double up = v[0] * abs(x - right_down.x);
	up += v[2] * abs(x - left_down.x);
	double down = v[1] * abs(x - right_up.x);
	down += v[3] * abs(x - left_up.x);
	double ans = up * abs(y - left_down.y) + down * abs(y - left_up.y);
	return ans;
}

double vavImage::GetBilinearB_if0(double x, double y)
{
	if (y < 1 || (y + 1 >= m_Image.rows) || x < 1 || (x + 1 >= m_Image.cols))
	{
		if (y < 1)
		{
			y = 0;
		}
		if (x < 1)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 1;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 1;
		}
		return m_Image.at<cv::Vec3b>((int)y, (int)x)[0];
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[0];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[0];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[0];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[0];
	if (v[0]*v[1]*v[2]*v[3] < 1)
	{
		return 0;
	}
	// bilinear interpolation
	double up = v[0] * abs(x - right_down.x);
	up += v[2] * abs(x - left_down.x);
	double down = v[1] * abs(x - right_up.x);
	down += v[3] * abs(x - left_up.x);
	double ans = up * abs(y - left_down.y) + down * abs(y - left_up.y);
	return ans;
}

double_vector vavImage::GetRingLight(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	for (int i = 0; i < div; ++i)
	{
		Vector2 ahead(0, -radius);
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearLight(x + move.x, y + move.y));
	}
	return ans;
}

double_vector vavImage::GetRingR(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	for (int i = 0; i < div; ++i)
	{
		Vector2 ahead(0, -radius);
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearR(x + move.x, y + move.y));
	}
	return ans;
}

double_vector vavImage::GetRingG(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	for (int i = 0; i < div; ++i)
	{
		Vector2 ahead(0, -radius);
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearG(x + move.x, y + move.y));
	}
	return ans;
}

double_vector vavImage::GetRingB(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	for (int i = 0; i < div; ++i)
	{
		Vector2 ahead(0, -radius);
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearB(x + move.x, y + move.y));
	}
	return ans;
}

double_vector vavImage::GetVerticalLight(double x, double y, double radius,
		int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearLight(x, y - radius + i * step));
	}
	return ans;
}

double_vector vavImage::GetVerticalR(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearR(x, y - radius + i * step));
	}
	return ans;
}

double_vector vavImage::GetVerticalG(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearG(x, y - radius + i * step));
	}
	return ans;
}

double_vector vavImage::GetVerticalB(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearB(x, y - radius + i * step));
	}
	return ans;
}

double_vector vavImage::GetHorizontalLight(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearLight(x - radius + i * step, y));
	}
	return ans;
}

double_vector vavImage::GetHorizontalR(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearR(x - radius + i * step, y));
	}
	return ans;
}

double_vector vavImage::GetHorizontalG(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearG(x - radius + i * step, y));
	}
	return ans;
}

double_vector vavImage::GetHorizontalB(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearB(x - radius + i * step, y));
	}
	return ans;
}
bool vavImage::IsBlackLine(double x, double y, double radius)
{
	double_vector checkdata = ConvertToSquareWave(ConvertToAngle(
								  GetRingLight(x, y, radius, 360)), 10, 50);
	if (::IsBlackLine(checkdata))
	{
		return true;
	}
	return false;
}

double_vector vavImage::GetLineLight(double x1, double y1, double x2, double y2,
									 int div)
{
	double_vector ans;
	double step = 1.0 / (div - 1);
	Vector2 ahead(x2 - x1, y2 - y1);
	ahead *= step;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearLight(x1 +  ahead.x * i, y1 +  ahead.y * i));
	}
	return ans;
}

Vector3s vavImage::GetLineColor(double x1, double y1, double x2, double y2, int div)
{
	Vector3s ans;
	double step = 1.0 / (div - 1);
	Vector2 ahead(x2 - x1, y2 - y1);
	ahead *= step;
	for (int i = 0; i < div; ++i)
	{
		double r = GetBilinearR_if0(x1 +  ahead.x * i, y1 +  ahead.y * i);
		double g = GetBilinearG_if0(x1 +  ahead.x * i, y1 +  ahead.y * i);
		double b = GetBilinearB_if0(x1 +  ahead.x * i, y1 +  ahead.y * i);
		ans.push_back(Vector3(r, g, b));
	}
	return ans;
}

void vavImage::ToExpImage()
{
	cv::Mat imgf;
	m_Image.convertTo(imgf, CV_32FC3, 1.0 / 255);
	for (int j = 0; j < m_Image.cols ; ++j)
	{
		for (int i = 0; i < m_Image.rows ; ++i)
		{
			cv::Vec3f& intensity = imgf.at<cv::Vec3f>(i, j);
			intensity[0] = - pow((1 - intensity[0]), 2);
			intensity[1] = - pow((1 - intensity[1]), 2);
			intensity[2] = - pow((1 - intensity[2]), 2);
		}
	}
	normalize(imgf, imgf, 0, 1, cv::NORM_MINMAX);
	imgf.convertTo(m_Image, CV_8UC3, 255);
}

void vavImage::ToLogImage()
{
	cv::Mat imgf;
	m_Image.convertTo(imgf, CV_32FC3, 1.0 / 255);
	for (int j = 0; j < m_Image.cols ; ++j)
	{
		for (int i = 0; i < m_Image.rows ; ++i)
		{
			cv::Vec3f& intensity = imgf.at<cv::Vec3f>(i, j);
			intensity[0] = - pow((1 - intensity[0]), 0.5);
			intensity[1] = - pow((1 - intensity[1]), 0.5);
			intensity[2] = - pow((1 - intensity[2]), 0.5);
		}
	}
	normalize(imgf, imgf, 0, 1, cv::NORM_MINMAX);
	imgf.convertTo(m_Image, CV_8UC3, 255);
}

Vector3 vavImage::GetBilinearColor(double x, double y)
{
	double r = GetBilinearR(x, y);
	double g = GetBilinearG(x, y);
	double b = GetBilinearB(x, y);
	return Vector3(r, g, b);
}

int vavImage::GetIndex_int(int x, int y)
{
	if (y < 0 || (y >= m_Image.rows) || x < 0 || (x >= m_Image.cols))
	{
		return -1;
	}
	if (m_Image.at<cv::Vec3b>(y, x)[2] != 255)
	{
		cv::Vec3b color = m_Image.at<cv::Vec3b>(y, x);
		int v = color[0] + color[1] * 256 + color[2] * 256 * 256;
		return v;
	}
	return -1;
}


int vavImage::GetIndex_no255(double x, double y)
{
	if (y < 1 || (y + 1 >= m_Image.rows) || x < 1 || (x + 1 >= m_Image.cols))
	{
		if (y < 0)
		{
			y = 0;
		}
		if (x < 0)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 2;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 2;
		}
	}
	if (m_Image.at<cv::Vec3b>((int)y, (int)x)[2] != 255)
	{
		cv::Vec3b color = m_Image.at<cv::Vec3b>((int)y, (int)x);
		int v = color[0] + color[1] * 256 + color[2] * 256 * 256;
		return v;
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	cv::Vec3b c[4];
	c[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x);
	c[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x);
	c[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x);
	c[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x);
	v[0] = c[0][0] + c[0][1] * 256 + c[0][2] * 256 * 256;
	v[1] = c[1][0] + c[1][1] * 256 + c[1][2] * 256 * 256;
	v[2] = c[2][0] + c[2][1] * 256 + c[2][2] * 256 * 256;
	v[3] = c[3][0] + c[3][1] * 256 + c[3][2] * 256 * 256;
	for (int i = 0; i < 4; ++i)
	{
		if (c[i][0] != 255)
		{
			return v[i];
		}
	}
	return -1;
}

int vavImage::GetIndex_hasno255(double x, double y)
{
	if (y < 1 || (y + 1 >= m_Image.rows) || x < 1 || (x + 1 >= m_Image.cols))
	{
		if (y < 0)
		{
			y = 0;
		}
		if (x < 0)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 2;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 2;
		}
	}
	if (m_Image.at<cv::Vec3b>((int)y, (int)x)[2] != 255)
	{
		cv::Vec3b color = m_Image.at<cv::Vec3b>((int)y, (int)x);
		int v = color[0] + color[1] * 256 + color[2] * 256 * 256;
		return v;
	}
	return -1;
}
int vavImage::GetIndex_if255(double x, double y)
{
	if (y < 1 || (y + 1 >= m_Image.rows) || x < 1 || (x + 1 >= m_Image.cols))
	{
		if (y < 0)
		{
			y = 0;
		}
		if (x < 0)
		{
			x = 0;
		}
		if (x + 1 >= m_Image.cols)
		{
			x = m_Image.cols - 2;
		}
		if (y + 1 >= m_Image.rows)
		{
			y = m_Image.rows - 2;
		}
	}
	Vector2 left_up, left_down;
	Vector2 right_up, right_down;
	left_up.x = floor(x);
	left_up.y = floor(y);
	left_down.x = left_up.x;
	left_down.y = left_up.y + 1;
	right_up.x = left_up.x + 1;
	right_up.y = left_up.y;
	right_down.x = left_up.x + 1;
	right_down.y = left_up.y + 1;
	double v[4];
	cv::Vec3b c[4];
	c[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x);
	c[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x);
	c[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x);
	c[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x);
	for (int i = 0; i < 4; ++i)
	{
		if (c[i][0] == 255)
		{
			return -1;
		}
	}
// 	v[0] = c[0][0] + c[0][1] * 256 + c[0][2] * 256 * 256;
// 	v[1] = c[1][0] + c[1][1] * 256 + c[1][2] * 256 * 256;
// 	v[2] = c[2][0] + c[2][1] * 256 + c[2][2] * 256 * 256;
// 	v[3] = c[3][0] + c[3][1] * 256 + c[3][2] * 256 * 256;
	if (m_Image.at<cv::Vec3b>((int)y, (int)x)[2] != 255)
	{
		cv::Vec3b color = m_Image.at<cv::Vec3b>((int)y, (int)x);
		int v = color[0] + color[1] * 256 + color[2] * 256 * 256;
		return v;
	}
	return -1;
}
