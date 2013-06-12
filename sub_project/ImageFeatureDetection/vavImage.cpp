#include "stdafx.h"
#include <auto_link_opencv.hpp>
#include <auto_link_cgal4.hpp>
#include <algorithm>
#include "vavImage.h"
#include "math\Quaternion.h"


vavImage::vavImage(void)
{
}

vavImage::vavImage(const cv::Mat& im)
{
	m_Image = im.clone();
}

ID3D11ShaderResourceView* vavImage::GetDx11Texture(ID3D11Device* dev, ID3D11DeviceContext* devc)
{
	if (m_Image.rows == 0 || m_Image.cols == 0) { return NULL; }

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
	ID3D11Texture2D* pTextureDraw;
	HRESULT d3dResult = dev->CreateTexture2D(&texDesc, &sSubData,
	                    &pTextureDraw);
	ID3D11ShaderResourceView* pShaderResView;
	int x = 0;
	delete[] characterImages;

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG(L"vavImage: Failed to create texture2D!");
		return 0;
	}

	d3dResult = dev->CreateShaderResourceView(pTextureDraw, &srDesc,
	            &pShaderResView);

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG(L"vavImage: Failed to create ShaderResourceView!");
		return 0;
	}

	{	// take draw texture
		ID3D11Texture2D* pTextureRead;
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
		HR(dev->CreateTexture2D(&texDescCV, 0, &pTextureRead));
		devc->CopyResource(pTextureRead, pTextureDraw);
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		float* pimg;
		//HR(m_DeviceContext->Map(pTextureRead, 0, D3D11_MAP_READ, 0, &MappedResource));
		unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
		HR(devc->Map(pTextureRead, subresource, D3D11_MAP_READ, 0,
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
				int offset = (j * (simg.rows+addoffset) + i) * 4;
				cv::Vec3b& intensity = simg.at<cv::Vec3b>(i, j);
				intensity[2] = pimg[offset  ] * 255.0f;
				intensity[1] = pimg[offset + 1] * 255.0f;
				intensity[0] = pimg[offset + 2] * 255.0f;
			}
		}
	}

	return pShaderResView;
}
vavImage::~vavImage(void)
{
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
	if (y < 0) { x = 0; }

	if (y >= m_Image.rows) { y = m_Image.rows - 1; }

	if (x < 0) { y = 0; }

	if (x >= m_Image.cols) { x = m_Image.cols - 1; }

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
