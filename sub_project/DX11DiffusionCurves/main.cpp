
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
ID3D11Device*           g_pd3dDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;

const int WIDTH = 1080;
const int HEIGHT = 1080;

float g_fWidth;
float g_fHeight;
float g_scale = 0.5;
float g_polySize = 0.5;
int diffTex = 0;
int diff2Tex = 0;
int diffSteps = 16;
int g_cNum;
int g_cSegNum;
D3D11_VIEWPORT g_vp;
D3DXVECTOR2 g_pan = D3DXVECTOR2(0, 0);
ID3D11InputLayout*      g_pCurveVertexLayout = NULL;
ID3D11Buffer*           g_pCurveVertexBuffer = NULL;
ID3D11InputLayout*      g_pCurveVertex2Layout = NULL;
ID3D11Buffer*           g_pCurveVertex2Buffer = NULL;
CURVE_Vertexes  g_CurveVertexes;
CURVEs g_curve;

D3D11_BUFFER_DESC       g_vbd;
// two textures used interleavedly for diffusion
ID3D11Texture2D*    g_diffuseTexture[2];
// two textures used interleavedly for diffusion (blurr texture)
ID3D11Texture2D*    g_distDirTexture;
ID3D11Texture2D*    g_Texture;
ID3D11Texture2D*    g_pDepthStencil;         // for z culling
// texture that keeps the color on the other side of a curve
ID3D11Texture2D*    g_otherTexture;

ID3D11ShaderResourceView* g_diffuseTextureRV[2];
ID3D11RenderTargetView*   g_diffuseTextureTV[2];
ID3D11ShaderResourceView* g_distDirTextureRV;
ID3D11RenderTargetView*   g_distDirTextureTV;
ID3D11ShaderResourceView* g_TextureRV;
ID3D11RenderTargetView*   g_TextureTV;
ID3D11DepthStencilView*   g_pDepthStencilView;
ID3D11ShaderResourceView* g_otherTextureRV;
ID3D11RenderTargetView*   g_otherTextureTV;

ID3DX11EffectTechnique* g_pDrawVectorsTechnique;
ID3DX11EffectTechnique* g_pDiffuseTechnique;
ID3DX11EffectTechnique* g_pLineAntiAliasTechnique;
ID3DX11EffectTechnique* g_pDisplayImage;

