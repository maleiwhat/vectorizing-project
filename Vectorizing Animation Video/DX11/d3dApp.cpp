#include "stdafx.h"
#include "d3dApp.h"
#include "dxut/DXUT.h"
#include "SplineShape.h"
#include <auto_link_effect11.hpp>

D3DApp::D3DApp()
{
	SplineShape ss;
	m_d3dDevice = NULL;
	m_SwapChain = NULL;
	m_DepthStencilBuffer = NULL;
	m_DepthStencilView2 = NULL;
	m_RenderTargetView = NULL;
	m_DepthStencilView = NULL;
	m_DeviceContext = NULL;
	m_Pics_Texture = NULL;
	m_Pics_Buffer = NULL;
	m_Pics_Effect = NULL;
	m_Pics_PTech = NULL;
	m_Pics_PLayout = NULL;
	m_Pics_Width = NULL;
	m_Pics_Height = NULL;
	m_Pics_Scale = NULL;
	m_Pics_CenterX = NULL;
	m_Pics_CenterY = NULL;
	m_Pics_Transparency = NULL;
	m_Triangle_Buffer = NULL;
	m_Triangle_Effect = NULL;
	m_Triangle_PTech = NULL;
	m_Triangle_PLayout = NULL;
	m_Triangle_Width = NULL;
	m_Triangle_Height = NULL;
	m_Triangle_Scale = NULL;
	m_Triangle_CenterX = NULL;
	m_Triangle_CenterY = NULL;
	m_Triangle_Transparency = NULL;
	m_TriangleLine_Buffer = NULL;
	m_TriangleLine_Effect = NULL;
	m_TriangleLine_PTech = NULL;
	m_TriangleLine_PLayout = NULL;
	m_TriangleLine_Width = NULL;
	m_TriangleLine_Height = NULL;
	m_TriangleLine_Scale = NULL;
	m_TriangleLine_CenterX = NULL;
	m_TriangleLine_CenterY = NULL;
	m_TriangleLine_Transparency = NULL;
	m_Patch_Buffer = NULL;
	m_Patch_Effect = NULL;
	m_Patch_PTech = NULL;
	m_Patch_PLayout = NULL;
	m_Patch_Width = NULL;
	m_Patch_Height = NULL;
	m_Patch_Scale = NULL;
	m_Patch_CenterX = NULL;
	m_Patch_CenterY = NULL;
	m_Patch_Transparency = NULL;
	m_Points_Buffer = NULL;
	m_Points_Effect = NULL;
	m_Points_PTech = NULL;
	m_Points_PLayout = NULL;
	m_Points_Width = NULL;
	m_Points_Height = NULL;
	m_Points_Scale = NULL;
	m_Points_CenterX = NULL;
	m_Points_CenterY = NULL;
	m_Points_Transparency = NULL;
	m_Lines_Buffer = NULL;
	m_Lines_Effect = NULL;
	m_Lines_PTech = NULL;
	m_Lines_PLayout = NULL;
	m_Lines_Width = NULL;
	m_Lines_Height = NULL;
	m_Lines_Scale = NULL;
	m_Lines_CenterX = NULL;
	m_Lines_CenterY = NULL;
	m_Lines_Transparency = NULL;
	m_SkeletonLines_Buffer = NULL;
	m_SkeletonLines_Effect = NULL;
	m_SkeletonLines_PTech = NULL;
	m_SkeletonLines_PLayout = NULL;
	m_SkeletonLines_Width = NULL;
	m_SkeletonLines_Height = NULL;
	m_SkeletonLines_Scale = NULL;
	m_SkeletonLines_CenterX = NULL;
	m_SkeletonLines_CenterY = NULL;
	m_SkeletonLines_Transparency = NULL;
	m_PicW = 0;
	m_PicH = 0;
	m_hAppInst   = GetModuleHandle(NULL);
	m_AppPaused  = false;
	m_Minimized  = false;
	m_Maximized  = false;
	m_Resizing   = false;
	m_FrameStats = L"";
	m_DXUT_UI = NULL;
	//mFont               = 0;
	m_MainWndCaption = L"D3D11 Application";
	m_d3dDriverType  = D3D_DRIVER_TYPE_HARDWARE;
	//md3dDriverType  = D3D_DRIVER_TYPE_REFERENCE;
	m_ClearColor     = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mClientWidth    = 1440;
	mClientHeight   = 900;
}

D3DApp::~D3DApp()
{
	ReleaseCOM(m_d3dDevice);
	ReleaseCOM(m_SwapChain);
	ReleaseCOM(m_DepthStencilBuffer);
	ReleaseCOM(m_DepthStencilView2);
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_DeviceContext);
	ReleaseCOM(m_Pics_Effect);
	ReleaseCOM(m_Pics_PLayout);

	if (m_DXUT_UI)
	{
		delete m_DXUT_UI;
	}

	//	ReleaseCOM(mFont);
}
HINSTANCE D3DApp::getAppInst()
{
	return m_hAppInst;
}

HWND D3DApp::getMainWnd()
{
	return m_hMainWnd;
}

