
//--------------------------------------------------------------------------------------
// File: Tutorial03.cpp
//
// This application displays a triangle using Direct3D 11
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "resource.h"
#include <tchar.h>
#include <d3dx11effect.h>
#include <d3dx10math.h>
#include <vector>
#include <dxerr.h>
#include <auto_link_effect11.hpp>
#include <strsafe.h>

#define V(x)           { hr = (x); if( FAILED(hr) ) { DXTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#define SAFE_RELEASE( p ) {if(p){(p)->Release();(p)=NULL;}}
//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

struct VSO_Vertex
{
	D3DXVECTOR3 pos; // Position
	D3DXVECTOR2 tex; // Texturecoords
};
typedef std::vector<VSO_Vertex> VSO_Vertexes;

struct CURVE_Vertex
{
	D3DXVECTOR2 pos;        // Position
	D3DXVECTOR4 lcolor;     // Color: left and Blur in w
	D3DXVECTOR4 rcolor;     // Color: right and Blur in w
	D3DXVECTOR2 nb;     // previous vertex and next vertex
};
typedef std::vector<CURVE_Vertex> CURVE_Vertexes;

D3D11_INPUT_ELEMENT_DESC VertexDesc_VSOVertex[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_CurveVertex[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 2, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


struct COLORPOINT
{
	D3DXVECTOR3 col;
	int off;
};

struct BLURRPOINT
{
	float   blurr;
	int     off;
};
typedef std::vector<D3DXVECTOR2> D3DXVECTOR2s;
typedef std::vector<COLORPOINT> COLORPOINTs;
typedef std::vector<BLURRPOINT> BLURRPOINTs;

struct CURVE
{
	int          pNum;
	D3DXVECTOR2s p;
	int          clNum;
	COLORPOINTs  cl;
	int          crNum;
	COLORPOINTs  cr;
	int          bNum;
	BLURRPOINTs  b;
};
typedef std::vector<CURVE> CURVEs;


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           m_pd3dDevice = NULL;
ID3D11DeviceContext*    m_DeviceContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* m_pRenderTargetView = NULL;

const int WIDTH = 1080;
const int HEIGHT = 1080;

float m_fWidth;
float m_fHeight;
float m_scale = 0.5;
float m_polySize = 0.5;
int diffTex = 0;
int diff2Tex = 0;
int diffSteps = 16;
int m_cNum;
int m_cSegNum;
D3D11_VIEWPORT m_vp;
D3DXVECTOR2 m_pan = D3DXVECTOR2(0, 0);
ID3D11InputLayout*      m_pCurveVertexLayout = NULL;
ID3D11Buffer*           m_pCurveVertexBuffer = NULL;
ID3D11InputLayout*      m_pCurveVertex2Layout = NULL;
ID3D11Buffer*           m_pCurveVertex2Buffer = NULL;
CURVE_Vertexes  m_CurveVertexes;
CURVEs m_curve;

D3D11_BUFFER_DESC       m_vbd;
// two textures used interleavedly for diffusion
ID3D11Texture2D*    m_diffuseTexture[2];
// two textures used interleavedly for diffusion (blurr texture)
ID3D11Texture2D*    m_distDirTexture;
ID3D11Texture2D*    m_Texture;
ID3D11Texture2D*    m_pDepthStencil;         // for z culling
// texture that keeps the color on the other side of a curve
ID3D11Texture2D*    m_otherTexture;

ID3D11ShaderResourceView* m_diffuseTextureRV[2];
ID3D11RenderTargetView*   m_diffuseTextureTV[2];
ID3D11ShaderResourceView* m_distDirTextureRV;
ID3D11RenderTargetView*   m_distDirTextureTV;
ID3D11ShaderResourceView* m_TextureRV;
ID3D11RenderTargetView*   m_TextureTV;
ID3D11DepthStencilView*   m_pDepthStencilView;
ID3D11ShaderResourceView* m_otherTextureRV;
ID3D11RenderTargetView*   m_otherTextureTV;

ID3DX11EffectTechnique* m_pDrawVectorsTechnique;
ID3DX11EffectTechnique* m_pDiffuseTechnique;
ID3DX11EffectTechnique* m_pLineAntiAliasTechnique;
ID3DX11EffectTechnique* m_pDisplayImage;

ID3DX11EffectShaderResourceVariable* m_pInTex[3];
ID3DX11EffectShaderResourceVariable* m_pDiffTex;
ID3DX11EffectScalarVariable* m_pDiffX;
ID3DX11EffectScalarVariable* m_blurOn;
ID3DX11EffectScalarVariable* m_pDiffY;
ID3DX11EffectScalarVariable* m_pScale;
ID3DX11EffectVectorVariable* m_pPan;
ID3DX11EffectScalarVariable* m_pPolySize;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();
void BuildShaderFX();
void ReadVectorFile(char* s);
void ConstructCurves();
void BuildPoint();
void RenderDiffusion();
void RenderToView();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		return 0;
	}
	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}
	BuildShaderFX();
	BuildPoint();
	// Main message loop
	MSG msg = {0};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
			Sleep(1000);
		}
	}
	CleanupDevice();
	return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
	{
		return E_FAIL;
	}
	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 DiffusionCurves",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_hWnd)
	{
		return E_FAIL;
	}
	ShowWindow(g_hWnd, nCmdShow);
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &m_pd3dDevice, &g_featureLevel, &m_DeviceContext);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if (FAILED(hr))
	{
		return hr;
	}
	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		return hr;
	}
	m_DeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_DeviceContext->RSSetViewports(1, &vp);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (m_DeviceContext)
	{
		m_DeviceContext->ClearState();
	}
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
	}
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
	}
	if (m_DeviceContext)
	{
		m_DeviceContext->Release();
	}
	if (m_pd3dDevice)
	{
		m_pd3dDevice->Release();
	}
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	RenderDiffusion();
	RenderToView();
	// Present the information rendered to the back buffer to the front buffer (the screen)
	g_pSwapChain->Present(0, 0);
}

