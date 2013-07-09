#include "stdafx.h"
#include <auto_link_opencv.hpp>
#include <auto_link_cgal4.hpp>
#include <algorithm>
#include "vavImage.h"
#include "math\Quaternion.h"
#include <cmath>
#include "IFExtenstion.h"
#include <cmath>

vavImage::vavImage(void): m_pTextureDraw(0), m_pShaderResView(0)
{
}

vavImage::vavImage(const cv::Mat& im): m_pTextureDraw(0), m_pShaderResView(0)
{
	m_Image = im.clone();
}
vavImage::~vavImage(void)
{
	if (m_pTextureDraw)
	{
		m_pTextureDraw->Release();
	}
	if (m_pShaderResView)
	{
		m_pShaderResView->Release();
	}
}
ID3D11ShaderResourceView* vavImage::GetDx11Texture(ID3D11Device* dev,
		ID3D11DeviceContext* devc)
{
	if (m_Image.rows == 0 || m_Image.cols == 0)
	{
		return NULL;
	}
	if (m_pTextureDraw)
	{
		m_pTextureDraw->Release();
		m_pTextureDraw = 0;
	}
	if (m_pShaderResView)
	{
		m_pShaderResView->Release();
		m_pShaderResView = 0;
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
	HRESULT d3dResult = dev->CreateTexture2D(&texDesc, &sSubData,
						&m_pTextureDraw);
	int x = 0;
	delete[] characterImages;
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG(L"vavImage: Failed to create texture2D!");
		return 0;
	}
	d3dResult = dev->CreateShaderResourceView(m_pTextureDraw, &srDesc,
				&m_pShaderResView);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG(L"vavImage: Failed to create ShaderResourceView!");
		return 0;
	}
	return m_pShaderResView;
}

bool vavImage::ReadImage(std::string path)
{
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
		y = 0;
	}
	if (y >= m_Image.rows)
	{
		y = m_Image.rows - 1;
	}
	if (x < 0)
	{
		x = 0;
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

void vavImage::Threshold(int v)
{
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
	double ans = v[0] * ((2 - abs(x - left_up.x) - abs(y - left_up.y)) * 0.5);
	ans += v[1] * ((2 - abs(x - left_down.x) - abs(y - left_down.y)) * 0.5);
	ans += v[2] * ((2 - abs(x - right_up.x) - abs(y - right_up.y)) * 0.5);
	ans += v[3] * ((2 - abs(x - right_down.x) - abs(y - right_down.y)) * 0.5);
	return ans * 0.5;
}

double vavImage::GetLight(int x, int y) const
{
	const cv::Vec3b& v = m_Image.at<cv::Vec3b>(y, x);
	return 0.299 * v[2] + 0.587 * v[1] + 0.114 * v[0];
}

double vavImage::GetBilinearR(double x, double y)
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
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[2];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[2];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[2];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[2];
	// bilinear interpolation
	double ans = v[0] * ((2 - abs(x - left_up.x) - abs(y - left_up.y)) * 0.5);
	ans += v[1] * ((2 - abs(x - left_down.x) - abs(y - left_down.y)) * 0.5);
	ans += v[2] * ((2 - abs(x - right_up.x) - abs(y - right_up.y)) * 0.5);
	ans += v[3] * ((2 - abs(x - right_down.x) - abs(y - right_down.y)) * 0.5);
	return ans * 0.5;
}

double vavImage::GetBilinearG(double x, double y)
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
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[1];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[1];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[1];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[1];
	// bilinear interpolation
	double ans = v[0] * ((2 - abs(x - left_up.x) - abs(y - left_up.y)) * 0.5);
	ans += v[1] * ((2 - abs(x - left_down.x) - abs(y - left_down.y)) * 0.5);
	ans += v[2] * ((2 - abs(x - right_up.x) - abs(y - right_up.y)) * 0.5);
	ans += v[3] * ((2 - abs(x - right_down.x) - abs(y - right_down.y)) * 0.5);
	return ans * 0.5;
}