void D3DApp::initApp(HWND hWnd, int w, int h)
{
	m_hMainWnd = hWnd;
	mClientWidth = w;
	mClientHeight = h;
	initDirect3D();
	LoadBlend();
}

void D3DApp::initDirect3D()
{
	m_DXUT_UI = new DXUTUI;
	m_DXUT_UI->InitDXUT();
	m_DXUT_UI->SetWindow(m_hMainWnd);
	m_DXUT_UI->CreateDevice(mClientWidth, mClientHeight);
	m_d3dDevice = m_DXUT_UI->GetDevice();
	m_DeviceContext = m_DXUT_UI->GetDeviceContext();
	m_SwapChain = m_DXUT_UI->GetSwapChaine();
	OnResize(mClientWidth, mClientHeight);
	m_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vbd.CPUAccessFlags = 0;
	m_vbd.MiscFlags = 0;
}


void D3DApp::OnResize(int w, int h)
{
	if (!m_d3dDevice) { return; }

	mClientWidth = w;
	mClientHeight = h;

	if (m_Pics_Width)
	{
		m_Pics_Width->SetFloat(mClientWidth);
		m_Pics_Height->SetFloat(mClientHeight);
		m_Triangle_Width->SetFloat(mClientWidth);
		m_Triangle_Height->SetFloat(mClientHeight);
		m_TriangleLine_Width->SetFloat(mClientWidth);
		m_TriangleLine_Height->SetFloat(mClientHeight);
		m_Patch_Width->SetFloat(mClientWidth);
		m_Patch_Height->SetFloat(mClientHeight);
		m_Points_Width->SetFloat(mClientWidth);
		m_Points_Height->SetFloat(mClientHeight);
		m_Lines_Width->SetFloat(mClientWidth);
		m_Lines_Height->SetFloat(mClientHeight);
		m_SkeletonLines_Width->SetFloat(mClientWidth);
		m_SkeletonLines_Height->SetFloat(mClientHeight);
	}

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_DepthStencilBuffer);
	DXUTResizeDXGIBuffers(mClientWidth, mClientHeight, 0);
	// Resize the swap chain and recreate the render target view.
	//HR(mSwapChain->ResizeBuffers(2, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0));
	ID3D11Texture2D* backBuffer;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView));
	ReleaseCOM(backBuffer);
	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = mClientWidth;
	depthStencilDesc.Height    = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 4; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 2; // swap chain values.
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags      = 0;
	HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));
	HR(m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView));
	// Bind the render target view and depth/stencil view to the pipeline.
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	// Set the viewport transform.
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float)mClientWidth;
	vp.Height   = (float)mClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_DeviceContext->RSSetViewports(1, &vp);
}

void D3DApp::DrawScene()
{
	if (m_Pics_Width)
	{
		m_Pics_Width->SetFloat(mClientWidth);
		m_Pics_Height->SetFloat(mClientHeight);
	}

	if (!m_DXUT_UI) { return; }

	m_DXUT_UI->UpdataUI(0.1f);
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_ClearColor);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Draw Picture
	if (m_PicsVertices.size() > 0)
	{
		UINT offset = 0;
		UINT stride2 = sizeof(PictureVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_Pics_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Pics_Buffer, &stride2, &offset);
		m_Pics_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_PicsVertices.size(), 0);
	}

	m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteOFF, 0);

	//Draw Triangles
	if (m_TriangleVertices.size() > 0)
	{
		UINT offset = 0;
		UINT stride2 = sizeof(TriangleVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_Triangle_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Triangle_Buffer, &stride2, &offset);
		m_Triangle_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_TriangleVertices.size(), 0);
	}

	if (m_TriangleLineVertices.size() > 0)
	{
		UINT offset = 0;
		UINT stride2 = sizeof(TriangleVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_TriangleLine_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_TriangleLine_Buffer, &stride2, &offset);
		m_TriangleLine_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_TriangleLineVertices.size(), 0);
	}

	if (m_PatchVertices.size() > 0)
	{
		UINT offset = 0;
		UINT stride2 = sizeof(TriangleVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		m_DeviceContext->IASetInputLayout(m_Patch_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Patch_Buffer, &stride2, &offset);
		m_Patch_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_PatchVertices.size(), 0);
	}

	if (m_PointsVertices.size() > 0)
	{
		m_Points_Transparency->SetFloat(0.9);
		UINT offset = 0;
		UINT stride2 = sizeof(PointVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_Points_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Points_Buffer, &stride2, &offset);
		m_Points_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_PointsVertices.size(), 0);
	}

	if (m_LinesVertices.size() > 0)
	{
		m_Lines_Transparency->SetFloat(0.5);
		UINT offset = 0;
		UINT stride2 = sizeof(LineVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_Lines_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Lines_Buffer, &stride2, &offset);
		m_Lines_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_LinesVertices.size(), 0);
	}

	if (m_SkeletonLinesVertices.size() > 0)
	{
		m_SkeletonLines_Transparency->SetFloat(0.9);
		UINT offset = 0;
		UINT stride2 = sizeof(SkeletonLineVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_SkeletonLines_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_SkeletonLines_Buffer, &stride2, &offset);
		m_SkeletonLines_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_SkeletonLinesVertices.size(), 0);
	}

	m_SwapChain->Present(0, 0);
}