void BuildShaderFX()
{
	m_diffuseTextureRV[0] = NULL;
	m_diffuseTextureTV[0] = NULL;
	m_distDirTextureRV = NULL;
	m_distDirTextureTV = NULL;
	m_TextureRV = NULL;
	m_TextureTV = NULL;
	m_pDepthStencilView = NULL;
	m_otherTextureRV = NULL;
	m_otherTextureTV = NULL;
	m_pDrawVectorsTechnique = NULL;
	m_pDiffuseTechnique = NULL;
	m_pLineAntiAliasTechnique = NULL;
	m_pDisplayImage = NULL;
	m_pInTex[0] = NULL;
	m_pInTex[1] = NULL;
	m_pInTex[2] = NULL;
	m_pDiffTex = NULL;
	m_pDiffX = NULL;
	m_pDiffY = NULL;
	m_pScale = NULL;
	m_pPan = NULL;
	m_pPolySize = NULL;
	m_vp.Width = (int)(WIDTH);
	m_vp.Height = (int)(HEIGHT);
	m_vp.MinDepth = 0.0f;
	m_vp.MaxDepth = 1.0f;
	m_vp.TopLeftX = 0;
	m_vp.TopLeftY = 0;
	ID3DBlob* pCode;
	ID3DBlob* pError;
	ID3DX11Effect*  pEffect11;
	//Picture
	HRESULT hr = 0;
	hr = D3DX11CompileFromFile(_T("DiffusionCurves.fx"), NULL, NULL, NULL,
		"fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
		&pCode, &pError, NULL);
	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			SAFE_RELEASE(pError);
		}
		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}
	V(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, m_pd3dDevice, &pEffect11));
	m_pDrawVectorsTechnique = pEffect11->GetTechniqueByName("DrawCurves");
	m_pDiffuseTechnique = pEffect11->GetTechniqueByName("Diffuse");
	m_pLineAntiAliasTechnique = pEffect11->GetTechniqueByName("LineAntiAlias");
	m_pDisplayImage = pEffect11->GetTechniqueByName("DisplayDiffusionImage");
	m_pDiffTex = pEffect11->GetVariableByName("g_diffTex")->AsShaderResource();
	for (int i = 0; i < 3; i++)
	{
		m_pInTex[i] = (pEffect11->GetVariableByName("g_inTex"))->GetElement(
			i)->AsShaderResource();
	}
	m_blurOn = pEffect11->GetVariableByName("g_blurOn")->AsScalar();
	m_pDiffX = pEffect11->GetVariableByName("g_diffX")->AsScalar();
	m_pDiffY = pEffect11->GetVariableByName("g_diffY")->AsScalar();
	m_pScale = pEffect11->GetVariableByName("g_scale")->AsScalar();
	m_pPolySize = pEffect11->GetVariableByName("g_polySize")->AsScalar();
	m_pPan = pEffect11->GetVariableByName("g_pan")->AsVector();
	D3DX11_PASS_DESC PassDesc;
	m_pDrawVectorsTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(m_pd3dDevice->CreateInputLayout(VertexDesc_CurveVertex,
		4,
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
		&m_pCurveVertexLayout)))
	{
		return;
	}
	m_pDiffuseTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(m_pd3dDevice->CreateInputLayout(VertexDesc_VSOVertex,
		2,
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
		&m_pCurveVertex2Layout)))
	{
		return;
	}
	// create the depth buffer (only needed for curve mask rendering)
	DXGI_SAMPLE_DESC samdesc;
	samdesc.Count = 1;
	samdesc.Quality = 0;
	D3D11_TEXTURE2D_DESC texdesc;
	ZeroMemory(&texdesc, sizeof(D3D10_TEXTURE2D_DESC));
	texdesc.MipLevels = 1;
	texdesc.ArraySize = 1;
	texdesc.SampleDesc = samdesc;
	texdesc.Width = (int)(WIDTH);
	texdesc.Height = (int)(HEIGHT);
	texdesc.Usage = D3D11_USAGE_DEFAULT;
	texdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texdesc.Format = DXGI_FORMAT_D32_FLOAT;
	if (m_pDepthStencil != NULL)
	{
		m_pDepthStencil->Release();
	}
	hr = m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_pDepthStencil);
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, NULL,
		&m_pDepthStencilView);
	//create diffusion textures (2 for ping pong rendering)
	if (m_diffuseTexture[0] != NULL)
	{
		m_diffuseTexture[0]->Release();
	}
	if (m_diffuseTexture[1] != NULL)
	{
		m_diffuseTexture[1]->Release();
	}
	if (m_distDirTexture != NULL)
	{
		m_distDirTexture->Release();
	}
	if (m_otherTexture != NULL)
	{
		m_otherTexture->Release();
	}
	texdesc.Width = (int)(WIDTH);
	texdesc.Height = (int)(HEIGHT);
	texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//texdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // use this for higher accuracy diffusion
	texdesc.BindFlags =  D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	hr = m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_diffuseTexture[0]);
	hr = m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_diffuseTexture[1]);
	hr = m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_otherTexture);
	// distance map + nearest point map
	texdesc.Usage = D3D11_USAGE_DEFAULT;
	texdesc.CPUAccessFlags = 0;
	texdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	hr = m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_distDirTexture);
	hr = m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_Texture);
	//create render target views
	hr = m_pd3dDevice->CreateShaderResourceView(m_diffuseTexture[0], NULL,
		&m_diffuseTextureRV[0]);
	hr = m_pd3dDevice->CreateRenderTargetView(m_diffuseTexture[0], NULL,
		&m_diffuseTextureTV[0]);
	hr = m_pd3dDevice->CreateShaderResourceView(m_diffuseTexture[1], NULL,
		&m_diffuseTextureRV[1]);
	hr = m_pd3dDevice->CreateRenderTargetView(m_diffuseTexture[1], NULL,
		&m_diffuseTextureTV[1]);
	hr = m_pd3dDevice->CreateShaderResourceView(m_distDirTexture, NULL,
		&m_distDirTextureRV);
	hr = m_pd3dDevice->CreateRenderTargetView(m_distDirTexture, NULL,
		&m_distDirTextureTV);
	hr = m_pd3dDevice->CreateShaderResourceView(m_Texture, NULL, &m_TextureRV);
	hr = m_pd3dDevice->CreateRenderTargetView(m_Texture, NULL, &m_TextureTV);
	hr = m_pd3dDevice->CreateShaderResourceView(m_otherTexture, NULL,
		&m_otherTextureRV);
	hr = m_pd3dDevice->CreateRenderTargetView(m_otherTexture, NULL,
		&m_otherTextureTV);
	char s[255] = "zephyr.xml";
	ReadVectorFile(s);
	ConstructCurves();
}

