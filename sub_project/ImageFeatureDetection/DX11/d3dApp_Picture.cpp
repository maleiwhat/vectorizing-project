#include "stdafx.h"
#include "d3dApp_Picture.h"
#include <d3d11.h>
#include <d3dx10.h>

D3DApp_Picture::D3DApp_Picture()
{
	m_Scale = 1;
	m_LookCenterX = 0;
	m_LookCenterY = 0;
	m_ClientWidth = 0;
	m_ClientHeight = 0;
	m_Pics_Texture = 0;
	m_PicW = 0;
	m_PicH = 0;
	m_d3dDevice = NULL;
	m_SwapChain = NULL;
	m_DepthStencilBuffer = NULL;
	m_DepthStencilView2 = NULL;
	m_RenderTargetView = NULL;
	m_DepthStencilView = NULL;
	m_DeviceContext = NULL;
	m_Pics_Width = NULL;
	m_Pics_Height = NULL;
	m_hAppInst   = GetModuleHandle( NULL );
	m_AppPaused  = false;
	m_Minimized  = false;
	m_Maximized  = false;
	m_Resizing   = false;
	m_FrameStats = L"";
	//mFont               = 0;
	m_MainWndCaption = L"D3D11 Application";
	m_d3dDriverType  = D3D_DRIVER_TYPE_HARDWARE;
	//md3dDriverType  = D3D_DRIVER_TYPE_REFERENCE;
	m_ClearColor     = D3DXCOLOR( 0, 0, 0, 0.0f );
	m_ClientWidth    = 1440;
	m_ClientHeight   = 900;
}

D3DApp_Picture::~D3DApp_Picture()
{
	ReleaseCOM( m_d3dDevice );
	ReleaseCOM( m_SwapChain );
	ReleaseCOM( m_DepthStencilBuffer );
	ReleaseCOM( m_DepthStencilView2 );
	ReleaseCOM( m_RenderTargetView );
	ReleaseCOM( m_DepthStencilView );
	ReleaseCOM( m_DeviceContext );
}

HINSTANCE D3DApp_Picture::getAppInst()
{
	return m_hAppInst;
}

HWND D3DApp_Picture::getMainWnd()
{
	return m_hMainWnd;
}

void D3DApp_Picture::initApp( HWND hWnd, int w, int h )
{
	m_hMainWnd = hWnd;
	m_ClientWidth = w;
	m_ClientHeight = h;
	if (m_ClientWidth == 0)
	{
		m_ClientWidth = 100;
		m_ClientHeight = 100;
	}
	initDirect3D();
	LoadBlend();
	float BlendFactor[4] = {0, 0, 0, 0};
	m_DeviceContext->OMSetBlendState( m_pBlendState_BLEND, BlendFactor, 0xffffffff );
}

void D3DApp_Picture::initDirect3D()
{
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = m_ClientWidth;
	sd.BufferDesc.Height = m_ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// No multisampling.
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = m_hMainWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;
	// Create the device.
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT               numLevelsRequested = 1;
	HR( D3D11CreateDeviceAndSwapChain(
	            0,                 //default adapter
	            m_d3dDriverType,
	            0,                 // no software device
	            createDeviceFlags,
	            &FeatureLevelsRequested,
	            numLevelsRequested,
	            D3D11_SDK_VERSION,
	            &sd,
	            &m_SwapChain,
	            &m_d3dDevice,
	            &m_FeatureLevelsSupported,
	            &m_DeviceContext ) );
	OnResize( m_ClientWidth, m_ClientHeight );
	m_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vbd.CPUAccessFlags = 0;
	m_vbd.MiscFlags = 0;
}


void D3DApp_Picture::OnResize( int w, int h )
{
	if ( !m_d3dDevice ) { return; }

	m_ClientWidth = w;
	m_ClientHeight = h;
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCOM( m_RenderTargetView );
	ReleaseCOM( m_DepthStencilView );
	ReleaseCOM( m_DepthStencilBuffer );
	// Resize the swap chain and recreate the render target view.
	HR( m_SwapChain->ResizeBuffers( 1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0 ) );
	ID3D11Texture2D* backBuffer;
	HR( m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) ) );
	HR( m_d3dDevice->CreateRenderTargetView( backBuffer, 0, &m_RenderTargetView ) );
	ReleaseCOM( backBuffer );
	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = m_ClientWidth;
	depthStencilDesc.Height    = m_ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags      = 0;
	HR( m_d3dDevice->CreateTexture2D( &depthStencilDesc, 0, &m_DepthStencilBuffer ) );
	HR( m_d3dDevice->CreateDepthStencilView( m_DepthStencilBuffer, 0, &m_DepthStencilView ) );
	// Bind the render target view and depth/stencil view to the pipeline.
	m_DeviceContext->OMSetRenderTargets( 1, &m_RenderTargetView, m_DepthStencilView );
	// Set the viewport transform.
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = ( float )m_ClientWidth;
	vp.Height   = ( float )m_ClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_DeviceContext->RSSetViewports( 1, &vp );
	
	if ( m_Pics_Width != NULL && m_Pics_Height != NULL )
	{
		m_Pics_Width->SetFloat( ( float )m_ClientWidth );
		m_Pics_Height->SetFloat( ( float )m_ClientHeight );
	}
}