void D3DApp::BuildShaderFX()
{
	ID3D10Blob* pCode;
	ID3D10Blob* pError;
	//Picture
	HRESULT hr = 0;
	hr = D3DX11CompileFromFile(_T("shader\\picture.fx"), NULL, NULL, NULL,
	                           "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL, &pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Pics_Effect));
	m_Pics_PTech = m_Pics_Effect->GetTechniqueByName("PointTech");
	m_Pics_Width = m_Pics_Effect->GetVariableByName("width")->AsScalar();
	m_Pics_Height = m_Pics_Effect->GetVariableByName("height")->AsScalar();
	m_Pics_CenterX = m_Pics_Effect->GetVariableByName("centerX")->AsScalar();
	m_Pics_CenterY = m_Pics_Effect->GetVariableByName("centerY")->AsScalar();
	m_Pics_Scale = m_Pics_Effect->GetVariableByName("scale")->AsScalar();
	m_Pics_PMap  = m_Pics_Effect->GetVariableByName("gMap")->AsShaderResource();
	m_Pics_Transparency = m_Pics_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDesc;
	m_Pics_PTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_PICVertex, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_Pics_PLayout));
	//Triangle
	hr = 0;
	hr = D3DX11CompileFromFile(_T("shader\\triangle.fx"), NULL, NULL, NULL,
	                           "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL, &pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Triangle_Effect));
	m_Triangle_PTech = m_Triangle_Effect->GetTechniqueByName("PointTech");
	m_Triangle_Width = m_Triangle_Effect->GetVariableByName("width")->AsScalar();
	m_Triangle_Height = m_Triangle_Effect->GetVariableByName("height")->AsScalar();
	m_Triangle_CenterX = m_Triangle_Effect->GetVariableByName("centerX")->AsScalar();
	m_Triangle_CenterY = m_Triangle_Effect->GetVariableByName("centerY")->AsScalar();
	m_Triangle_Scale = m_Triangle_Effect->GetVariableByName("scale")->AsScalar();
	m_Triangle_Transparency = m_Triangle_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri;
	m_Triangle_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_TRIVertex, 6, PassDescTri.pIAInputSignature,
	                                  PassDescTri.IAInputSignatureSize, &m_Triangle_PLayout));
	hr = D3DX11CompileFromFile(_T("shader\\triangleline.fx"), NULL, NULL, NULL,
	                           "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL, &pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, m_d3dDevice, &m_TriangleLine_Effect));
	m_TriangleLine_PTech = m_TriangleLine_Effect->GetTechniqueByName("PointTech");
	m_TriangleLine_Width = m_TriangleLine_Effect->GetVariableByName("width")->AsScalar();
	m_TriangleLine_Height = m_TriangleLine_Effect->GetVariableByName("height")->AsScalar();
	m_TriangleLine_CenterX = m_TriangleLine_Effect->GetVariableByName("centerX")->AsScalar();
	m_TriangleLine_CenterY = m_TriangleLine_Effect->GetVariableByName("centerY")->AsScalar();
	m_TriangleLine_Scale = m_TriangleLine_Effect->GetVariableByName("scale")->AsScalar();
	m_TriangleLine_Transparency = m_TriangleLine_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri2;
	m_TriangleLine_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri2);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_TRIVertex, 6, PassDescTri2.pIAInputSignature,
	                                  PassDescTri2.IAInputSignatureSize, &m_TriangleLine_PLayout));
	hr = D3DX11CompileFromFile(_T("shader\\patch.fx"), NULL, NULL, NULL,
	                           "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL, &pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Patch_Effect));
	m_Patch_PTech = m_Patch_Effect->GetTechniqueByName("PointTech");
	m_Patch_Width = m_Patch_Effect->GetVariableByName("width")->AsScalar();
	m_Patch_Height = m_Patch_Effect->GetVariableByName("height")->AsScalar();
	m_Patch_CenterX = m_Patch_Effect->GetVariableByName("centerX")->AsScalar();
	m_Patch_CenterY = m_Patch_Effect->GetVariableByName("centerY")->AsScalar();
	m_Patch_Scale = m_Patch_Effect->GetVariableByName("scale")->AsScalar();
	m_Patch_Transparency = m_Patch_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri3;
	m_Patch_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri3);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_TRIVertex, 6, PassDescTri3.pIAInputSignature,
	                                  PassDescTri3.IAInputSignatureSize, &m_Patch_PLayout));
	hr = D3DX11CompileFromFile(_T("shader\\bigpoint.fx"), NULL, NULL, NULL,
	                           "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL, &pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Points_Effect));
	m_Points_PTech = m_Points_Effect->GetTechniqueByName("PointTech");
	m_Points_Width = m_Points_Effect->GetVariableByName("width")->AsScalar();
	m_Points_Height = m_Points_Effect->GetVariableByName("height")->AsScalar();
	m_Points_CenterX = m_Points_Effect->GetVariableByName("centerX")->AsScalar();
	m_Points_CenterY = m_Points_Effect->GetVariableByName("centerY")->AsScalar();
	m_Points_Scale = m_Points_Effect->GetVariableByName("scale")->AsScalar();
	m_Points_Transparency = m_Points_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri4;
	m_Points_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri4);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_PointVertex, 3, PassDescTri4.pIAInputSignature,
	                                  PassDescTri4.IAInputSignatureSize, &m_Points_PLayout));
	hr = D3DX11CompileFromFile(_T("shader\\Line.fx"), NULL, NULL, NULL,
	                           "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL, &pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Lines_Effect));
	m_Lines_PTech = m_Lines_Effect->GetTechniqueByName("PointTech");
	m_Lines_Width = m_Lines_Effect->GetVariableByName("width")->AsScalar();
	m_Lines_Height = m_Lines_Effect->GetVariableByName("height")->AsScalar();
	m_Lines_CenterX = m_Lines_Effect->GetVariableByName("centerX")->AsScalar();
	m_Lines_CenterY = m_Lines_Effect->GetVariableByName("centerY")->AsScalar();
	m_Lines_Scale = m_Lines_Effect->GetVariableByName("scale")->AsScalar();
	m_Lines_Transparency = m_Lines_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri5;
	m_Lines_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri5);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_LineVertex, 6, PassDescTri5.pIAInputSignature,
	                                  PassDescTri5.IAInputSignatureSize, &m_Lines_PLayout));
	hr = D3DX11CompileFromFile(_T("shader\\SkeletonLine.fx"), NULL, NULL, NULL,
	                           "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL, &pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, m_d3dDevice, &m_SkeletonLines_Effect));
	m_SkeletonLines_PTech = m_SkeletonLines_Effect->GetTechniqueByName("PointTech");
	m_SkeletonLines_Width = m_SkeletonLines_Effect->GetVariableByName("width")->AsScalar();
	m_SkeletonLines_Height = m_SkeletonLines_Effect->GetVariableByName("height")->AsScalar();
	m_SkeletonLines_CenterX = m_SkeletonLines_Effect->GetVariableByName("centerX")->AsScalar();
	m_SkeletonLines_CenterY = m_SkeletonLines_Effect->GetVariableByName("centerY")->AsScalar();
	m_SkeletonLines_Scale = m_SkeletonLines_Effect->GetVariableByName("scale")->AsScalar();
	m_SkeletonLines_Transparency = m_SkeletonLines_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDescTri6;
	m_SkeletonLines_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri6);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_SkeletonLineVertex, 3, PassDescTri6.pIAInputSignature,
	                                  PassDescTri6.IAInputSignatureSize, &m_SkeletonLines_PLayout));
	m_Pics_Width->SetFloat(mClientWidth);
	m_Pics_Height->SetFloat(mClientHeight);
	m_Triangle_Width->SetFloat(mClientWidth);
	m_Triangle_Height->SetFloat(mClientHeight);
	m_TriangleLine_Width->SetFloat(mClientWidth);
	m_TriangleLine_Height->SetFloat(mClientHeight);
	m_Patch_Width->SetFloat(mClientWidth);
	m_Patch_Height->SetFloat(mClientHeight);
	m_Points_Width->SetFloat(mClientWidth);
	m_Points_Height->SetFloat(mClientHeight);
	m_Lines_Width->SetFloat(mClientWidth);
	m_Lines_Height->SetFloat(mClientHeight);
	m_SkeletonLines_Width->SetFloat(mClientWidth);
	m_SkeletonLines_Height->SetFloat(mClientHeight);
}