bool stringStartsWith(const char* s, const char* val)
{
	return !strncmp(s, val, strlen(val));
}

void ReadVectorFile(char* s)
{
	char buff[256];
	WCHAR wcFileInfo[512];
	char* token;
	FILE* F = fopen(s, "rb");
	while (fgets(buff, 255, F))
		if (stringStartsWith(buff, "<!DOCTYPE CurveSetXML"))
		{
			StringCchPrintf(wcFileInfo, 512,
				L"(INFO) : This seems to be a diffusion curves file.\n");
			OutputDebugString(wcFileInfo);
			break;
		}
		fgets(buff, 255, F);
		token = strtok(buff, " \"\t");
		while (!stringStartsWith(token, "image_width="))
		{
			token = strtok(NULL, " \"\t");
		}
		token = strtok(NULL, " \"\t");
		m_fWidth = atof(token);
		while (!stringStartsWith(token, "image_height="))
		{
			token = strtok(NULL, " \"\t");
		}
		token = strtok(NULL, " \"\t");
		m_fHeight = atof(token);
		while (!stringStartsWith(token, "nb_curves="))
		{
			token = strtok(NULL, " \"\t");
		}
		token = strtok(NULL, " \"\t");
		m_cNum = atof(token);
		StringCchPrintf(wcFileInfo, 512, L"(INFO) : %d curves found in file.\n",
			m_cNum);
		OutputDebugString(wcFileInfo);
		m_curve.resize(m_cNum);
		m_cSegNum = 0;
		D3DXVECTOR2 maxBound = D3DXVECTOR2(-1000000, -1000000);
		D3DXVECTOR2 minBound = D3DXVECTOR2(1000000, 1000000);
		for (int i1 = 0; i1 < m_cNum; i1++)
		{
			while (!stringStartsWith(buff, " <curve nb_control_points"))
			{
				fgets(buff, 255, F);
			}
			token = strtok(buff, " \"\t");
			while (!stringStartsWith(token, "nb_control_points="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			m_curve[i1].pNum = atof(token);
			m_cSegNum += ((m_curve[i1].pNum - 1) / 3);
			while (!stringStartsWith(token, "nb_left_colors="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			m_curve[i1].clNum = atof(token);
			while (!stringStartsWith(token, "nb_right_colors="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			m_curve[i1].crNum = atof(token);
			while (!stringStartsWith(token, "nb_blur_points="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			m_curve[i1].bNum = atof(token);
			// read in individual curve data
			m_curve[i1].p.resize(m_curve[i1].pNum);
			for (int i2 = 0; i2 < m_curve[i1].pNum; i2++)
			{
				while (!stringStartsWith(buff, "   <control_point "))
				{
					fgets(buff, 255, F);
				}
				token = strtok(buff, " \"\t");
				while (!stringStartsWith(token, "x="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].p[i2].y = atof(token);
				while (!stringStartsWith(token, "y="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].p[i2].x = atof(token);
				fgets(buff, 255, F);
				//extend the bounds if necessary
				if (m_curve[i1].p[i2].y < minBound.y)
				{
					minBound.y = m_curve[i1].p[i2].y;
				}
				if (m_curve[i1].p[i2].y > maxBound.y)
				{
					maxBound.y = m_curve[i1].p[i2].y;
				}
				if (m_curve[i1].p[i2].x < minBound.x)
				{
					minBound.x = m_curve[i1].p[i2].x;
				}
				if (m_curve[i1].p[i2].x > maxBound.x)
				{
					maxBound.x = m_curve[i1].p[i2].x;
				}
			}
			m_curve[i1].cl.resize(m_curve[i1].clNum);
			for (int i2 = 0; i2 < m_curve[i1].clNum; i2++)
			{
				while (!stringStartsWith(buff, "   <left_color "))
				{
					fgets(buff, 255, F);
				}
				token = strtok(buff, " \"\t");
				while (!stringStartsWith(token, "G="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cl[i2].col.y = atof(token) / 256.0;
				while (!stringStartsWith(token, "R="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cl[i2].col.z  = atof(token) / 256.0;
				while (!stringStartsWith(token, "globalID="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cl[i2].off = atof(token);
				while (!stringStartsWith(token, "B="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cl[i2].col.x  = atof(token) / 256.0;
				fgets(buff, 255, F);
			}
			m_curve[i1].cr.resize(m_curve[i1].crNum);
			for (int i2 = 0; i2 < m_curve[i1].crNum; i2++)
			{
				while (!stringStartsWith(buff, "   <right_color "))
				{
					fgets(buff, 255, F);
				}
				token = strtok(buff, " \"\t");
				while (!stringStartsWith(token, "G="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cr[i2].col.y = atof(token) / 256.0;
				while (!stringStartsWith(token, "R="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cr[i2].col.z  = atof(token) / 256.0;
				while (!stringStartsWith(token, "globalID="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cr[i2].off = atof(token);
				while (!stringStartsWith(token, "B="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].cr[i2].col.x  = atof(token) / 256.0;
				fgets(buff, 255, F);
			}
			m_curve[i1].b.resize(m_curve[i1].bNum);
			for (int i2 = 0; i2 < m_curve[i1].bNum; i2++)
			{
				while (!stringStartsWith(buff, "   <best_scale"))
				{
					fgets(buff, 255, F);
				}
				token = strtok(buff, " \"\t");
				while (!stringStartsWith(token, "value="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].b[i2].blurr = atof(token);
				while (!stringStartsWith(token, "globalID="))
				{
					token = strtok(NULL, " \"\t");
				}
				token = strtok(NULL, " \"\t");
				m_curve[i1].b[i2].off = atof(token);
				fgets(buff, 255, F);
			}
		}
		fclose(F);
		//scale the whole image between -1 and 1
		D3DXVECTOR2 middlePan = D3DXVECTOR2(0.5 * (maxBound.x + minBound.x),
			0.5 * (maxBound.y + minBound.y));
		for (int i1 = 0; i1 < m_cNum; i1++)
			for (int i2 = 0; i2 < m_curve[i1].pNum; i2++)
			{
				m_curve[i1].p[i2].x = 2.0f * (m_curve[i1].p[i2].x - middlePan.x) / m_fWidth;
				m_curve[i1].p[i2].y = 2.0f * (m_curve[i1].p[i2].y - middlePan.y) / m_fHeight;
			}
			StringCchPrintf(wcFileInfo, 512, L"(INFO) : %d curve segments found in file.\n",
				m_cSegNum);
			OutputDebugString(wcFileInfo);
}

// convert the vectors into triangle strips and draw these to the finest pyramid level
void ConstructCurves()
{
	D3DXVECTOR2     pLoop;
	int     iLoopStart;
	HRESULT hr;
	float subSegNum = 1.0f;
	m_CurveVertexes.resize(m_cSegNum * (int)(subSegNum) * 10 * 2);
	CURVE_Vertex* pd = &m_CurveVertexes[0];
	char*     used = new char[m_cNum];
	ZeroMemory(used, m_cNum * sizeof(char));
	int cPos = 0;
	double step = 0.7 / (m_curve.size()+1);
	for (int iX = 0; iX < m_cNum; iX++)
	{
		int i1 = iX;
		while (used[i1] > 0)
		{
			i1++;
			if (i1 == m_cNum)
			{
				i1 = 0;
			}
		}
		used[i1] = 1;
		int cSeg = 0;
		int lID = 0;
		while (m_curve[i1].cl[lID + 1].off <= 0)
		{
			lID++;
		}
		int lS = m_curve[i1].cl[lID].off;
		int lN = m_curve[i1].cl[lID + 1].off;
		int rID = 0;
		while (m_curve[i1].cr[rID + 1].off <= 0)
		{
			rID++;
		}
		int rS = m_curve[i1].cr[rID].off;
		int rN = m_curve[i1].cr[rID + 1].off;
		int bID = 0;
		while (m_curve[i1].b[bID + 1].off <= 0)
		{
			bID++;
		}
		int bS = m_curve[i1].b[rID].off;
		int bN2 = m_curve[i1].b[rID + 1].off;
		for (int i2 = 0; i2 < m_curve[i1].pNum - 3; i2 += 3)
		{
			for (float t = 0; t < 1.0f; t += 0.1f)
			{
				float t1 = t;
				float t2 = t + 0.1f;
				float f = (float)(cSeg - lS) / (float)(lN - lS);
				float fN = (float)(cSeg + 1 - lS) / (float)(lN - lS);
				D3DXVECTOR4 cL = D3DXVECTOR4((1.0f - f) * m_curve[i1].cl[lID].col + f *
					m_curve[i1].cl[lID + 1].col,
					1.0f);
				D3DXVECTOR4 cNL = D3DXVECTOR4((1.0f - fN) * m_curve[i1].cl[lID].col + fN *
					m_curve[i1].cl[lID + 1].col, 1.0f);
				f = (float)(cSeg - rS) / (float)(rN - rS);
				fN = (float)(cSeg + 1 - rS) / (float)(rN - rS);
				D3DXVECTOR4 cR = D3DXVECTOR4((1.0f - f) * m_curve[i1].cr[rID].col + f *
					m_curve[i1].cr[rID + 1].col,
					1.0f);
				D3DXVECTOR4 cNR = D3DXVECTOR4((1.0f - fN) * m_curve[i1].cr[rID].col + fN *
					m_curve[i1].cr[rID + 1].col, 1.0f);
				f = ((float)(cSeg - bS)) / ((float)(bN2 - bS));
				fN = (float)(cSeg + 1 - bS) / (float)(bN2 - bS);
				float b = (1.0f - f) * m_curve[i1].b[bID].blurr + f *
					m_curve[i1].b[bID + 1].blurr;
				float bN = (1.0f - fN) * m_curve[i1].b[bID].blurr + fN *
					m_curve[i1].b[bID + 1].blurr;
				// it is not entirely clear how [Orzan et al.08] encode the blurr in the files, this has been found to work ok..
				b = b * 1; 
				bN = bN * 1;
				//b = pow(1.5f,b);
				//bN = pow(1.5f,bN);
				for (float sI = 0.0f; sI < 1.0f; sI += 1.0f / subSegNum)
				{
					float sN = sI + 1.0f / subSegNum;
					float s1  = (1.0f - sI) * t1 + sI * t2;
					float s2 = (1.0f - sN) * t1 + sN * t2;
					D3DXVECTOR2 p0 = (1.0f - s1) * (1.0f - s1) * (1.0f - s1) * m_curve[i1].p[i2] +
						3 * s1 *
						(1.0f - s1) * (1.0f - s1) * m_curve[i1].p[i2 + 1] + 3 * s1 * s1 * (1.0f - s1) *
						m_curve[i1].p[i2 + 2] + s1 * s1 * s1 * m_curve[i1].p[i2 + 3];
					D3DXVECTOR2 p1 = (1.0f - s2) * (1.0f - s2) * (1.0f - s2) * m_curve[i1].p[i2] +
						3 * s2 *
						(1.0f - s2) * (1.0f - s2) * m_curve[i1].p[i2 + 1] + 3 * s2 * s2 * (1.0f - s2) *
						m_curve[i1].p[i2 + 2] + s2 * s2 * s2 * m_curve[i1].p[i2 + 3];
					pd[cPos + 0].lcolor = (1.0f - sI) * cL + sI * cNL;
					pd[cPos + 1].lcolor = (1.0f - sN) * cL + sN * cNL;
					pd[cPos + 0].rcolor = (1.0f - sI) * cR + sI * cNR;
					pd[cPos + 1].rcolor = (1.0f - sN) * cR + sN * cNR;
					pd[cPos + 0].lcolor.w = (1.0f - sI) * b + sI * bN;
					pd[cPos + 1].lcolor.w = (1.0f - sN) * b + sN * bN;
					pd[cPos + 0].rcolor.w = (1.0f - sI) * b + sI * bN;
					pd[cPos + 1].rcolor.w = (1.0f - sN) * b + sN * bN;
					pd[cPos + 0].pos = D3DXVECTOR2(p0.x, -p0.y);
					pd[cPos + 1].pos = D3DXVECTOR2(p1.x, -p1.y);
					if ((i2 == 0) && (t == 0.0f) && (sI < 0.5f / subSegNum))
					{
						pd[cPos + 0].nb = D3DXVECTOR2(10000.0f, 10000.0f);
						// if we are at the begin of a loop, do not declare endpoints
						if ((m_curve[i1].p[0] == m_curve[i1].p[m_curve[i1].pNum - 1])
							&& (sI < 0.5f / subSegNum))
						{
							pLoop.x = pd[cPos + 1].pos.x;
							pLoop.y = pd[cPos + 1].pos.y;
							iLoopStart = cPos;
						}
					}
					else
					{
						pd[cPos + 0].nb = pd[cPos - 2].pos;
						pd[cPos - 1].nb = pd[cPos + 1].pos;
					}
					pd[cPos + 1].nb = D3DXVECTOR2(10000.0f, 10000.0f);
					// if we are at the end of a loop, do not declare endpoints
					if ((m_curve[i1].p[0] == m_curve[i1].p[m_curve[i1].pNum - 1])
						&& (i2 == m_curve[i1].pNum - 1 - 3)
						&& (t >= 0.89f) && (sI + 1.1f / subSegNum >= 1.0))
					{
						pd[iLoopStart].nb = pd[cPos + 0].pos;
						pd[cPos + 1].nb = pLoop;
					}
					cPos += 2;
				}
				cSeg++;
				while ((cSeg >= lN) && (lID < m_curve[i1].clNum - 2))
				{
					lID++;
					lS = m_curve[i1].cl[lID].off;
					lN = m_curve[i1].cl[lID + 1].off;
				}
				while ((cSeg >= rN) && (rID < m_curve[i1].crNum - 2))
				{
					rID++;
					rS = m_curve[i1].cr[rID].off;
					rN = m_curve[i1].cr[rID + 1].off;
				}
				while ((cSeg >= bN2)  && (bID < m_curve[i1].bNum - 2))
				{
					bID++;
					bS = m_curve[i1].b[bID].off;
					bN2 = m_curve[i1].b[bID + 1].off;
				}
			}
		}
	}
	delete[]used;
	WCHAR wcFileInfo[512];
	StringCchPrintf(wcFileInfo, 512, L"(INFO) : Number of curve segments: %d \n",
		m_cSegNum * (int)(subSegNum) * 10);
	OutputDebugString(wcFileInfo);
}

void BuildPoint()
{
	m_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vbd.CPUAccessFlags = 0;
	m_vbd.MiscFlags = 0;
	SAFE_RELEASE(m_pCurveVertexBuffer);
	SAFE_RELEASE(m_pCurveVertex2Buffer);
	if (!m_CurveVertexes.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(CURVE_Vertex) * m_CurveVertexes.size());
		m_vbd.StructureByteStride = sizeof(CURVE_Vertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_CurveVertexes[0];
		m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_pCurveVertexBuffer);
	}
	{
		//create the screen space quad
		D3DXVECTOR3 pos[3 * 2] =
		{
			D3DXVECTOR3(-1.0f, -1.0f, +0.5f), D3DXVECTOR3(1.0f, -1.0f, +0.5f), D3DXVECTOR3(-1.0f, 1.0f, +0.5f),
			D3DXVECTOR3(1.0f, -1.0f, +0.5f), D3DXVECTOR3(1.0f, 1.0f, +0.5f), D3DXVECTOR3(-1.0f, 1.0f, +0.5f)
		};
		D3DXVECTOR2 tex[3 * 2] =
		{
			D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f),
			D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)
		};
		VSO_Vertex pd[2 * 3];
		for (int ctri = 0; ctri < 2; ctri++)
		{
			UINT32 i = 3 * ctri;
			pd[i + 0].pos = pos[i + 0];
			pd[i + 1].pos = pos[i + 1];
			pd[i + 2].pos = pos[i + 2];
			pd[i + 0].tex = tex[i + 0];
			pd[i + 1].tex = tex[i + 1];
			pd[i + 2].tex = tex[i + 2];
		}
		m_vbd.ByteWidth = (UINT)(sizeof(VSO_Vertex) * 6);
		m_vbd.StructureByteStride = sizeof(VSO_Vertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &pd[0];
		m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_pCurveVertex2Buffer);
	}
}

void RenderDiffusion()
{
	HRESULT hr;
	D3DX11_TECHNIQUE_DESC techDesc;
	D3DX11_PASS_DESC PassDesc;
	UINT stride, offset;
	//store the old render targets and viewports
	ID3D11RenderTargetView* old_pRTV = m_pRenderTargetView;
	//ID3D11DepthStencilView* old_pDSV = DXUTGetD3D10DepthStencilView();
	UINT NumViewports = 1;
	D3D11_VIEWPORT pViewports[20];
	m_DeviceContext->RSGetViewports(&NumViewports, &pViewports[0]);
	// set the shader variables, they are valid through the whole rendering pipeline
	V(m_pScale->SetFloat(m_scale));
	V(m_pPan->SetFloatVector(m_pan));
	V(m_pDiffX->SetFloat(WIDTH));
	V(m_pDiffY->SetFloat(HEIGHT));
	V(m_pPolySize->SetFloat(m_polySize));
	// render the triangles to the highest input texture level (assuming they are already defined!)
	m_DeviceContext->IASetInputLayout(m_pCurveVertexLayout);
	stride = sizeof(CURVE_Vertex);
	offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertexBuffer, &stride, &offset);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_DeviceContext->RSSetViewports(1, &m_vp);
	m_DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f,
		0);
	//construct the curve triangles in the geometry shader and render them directly
	ID3D11RenderTargetView* destTexTV[3];
	destTexTV[0] = m_diffuseTextureTV[1 - diffTex];
	destTexTV[1] = m_distDirTextureTV;
	destTexTV[2] = m_otherTextureTV;
	m_DeviceContext->OMSetRenderTargets(3, destTexTV, m_pDepthStencilView);
	m_pDrawVectorsTechnique->GetDesc(&techDesc);
	for (UINT p = 0;p<5 && p < techDesc.Passes; ++p)
	{
		m_pDrawVectorsTechnique->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw(m_CurveVertexes.size(), 0);
	}
	diffTex = 1 - diffTex;
	diff2Tex = 1 - diff2Tex;
	// setup the pipeline for the following image-space algorithms
	m_DeviceContext->IASetInputLayout(m_pCurveVertex2Layout);
	stride = sizeof(VSO_Vertex);
	offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertex2Buffer, &stride, &offset);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// diffuse the texture in both directions
	V(m_pInTex[1]->SetResource(m_distDirTextureRV));

	for (int i = 0; i < diffSteps; i++)
	{
		if (i > diffSteps*0.5)
		{
			m_blurOn->SetFloat(1);
		}
		else
		{
			m_blurOn->SetFloat(0);
		}
		// SA strategy
		V(m_pPolySize->SetFloat(1.0 - (float)(i) / (float)diffSteps));
		// SH strategy
		// 		V( g_pPolySize->SetFloat( 1.0 ) );
		// 		if (i>diffSteps-diffSteps/2)
		// 		{
		// 				V( g_pPolySize->SetFloat( (float)(diffSteps-i)/(float)(diffSteps/2) ) );
		// 		}

		m_DeviceContext->OMSetRenderTargets(1, &m_diffuseTextureTV[1 - diffTex], NULL);
		(m_pInTex[0]->SetResource(m_diffuseTextureRV[diffTex]));
		diffTex = 1 - diffTex;
		m_pDiffuseTechnique->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pDiffuseTechnique->GetPassByIndex(p)->Apply(0, m_DeviceContext);
			m_DeviceContext->Draw(6, 0);
		}
	}
	// anti alias the lines
	m_DeviceContext->OMSetRenderTargets(1, &m_diffuseTextureTV[1 - diffTex], NULL);
	V(m_pInTex[0]->SetResource(m_diffuseTextureRV[diffTex]));
	V(m_pInTex[1]->SetResource(m_otherTextureRV));
	diffTex = 1 - diffTex;
	V(m_pDiffTex->SetResource(m_distDirTextureRV));
	m_pLineAntiAliasTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pLineAntiAliasTechnique->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw(6, 0);
	}
	//restore old render targets
	m_DeviceContext->OMSetRenderTargets(1,  &old_pRTV,  NULL);
	m_DeviceContext->RSSetViewports(NumViewports, &pViewports[0]);
}

// this renders the final image to the screen
void RenderToView()
{
	ID3D11RenderTargetView* old_pRTV = m_pRenderTargetView;
	//ID3D11DepthStencilView* old_pDSV = DXUTGetD3D10DepthStencilView();
	m_DeviceContext->ClearRenderTargetView(m_TextureTV, D3DXCOLOR(0, 0, 0, 0));
	m_DeviceContext->OMSetRenderTargets(1, &m_TextureTV, m_pDepthStencilView);
	HRESULT hr;
	// Set the input layout
	m_DeviceContext->IASetInputLayout(m_pCurveVertex2Layout);
	// Set vertex buffer
	UINT stride = sizeof(VSO_Vertex);
	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertex2Buffer, &stride, &offset);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	V(m_pDiffTex->SetResource(m_diffuseTextureRV[diffTex]));
	//V(m_pDiffTex->SetResource(m_distDirTextureRV));
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pDisplayImage->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDisplayImage->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw(6, 0);
	}
	m_DeviceContext->OMSetRenderTargets(1,  &old_pRTV, NULL);
	offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertex2Buffer, &stride, &offset);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	V(m_pDiffTex->SetResource(m_TextureRV));
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDisplayImage->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw(6, 0);
	}
}