void D3DApp_Picture::DrawScene()
{
	if (!m_DeviceContext)
	{
		return;
	}
	m_DeviceContext->ClearRenderTargetView( m_RenderTargetView, m_ClearColor );
	m_DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
	m_DeviceContext->OMSetDepthStencilState( m_pDepthStencil_ZWriteOFF, 0 );

	InterSetSize(m_ClientWidth, m_ClientHeight);
	InterSetLookCenter(m_LookCenterX, m_LookCenterY);
	printf("scale: %f \n", m_Scale);
	InterSetScale(m_Scale);

	if (m_PicsVertices.size() > 0)
	{
		m_Pics_PMap->SetResource(m_Pics_Texture);
		UINT offset = 0;
		UINT stride2 = sizeof(PictureVertex);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->IASetInputLayout(m_Pics_PLayout);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_Pics_Buffer, &stride2, &offset);
		m_Pics_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
		m_DeviceContext->Draw((UINT)m_PicsVertices.size(), 0);
	}

	m_SwapChain->Present( 0, 0 );
}

void D3DApp_Picture::buildShaderFX()
{
	ID3D10Blob* pCode;
	ID3D10Blob* pError;
	HRESULT hr = 0;

	hr = D3DX11CompileFromFile(_T("shader\\picture.fx"), NULL, NULL, NULL,
		"fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
		&pCode, &pError, NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
			ReleaseCOM(pError);
		}

		DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
	}

	HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, m_d3dDevice, &m_Pics_Effect));
	m_Pics_PTech = m_Pics_Effect->GetTechniqueByName("PointTech");
	m_Pics_Width = m_Pics_Effect->GetVariableByName("width")->AsScalar();
	m_Pics_Height = m_Pics_Effect->GetVariableByName("height")->AsScalar();
	m_Pics_CenterX = m_Pics_Effect->GetVariableByName("centerX")->AsScalar();
	m_Pics_CenterY = m_Pics_Effect->GetVariableByName("centerY")->AsScalar();
	m_Pics_Scale = m_Pics_Effect->GetVariableByName("scale")->AsScalar();
	m_Pics_PMap  = m_Pics_Effect->GetVariableByName("gMap")->AsShaderResource();
	m_TransparencySV_Picture =
		m_Pics_Effect->GetVariableByName("transparency")->AsScalar();
	D3DX11_PASS_DESC PassDesc;
	m_Pics_PTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(m_d3dDevice->CreateInputLayout(VertexDesc_PICVertex, 2,
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_Pics_PLayout));

	m_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vbd.CPUAccessFlags = 0;
	m_vbd.MiscFlags = 0;
}

void D3DApp_Picture::BuildPoint()
{
	ReleaseCOM(m_Pics_Buffer);

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
}

void D3DApp_Picture::LoadBlend()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory( &depth_stencil_desc, sizeof( depth_stencil_desc ) );
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// 開啟zbuffer write
	if ( D3D_OK != m_d3dDevice->CreateDepthStencilState( &depth_stencil_desc, &m_pDepthStencil_ZWriteON ) )
	{
		return ;
	}

	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	// 關閉zbuffer write
	if ( D3D_OK != m_d3dDevice->CreateDepthStencilState( &depth_stencil_desc, &m_pDepthStencil_ZWriteOFF ) )
	{
		return ;
	}

	m_DeviceContext->OMSetDepthStencilState( m_pDepthStencil_ZWriteON, 0 );
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
	        D3D11_COLOR_WRITE_ENABLE_ALL );

	// ADD混色模式
	if ( D3D_OK != m_d3dDevice->CreateBlendState( &blend_state_desc, &m_pBlendState_ADD ) )
	{
		return;
	}

	blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA ;
	blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

	// Alpha Blend混色模式
	if ( D3D_OK != m_d3dDevice->CreateBlendState( &blend_state_desc, &m_pBlendState_BLEND ) )
	{
		return ;
	}
}

void D3DApp_Picture::SetTexture( ID3D11ShaderResourceView* tex )
{
	if (!tex) { return; }

	ReleaseCOM(m_Pics_Texture);
	m_Pics_PMap->SetResource(tex);
	m_Pics_Texture = tex;
	OnResize(m_ClientWidth, m_ClientHeight);
}

void D3DApp_Picture::SetPictureSize( int w, int h )
{
	if (w == h && w == 0)
	{
		m_PicW = m_ClientWidth;
		m_PicH = m_ClientHeight;
	}
	else
	{
		m_PicW = w;
		m_PicH = h;
	}
}