void D3DApp::SetTexture(ID3D11ShaderResourceView* tex)
{
	if (!tex) { return; }

	ReleaseCOM(m_Pics_Texture);
	m_Pics_PMap->SetResource(tex);
	m_Pics_Width->SetFloat(mClientWidth);
	m_Pics_Height->SetFloat(mClientHeight);
	m_Triangle_Width->SetFloat(mClientWidth);
	m_Triangle_Height->SetFloat(mClientHeight);
	m_TriangleLine_Width->SetFloat(mClientWidth);
	m_TriangleLine_Height->SetFloat(mClientHeight);
	m_Patch_Width->SetFloat(mClientWidth);
	m_Patch_Height->SetFloat(mClientHeight);
	m_Points_Width->SetFloat(mClientWidth);
	m_Points_Height->SetFloat(mClientHeight);
	m_Lines_Width->SetFloat(mClientWidth);
	m_Lines_Height->SetFloat(mClientHeight);
	m_SkeletonLines_Width->SetFloat(mClientWidth);
	m_SkeletonLines_Height->SetFloat(mClientHeight);
	m_Pics_Texture = tex;
}

void D3DApp::ClearTriangles()
{
	ReleaseCOM(m_Triangle_Buffer);
	m_TriangleVertices.clear();
	ReleaseCOM(m_TriangleLine_Buffer);
	m_TriangleLineVertices.clear();
	ReleaseCOM(m_Patch_Buffer);
	m_PatchVertices.clear();
	ReleaseCOM(m_Points_Buffer);
	m_PointsVertices.clear();
	ReleaseCOM(m_Lines_Buffer);
	m_LinesVertices.clear();
	ReleaseCOM(m_SkeletonLines_Buffer);
	m_SkeletonLinesVertices.clear();
}