double vavImage::GetBilinearB(double x, double y)
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
	v[0] = m_Image.at<cv::Vec3b>(left_up.y, left_up.x)[0];
	v[1] = m_Image.at<cv::Vec3b>(left_down.y, left_down.x)[0];
	v[2] = m_Image.at<cv::Vec3b>(right_up.y, right_up.x)[0];
	v[3] = m_Image.at<cv::Vec3b>(right_down.y, right_down.x)[0];
	// bilinear interpolation
	double ans = v[0] * ((2 - abs(x - left_up.x) - abs(y - left_up.y)) * 0.5);
	ans += v[1] * ((2 - abs(x - left_down.x) - abs(y - left_down.y)) * 0.5);
	ans += v[2] * ((2 - abs(x - right_up.x) - abs(y - right_up.y)) * 0.5);
	ans += v[3] * ((2 - abs(x - right_down.x) - abs(y - right_down.y)) * 0.5);
	return ans * 0.5;
}

double_vector vavImage::GetRingLight(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	Vector2 ahead(0, -radius);
	for (int i = 0; i < div; ++i)
	{
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearLight(x + move.x, y + move.y));
	}
	return ans;
}

double_vector vavImage::GetRingR(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	Vector2 ahead(0, -radius);
	for (int i = 0; i < div; ++i)
	{
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearR(x + move.x, y + move.y));
	}
	return ans;
}

double_vector vavImage::GetRingG(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	Vector2 ahead(0, -radius);
	for (int i = 0; i < div; ++i)
	{
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearG(x + move.x, y + move.y));
	}
	return ans;
}

double_vector vavImage::GetRingB(double x, double y, double radius, int div)
{
	double_vector ans;
	double step = 360.0 / div;
	Vector2 ahead(0, -radius);
	for (int i = 0; i < div; ++i)
	{
		Vector2 move = Quaternion::GetRotation(ahead, i * step);
		ans.push_back(GetBilinearB(x + move.x, y + move.y));
	}
	return ans;
}

void vavImage::ConvertToHSV()
{
	cv::cvtColor(m_Image, m_Image, CV_BGR2YUV);
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

double_vector vavImage::GetHorizontalLight(double x, double y, double radius,
		int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearLight(x - radius + i * step, y));
	}
	return ans;
}

double_vector vavImage::GetHorizontalR(double x, double y, double radius,
									   int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearR(x - radius + i * step, y));
	}
	return ans;
}

double_vector vavImage::GetHorizontalG(double x, double y, double radius,
									   int div)
{
	double_vector ans;
	double step = radius / div * 2;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearG(x - radius + i * step, y));
	}
	return ans;
}

double_vector vavImage::GetHorizontalB(double x, double y, double radius,
									   int div)
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
								  GetRingLight(x, y, radius, 25)), 10, 50);
	if (::IsBlackLine(checkdata))
	{
		return true;
	}
	return false;
}

void vavImage::ToExpImage()
{
	cv::Mat imgf;
	m_Image.convertTo(imgf, CV_32FC3, 1.0 / 255);
	normalize(imgf, imgf, 0, 1, cv::NORM_MINMAX);
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
	normalize(imgf, imgf, 0, 255, cv::NORM_MINMAX);
	imgf.convertTo(m_Image, CV_8UC3);
	int tmin = 255, tmax = 0;
	for (int j = 0; j < m_Image.cols ; ++j)
	{
		for (int i = 0; i < m_Image.rows ; ++i)
		{
			cv::Vec3f& intensity = imgf.at<cv::Vec3f>(i, j);
			if (tmin > intensity[0])
			{
				tmin = intensity[0];
			}
			if (tmax < intensity[0])
			{
				tmax = intensity[0];
			}
		}
	}
	printf("max %d min %d\n", tmax, tmin);
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

double_vector vavImage::GetLineR(double x1, double y1, double x2, double y2,
								 int div)
{
	double_vector ans;
	double step = 1.0 / (div - 1);
	Vector2 ahead(x2 - x1, y2 - y1);
	ahead *= step;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearR(x1 +  ahead.x * i, y1 +  ahead.y * i));
	}
	return ans;
}

double_vector vavImage::GetLineG(double x1, double y1, double x2, double y2,
								 int div)
{
	double_vector ans;
	double step = 1.0 / (div - 1);
	Vector2 ahead(x2 - x1, y2 - y1);
	ahead *= step;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearG(x1 +  ahead.x * i, y1 +  ahead.y * i));
	}
	return ans;
}

double_vector vavImage::GetLineB(double x1, double y1, double x2, double y2,
								 int div)
{
	double_vector ans;
	double step = 1.0 / (div - 1);
	Vector2 ahead(x2 - x1, y2 - y1);
	ahead *= step;
	for (int i = 0; i < div; ++i)
	{
		ans.push_back(GetBilinearB(x1 +  ahead.x * i, y1 +  ahead.y * i));
	}
	return ans;
}