ID3DX11EffectShaderResourceVariable* g_pInTex[3];
ID3DX11EffectShaderResourceVariable* g_pDiffTex;
ID3DX11EffectScalarVariable* g_pDiffX;
ID3DX11EffectScalarVariable* g_blurOn;
ID3DX11EffectScalarVariable* g_pDiffY;
ID3DX11EffectScalarVariable* g_pScale;
ID3DX11EffectVectorVariable* g_pPan;
ID3DX11EffectScalarVariable* g_pPolySize;

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
										   D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
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
	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		return hr;
	}
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (g_pImmediateContext)
	{
		g_pImmediateContext->ClearState();
	}
	if (g_pRenderTargetView)
	{
		g_pRenderTargetView->Release();
	}
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
	}
	if (g_pImmediateContext)
	{
		g_pImmediateContext->Release();
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
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
	g_diffuseTextureRV[0] = NULL;
	g_diffuseTextureTV[0] = NULL;
	g_distDirTextureRV = NULL;
	g_distDirTextureTV = NULL;
	g_TextureRV = NULL;
	g_TextureTV = NULL;
	g_pDepthStencilView = NULL;
	g_otherTextureRV = NULL;
	g_otherTextureTV = NULL;
	g_pDrawVectorsTechnique = NULL;
	g_pDiffuseTechnique = NULL;
	g_pLineAntiAliasTechnique = NULL;
	g_pDisplayImage = NULL;
	g_pInTex[0] = NULL;
	g_pInTex[1] = NULL;
	g_pInTex[2] = NULL;
	g_pDiffTex = NULL;
	g_pDiffX = NULL;
	g_pDiffY = NULL;
	g_pScale = NULL;
	g_pPan = NULL;
	g_pPolySize = NULL;
	g_vp.Width = (int)(WIDTH);
	g_vp.Height = (int)(HEIGHT);
	g_vp.MinDepth = 0.0f;
	g_vp.MaxDepth = 1.0f;
	g_vp.TopLeftX = 0;
	g_vp.TopLeftY = 0;
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
	(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
								  pCode->GetBufferSize(), NULL, g_pd3dDevice, &pEffect11));
	g_pDrawVectorsTechnique = pEffect11->GetTechniqueByName("DrawCurves");
	g_pDiffuseTechnique = pEffect11->GetTechniqueByName("Diffuse");
	g_pLineAntiAliasTechnique = pEffect11->GetTechniqueByName("LineAntiAlias");
	g_pDisplayImage = pEffect11->GetTechniqueByName("DisplayDiffusionImage");
	g_pDiffTex = pEffect11->GetVariableByName("g_diffTex")->AsShaderResource();
	for (int i = 0; i < 3; i++)
	{
		g_pInTex[i] = (pEffect11->GetVariableByName("g_inTex"))->GetElement(
						  i)->AsShaderResource();
	}
	g_blurOn = pEffect11->GetVariableByName("g_blurOn")->AsScalar();
	g_pDiffX = pEffect11->GetVariableByName("g_diffX")->AsScalar();
	g_pDiffY = pEffect11->GetVariableByName("g_diffY")->AsScalar();
	g_pScale = pEffect11->GetVariableByName("g_scale")->AsScalar();
	g_pPolySize = pEffect11->GetVariableByName("g_polySize")->AsScalar();
	g_pPan = pEffect11->GetVariableByName("g_pan")->AsVector();
	D3DX11_PASS_DESC PassDesc;
	g_pDrawVectorsTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(g_pd3dDevice->CreateInputLayout(VertexDesc_CurveVertex,
			   4,
			   PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
			   &g_pCurveVertexLayout)))
	{
		return;
	}
	g_pDiffuseTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(g_pd3dDevice->CreateInputLayout(VertexDesc_VSOVertex,
			   2,
			   PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
			   &g_pCurveVertex2Layout)))
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
	if (g_pDepthStencil != NULL)
	{
		g_pDepthStencil->Release();
	}
	hr = g_pd3dDevice->CreateTexture2D(&texdesc, NULL, &g_pDepthStencil);
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, NULL,
			&g_pDepthStencilView);
	//create diffusion textures (2 for ping pong rendering)
	if (g_diffuseTexture[0] != NULL)
	{
		g_diffuseTexture[0]->Release();
	}
	if (g_diffuseTexture[1] != NULL)
	{
		g_diffuseTexture[1]->Release();
	}
	if (g_distDirTexture != NULL)
	{
		g_distDirTexture->Release();
	}
	if (g_otherTexture != NULL)
	{
		g_otherTexture->Release();
	}
	texdesc.Width = (int)(WIDTH);
	texdesc.Height = (int)(HEIGHT);
	texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//texdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // use this for higher accuracy diffusion
	texdesc.BindFlags =  D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	hr = g_pd3dDevice->CreateTexture2D(&texdesc, NULL, &g_diffuseTexture[0]);
	hr = g_pd3dDevice->CreateTexture2D(&texdesc, NULL, &g_diffuseTexture[1]);
	hr = g_pd3dDevice->CreateTexture2D(&texdesc, NULL, &g_otherTexture);
	// distance map + nearest point map
	texdesc.Usage = D3D11_USAGE_DEFAULT;
	texdesc.CPUAccessFlags = 0;
	texdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	hr = g_pd3dDevice->CreateTexture2D(&texdesc, NULL, &g_distDirTexture);
	hr = g_pd3dDevice->CreateTexture2D(&texdesc, NULL, &g_Texture);
	//create render target views
	hr = g_pd3dDevice->CreateShaderResourceView(g_diffuseTexture[0], NULL,
			&g_diffuseTextureRV[0]);
	hr = g_pd3dDevice->CreateRenderTargetView(g_diffuseTexture[0], NULL,
			&g_diffuseTextureTV[0]);
	hr = g_pd3dDevice->CreateShaderResourceView(g_diffuseTexture[1], NULL,
			&g_diffuseTextureRV[1]);
	hr = g_pd3dDevice->CreateRenderTargetView(g_diffuseTexture[1], NULL,
			&g_diffuseTextureTV[1]);
	hr = g_pd3dDevice->CreateShaderResourceView(g_distDirTexture, NULL,
			&g_distDirTextureRV);
	hr = g_pd3dDevice->CreateRenderTargetView(g_distDirTexture, NULL,
			&g_distDirTextureTV);
	hr = g_pd3dDevice->CreateShaderResourceView(g_Texture, NULL, &g_TextureRV);
	hr = g_pd3dDevice->CreateRenderTargetView(g_Texture, NULL, &g_TextureTV);
	hr = g_pd3dDevice->CreateShaderResourceView(g_otherTexture, NULL,
			&g_otherTextureRV);
	hr = g_pd3dDevice->CreateRenderTargetView(g_otherTexture, NULL,
			&g_otherTextureTV);
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
	g_fWidth = atof(token);
	while (!stringStartsWith(token, "image_height="))
	{
		token = strtok(NULL, " \"\t");
	}
	token = strtok(NULL, " \"\t");
	g_fHeight = atof(token);
	while (!stringStartsWith(token, "nb_curves="))
	{
		token = strtok(NULL, " \"\t");
	}
	token = strtok(NULL, " \"\t");
	g_cNum = atof(token);
	StringCchPrintf(wcFileInfo, 512, L"(INFO) : %d curves found in file.\n",
					g_cNum);
	OutputDebugString(wcFileInfo);
	g_curve.resize(g_cNum);
	g_cSegNum = 0;
	D3DXVECTOR2 maxBound = D3DXVECTOR2(-1000000, -1000000);
	D3DXVECTOR2 minBound = D3DXVECTOR2(1000000, 1000000);
	for (int i1 = 0; i1 < g_cNum; i1++)
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
		g_curve[i1].pNum = atof(token);
		g_cSegNum += ((g_curve[i1].pNum - 1) / 3);
		while (!stringStartsWith(token, "nb_left_colors="))
		{
			token = strtok(NULL, " \"\t");
		}
		token = strtok(NULL, " \"\t");
		g_curve[i1].clNum = atof(token);
		while (!stringStartsWith(token, "nb_right_colors="))
		{
			token = strtok(NULL, " \"\t");
		}
		token = strtok(NULL, " \"\t");
		g_curve[i1].crNum = atof(token);
		while (!stringStartsWith(token, "nb_blur_points="))
		{
			token = strtok(NULL, " \"\t");
		}
		token = strtok(NULL, " \"\t");
		g_curve[i1].bNum = atof(token);
		// read in individual curve data
		g_curve[i1].p.resize(g_curve[i1].pNum);
		for (int i2 = 0; i2 < g_curve[i1].pNum; i2++)
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
			g_curve[i1].p[i2].y = atof(token);
			while (!stringStartsWith(token, "y="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].p[i2].x = atof(token);
			fgets(buff, 255, F);
			//extend the bounds if necessary
			if (g_curve[i1].p[i2].y < minBound.y)
			{
				minBound.y = g_curve[i1].p[i2].y;
			}
			if (g_curve[i1].p[i2].y > maxBound.y)
			{
				maxBound.y = g_curve[i1].p[i2].y;
			}
			if (g_curve[i1].p[i2].x < minBound.x)
			{
				minBound.x = g_curve[i1].p[i2].x;
			}
			if (g_curve[i1].p[i2].x > maxBound.x)
			{
				maxBound.x = g_curve[i1].p[i2].x;
			}
		}
		g_curve[i1].cl.resize(g_curve[i1].clNum);
		for (int i2 = 0; i2 < g_curve[i1].clNum; i2++)
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
			g_curve[i1].cl[i2].col.y = atof(token) / 256.0;
			while (!stringStartsWith(token, "R="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].cl[i2].col.z  = atof(token) / 256.0;
			while (!stringStartsWith(token, "globalID="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].cl[i2].off = atof(token);
			while (!stringStartsWith(token, "B="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].cl[i2].col.x  = atof(token) / 256.0;
			fgets(buff, 255, F);
		}
		g_curve[i1].cr.resize(g_curve[i1].crNum);
		for (int i2 = 0; i2 < g_curve[i1].crNum; i2++)
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
			g_curve[i1].cr[i2].col.y = atof(token) / 256.0;
			while (!stringStartsWith(token, "R="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].cr[i2].col.z  = atof(token) / 256.0;
			while (!stringStartsWith(token, "globalID="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].cr[i2].off = atof(token);
			while (!stringStartsWith(token, "B="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].cr[i2].col.x  = atof(token) / 256.0;
			fgets(buff, 255, F);
		}
		g_curve[i1].b.resize(g_curve[i1].bNum);
		for (int i2 = 0; i2 < g_curve[i1].bNum; i2++)
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
			g_curve[i1].b[i2].blurr = atof(token);
			while (!stringStartsWith(token, "globalID="))
			{
				token = strtok(NULL, " \"\t");
			}
			token = strtok(NULL, " \"\t");
			g_curve[i1].b[i2].off = atof(token);
			fgets(buff, 255, F);
		}
	}
	fclose(F);
	//scale the whole image between -1 and 1
	D3DXVECTOR2 middlePan = D3DXVECTOR2(0.5 * (maxBound.x + minBound.x),
										0.5 * (maxBound.y + minBound.y));
	for (int i1 = 0; i1 < g_cNum; i1++)
		for (int i2 = 0; i2 < g_curve[i1].pNum; i2++)
		{
			g_curve[i1].p[i2].x = 2.0f * (g_curve[i1].p[i2].x - middlePan.x) / g_fWidth;
			g_curve[i1].p[i2].y = 2.0f * (g_curve[i1].p[i2].y - middlePan.y) / g_fHeight;
		}
	StringCchPrintf(wcFileInfo, 512, L"(INFO) : %d curve segments found in file.\n",
					g_cSegNum);
	OutputDebugString(wcFileInfo);
}

// convert the vectors into triangle strips and draw these to the finest pyramid level
void ConstructCurves()
{
	D3DXVECTOR2     pLoop;
	int     iLoopStart;
	HRESULT hr;
	float subSegNum = 1.0f;
	g_CurveVertexes.resize(g_cSegNum * (int)(subSegNum) * 10 * 2);
	CURVE_Vertex* pd = &g_CurveVertexes[0];
	char*     used = new char[g_cNum];
	ZeroMemory(used, g_cNum * sizeof(char));
	int cPos = 0;
	double step = 0.7 / (g_curve.size()+1);
	for (int iX = 0; iX < g_cNum; iX++)
	{
		int i1 = iX;
		while (used[i1] > 0)
		{
			i1++;
			if (i1 == g_cNum)
			{
				i1 = 0;
			}
		}
		used[i1] = 1;
		int cSeg = 0;
		int lID = 0;
		while (g_curve[i1].cl[lID + 1].off <= 0)
		{
			lID++;
		}
		int lS = g_curve[i1].cl[lID].off;
		int lN = g_curve[i1].cl[lID + 1].off;
		int rID = 0;
		while (g_curve[i1].cr[rID + 1].off <= 0)
		{
			rID++;
		}
		int rS = g_curve[i1].cr[rID].off;
		int rN = g_curve[i1].cr[rID + 1].off;
		int bID = 0;
		while (g_curve[i1].b[bID + 1].off <= 0)
		{
			bID++;
		}
		int bS = g_curve[i1].b[rID].off;
		int bN2 = g_curve[i1].b[rID + 1].off;
		for (int i2 = 0; i2 < g_curve[i1].pNum - 3; i2 += 3)
		{
			for (float t = 0; t < 1.0f; t += 0.1f)
			{
				float t1 = t;
				float t2 = t + 0.1f;
				float f = (float)(cSeg - lS) / (float)(lN - lS);
				float fN = (float)(cSeg + 1 - lS) / (float)(lN - lS);
				D3DXVECTOR4 cL = D3DXVECTOR4((1.0f - f) * g_curve[i1].cl[lID].col + f *
											 g_curve[i1].cl[lID + 1].col,
											 1.0f);
				D3DXVECTOR4 cNL = D3DXVECTOR4((1.0f - fN) * g_curve[i1].cl[lID].col + fN *
											  g_curve[i1].cl[lID + 1].col, 1.0f);
				f = (float)(cSeg - rS) / (float)(rN - rS);
				fN = (float)(cSeg + 1 - rS) / (float)(rN - rS);
				D3DXVECTOR4 cR = D3DXVECTOR4((1.0f - f) * g_curve[i1].cr[rID].col + f *
											 g_curve[i1].cr[rID + 1].col,
											 1.0f);
				D3DXVECTOR4 cNR = D3DXVECTOR4((1.0f - fN) * g_curve[i1].cr[rID].col + fN *
											  g_curve[i1].cr[rID + 1].col, 1.0f);
				f = ((float)(cSeg - bS)) / ((float)(bN2 - bS));
				fN = (float)(cSeg + 1 - bS) / (float)(bN2 - bS);
				float b = (1.0f - f) * g_curve[i1].b[bID].blurr + f *
						  g_curve[i1].b[bID + 1].blurr;
				float bN = (1.0f - fN) * g_curve[i1].b[bID].blurr + fN *
						   g_curve[i1].b[bID + 1].blurr;
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
					D3DXVECTOR2 p0 = (1.0f - s1) * (1.0f - s1) * (1.0f - s1) * g_curve[i1].p[i2] +
									 3 * s1 *
									 (1.0f - s1) * (1.0f - s1) * g_curve[i1].p[i2 + 1] + 3 * s1 * s1 * (1.0f - s1) *
									 g_curve[i1].p[i2 + 2] + s1 * s1 * s1 * g_curve[i1].p[i2 + 3];
					D3DXVECTOR2 p1 = (1.0f - s2) * (1.0f - s2) * (1.0f - s2) * g_curve[i1].p[i2] +
									 3 * s2 *
									 (1.0f - s2) * (1.0f - s2) * g_curve[i1].p[i2 + 1] + 3 * s2 * s2 * (1.0f - s2) *
									 g_curve[i1].p[i2 + 2] + s2 * s2 * s2 * g_curve[i1].p[i2 + 3];
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
						if ((g_curve[i1].p[0] == g_curve[i1].p[g_curve[i1].pNum - 1])
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
					if ((g_curve[i1].p[0] == g_curve[i1].p[g_curve[i1].pNum - 1])
							&& (i2 == g_curve[i1].pNum - 1 - 3)
							&& (t >= 0.89f) && (sI + 1.1f / subSegNum >= 1.0))
					{
						pd[iLoopStart].nb = pd[cPos + 0].pos;
						pd[cPos + 1].nb = pLoop;
					}
					cPos += 2;
				}
				cSeg++;
				while ((cSeg >= lN) && (lID < g_curve[i1].clNum - 2))
				{
					lID++;
					lS = g_curve[i1].cl[lID].off;
					lN = g_curve[i1].cl[lID + 1].off;
				}
				while ((cSeg >= rN) && (rID < g_curve[i1].crNum - 2))
				{
					rID++;
					rS = g_curve[i1].cr[rID].off;
					rN = g_curve[i1].cr[rID + 1].off;
				}
				while ((cSeg >= bN2)  && (bID < g_curve[i1].bNum - 2))
				{
					bID++;
					bS = g_curve[i1].b[bID].off;
					bN2 = g_curve[i1].b[bID + 1].off;
				}
			}
		}
	}
	delete[]used;
	WCHAR wcFileInfo[512];
	StringCchPrintf(wcFileInfo, 512, L"(INFO) : Number of curve segments: %d \n",
					g_cSegNum * (int)(subSegNum) * 10);
	OutputDebugString(wcFileInfo);
}

void BuildPoint()
{
	g_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	g_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	g_vbd.CPUAccessFlags = 0;
	g_vbd.MiscFlags = 0;
	SAFE_RELEASE(g_pCurveVertexBuffer);
	SAFE_RELEASE(g_pCurveVertex2Buffer);
	if (!g_CurveVertexes.empty())
	{
		g_vbd.ByteWidth = (UINT)(sizeof(CURVE_Vertex) * g_CurveVertexes.size());
		g_vbd.StructureByteStride = sizeof(CURVE_Vertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &g_CurveVertexes[0];
		g_pd3dDevice->CreateBuffer(&g_vbd, &vinitData, &g_pCurveVertexBuffer);
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
		g_vbd.ByteWidth = (UINT)(sizeof(VSO_Vertex) * 6);
		g_vbd.StructureByteStride = sizeof(VSO_Vertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &pd[0];
		g_pd3dDevice->CreateBuffer(&g_vbd, &vinitData, &g_pCurveVertex2Buffer);
	}
}

void RenderDiffusion()
{
	HRESULT hr;
	D3DX11_TECHNIQUE_DESC techDesc;
	D3DX11_PASS_DESC PassDesc;
	UINT stride, offset;
	//store the old render targets and viewports
	ID3D11RenderTargetView* old_pRTV = g_pRenderTargetView;
	//ID3D11DepthStencilView* old_pDSV = DXUTGetD3D10DepthStencilView();
	UINT NumViewports = 1;
	D3D11_VIEWPORT pViewports[20];
	g_pImmediateContext->RSGetViewports(&NumViewports, &pViewports[0]);
	// set the shader variables, they are valid through the whole rendering pipeline
	V(g_pScale->SetFloat(g_scale));
	V(g_pPan->SetFloatVector(g_pan));
	V(g_pDiffX->SetFloat(WIDTH));
	V(g_pDiffY->SetFloat(HEIGHT));
	V(g_pPolySize->SetFloat(g_polySize));
	// render the triangles to the highest input texture level (assuming they are already defined!)
	g_pImmediateContext->IASetInputLayout(g_pCurveVertexLayout);
	stride = sizeof(CURVE_Vertex);
	offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pCurveVertexBuffer, &stride, &offset);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	g_pImmediateContext->RSSetViewports(1, &g_vp);
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f,
			0);
	//construct the curve triangles in the geometry shader and render them directly
	ID3D11RenderTargetView* destTexTV[3];
	destTexTV[0] = g_diffuseTextureTV[1 - diffTex];
	destTexTV[1] = g_distDirTextureTV;
	destTexTV[2] = g_otherTextureTV;
	g_pImmediateContext->OMSetRenderTargets(3, destTexTV, g_pDepthStencilView);
	g_pDrawVectorsTechnique->GetDesc(&techDesc);
	for (UINT p = 0;p<5 && p < techDesc.Passes; ++p)
	{
		g_pDrawVectorsTechnique->GetPassByIndex(p)->Apply(0, g_pImmediateContext);
		g_pImmediateContext->Draw(g_CurveVertexes.size(), 0);
	}
	diffTex = 1 - diffTex;
	diff2Tex = 1 - diff2Tex;
	// setup the pipeline for the following image-space algorithms
	g_pImmediateContext->IASetInputLayout(g_pCurveVertex2Layout);
	stride = sizeof(VSO_Vertex);
	offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pCurveVertex2Buffer, &stride, &offset);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// diffuse the texture in both directions
	V(g_pInTex[1]->SetResource(g_distDirTextureRV));
	
	for (int i = 0; i < diffSteps; i++)
	{
		if (i > diffSteps*0.5)
		{
			g_blurOn->SetFloat(1);
		}
		else
		{
			g_blurOn->SetFloat(0);
		}
		// SA strategy
		V(g_pPolySize->SetFloat(1.0 - (float)(i) / (float)diffSteps));
		// SH strategy
// 		V( g_pPolySize->SetFloat( 1.0 ) );
// 		if (i>diffSteps-diffSteps/2)
// 		{
// 				V( g_pPolySize->SetFloat( (float)(diffSteps-i)/(float)(diffSteps/2) ) );
// 		}
		
		g_pImmediateContext->OMSetRenderTargets(1, &g_diffuseTextureTV[1 - diffTex], NULL);
		(g_pInTex[0]->SetResource(g_diffuseTextureRV[diffTex]));
		diffTex = 1 - diffTex;
		g_pDiffuseTechnique->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			g_pDiffuseTechnique->GetPassByIndex(p)->Apply(0, g_pImmediateContext);
			g_pImmediateContext->Draw(6, 0);
		}
	}
	// anti alias the lines
	g_pImmediateContext->OMSetRenderTargets(1, &g_diffuseTextureTV[1 - diffTex], NULL);
	V(g_pInTex[0]->SetResource(g_diffuseTextureRV[diffTex]));
	V(g_pInTex[1]->SetResource(g_otherTextureRV));
	diffTex = 1 - diffTex;
	V(g_pDiffTex->SetResource(g_distDirTextureRV));
	g_pLineAntiAliasTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		g_pLineAntiAliasTechnique->GetPassByIndex(p)->Apply(0, g_pImmediateContext);
		g_pImmediateContext->Draw(6, 0);
	}
	//restore old render targets
	g_pImmediateContext->OMSetRenderTargets(1,  &old_pRTV,  NULL);
	g_pImmediateContext->RSSetViewports(NumViewports, &pViewports[0]);
}