void D3DApp::BuildPoint()
{
	ReleaseCOM(m_Pics_Buffer);
	ReleaseCOM(m_Triangle_Buffer);
	ReleaseCOM(m_TriangleLine_Buffer);
	ReleaseCOM(m_Patch_Buffer);
	ReleaseCOM(m_Points_Buffer);
	ReleaseCOM(m_Lines_Buffer);
	ReleaseCOM(m_SkeletonLines_Buffer);

	if (!m_DeviceContext) { return; }

	m_PicsVertices.clear();

	if (m_Pics_Texture)
	{
		PictureVertex pv;
		pv.position.x = m_PicW / 2;
		pv.position.y = m_PicH / 2;
		pv.position.z = 0;
		pv.size.x = m_PicW;
		pv.size.y = m_PicH;
		m_PicsVertices.push_back(pv);
	}

	if (!m_PicsVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(PictureVertex) * m_PicsVertices.size());
		m_vbd.StructureByteStride = sizeof(PictureVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_PicsVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Pics_Buffer));
	}

	m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteOFF, 0);

	if (!m_TriangleVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(TriangleVertex) * m_TriangleVertices.size());
		m_vbd.StructureByteStride = sizeof(TriangleVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_TriangleVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Triangle_Buffer));
	}

	if (!m_TriangleLineVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(TriangleVertex) * m_TriangleLineVertices.size());
		m_vbd.StructureByteStride = sizeof(TriangleVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_TriangleLineVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_TriangleLine_Buffer));
	}

	if (!m_PatchVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(TriangleVertex) * m_PatchVertices.size());
		m_vbd.StructureByteStride = sizeof(TriangleVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_PatchVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Patch_Buffer));
	}

	if (!m_PointsVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(PointVertex) * m_PointsVertices.size());
		m_vbd.StructureByteStride = sizeof(PointVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_PointsVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Points_Buffer));
	}

	if (!m_LinesVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(LineVertex) * m_LinesVertices.size());
		m_vbd.StructureByteStride = sizeof(LineVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_LinesVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Lines_Buffer));
	}

	if (!m_SkeletonLinesVertices.empty())
	{
		m_vbd.ByteWidth = (UINT)(sizeof(SkeletonLineVertex) * m_SkeletonLinesVertices.size());
		m_vbd.StructureByteStride = sizeof(SkeletonLineVertex);
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &m_SkeletonLinesVertices[0];
		HR(m_d3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_SkeletonLines_Buffer));
	}
}

void D3DApp::SetPictureSize(int w, int h)
{
	if (w == h && w == 0)
	{
		m_PicW = mClientWidth;
		m_PicH = mClientHeight;
	}
	else
	{
		m_PicW = w;
		m_PicH = h;
	}
}

void special_normal(Vector2& v)
{
	if (v.x > 1) { v.x = 1; }

	if (v.x < -1) { v.x = -1; }

	if (v.y > 1) { v.y = 1; }

	if (v.y < -1) { v.y = -1; }
}

void D3DApp::AddTriangles( Triangles& tris, const vavImage& vimage )
{
	TriangleVertex tv;

	for (int i = 0; i < tris.size(); ++i)
	{
		Vector2& v1 = tris[i].m_Points[0];
		Vector2& v2 = tris[i].m_Points[1];
		Vector2& v3 = tris[i].m_Points[2];
		Vector2 vmove;
		vmove = v2 - v1 + v3 - v1;
		special_normal(vmove);
		vmove += v1;
		const const cv::Vec3b& c1 = vimage.GetColor(v1.x, v1.y);
		vmove = v1 - v2 + v3 - v2;
		special_normal(vmove);
		vmove += v2;
		const const cv::Vec3b& c2 = vimage.GetColor(v2.x, v2.y);
		vmove = v1 - v3 + v2 - v3;
		special_normal(vmove);
		vmove += v3;
		const const cv::Vec3b& c3 = vimage.GetColor(v3.x, v3.y);
		tv.p1.x = tris[i].m_Points[0].x;
		tv.p1.y = m_PicH - tris[i].m_Points[0].y;
		tv.c1.x = c1[2] / 255.0f;
		tv.c1.y = c1[1] / 255.0f;
		tv.c1.z = c1[0] / 255.0f;
		tv.p2.x = tris[i].m_Points[1].x;
		tv.p2.y = m_PicH - tris[i].m_Points[1].y;
		tv.c2.x = c2[2] / 255.0f;
		tv.c2.y = c2[1] / 255.0f;
		tv.c2.z = c2[0] / 255.0f;
		tv.p3.x = tris[i].m_Points[2].x;
		tv.p3.y = m_PicH - tris[i].m_Points[2].y;
		tv.c3.x = c3[2] / 255.0f;
		tv.c3.y = c3[1] / 255.0f;
		tv.c3.z = c3[0] / 255.0f;
		m_TriangleVertices.push_back(tv);
	}
}

void D3DApp::AddPatchTriangles( Triangles& tris )
{
	TriangleVertex tv;
	float r, g, b;
	r = 50 / 255.0f;
	g = 200 / 255.0f;
	b = 50 / 255.0f;

	for (int i = 0; i < tris.size(); ++i)
	{
		Vector2& v1 = tris[i].m_Points[0];
		Vector2& v2 = tris[i].m_Points[1];
		Vector2& v3 = tris[i].m_Points[2];
		tv.p1.x = tris[i].m_Points[0].x;
		tv.p1.y = m_PicH - tris[i].m_Points[0].y;
		tv.c1.x = r;
		tv.c1.y = g;
		tv.c1.z = b;
		tv.p2.x = tris[i].m_Points[1].x;
		tv.p2.y = m_PicH - tris[i].m_Points[1].y;
		tv.c2.x = r;
		tv.c2.y = g;
		tv.c2.z = b;
		tv.p3.x = tris[i].m_Points[2].x;
		tv.p3.y = m_PicH - tris[i].m_Points[2].y;
		tv.c3.x = r;
		tv.c3.y = g;
		tv.c3.z = b;
		m_PatchVertices.push_back(tv);
	}
}

void D3DApp::AddColorTriangles( Triangles& tris )
{
	TriangleVertex tv;

	for (int i = 0; i < tris.size(); ++i)
	{
		Vector2& v1 = tris[i].m_Points[0];
		Vector2& v2 = tris[i].m_Points[1];
		Vector2& v3 = tris[i].m_Points[2];
		tv.p1.x = tris[i].m_Points[0].x;
		tv.p1.y = m_PicH - tris[i].m_Points[0].y;
		tv.c1.x = tris[i].m_Colors[0][2] / 255.0f;
		tv.c1.y = tris[i].m_Colors[0][1] / 255.0f;
		tv.c1.z = tris[i].m_Colors[0][0] / 255.0f;
		tv.p2.x = tris[i].m_Points[1].x;
		tv.p2.y = m_PicH - tris[i].m_Points[1].y;
		tv.c2.x = tris[i].m_Colors[1][2] / 255.0f;
		tv.c2.y = tris[i].m_Colors[1][1] / 255.0f;
		tv.c2.z = tris[i].m_Colors[1][0] / 255.0f;
		tv.p3.x = tris[i].m_Points[2].x;
		tv.p3.y = m_PicH - tris[i].m_Points[2].y;
		tv.c3.x = tris[i].m_Colors[2][2] / 255.0f;
		tv.c3.y = tris[i].m_Colors[2][1] / 255.0f;
		tv.c3.z = tris[i].m_Colors[2][0] / 255.0f;
		m_TriangleVertices.push_back(tv);
	}
}

void D3DApp::AddTrianglesLine( Triangles& tris )
{
	TriangleVertex tv;
	float r, g, b;
	r = (rand() % 255) / 255.0f;
	g = (rand() % 255) / 255.0f;
	b = (rand() % 255) / 255.0f;

	for (int i = 0; i < tris.size(); ++i)
	{
		Vector2& v1 = tris[i].m_Points[0];
		Vector2& v2 = tris[i].m_Points[1];
		Vector2& v3 = tris[i].m_Points[2];
		tv.p1.x = tris[i].m_Points[0].x;
		tv.p1.y = m_PicH - tris[i].m_Points[0].y;
		tv.c1.x = r;
		tv.c1.y = g;
		tv.c1.z = b;
		tv.p2.x = tris[i].m_Points[1].x;
		tv.p2.y = m_PicH - tris[i].m_Points[1].y;
		tv.c2.x = r;
		tv.c2.y = g;
		tv.c2.z = b;
		tv.p3.x = tris[i].m_Points[2].x;
		tv.p3.y = m_PicH - tris[i].m_Points[2].y;
		tv.c3.x = r;
		tv.c3.y = g;
		tv.c3.z = b;
		m_TriangleLineVertices.push_back(tv);
	}
}