// this renders the final image to the screen
void RenderToView()
{
	ID3D11RenderTargetView* old_pRTV = g_pRenderTargetView;
	//ID3D11DepthStencilView* old_pDSV = DXUTGetD3D10DepthStencilView();
	g_pImmediateContext->ClearRenderTargetView(g_TextureTV, D3DXCOLOR(0, 0, 0, 0));
	g_pImmediateContext->OMSetRenderTargets(1, &g_TextureTV, g_pDepthStencilView);
	HRESULT hr;
	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pCurveVertex2Layout);
	// Set vertex buffer
	UINT stride = sizeof(VSO_Vertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pCurveVertex2Buffer, &stride, &offset);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	V(g_pDiffTex->SetResource(g_diffuseTextureRV[diffTex]));
	//V(g_pDiffTex->SetResource(g_distDirTextureRV));
	D3DX11_TECHNIQUE_DESC techDesc;
	g_pDisplayImage->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		g_pDisplayImage->GetPassByIndex(p)->Apply(0, g_pImmediateContext);
		g_pImmediateContext->Draw(6, 0);
	}
	g_pImmediateContext->OMSetRenderTargets(1,  &old_pRTV, NULL);
	offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pCurveVertex2Buffer, &stride, &offset);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	V(g_pDiffTex->SetResource(g_TextureRV));
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		g_pDisplayImage->GetPassByIndex(p)->Apply(0, g_pImmediateContext);
		g_pImmediateContext->Draw(6, 0);
	}
}