void D3DApp::LoadBlend()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// 開啟zbuffer write
	if (D3D_OK != m_d3dDevice->CreateDepthStencilState(&depth_stencil_desc, &m_pDepthStencil_ZWriteON))
	{
		return ;
	}

	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	// 關閉zbuffer write
	if (D3D_OK != m_d3dDevice->CreateDepthStencilState(&depth_stencil_desc, &m_pDepthStencil_ZWriteOFF))
	{
		return ;
	}

	m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteOFF, 0);
	CD3D11_BLEND_DESCX blend_state_desc(
	        FALSE,
	        FALSE,
	        TRUE,
	        D3D11_BLEND_ONE,
	        D3D11_BLEND_ONE,
	        D3D11_BLEND_OP_ADD,
	        D3D11_BLEND_ONE,
	        D3D11_BLEND_ONE,
	        D3D11_BLEND_OP_ADD,
	        D3D11_COLOR_WRITE_ENABLE_ALL);

	// ADD混色模式
	if (D3D_OK != m_d3dDevice->CreateBlendState(&blend_state_desc, &m_pBlendState_ADD))
	{
		return;
	}

	blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA ;
	blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

	// Alpha Blend混色模式
	if (D3D_OK != m_d3dDevice->CreateBlendState(&blend_state_desc, &m_pBlendState_BLEND))
	{
		return ;
	}

	// Set blend
	float BlendFactor[4] = {0, 0, 0, 0};
	m_DeviceContext->OMSetBlendState(m_pBlendState_BLEND, BlendFactor, 0xffffffff);
}

void D3DApp::SetLookCenter(float x, float y)
{
	m_Pics_CenterX->SetFloat(x);
	m_Pics_CenterY->SetFloat(y);
	m_Triangle_CenterX->SetFloat(x);
	m_Triangle_CenterY->SetFloat(y);
	m_TriangleLine_CenterX->SetFloat(x);
	m_TriangleLine_CenterY->SetFloat(y);
	m_Patch_CenterX->SetFloat(x);
	m_Patch_CenterY->SetFloat(y);
	m_Points_CenterX->SetFloat(x);
	m_Points_CenterY->SetFloat(y);
	m_Lines_CenterX->SetFloat(x);
	m_Lines_CenterY->SetFloat(y);
	m_SkeletonLines_CenterX->SetFloat(x);
	m_SkeletonLines_CenterY->SetFloat(y);
	BuildPoint();
	DrawScene();
}

void D3DApp::SetScale(float s)
{
	m_Triangle_Scale->SetFloat(s);
	m_Pics_Scale->SetFloat(s);
	m_TriangleLine_Scale->SetFloat(s);
	m_Patch_Scale->SetFloat(s);
	m_Points_Scale->SetFloat(s);
	m_Lines_Scale->SetFloat(s);
	m_SkeletonLines_Scale->SetFloat(s);
}

void D3DApp::ClearPatchs()
{
	m_PatchVertices.clear();
}

void D3DApp::SetPatchTransparency(float t)
{
	m_Triangle_Transparency->SetFloat(t);
}

void D3DApp::SetSelectPatchTransparency(float t)
{
	m_Patch_Transparency->SetFloat(t);
}

void D3DApp::SetLineTransparency(float t)
{
	m_TriangleLine_Transparency->SetFloat(t);
}

void D3DApp::SetPictureTransparency(float t)
{
	m_Pics_Transparency->SetFloat(t);
}

void D3DApp::AddBigPoint(float x, float y, D3DXVECTOR3 color)
{
	PointVertex pv;
	pv.color = color;
	pv.size.x = 1;
	pv.size.y = 1;
	pv.position.x = x;
	pv.position.y = m_PicH - y;
	m_PointsVertices.push_back(pv);
}

void D3DApp::AddLines(Lines& lines, double_vector2d& linewidths)
{
	for (int i = 0; i < lines.size(); ++i)
	{
		Line& now_line = lines[i];
		double_vector& now_linewidth = linewidths[i];
		float r, g, b;
		r = (rand() % 155 + 100) / 255.0f;
		g = (rand() % 155 + 100) / 255.0f;
		b = (rand() % 155 + 100) / 255.0f;

		if (now_line.size() < 3)
		{
			if (now_line.size() == 2)
			{
				LineVertex lv;
				lv.color.x = r;
				lv.color.y = g;
				lv.color.z = b;
				lv.p1.x = now_line[0].x;
				lv.p1.y = m_PicH - now_line[0].y;
				lv.p2.x = now_line[0].x;
				lv.p2.y = m_PicH - now_line[0].y;
				lv.p3.x = now_line[1].x;
				lv.p3.y = m_PicH - now_line[1].y;
				lv.p4.x = now_line[1].x;
				lv.p4.y = m_PicH - now_line[1].y;
				lv.width.x = now_linewidth[0];
				lv.width.y = now_linewidth[1];
				m_LinesVertices.push_back(lv);
			}
			continue;
		}

		LineVertex lv;
		lv.color.x = r;
		lv.color.y = g;
		lv.color.z = b;
		lv.p1.x = now_line[0].x;
		lv.p1.y = m_PicH - now_line[0].y;
		lv.p2.x = now_line[0].x;
		lv.p2.y = m_PicH - now_line[0].y;
		lv.p3.x = now_line[1].x;
		lv.p3.y = m_PicH - now_line[1].y;
		lv.p4.x = now_line[2].x;
		lv.p4.y = m_PicH - now_line[2].y;
		lv.width.x = now_linewidth[0];
		lv.width.y = now_linewidth[1];
		m_LinesVertices.push_back(lv);

		SkeletonLineVertex slv;
		slv.color.x = r;
		slv.color.y = g;
		slv.color.z = b;
		slv.p1.x = now_line[0].x;
		slv.p1.y = m_PicH - now_line[0].y;
		slv.p2.x = now_line[1].x;
		slv.p2.y = m_PicH - now_line[1].y;
		m_SkeletonLinesVertices.push_back(slv);
		
		for (int j = 1; j < now_line.size() - 2; ++j)
		{
			lv.p1.x = now_line[j - 1].x;
			lv.p1.y = m_PicH - now_line[j - 1].y;
			lv.p2.x = now_line[j].x;
			lv.p2.y = m_PicH - now_line[j].y;
			lv.p3.x = now_line[j + 1].x;
			lv.p3.y = m_PicH - now_line[j + 1].y;
			lv.p4.x = now_line[j + 2].x;
			lv.p4.y = m_PicH - now_line[j + 2].y;
			lv.width.x = now_linewidth[j];
			lv.width.y = now_linewidth[j + 1];
			m_LinesVertices.push_back(lv);

			slv.p1.x = now_line[j].x;
			slv.p1.y = m_PicH - now_line[j].y;
			slv.p2.x = now_line[j+1].x;
			slv.p2.y = m_PicH - now_line[j+1].y;
			m_SkeletonLinesVertices.push_back(slv);
		}
		lv.p1.x = now_line[now_line.size() - 3].x;
		lv.p1.y = m_PicH - now_line[now_line.size() - 3].y;
		lv.p2.x = now_line[now_line.size() - 2].x;
		lv.p2.y = m_PicH - now_line[now_line.size() - 2].y;
		lv.p3.x = now_line[now_line.size() - 1].x;
		lv.p3.y = m_PicH - now_line[now_line.size() - 1].y;
		lv.p4.x = now_line[now_line.size() - 1].x;
		lv.p4.y = m_PicH - now_line[now_line.size() - 1].y;
		lv.width.x = now_linewidth[now_line.size() - 2];
		lv.width.y = now_linewidth[now_line.size() - 1];
		m_LinesVertices.push_back(lv);
		slv.p1.x = now_line[now_line.size() - 2].x;
		slv.p1.y = m_PicH - now_line[now_line.size() - 2].y;
		slv.p2.x = now_line[now_line.size() - 1].x;
		slv.p2.y = m_PicH - now_line[now_line.size() - 1].y;
		m_SkeletonLinesVertices.push_back(slv);
	}
}

void D3DApp::AddLines(Lines& lines)
{
	static int ii = 0;
	ii ++;
	for (int i = 0; i < lines.size() /*&& i < ii*/; ++i)
	{
		Line& now_line = lines[i];

		if (now_line.size() < 2)
		{
			continue;
		}

		float r, g, b;
		r = (rand() % 155 + 100) / 255.0f;
		g = (rand() % 155 + 100) / 255.0f;
		b = (rand() % 155 + 100) / 255.0f;
		SkeletonLineVertex slv;
		slv.color.x = r;
		slv.color.y = g;
		slv.color.z = b;

		for (int j = 1; j < now_line.size(); ++j)
		{
			slv.p1.x = now_line[j].x;
			slv.p1.y = m_PicH - now_line[j].y;
			slv.p2.x = now_line[j-1].x;
			slv.p2.y = m_PicH - now_line[j-1].y;
			m_SkeletonLinesVertices.push_back(slv);
		}
	}
}

void D3DApp::AddLines( Points2d& lines )
{
	for (int i = 0; i < lines.size(); ++i)
	{
		Points& now_line = lines[i];

		if (now_line.size() < 2)
		{
			continue;
		}

		float r, g, b;
		r = (rand() % 155 + 100) / 255.0f;
		g = (rand() % 155 + 100) / 255.0f;
		b = (rand() % 155 + 100) / 255.0f;
		SkeletonLineVertex slv;
		slv.color.x = r;
		slv.color.y = g;
		slv.color.z = b;

		for (int j = 1; j < now_line.size(); ++j)
		{
			slv.p1.x = now_line[j].hx();
			slv.p1.y = m_PicH - now_line[j].hy();
			slv.p2.x = now_line[j-1].hx();
			slv.p2.y = m_PicH - now_line[j-1].hy();
			m_SkeletonLinesVertices.push_back(slv);
		}
	}
}

void D3DApp::AddLineSegs( LineSegs& lines )
{
	for (int i = 0; i < lines.size(); ++i)
	{
		LineSeg& now_line = lines[i];
		
		float r, g, b;
		r = (rand() % 155 + 100) / 255.0f;
		g = (rand() % 155 + 100) / 255.0f;
		b = (rand() % 155 + 100) / 255.0f;
		g = 1;
		r = b = 0;
		
		SkeletonLineVertex slv;
		slv.color.x = r;
		slv.color.y = g;
		slv.color.z = b;
		slv.p1.x = now_line.beg.x;
		slv.p1.y = m_PicH - now_line.beg.y;
		slv.p2.x = now_line.end.x;
		slv.p2.y = m_PicH - now_line.end.y;
		m_SkeletonLinesVertices.push_back(slv);
	}
}

