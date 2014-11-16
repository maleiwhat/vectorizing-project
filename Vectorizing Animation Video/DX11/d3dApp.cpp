#include "stdafx.h"
#include "cvshowEX.h"
#include "d3dApp.h"
#include "dxut/DXUT.h"
#include "SplineShape.h"
#include <auto_link_effect11.hpp>
#include "math/Quaternion.h"
#include <strsafe.h>
#include "Line.h"


D3DApp::D3DApp(): m_RegionDiffusion(this)
{
    SplineShape ss;
    m_pd3dDevice = NULL;
    m_SwapChain = NULL;
    m_DepthStencilBuffer = NULL;
    m_Backup_Buffer = NULL;
    m_DrawTexture = NULL;
    m_distDirTextureRV = NULL;
    m_distDirTextureTV = NULL;
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
    m_TransparencySV_Picture = NULL;
    m_Triangle_Buffer = NULL;
    m_Triangle_Effect = NULL;
    m_Triangle_PTech = NULL;
    m_Triangle_PLayout = NULL;
    m_Triangle_Width = NULL;
    m_Triangle_Height = NULL;
    m_Triangle_Scale = NULL;
    m_Triangle_CenterX = NULL;
    m_Triangle_CenterY = NULL;
    m_Triangle_Alpha = NULL;
    m_TriangleLine_Buffer = NULL;
    m_TriangleLine_Effect = NULL;
    m_TriangleLine_PTech = NULL;
    m_TriangleLine_PLayout = NULL;
    m_TriangleLine_Width = NULL;
    m_TriangleLine_Height = NULL;
    m_TriangleLine_Scale = NULL;
    m_TriangleLine_CenterX = NULL;
    m_TriangleLine_CenterY = NULL;
    m_TriangleLine_Alpha = NULL;
    m_Patch_Buffer = NULL;
    m_Patch_Effect = NULL;
    m_Patch_PTech = NULL;
    m_Patch_PLayout = NULL;
    m_Patch_Width = NULL;
    m_Patch_Height = NULL;
    m_Patch_Scale = NULL;
    m_Patch_CenterX = NULL;
    m_Patch_CenterY = NULL;
    m_SelectPatch_Alpha = NULL;
    m_Points_Buffer = NULL;
    m_Points_Effect = NULL;
    m_Points_PTech = NULL;
    m_Points_PLayout = NULL;
    m_Points_Width = NULL;
    m_Points_Height = NULL;
    m_Points_Scale = NULL;
    m_Points_CenterX = NULL;
    m_Points_CenterY = NULL;
    m_Points_Alpha = NULL;
    m_Lines_Buffer = NULL;
    m_Lines_Effect = NULL;
    m_Lines_PTech = NULL;
    m_Lines_PLayout = NULL;
    m_Lines_Width = NULL;
    m_Lines_Height = NULL;
    m_Lines_Scale = NULL;
    m_Lines_CenterX = NULL;
    m_Lines_CenterY = NULL;
    m_Lines_Alpha = NULL;
    m_Lines2w_Buffer = NULL;
    m_Lines2w_Effect = NULL;
    m_Lines2w_PTech = NULL;
    m_Lines2w_PLayout = NULL;
    m_Lines2w_Width = NULL;
    m_Lines2w_Height = NULL;
    m_Lines2w_Scale = NULL;
    m_Lines2w_CenterX = NULL;
    m_Lines2w_CenterY = NULL;
    m_Lines2w_CenterAlpha = NULL;
    m_Lines2w_PeripheryAlpha = NULL;
    m_SkeletonLines_Buffer = NULL;
    m_SkeletonLines_Effect = NULL;
    m_SkeletonLines_PTech = NULL;
    m_SkeletonLines_PLayout = NULL;
    m_SkeletonLines_Width = NULL;
    m_SkeletonLines_Height = NULL;
    m_SkeletonLines_Scale = NULL;
    m_SkeletonLines_CenterX = NULL;
    m_SkeletonLines_CenterY = NULL;
    m_BackBuffer = NULL;
    m_SkeletonLines_Alpha = NULL;
    m_DrawTextureDepthStencilView = NULL;
    m_DrawTextureDepthStencilBuffer = NULL;
    m_Transparency_Triangle = 0;
    m_Transparency_SelectPatch = 0;
    m_Transparency_TriangleLine = 0;
    m_Transparency_Lines = 0;
    m_Transparency_LineSkeleton = 0;
    m_Transparency_Picture = 0;
    m_PicW = 0;
    m_PicH = 0;
    m_LookCenterX = 0;
    m_LookCenterY = 0;
    m_hAppInst   = GetModuleHandle(NULL);
    m_AppPaused  = false;
    m_Minimized  = false;
    m_Maximized  = false;
    m_Resizing   = false;
    m_Init       = false;
    m_FrameStats = L"";
    m_DXUT_UI = NULL;
    //mFont               = 0;
    m_d3dDriverType  = D3D_DRIVER_TYPE_HARDWARE;
    //md3dDriverType  = D3D_DRIVER_TYPE_REFERENCE;
    m_ClearColor     = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
    m_ClientWidth    = 1440;
    m_ClientHeight   = 900;
    // diffusion curve part
    m_pan = D3DXVECTOR2(0, 0);
    m_pCurveVertexLayout = NULL;
    m_pCurveVertexBuffer = NULL;
    m_pCurveVertex2Layout = NULL;
    m_pCurveVertex2Buffer = NULL;
    m_pDepthStencil = NULL;
    m_polySize = 2;
    m_diffTex = 0;
    m_diffSteps = 8;
    m_diffdistDirTexture = NULL;
    m_diffuseTexture[0] = NULL;
    m_diffuseTexture[1] = NULL;
    m_diffuseTextureRV[0] = NULL;
    m_diffuseTextureTV[0] = NULL;
    m_diffuseTextureRV[1] = NULL;
    m_diffuseTextureTV[1] = NULL;
    m_diffdistDirTextureRV = NULL;
    m_diffdistDirTextureTV = NULL;
    m_Texture = NULL;
    m_TextureRV = NULL;
    m_TextureTV = NULL;
    m_pDepthStencilView = NULL;
    m_otherTexture = NULL;
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
    m_pAlpha = NULL;
}

D3DApp::~D3DApp()
{
    ReleaseCOM(m_pd3dDevice);
    ReleaseCOM(m_SwapChain);
    ReleaseCOM(m_DepthStencilBuffer);
    ReleaseCOM(m_DrawTexture);
    ReleaseCOM(m_RenderTargetView);
    ReleaseCOM(m_DepthStencilView);
    ReleaseCOM(m_DeviceContext);
    ReleaseCOM(m_Pics_Effect);
    ReleaseCOM(m_Triangle_Effect);
    ReleaseCOM(m_TriangleLine_Effect);
    ReleaseCOM(m_Points_Effect);
    ReleaseCOM(m_Lines_Effect);
    ReleaseCOM(m_Lines2w_Effect);
    ReleaseCOM(m_SkeletonLines_Effect);
    ReleaseCOM(m_Pics_Buffer);
    ReleaseCOM(m_Triangle_Buffer);
    ReleaseCOM(m_TriangleLine_Buffer);
    ReleaseCOM(m_Patch_Buffer);
    ReleaseCOM(m_Points_Buffer);
    ReleaseCOM(m_Lines_Buffer);
    ReleaseCOM(m_Lines2w_Buffer);
    ReleaseCOM(m_SkeletonLines_Buffer);
    ReleaseCOM(m_Pics_PLayout);
    ReleaseCOM(m_BackBuffer);
    ReleaseCOM(m_distDirTextureRV);
    ReleaseCOM(m_distDirTextureTV);
    ReleaseCOM(m_diffdistDirTextureRV);
    ReleaseCOM(m_diffdistDirTextureTV);
    if(m_DXUT_UI)
    {
        delete m_DXUT_UI;
    }
    //  ReleaseCOM(mFont);
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
    m_ClientWidth = w;
    m_ClientHeight = h;
    initDirect3D();
    LoadBlend();
    m_ClientWidth = 0;
    m_ClientHeight = 0;
    OnResize(w, h);
}

void D3DApp::initDirect3D()
{
    m_DXUT_UI = new DXUTUI;
    m_DXUT_UI->InitDXUT();
    m_DXUT_UI->SetWindow(m_hMainWnd);
    m_DXUT_UI->CreateDevice(m_ClientWidth, m_ClientHeight);
    m_pd3dDevice = m_DXUT_UI->GetDevice();
    m_DeviceContext = m_DXUT_UI->GetDeviceContext();
    m_SwapChain = m_DXUT_UI->GetSwapChaine();
    m_vbd.Usage = D3D11_USAGE_IMMUTABLE;
    m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_vbd.CPUAccessFlags = 0;
    m_vbd.MiscFlags = 0;
}


void D3DApp::OnResize(int w, int h)
{
    m_RegionDiffusion.ReadD3DSetting();
    if(!m_pd3dDevice)
    {
        return;
    }
    if(0 == w || 0 == h)
    {
        w = m_ClientWidth;
        h = m_ClientHeight;
    }
    else if(m_ClientWidth == w && m_ClientHeight == h)
    {
        return;
    }
    m_ClientWidth = w;
    m_ClientHeight = h;
    printf("w: %d h:%d\n", m_ClientWidth, m_ClientHeight);
    // Release the old views, as they hold references to the buffers we
    // will be destroying.  Also release the old depth/stencil buffer.
    m_RegionDiffusion.OnResizeRelease();
    // main
    ReleaseCOM(m_RenderTargetView);
    ReleaseCOM(m_DepthStencilView);
    ReleaseCOM(m_DepthStencilBuffer);
    ReleaseCOM(m_BackBuffer);
    ReleaseCOM(m_distDirTextureRV);
    // bitmap
    ReleaseCOM(m_distDirTextureTV);
    ReleaseCOM(m_distDirTextureRV);
    ReleaseCOM(m_DrawTextureDepthStencilBuffer);
    ReleaseCOM(m_DrawTextureDepthStencilView);
    ReleaseCOM(m_DrawTexture);
    ReleaseCOM(m_BackBuffer);
    // diffusion
    ReleaseCOM(m_diffuseTexture[0]);
    ReleaseCOM(m_diffuseTexture[1]);
    ReleaseCOM(m_diffdistDirTexture);
    ReleaseCOM(m_Texture);
    ReleaseCOM(m_pDepthStencil);
    ReleaseCOM(m_otherTexture);
    ReleaseCOM(m_diffuseTextureRV[0]);
    ReleaseCOM(m_diffuseTextureTV[0]);
    ReleaseCOM(m_diffuseTextureRV[1]);
    ReleaseCOM(m_diffuseTextureTV[1]);
    ReleaseCOM(m_diffdistDirTextureRV);
    ReleaseCOM(m_diffdistDirTextureTV);
    ReleaseCOM(m_TextureRV);
    ReleaseCOM(m_TextureTV);
    ReleaseCOM(m_otherTextureRV);
    ReleaseCOM(m_otherTextureTV);
    DXUTResizeDXGIBuffers(w, h, 0);
    // Resize the swap chain and recreate the render target view.
    //HR(mSwapChain->ResizeBuffers(2, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0));
    HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                              reinterpret_cast<void**>(&m_BackBuffer)));
    HR(m_pd3dDevice->CreateRenderTargetView(m_BackBuffer, 0, &m_RenderTargetView));
    // Create the depth/stencil buffer and view.
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width     = w;
    depthStencilDesc.Height    = h;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format    = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
    depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
    depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags      = 0;
    HR(m_pd3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));
    HR(m_pd3dDevice->CreateDepthStencilView(m_DepthStencilBuffer, 0,
                                            &m_DepthStencilView));
    // Bind the render target view and depth/stencil view to the pipeline.
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
    // Set the viewport transform.
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = (float)m_ClientWidth;
    vp.Height   = (float)m_ClientHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_DeviceContext->RSSetViewports(1, &vp);
    // create the depth buffer (only needed for curve mask rendering)
    DXGI_SAMPLE_DESC samdesc;
    samdesc.Count = 1;
    samdesc.Quality = 0;
    D3D11_TEXTURE2D_DESC texdesc;
    ZeroMemory(&texdesc, sizeof(D3D11_TEXTURE2D_DESC));
    texdesc.MipLevels = 1;
    texdesc.ArraySize = 1;
    texdesc.SampleDesc = samdesc;
    texdesc.Width = w;
    texdesc.Height = h;
    texdesc.Usage = D3D11_USAGE_DEFAULT;
    texdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texdesc.Format = DXGI_FORMAT_D32_FLOAT;
    if(m_pDepthStencil != NULL)
    {
        m_pDepthStencil->Release();
    }
    HR(m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_pDepthStencil));
    HR(m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, NULL, &m_pDepthStencilView));
    //create diffusion textures (2 for ping pong rendering)
    texdesc.Width = w;
    texdesc.Height = h;
    //texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  // use this for higher accuracy diffusion
    texdesc.Usage = D3D11_USAGE_DEFAULT;
    texdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    HR(m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_diffuseTexture[0]));
    HR(m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_diffuseTexture[1]));
    HR(m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_otherTexture));
    // distance map + nearest point map
    texdesc.Usage = D3D11_USAGE_DEFAULT;
    texdesc.CPUAccessFlags = 0;
    texdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    HR(m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_diffdistDirTexture));
    HR(m_pd3dDevice->CreateTexture2D(&texdesc, NULL, &m_Texture));
    //create render target views
    HR(m_pd3dDevice->CreateShaderResourceView(m_diffuseTexture[0], NULL,
            &m_diffuseTextureRV[0]));
    HR(m_pd3dDevice->CreateRenderTargetView(m_diffuseTexture[0], NULL,
                                            &m_diffuseTextureTV[0]));
    HR(m_pd3dDevice->CreateShaderResourceView(m_diffuseTexture[1], NULL,
            &m_diffuseTextureRV[1]));
    HR(m_pd3dDevice->CreateRenderTargetView(m_diffuseTexture[1], NULL,
                                            &m_diffuseTextureTV[1]));
    HR(m_pd3dDevice->CreateShaderResourceView(m_diffdistDirTexture, NULL,
            &m_diffdistDirTextureRV));
    HR(m_pd3dDevice->CreateRenderTargetView(m_diffdistDirTexture, NULL,
                                            &m_diffdistDirTextureTV));
    HR(m_pd3dDevice->CreateShaderResourceView(m_Texture, NULL, &m_TextureRV));
    HR(m_pd3dDevice->CreateRenderTargetView(m_Texture, NULL, &m_TextureTV));
    HR(m_pd3dDevice->CreateShaderResourceView(m_otherTexture, NULL,
            &m_otherTextureRV));
    HR(m_pd3dDevice->CreateRenderTargetView(m_otherTexture, NULL,
                                            &m_otherTextureTV));
    m_RegionDiffusion.OnResize(w, h);
}
void D3DApp::OnDrawToBimapResize()
{
    ReleaseCOM(m_distDirTextureTV);
    ReleaseCOM(m_distDirTextureRV);
    ReleaseCOM(m_DrawTextureDepthStencilBuffer);
    ReleaseCOM(m_DrawTextureDepthStencilView);
    ReleaseCOM(m_DrawTexture);
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width     = m_ClientWidth;
    depthStencilDesc.Height    = m_ClientHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format    = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
    depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
    depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags      = 0;
    int TexWidth = m_PicW * m_Scale;
    int TexHeight = m_PicH * m_Scale;
    if(TexWidth > 16384)
    {
        TexHeight *= 16384.0 / TexWidth;
        TexWidth = 16384;
    }
    if(TexHeight > 16384)
    {
        TexWidth *= 16384.0 / TexHeight;
        TexHeight = 16384;
    }
    if(TexWidth > 0)
    {
        depthStencilDesc.Width     = TexWidth;
        depthStencilDesc.Height    = TexHeight;
    }
    HR(m_pd3dDevice->CreateTexture2D(&depthStencilDesc, 0,
                                     &m_DrawTextureDepthStencilBuffer));
    HR(m_pd3dDevice->CreateDepthStencilView(m_DrawTextureDepthStencilBuffer, 0,
                                            &m_DrawTextureDepthStencilView));
    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    if(TexWidth > 0)
    {
        texDesc.Width     = TexWidth;
        texDesc.Height    = TexHeight;
    }
    else
    {
        texDesc.Width     = m_ClientWidth;
        texDesc.Height    = m_ClientHeight;
    }
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    printf("%d %d\n", TexWidth, TexHeight);
    HR(m_pd3dDevice->CreateTexture2D(&texDesc, 0, &m_DrawTexture));
    HR(m_pd3dDevice->CreateRenderTargetView(m_DrawTexture, NULL, &m_distDirTextureTV));
    HR(m_pd3dDevice->CreateShaderResourceView(m_DrawTexture, NULL, &m_distDirTextureRV));
}

void D3DApp::DrawScene()
{
    if(!m_DXUT_UI || !m_RenderTargetView || !m_DepthStencilView)
    {
        return;
    }
    //m_DXUT_UI->UpdataUI(0.1f);
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_ClearColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView,
                                           D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    InterSetSize(m_ClientWidth, m_ClientHeight);
    InterSetLookCenter(m_LookCenterX, m_LookCenterY);
    InterSetScale(m_Scale);
    InterDraw();
//     m_RegionDiffusion.ReadD3DSetting();
//     m_RegionDiffusion.DrawScene();
    m_SwapChain->Present(0, 0);
}



void D3DApp::BuildShaderFX()
{
    m_RegionDiffusion.ReadD3DSetting();
    m_RegionDiffusion.BuildShaderFX();
    ID3DBlob* pCode;
    ID3DBlob* pError;
    //Picture
    HRESULT hr = 0;
    hr = D3DX11CompileFromFile(_T("shader\\picture.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_Pics_Effect));
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
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_PICVertex, 2,
                                       PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_Pics_PLayout));
    //Triangle
    hr = 0;
    hr = D3DX11CompileFromFile(_T("shader\\triangle.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_Triangle_Effect));
    m_Triangle_PTech = m_Triangle_Effect->GetTechniqueByName("PointTech");
    m_Triangle_Width = m_Triangle_Effect->GetVariableByName("width")->AsScalar();
    m_Triangle_Height = m_Triangle_Effect->GetVariableByName("height")->AsScalar();
    m_Triangle_CenterX =
        m_Triangle_Effect->GetVariableByName("centerX")->AsScalar();
    m_Triangle_CenterY =
        m_Triangle_Effect->GetVariableByName("centerY")->AsScalar();
    m_Triangle_Scale = m_Triangle_Effect->GetVariableByName("scale")->AsScalar();
    m_Triangle_Alpha =
        m_Triangle_Effect->GetVariableByName("transparency")->AsScalar();
    D3DX11_PASS_DESC PassDescTri;
    m_Triangle_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri);
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_TRIVertex, 6,
                                       PassDescTri.pIAInputSignature,
                                       PassDescTri.IAInputSignatureSize, &m_Triangle_PLayout));
    hr = D3DX11CompileFromFile(_T("shader\\triangleline.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_TriangleLine_Effect));
    m_TriangleLine_PTech = m_TriangleLine_Effect->GetTechniqueByName("PointTech");
    m_TriangleLine_Width =
        m_TriangleLine_Effect->GetVariableByName("width")->AsScalar();
    m_TriangleLine_Height =
        m_TriangleLine_Effect->GetVariableByName("height")->AsScalar();
    m_TriangleLine_CenterX =
        m_TriangleLine_Effect->GetVariableByName("centerX")->AsScalar();
    m_TriangleLine_CenterY =
        m_TriangleLine_Effect->GetVariableByName("centerY")->AsScalar();
    m_TriangleLine_Scale =
        m_TriangleLine_Effect->GetVariableByName("scale")->AsScalar();
    m_TriangleLine_Alpha =
        m_TriangleLine_Effect->GetVariableByName("transparency")->AsScalar();
    D3DX11_PASS_DESC PassDescTri2;
    m_TriangleLine_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri2);
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_TRIVertex, 6,
                                       PassDescTri2.pIAInputSignature,
                                       PassDescTri2.IAInputSignatureSize, &m_TriangleLine_PLayout));
    hr = D3DX11CompileFromFile(_T("shader\\patch.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_Patch_Effect));
    m_Patch_PTech = m_Patch_Effect->GetTechniqueByName("PointTech");
    m_Patch_Width = m_Patch_Effect->GetVariableByName("width")->AsScalar();
    m_Patch_Height = m_Patch_Effect->GetVariableByName("height")->AsScalar();
    m_Patch_CenterX = m_Patch_Effect->GetVariableByName("centerX")->AsScalar();
    m_Patch_CenterY = m_Patch_Effect->GetVariableByName("centerY")->AsScalar();
    m_Patch_Scale = m_Patch_Effect->GetVariableByName("scale")->AsScalar();
    m_SelectPatch_Alpha =
        m_Patch_Effect->GetVariableByName("transparency")->AsScalar();
    D3DX11_PASS_DESC PassDescTri3;
    m_Patch_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri3);
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_TRIVertex, 6,
                                       PassDescTri3.pIAInputSignature,
                                       PassDescTri3.IAInputSignatureSize, &m_Patch_PLayout));
    hr = D3DX11CompileFromFile(_T("shader\\bigpoint.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_Points_Effect));
    m_Points_PTech = m_Points_Effect->GetTechniqueByName("PointTech");
    m_Points_Width = m_Points_Effect->GetVariableByName("width")->AsScalar();
    m_Points_Height = m_Points_Effect->GetVariableByName("height")->AsScalar();
    m_Points_CenterX = m_Points_Effect->GetVariableByName("centerX")->AsScalar();
    m_Points_CenterY = m_Points_Effect->GetVariableByName("centerY")->AsScalar();
    m_Points_Scale = m_Points_Effect->GetVariableByName("scale")->AsScalar();
    m_Points_Alpha =
        m_Points_Effect->GetVariableByName("transparency")->AsScalar();
    D3DX11_PASS_DESC PassDescTri4;
    m_Points_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri4);
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_PointVertex, 3,
                                       PassDescTri4.pIAInputSignature,
                                       PassDescTri4.IAInputSignatureSize, &m_Points_PLayout));
    hr = D3DX11CompileFromFile(_T("shader\\Line.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_Lines_Effect));
    m_Lines_PTech = m_Lines_Effect->GetTechniqueByName("PointTech");
    m_Lines_Width = m_Lines_Effect->GetVariableByName("width")->AsScalar();
    m_Lines_Height = m_Lines_Effect->GetVariableByName("height")->AsScalar();
    m_Lines_CenterX = m_Lines_Effect->GetVariableByName("centerX")->AsScalar();
    m_Lines_CenterY = m_Lines_Effect->GetVariableByName("centerY")->AsScalar();
    m_Lines_Scale = m_Lines_Effect->GetVariableByName("scale")->AsScalar();
    m_Lines_Alpha =
        m_Lines_Effect->GetVariableByName("transparency")->AsScalar();
    D3DX11_PASS_DESC PassDescTri5;
    m_Lines_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri5);
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_LineVertex, 6,
                                       PassDescTri5.pIAInputSignature,
                                       PassDescTri5.IAInputSignatureSize, &m_Lines_PLayout));
    hr = D3DX11CompileFromFile(_T("shader\\Line2w.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_Lines2w_Effect));
    m_Lines2w_PTech = m_Lines2w_Effect->GetTechniqueByName("PointTech");
    m_Lines2w_Width = m_Lines2w_Effect->GetVariableByName("width")->AsScalar();
    m_Lines2w_Height = m_Lines2w_Effect->GetVariableByName("height")->AsScalar();
    m_Lines2w_CenterX = m_Lines2w_Effect->GetVariableByName("centerX")->AsScalar();
    m_Lines2w_CenterY = m_Lines2w_Effect->GetVariableByName("centerY")->AsScalar();
    m_Lines2w_Scale = m_Lines2w_Effect->GetVariableByName("scale")->AsScalar();
    m_Lines2w_CenterAlpha = m_Lines2w_Effect->GetVariableByName("centerAlpha")->AsScalar();
    m_Lines2w_PeripheryAlpha = m_Lines2w_Effect->GetVariableByName("peripheryAlpha")->AsScalar();
    D3DX11_PASS_DESC PassDescTri6;
    m_Lines2w_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri6);
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_LineVertex2w, 7,
                                       PassDescTri6.pIAInputSignature,
                                       PassDescTri6.IAInputSignatureSize, &m_Lines2w_PLayout));
    hr = D3DX11CompileFromFile(_T("shader\\SkeletonLine.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            ReleaseCOM(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &m_SkeletonLines_Effect));
    m_SkeletonLines_PTech = m_SkeletonLines_Effect->GetTechniqueByName("PointTech");
    m_SkeletonLines_Width =
        m_SkeletonLines_Effect->GetVariableByName("width")->AsScalar();
    m_SkeletonLines_Height =
        m_SkeletonLines_Effect->GetVariableByName("height")->AsScalar();
    m_SkeletonLines_CenterX =
        m_SkeletonLines_Effect->GetVariableByName("centerX")->AsScalar();
    m_SkeletonLines_CenterY =
        m_SkeletonLines_Effect->GetVariableByName("centerY")->AsScalar();
    m_SkeletonLines_Scale =
        m_SkeletonLines_Effect->GetVariableByName("scale")->AsScalar();
    m_SkeletonLines_Alpha =
        m_SkeletonLines_Effect->GetVariableByName("transparency")->AsScalar();
    D3DX11_PASS_DESC PassDescTri7;
    m_SkeletonLines_PTech->GetPassByIndex(0)->GetDesc(&PassDescTri7);
    HR(m_pd3dDevice->CreateInputLayout(VertexDesc_SkeletonLineVertex, 3,
                                       PassDescTri7.pIAInputSignature,
                                       PassDescTri7.IAInputSignatureSize, &m_SkeletonLines_PLayout));
    // diffusion curve part
    hr = D3DX11CompileFromFile(_T("shader\\DiffusionCurves.fx"), NULL, NULL, NULL,
                               "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, NULL, NULL,
                               &pCode, &pError, NULL);
    if(FAILED(hr))
    {
        if(pError)
        {
            MessageBoxA(0, (char*)pError->GetBufferPointer(), 0, 0);
            SAFE_RELEASE(pError);
        }
        DXTrace(__FILE__, __LINE__, hr, _T("D3DX11CreateEffectFromFile"), TRUE);
    }
    ID3DX11Effect*  pEffect11;
    HR(D3DX11CreateEffectFromMemory(pCode->GetBufferPointer(),
                                    pCode->GetBufferSize(), NULL, m_pd3dDevice, &pEffect11));
    m_pDrawVectorsTechnique = pEffect11->GetTechniqueByName("DrawCurves");
    m_pDiffuseTechnique = pEffect11->GetTechniqueByName("Diffuse");
    m_pLineAntiAliasTechnique = pEffect11->GetTechniqueByName("LineAntiAlias");
    m_pDisplayImage = pEffect11->GetTechniqueByName("DisplayDiffusionImage");
    m_pDiffTex = pEffect11->GetVariableByName("g_diffTex")->AsShaderResource();
    for(int i = 0; i < 3; i++)
    {
        m_pInTex[i] = (pEffect11->GetVariableByName("g_inTex"))->GetElement(
                          i)->AsShaderResource();
    }
    m_blurOn = pEffect11->GetVariableByName("g_blurOn")->AsScalar();
    m_pDiffX = pEffect11->GetVariableByName("g_diffX")->AsScalar();
    m_pDiffY = pEffect11->GetVariableByName("g_diffY")->AsScalar();
    m_pScale = pEffect11->GetVariableByName("g_scale")->AsScalar();
    m_pAlpha = pEffect11->GetVariableByName("g_alpha")->AsScalar();
    m_pPolySize = pEffect11->GetVariableByName("g_polySize")->AsScalar();
    m_pPan = pEffect11->GetVariableByName("g_pan")->AsVector();
    m_pDrawVectorsTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
    if(FAILED(m_pd3dDevice->CreateInputLayout(VertexDesc_CurveVertex,
              4,
              PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
              &m_pCurveVertexLayout)))
    {
        return;
    }
    m_pDiffuseTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
    if(FAILED(m_pd3dDevice->CreateInputLayout(VertexDesc_VSOVertex,
              2,
              PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
              &m_pCurveVertex2Layout)))
    {
        return;
    }
    // diffusion curve part end
    m_Init = true;
}

void D3DApp::SetTexture(ID3D11ShaderResourceView* tex)
{
    if(!tex)
    {
        return;
    }
    ReleaseCOM(m_Pics_Texture);
    m_Pics_PMap->SetResource(tex);
    m_Pics_Texture = tex;
    OnResize(m_ClientWidth, m_ClientHeight);
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
    ReleaseCOM(m_Lines2w_Buffer);
    m_Lines2wVertices.clear();
    ReleaseCOM(m_SkeletonLines_Buffer);
    m_SkeletonLinesVertices.clear();
    ReleaseCOM(m_pCurveVertexBuffer);
    ReleaseCOM(m_pCurveVertex2Buffer);
    m_CurveVertexes.clear();
}

void D3DApp::BuildPoint()
{
    m_RegionDiffusion.ReadD3DSetting();
    m_RegionDiffusion.BuildPoint();
    if(!m_Init)
    {
        return;
    }
    ReleaseCOM(m_Pics_Buffer);
    ReleaseCOM(m_Triangle_Buffer);
    ReleaseCOM(m_TriangleLine_Buffer);
    ReleaseCOM(m_Patch_Buffer);
    ReleaseCOM(m_Points_Buffer);
    ReleaseCOM(m_Lines_Buffer);
    ReleaseCOM(m_Lines2w_Buffer);
    ReleaseCOM(m_SkeletonLines_Buffer);
    ReleaseCOM(m_pCurveVertexBuffer);
    ReleaseCOM(m_pCurveVertex2Buffer);
    if(!m_DeviceContext)
    {
        return;
    }
    m_PicsVertices.clear();
    if(m_Pics_Texture)
    {
        PictureVertex pv;
        pv.position.x = m_PicW / 2;
        pv.position.y = m_PicH / 2;
        pv.position.z = 0;
        pv.size.x = m_PicW;
        pv.size.y = m_PicH;
        m_PicsVertices.push_back(pv);
    }
    if(!m_PicsVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(PictureVertex) * m_PicsVertices.size());
        m_vbd.StructureByteStride = sizeof(PictureVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_PicsVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Pics_Buffer));
    }
    m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteOFF, 0);
    if(!m_TriangleVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(TriangleVertex) * m_TriangleVertices.size());
        m_vbd.StructureByteStride = sizeof(TriangleVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_TriangleVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Triangle_Buffer));
    }
    if(!m_TriangleLineVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(TriangleVertex) *
                                 m_TriangleLineVertices.size());
        m_vbd.StructureByteStride = sizeof(TriangleVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_TriangleLineVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_TriangleLine_Buffer));
    }
    if(!m_PatchVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(TriangleVertex) * m_PatchVertices.size());
        m_vbd.StructureByteStride = sizeof(TriangleVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_PatchVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Patch_Buffer));
    }
    if(!m_PointsVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(PointVertex) * m_PointsVertices.size());
        m_vbd.StructureByteStride = sizeof(PointVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_PointsVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Points_Buffer));
    }
    if(!m_LinesVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(LineVertex) * m_LinesVertices.size());
        m_vbd.StructureByteStride = sizeof(LineVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_LinesVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Lines_Buffer));
    }
    if(!m_Lines2wVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(LineVertex2w) * m_Lines2wVertices.size());
        m_vbd.StructureByteStride = sizeof(LineVertex2w);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_Lines2wVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_Lines2w_Buffer));
    }
    int originSize = m_SkeletonLinesVertices.size();
    m_SkeletonLinesVertices.insert(m_SkeletonLinesVertices.end(),
                                   m_SkeletonLinesVerticesCover.begin(),
                                   m_SkeletonLinesVerticesCover.end());
    if(!m_SkeletonLinesVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(SkeletonLineVertex) *
                                 m_SkeletonLinesVertices.size());
        m_vbd.StructureByteStride = sizeof(SkeletonLineVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_SkeletonLinesVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_SkeletonLines_Buffer));
    }
    m_SkeletonLinesVertices.erase(m_SkeletonLinesVertices.begin() + originSize,
                                  m_SkeletonLinesVertices.end());
    // diffusion curve part
    if(!m_CurveVertexes.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(CURVE_Vertex) * m_CurveVertexes.size());
        m_vbd.StructureByteStride = sizeof(CURVE_Vertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_CurveVertexes[0];
        m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_pCurveVertexBuffer);
        // render to view
        //create the screen space quad
        float xmin = -1, xmax = 1;
        float ymin = -1, ymax = 1;
        D3DXVECTOR3 pos[3 * 2] =
        {
            D3DXVECTOR3(xmin, ymin, +0.5f), D3DXVECTOR3(xmax, ymin, +0.5f), D3DXVECTOR3(xmin, ymax, +0.5f),
            D3DXVECTOR3(xmax, ymin, +0.5f), D3DXVECTOR3(xmax, ymax, +0.5f), D3DXVECTOR3(xmin, ymax, +0.5f)
        };
        D3DXVECTOR2 tex[3 * 2] =
        {
            D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f),
            D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)
        };
        VSO_Vertex pd[2 * 3];
        for(int ctri = 0; ctri < 2; ctri++)
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
        vinitData.pSysMem = &pd[0];
        m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_pCurveVertex2Buffer);
    }
}

void D3DApp::SetPictureSize(int w, int h)
{
    if(w == h && w == 0)
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

void special_normal(Vector2& v)
{
    if(v.x > 1)
    {
        v.x = 1;
    }
    if(v.x < -1)
    {
        v.x = -1;
    }
    if(v.y > 1)
    {
        v.y = 1;
    }
    if(v.y < -1)
    {
        v.y = -1;
    }
}

void D3DApp::AddTriangles(const ColorTriangles& tris, const vavImage& vimage)
{
    TriangleVertex tv;
    for(int i = 0; i < tris.size(); ++i)
    {
        const Vector2& v1 = tris[i].pts[0];
        const Vector2& v2 = tris[i].pts[1];
        const Vector2& v3 = tris[i].pts[2];
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
        tv.p1.x = tris[i].pts[0].x;
        tv.p1.y = m_PicH - tris[i].pts[0].y;
        tv.c1.x = c1[2] / 255.0f;
        tv.c1.y = c1[1] / 255.0f;
        tv.c1.z = c1[0] / 255.0f;
        tv.p2.x = tris[i].pts[1].x;
        tv.p2.y = m_PicH - tris[i].pts[1].y;
        tv.c2.x = c2[2] / 255.0f;
        tv.c2.y = c2[1] / 255.0f;
        tv.c2.z = c2[0] / 255.0f;
        tv.p3.x = tris[i].pts[2].x;
        tv.p3.y = m_PicH - tris[i].pts[2].y;
        tv.c3.x = c3[2] / 255.0f;
        tv.c3.y = c3[1] / 255.0f;
        tv.c3.z = c3[0] / 255.0f;
        m_TriangleVertices.push_back(tv);
    }
}

void D3DApp::AddPatchTriangles(const ColorTriangles& tris)
{
    TriangleVertex tv;
    float r, g, b;
    r = 50 / 255.0f;
    g = 200 / 255.0f;
    b = 50 / 255.0f;
    for(int i = 0; i < tris.size(); ++i)
    {
        const Vector2& v1 = tris[i].pts[0];
        const Vector2& v2 = tris[i].pts[1];
        const Vector2& v3 = tris[i].pts[2];
        tv.p1.x = tris[i].pts[0].x;
        tv.p1.y = m_PicH - tris[i].pts[0].y;
        tv.c1.x = r;
        tv.c1.y = g;
        tv.c1.z = b;
        tv.p2.x = tris[i].pts[1].x;
        tv.p2.y = m_PicH - tris[i].pts[1].y;
        tv.c2.x = r;
        tv.c2.y = g;
        tv.c2.z = b;
        tv.p3.x = tris[i].pts[2].x;
        tv.p3.y = m_PicH - tris[i].pts[2].y;
        tv.c3.x = r;
        tv.c3.y = g;
        tv.c3.z = b;
        m_PatchVertices.push_back(tv);
    }
}

void D3DApp::AddColorTriangles(const ColorTriangles& tris)
{
    TriangleVertex tv;
    for(int i = 0; i < tris.size(); ++i)
    {
        const Vector2& v1 = tris[i].pts[0];
        const Vector2& v2 = tris[i].pts[1];
        const Vector2& v3 = tris[i].pts[2];
        tv.p1.x = tris[i].pts[0].x;
        tv.p1.y = m_PicH - tris[i].pts[0].y;
        tv.c1.x = tris[i].color[0][2] / 255.0f;
        tv.c1.y = tris[i].color[0][1] / 255.0f;
        tv.c1.z = tris[i].color[0][0] / 255.0f;
        tv.p2.x = tris[i].pts[1].x;
        tv.p2.y = m_PicH - tris[i].pts[1].y;
        tv.c2.x = tris[i].color[1][2] / 255.0f;
        tv.c2.y = tris[i].color[1][1] / 255.0f;
        tv.c2.z = tris[i].color[1][0] / 255.0f;
        tv.p3.x = tris[i].pts[2].x;
        tv.p3.y = m_PicH - tris[i].pts[2].y;
        tv.c3.x = tris[i].color[2][2] / 255.0f;
        tv.c3.y = tris[i].color[2][1] / 255.0f;
        tv.c3.z = tris[i].color[2][0] / 255.0f;
        m_TriangleVertices.push_back(tv);
    }
}

void D3DApp::AddTrianglesLine(const ColorTriangles& tris)
{
    TriangleVertex tv;
    float r, g, b;
    r = (rand() % 255) / 255.0f;
    g = (rand() % 255) / 255.0f;
    b = (rand() % 255) / 255.0f;
    for(int i = 0; i < tris.size(); ++i)
    {
        const Vector2& v1 = tris[i].pts[0];
        const Vector2& v2 = tris[i].pts[1];
        const Vector2& v3 = tris[i].pts[2];
        tv.p1.x = tris[i].pts[0].x;
        tv.p1.y = m_PicH - tris[i].pts[0].y;
        tv.c1.x = r;
        tv.c1.y = g;
        tv.c1.z = b;
        tv.p2.x = tris[i].pts[1].x;
        tv.p2.y = m_PicH - tris[i].pts[1].y;
        tv.c2.x = r;
        tv.c2.y = g;
        tv.c2.z = b;
        tv.p3.x = tris[i].pts[2].x;
        tv.p3.y = m_PicH - tris[i].pts[2].y;
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
    if(D3D_OK != m_pd3dDevice->CreateDepthStencilState(&depth_stencil_desc,
            &m_pDepthStencil_ZWriteON))
    {
        return ;
    }
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    // 關閉zbuffer write
    if(D3D_OK != m_pd3dDevice->CreateDepthStencilState(&depth_stencil_desc,
            &m_pDepthStencil_ZWriteOFF))
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
    if(D3D_OK != m_pd3dDevice->CreateBlendState(&blend_state_desc,
            &m_pBlendState_ADD))
    {
        return;
    }
    blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA ;
    blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    // Alpha Blend混色模式
    if(D3D_OK != m_pd3dDevice->CreateBlendState(&blend_state_desc,
            &m_pBlendState_BLEND))
    {
        return ;
    }
    // Set blend
    float BlendFactor[4] = {0, 0, 0, 0};
    m_DeviceContext->OMSetBlendState(m_pBlendState_BLEND, BlendFactor, 0xffffffff);
}

void D3DApp::SetLookCenter(float x, float y)
{
    m_LookCenterX = x;
    m_LookCenterY = y;
    BuildPoint();
    DrawScene();
}

void D3DApp::SetScale(float s)
{
    m_Scale = s;
    OnDrawToBimapResize();
}


void D3DApp::InterSetScale(float s)
{
    if(!m_Init)
    {
        return;
    }
    m_Triangle_Scale->SetFloat(s);
    m_Pics_Scale->SetFloat(s);
    m_TriangleLine_Scale->SetFloat(s);
    m_Patch_Scale->SetFloat(s);
    m_Points_Scale->SetFloat(s);
    m_Lines_Scale->SetFloat(s);
    m_Lines2w_Scale->SetFloat(s);
    m_SkeletonLines_Scale->SetFloat(s);
    m_pScale->SetFloat(s);
}

void D3DApp::ClearPatchs()
{
    m_PatchVertices.clear();
}

void D3DApp::SetTransparency_Triangle(float t)
{
    m_Transparency_Triangle = t;
    m_Triangle_Alpha->SetFloat(t);
}

void D3DApp::SetTransparency_SelectPatch(float t)
{
    m_Transparency_SelectPatch = t;
    m_SelectPatch_Alpha->SetFloat(t);
    m_pAlpha->SetFloat(t);
}

void D3DApp::SetTransparency_TriangleLine(float t)
{
    m_Transparency_TriangleLine = t;
    m_TriangleLine_Alpha->SetFloat(t);
}

void D3DApp::SetTransparency_Line(float t)
{
    m_Transparency_Lines = t;
    m_Lines_Alpha->SetFloat(t);
    m_Lines2w_CenterAlpha->SetFloat(t);
    m_Lines2w_PeripheryAlpha->SetFloat(t * 0.5);
}

void D3DApp::SetTransparency_LineSkeleton(float t)
{
    m_Transparency_LineSkeleton = t;
    m_SkeletonLines_Alpha->SetFloat(t);
}

void D3DApp::SetTransparency_Picture(float t)
{
    m_Transparency_Picture = t;
    m_TransparencySV_Picture->SetFloat(t);
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

void D3DApp::AddLines(const Lines& lines, const double_vector2d& linewidths,
                      const Vector3s2d& colors)
{
    float scale = 1 / 255.0f;
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        const double_vector& now_linewidth = linewidths[i];
        const Vector3s& now_color = colors[i];
        if(now_line.size() < 3)
        {
            if(now_line.size() == 2)
            {
                LineVertex lv;
                lv.color.x = now_color[0][0] * scale;
                lv.color.y = now_color[0][1] * scale;
                lv.color.z = now_color[0][2] * scale;
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
        lv.color.x = now_color[0][0] * scale;
        lv.color.y = now_color[0][1] * scale;
        lv.color.z = now_color[0][2] * scale;
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
        for(int j = 1; j < now_line.size() - 2; ++j)
        {
            lv.color.x = now_color[j][0] * scale;
            lv.color.y = now_color[j][1] * scale;
            lv.color.z = now_color[j][2] * scale;
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
        }
        lv.color.x = now_color[now_line.size() - 1][0] * scale;
        lv.color.y = now_color[now_line.size() - 1][1] * scale;
        lv.color.z = now_color[now_line.size() - 1][2] * scale;
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
    }
}

void D3DApp::AddLines(const Lines& lines, const double_vector2d& linewidths)
{
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        const double_vector& now_linewidth = linewidths[i];
        float r, g, b;
        r = (rand() % 155 + 100) / 255.0f;
        g = (rand() % 155 + 100) / 255.0f;
        b = (rand() % 155 + 100) / 255.0f;
        if(now_line.size() < 3)
        {
            if(now_line.size() == 2)
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
        for(int j = 1; j < now_line.size() - 2; ++j)
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
    }
}

void D3DApp::AddLines(const Lines& lines)
{
    m_RegionDiffusion.AddLines(lines);
    return;
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        float r, g, b;
        r = (rand() % 155 + 100) / 255.0f;
        g = (rand() % 155 + 100) / 255.0f;
        b = (rand() % 155 + 100) / 255.0f;
        SkeletonLineVertex slv;
        slv.color.x = r;
        slv.color.y = g;
        slv.color.z = b;
        for(int j = 1; j < now_line.size(); ++j)
        {
            slv.p1.x = now_line[j].x;
            slv.p1.y = m_PicH - now_line[j].y;
            slv.p2.x = now_line[j - 1].x;
            slv.p2.y = m_PicH - now_line[j - 1].y;
            m_SkeletonLinesVertices.push_back(slv);
        }
    }
}

#ifdef USE_CGAL
void D3DApp::AddLines(const CgalLines& lines)
{
    for(int i = 0; i < lines.size(); ++i)
    {
        const CgalLine& now_line = lines[i];
        if(now_line.size() < 2)
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
        for(int j = 1; j < now_line.size(); ++j)
        {
            slv.p1.x = now_line[j].hx();
            slv.p1.y = m_PicH - now_line[j].hy();
            slv.p2.x = now_line[j - 1].hx();
            slv.p2.y = m_PicH - now_line[j - 1].hy();
            m_SkeletonLinesVertices.push_back(slv);
        }
    }
}
#endif // USE_CGAL

void D3DApp::AddLines(const CvLine& now_line)
{
    if(now_line.size() < 2)
    {
        return;
    }
    float r, g, b;
    r = (rand() % 155 + 100) / 255.0f;
    g = (rand() % 155 + 100) / 255.0f;
    b = (rand() % 155 + 100) / 255.0f;
    SkeletonLineVertex slv;
    slv.color.x = r;
    slv.color.y = g;
    slv.color.z = b;
    for(int j = 1; j < now_line.size(); ++j)
    {
        slv.p1.x = now_line[j].x;
        slv.p1.y = m_PicH - now_line[j].y;
        slv.p2.x = now_line[j - 1].x;
        slv.p2.y = m_PicH - now_line[j - 1].y;
        m_SkeletonLinesVertices.push_back(slv);
    }
}

void D3DApp::AddLinesWidth(const Lines& lines, const Lines& linewidths, const Vector3s2d& colors)
{
    float scale = 1 / 255.0f;
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        const Line& now_linewidth = linewidths[i];
        const Vector3s& now_color = colors[i];
        if(now_line.size() < 3)
        {
            if(now_line.size() == 2)
            {
                LineVertex2w lv;
                lv.color.x = now_color[0][0] * scale;
                lv.color.y = now_color[0][1] * scale;
                lv.color.z = now_color[0][2] * scale;
                lv.p1.x = now_line[0].x;
                lv.p1.y = m_PicH - now_line[0].y;
                lv.p2.x = now_line[0].x;
                lv.p2.y = m_PicH - now_line[0].y;
                lv.p3.x = now_line[1].x;
                lv.p3.y = m_PicH - now_line[1].y;
                lv.p4.x = now_line[1].x;
                lv.p4.y = m_PicH - now_line[1].y;
                lv.widthR.x = now_linewidth[0].x;
                lv.widthR.y = now_linewidth[1].x;
                lv.widthL.x = now_linewidth[0].y;
                lv.widthL.y = now_linewidth[1].y;
                m_Lines2wVertices.push_back(lv);
            }
            continue;
        }
        LineVertex2w lv;
        lv.color.x = now_color[0][0] * scale;
        lv.color.y = now_color[0][1] * scale;
        lv.color.z = now_color[0][2] * scale;
        lv.p1.x = now_line[0].x;
        lv.p1.y = m_PicH - now_line[0].y;
        lv.p2.x = now_line[0].x;
        lv.p2.y = m_PicH - now_line[0].y;
        lv.p3.x = now_line[1].x;
        lv.p3.y = m_PicH - now_line[1].y;
        lv.p4.x = now_line[2].x;
        lv.p4.y = m_PicH - now_line[2].y;
        lv.widthR.x = now_linewidth[0].x;
        lv.widthR.y = now_linewidth[1].x;
        lv.widthL.x = now_linewidth[0].y;
        lv.widthL.y = now_linewidth[1].y;
        m_Lines2wVertices.push_back(lv);
        for(int j = 1; j < now_line.size() - 2; ++j)
        {
            lv.color.x = now_color[j][0] * scale;
            lv.color.y = now_color[j][1] * scale;
            lv.color.z = now_color[j][2] * scale;
            lv.p1.x = now_line[j - 1].x;
            lv.p1.y = m_PicH - now_line[j - 1].y;
            lv.p2.x = now_line[j].x;
            lv.p2.y = m_PicH - now_line[j].y;
            lv.p3.x = now_line[j + 1].x;
            lv.p3.y = m_PicH - now_line[j + 1].y;
            lv.p4.x = now_line[j + 2].x;
            lv.p4.y = m_PicH - now_line[j + 2].y;
            lv.widthR.x = now_linewidth[j].x;
            lv.widthR.y = now_linewidth[j + 1].x;
            lv.widthL.x = now_linewidth[j].y;
            lv.widthL.y = now_linewidth[j + 1].y;
            m_Lines2wVertices.push_back(lv);
        }
        lv.color.x = now_color[now_line.size() - 1][0] * scale;
        lv.color.y = now_color[now_line.size() - 1][1] * scale;
        lv.color.z = now_color[now_line.size() - 1][2] * scale;
        lv.p1.x = now_line[now_line.size() - 3].x;
        lv.p1.y = m_PicH - now_line[now_line.size() - 3].y;
        lv.p2.x = now_line[now_line.size() - 2].x;
        lv.p2.y = m_PicH - now_line[now_line.size() - 2].y;
        lv.p3.x = now_line[now_line.size() - 1].x;
        lv.p3.y = m_PicH - now_line[now_line.size() - 1].y;
        lv.p4.x = now_line[now_line.size() - 1].x;
        lv.p4.y = m_PicH - now_line[now_line.size() - 1].y;
        lv.widthR.x = now_linewidth[now_line.size() - 2].x;
        lv.widthR.y = now_linewidth[now_line.size() - 1].x;
        lv.widthL.x = now_linewidth[now_line.size() - 2].y;
        lv.widthL.y = now_linewidth[now_line.size() - 1].y;
        m_Lines2wVertices.push_back(lv);
    }
}

void D3DApp::AddLines(const Lines& lines, const Lines& linewidths)
{
    Lines normals = GetNormalsLen2(lines);
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        const Line& now_width = linewidths[i];
        const Line& now_normal = normals[i];
        if(now_line.size() < 2)
        {
            continue;
        }
        float r, g, b;
        r = 0;
        g = 1;
        b = 0;
        SkeletonLineVertex slv;
        slv.color.x = r;
        slv.color.y = g;
        slv.color.z = b;
        for(int j = 1; j < now_line.size(); ++j)
        {
            Vector2 gapl = now_normal[j] * now_width[j].x;
            Vector2 gapr = now_normal[j] * now_width[j].y;
            slv.p1.x = now_line[j].x - gapl.x;
            slv.p1.y = m_PicH - (now_line[j].y - gapl.y);
            slv.p2.x = now_line[j - 1].x - gapl.x;
            slv.p2.y = m_PicH - (now_line[j - 1].y - gapl.y);
            m_SkeletonLinesVertices.push_back(slv);
            slv.p1.x = now_line[j].x + gapr.x;
            slv.p1.y = m_PicH - (now_line[j].y + gapr.y);
            slv.p2.x = now_line[j - 1].x + gapr.x;
            slv.p2.y = m_PicH - (now_line[j - 1].y + gapr.y);
            m_SkeletonLinesVertices.push_back(slv);
        }
    }
}

void D3DApp::AddLinesWidth(const Lines& lines, const Lines& linewidths)
{
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        const Line& now_linewidth = linewidths[i];
        float r, g, b;
        r = (rand() % 155 + 100) / 255.0f;
        g = (rand() % 155 + 100) / 255.0f;
        b = (rand() % 155 + 100) / 255.0f;
        if(now_line.size() < 3)
        {
            if(now_line.size() == 2)
            {
                LineVertex2w lv;
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
                lv.widthR.x = now_linewidth[0].x;
                lv.widthR.y = now_linewidth[1].x;
                lv.widthL.x = now_linewidth[0].y;
                lv.widthL.y = now_linewidth[1].y;
                m_Lines2wVertices.push_back(lv);
            }
            continue;
        }
        LineVertex2w lv;
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
        lv.widthR.x = now_linewidth[0].x;
        lv.widthR.y = now_linewidth[1].x;
        lv.widthL.x = now_linewidth[0].y;
        lv.widthL.y = now_linewidth[1].y;
        m_Lines2wVertices.push_back(lv);
        for(int j = 1; j < now_line.size() - 2; ++j)
        {
            lv.p1.x = now_line[j - 1].x;
            lv.p1.y = m_PicH - now_line[j - 1].y;
            lv.p2.x = now_line[j].x;
            lv.p2.y = m_PicH - now_line[j].y;
            lv.p3.x = now_line[j + 1].x;
            lv.p3.y = m_PicH - now_line[j + 1].y;
            lv.p4.x = now_line[j + 2].x;
            lv.p4.y = m_PicH - now_line[j + 2].y;
            lv.widthR.x = now_linewidth[j].x;
            lv.widthR.y = now_linewidth[j + 1].x;
            lv.widthL.x = now_linewidth[j].y;
            lv.widthL.y = now_linewidth[j + 1].y;
            m_Lines2wVertices.push_back(lv);
        }
        lv.p1.x = now_line[now_line.size() - 3].x;
        lv.p1.y = m_PicH - now_line[now_line.size() - 3].y;
        lv.p2.x = now_line[now_line.size() - 2].x;
        lv.p2.y = m_PicH - now_line[now_line.size() - 2].y;
        lv.p3.x = now_line[now_line.size() - 1].x;
        lv.p3.y = m_PicH - now_line[now_line.size() - 1].y;
        lv.p4.x = now_line[now_line.size() - 1].x;
        lv.p4.y = m_PicH - now_line[now_line.size() - 1].y;
        lv.widthR.x = now_linewidth[now_line.size() - 2].x;
        lv.widthR.y = now_linewidth[now_line.size() - 1].x;
        lv.widthL.x = now_linewidth[now_line.size() - 2].y;
        lv.widthL.y = now_linewidth[now_line.size() - 1].y;
        m_Lines2wVertices.push_back(lv);
    }
}

void D3DApp::AddLineSegs(const LineSegs& lines)
{
    for(int i = 0; i < lines.size(); ++i)
    {
        const LineSeg& now_line = lines[i];
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

void D3DApp::AddLinesLine(const Lines& lines, const double_vector2d& linewidths)
{
    LineSegs lineSegs;
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        Line rights;
        rights.resize(now_line.size());
        const double_vector& now_linewidth = linewidths[i];
        rights[0] = Quaternion::GetRotation(now_line[1] - now_line[0], 90);
        for(int j = 1; j < now_line.size(); ++j)
        {
            rights[j] = Quaternion::GetRotation(now_line[j] - now_line[j - 1], 90);
        }
        lineSegs.push_back(LineSeg(now_line.front() - rights.front()
                                   *now_linewidth.front() * 0.5,
                                   now_line.front() + rights.front()*now_linewidth.front() * 0.5));
        for(int j = 1; j < now_line.size(); ++j)
        {
            lineSegs.push_back(LineSeg(now_line[j] + rights[j]*now_linewidth[j] * 0.5,
                                       now_line[j - 1] + rights[j - 1]*now_linewidth[j - 1] * 0.5));
        }
        lineSegs.push_back(LineSeg(now_line.back() - rights.back()*now_linewidth.back()
                                   * 0.5,
                                   now_line.back() + rights.back()*now_linewidth.back() * 0.5));
        for(int j = now_line.size() - 1; j > 0; --j)
        {
            lineSegs.push_back(LineSeg(now_line[j] - rights[j]*now_linewidth[j] * 0.5,
                                       now_line[j - 1] - rights[j - 1]*now_linewidth[j - 1] * 0.5));
        }
    }
    AddLineSegs(lineSegs);
}

void D3DApp::InterDraw(bool drawDiffusion)
{
    if(!m_Init)
    {
        return;
    }
    float BlendFactor[4] = {0, 0, 0, 0};
    m_DeviceContext->OMSetBlendState(m_pBlendState_BLEND, BlendFactor, 0xffffffff);
    m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteON, 0);
    //Draw Diffusion
    if(drawDiffusion && m_CurveVertexes.size() > 0)
    {
        m_DeviceContext->ClearRenderTargetView(m_TextureTV, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        m_DeviceContext->ClearRenderTargetView(m_diffuseTextureTV[0], D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        m_DeviceContext->ClearRenderTargetView(m_diffuseTextureTV[1], D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        m_DeviceContext->ClearRenderTargetView(m_otherTextureTV, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        m_DeviceContext->ClearRenderTargetView(m_diffdistDirTextureTV, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
        m_DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        // diffusion curve part
        HRESULT hr;
        D3DX11_TECHNIQUE_DESC techDesc;
        D3DX11_PASS_DESC PassDesc;
        UINT stride, offset;
        //store the old render targets and viewports
        ID3D11RenderTargetView* old_pRTV = DXUTGetD3D11RenderTargetView();
        ID3D11DepthStencilView* old_pDSV = DXUTGetD3D11DepthStencilView();
        m_DeviceContext->OMGetRenderTargets(1, &old_pRTV, &old_pDSV);
        // set the shader variables, they are valid through the whole rendering pipeline
        HR(m_pPolySize->SetFloat(m_polySize));
        // render the triangles to the highest input texture level (assuming they are already defined!)
        m_DeviceContext->IASetInputLayout(m_pCurveVertexLayout);
        stride = sizeof(CURVE_Vertex);
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertexBuffer, &stride, &offset);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        //construct the curve triangles in the geometry shader and render them directly
        ID3D11RenderTargetView* destTexTV[3];
        destTexTV[0] = m_diffuseTextureTV[m_diffTex];
        destTexTV[1] = m_diffdistDirTextureTV;
        destTexTV[2] = m_otherTextureTV;
        m_DeviceContext->OMSetRenderTargets(3, destTexTV, m_pDepthStencilView);
        m_pDrawVectorsTechnique->GetDesc(&techDesc);
        for(int p = techDesc.Passes - 1; p >= 0; --p)
        {
            if(p == 4)
            {
                //continue;
            }
            m_pDrawVectorsTechnique->GetPassByIndex(p)->Apply(0, m_DeviceContext);
            m_DeviceContext->Draw(m_CurveVertexes.size(), 0);
        }
        // setup the pipeline for the following image-space algorithms
        m_DeviceContext->IASetInputLayout(m_pCurveVertex2Layout);
        stride = sizeof(VSO_Vertex);
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertex2Buffer, &stride, &offset);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // diffuse the texture in both directions
        HR(m_pInTex[1]->SetResource(m_diffdistDirTextureRV));
        for(int i = 0; i < m_diffSteps; i++)
        {
            if(i > m_diffSteps * 0.5)
            {
                m_blurOn->SetFloat(1);
            }
            else
            {
                m_blurOn->SetFloat(0);
            }
            // SA strategy
            HR(m_pPolySize->SetFloat(1.0 - (float)(i) / (float)m_diffSteps));
            // SH strategy
            // V( m_pPolySize->SetFloat( 1.0 ) );
            // if (i>diffSteps-diffSteps/2)
            // {
            //         V( m_pPolySize->SetFloat( (float)(diffSteps-i)/(float)(diffSteps/2) ) );
            // }
            m_DeviceContext->OMSetRenderTargets(1, &m_diffuseTextureTV[1 - m_diffTex], NULL);
            (m_pInTex[0]->SetResource(m_diffuseTextureRV[m_diffTex]));
            m_diffTex = 1 - m_diffTex;
            m_pDiffuseTechnique->GetDesc(&techDesc);
            for(UINT p = 0; p < techDesc.Passes; ++p)
            {
                m_pDiffuseTechnique->GetPassByIndex(p)->Apply(0, m_DeviceContext);
                m_DeviceContext->Draw(6, 0);
            }
        }
        // anti alias the lines
        m_DeviceContext->OMSetRenderTargets(1, &m_diffuseTextureTV[1 - m_diffTex], NULL);
        HR(m_pInTex[0]->SetResource(m_diffuseTextureRV[m_diffTex]));
        HR(m_pInTex[1]->SetResource(m_otherTextureRV));
        m_diffTex = 1 - m_diffTex;
        HR(m_pDiffTex->SetResource(m_diffdistDirTextureRV));
        m_pLineAntiAliasTechnique->GetDesc(&techDesc);
        for(UINT p = 0; p < techDesc.Passes; ++p)
        {
            m_pLineAntiAliasTechnique->GetPassByIndex(p)->Apply(0, m_DeviceContext);
            m_DeviceContext->Draw(6, 0);
        }
        // render to view
        m_DeviceContext->OMSetRenderTargets(1, &m_TextureTV, 0);
        // Set the input layout
        m_DeviceContext->IASetInputLayout(m_pCurveVertex2Layout);
        // Set vertex buffer
        stride = sizeof(VSO_Vertex);
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertex2Buffer, &stride, &offset);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        HR(m_pDiffTex->SetResource(m_diffuseTextureRV[m_diffTex]));
        //HR(m_pDiffTex->SetResource(m_diffdistDirTextureRV));
        m_pDisplayImage->GetDesc(&techDesc);
        for(UINT p = 0; p < techDesc.Passes; ++p)
        {
            m_pDisplayImage->GetPassByIndex(p)->Apply(0, m_DeviceContext);
            m_DeviceContext->Draw(6, 0);
        }
        //restore old render targets
        m_DeviceContext->OMSetRenderTargets(1,  &old_pRTV, old_pDSV);
        offset = 0;
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertex2Buffer, &stride, &offset);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        HR(m_pDiffTex->SetResource(m_TextureRV));
        for(UINT p = 0; p < techDesc.Passes; ++p)
        {
            m_pDisplayImage->GetPassByIndex(p)->Apply(0, m_DeviceContext);
            m_DeviceContext->Draw(6, 0);
        }
    }
    m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteOFF, 0);
	m_DeviceContext->OMSetBlendState(m_pBlendState_BLEND, BlendFactor, 0xffffffff);
    //Draw Picture
    if(m_PicsVertices.size() > 0)
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
    //Draw Triangles
    if(m_TriangleVertices.size() > 0)
    {
        UINT offset = 0;
        UINT stride2 = sizeof(TriangleVertex);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        m_DeviceContext->IASetInputLayout(m_Triangle_PLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_Triangle_Buffer, &stride2,
                                            &offset);
        m_Triangle_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
        m_DeviceContext->Draw((UINT)m_TriangleVertices.size(), 0);
    }
    if(m_TriangleLineVertices.size() > 0)
    {
        UINT offset = 0;
        UINT stride2 = sizeof(TriangleVertex);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        m_DeviceContext->IASetInputLayout(m_TriangleLine_PLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_TriangleLine_Buffer, &stride2,
                                            &offset);
        m_TriangleLine_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
        m_DeviceContext->Draw((UINT)m_TriangleLineVertices.size(), 0);
    }
    if(m_PatchVertices.size() > 0)
    {
        UINT offset = 0;
        UINT stride2 = sizeof(TriangleVertex);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        m_DeviceContext->IASetInputLayout(m_Patch_PLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_Patch_Buffer, &stride2, &offset);
        m_Patch_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
        m_DeviceContext->Draw((UINT)m_PatchVertices.size(), 0);
    }
    if(m_PointsVertices.size() > 0)
    {
        m_Points_Alpha->SetFloat(0.9);
        UINT offset = 0;
        UINT stride2 = sizeof(PointVertex);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        m_DeviceContext->IASetInputLayout(m_Points_PLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_Points_Buffer, &stride2, &offset);
        m_Points_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
        m_DeviceContext->Draw((UINT)m_PointsVertices.size(), 0);
    }
    if(m_LinesVertices.size() > 0)
    {
        UINT offset = 0;
        UINT stride2 = sizeof(LineVertex);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        m_DeviceContext->IASetInputLayout(m_Lines_PLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_Lines_Buffer, &stride2, &offset);
        m_Lines_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
        m_DeviceContext->Draw((UINT)m_LinesVertices.size(), 0);
    }
    if(m_Lines2wVertices.size() > 0)
    {
        UINT offset = 0;
        UINT stride2 = sizeof(LineVertex2w);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        m_DeviceContext->IASetInputLayout(m_Lines2w_PLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_Lines2w_Buffer, &stride2, &offset);
        m_Lines2w_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
        m_DeviceContext->Draw((UINT)m_Lines2wVertices.size(), 0);
    }
    if(m_SkeletonLinesVertices.size() > 0)
    {
        UINT offset = 0;
        UINT stride2 = sizeof(SkeletonLineVertex);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        m_DeviceContext->IASetInputLayout(m_SkeletonLines_PLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_SkeletonLines_Buffer, &stride2,
                                            &offset);
        m_SkeletonLines_PTech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
        m_DeviceContext->Draw((UINT)m_SkeletonLinesVertices.size() +
                              m_SkeletonLinesVerticesCover.size(), 0);
    }
}

void D3DApp::InterSetLookCenter(float x, float y)
{
    if(!m_Init)
    {
        return;
    }
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
    m_Lines2w_CenterX->SetFloat(x);
    m_Lines2w_CenterY->SetFloat(y);
    m_SkeletonLines_CenterX->SetFloat(x);
    m_SkeletonLines_CenterY->SetFloat(y);
    m_pan.x = x;
    m_pan.y = y;
    m_pPan->SetFloatVector(m_pan);
}

void D3DApp::InterSetSize(float w, float h)
{
    if(!m_Init)
    {
        return;
    }
    m_Pics_Width->SetFloat(w);
    m_Pics_Height->SetFloat(h);
    m_Triangle_Width->SetFloat(w);
    m_Triangle_Height->SetFloat(h);
    m_TriangleLine_Width->SetFloat(w);
    m_TriangleLine_Height->SetFloat(h);
    m_Patch_Width->SetFloat(w);
    m_Patch_Height->SetFloat(h);
    m_Points_Width->SetFloat(w);
    m_Points_Height->SetFloat(h);
    m_Lines_Width->SetFloat(w);
    m_Lines_Height->SetFloat(h);
    m_Lines2w_Width->SetFloat(w);
    m_Lines2w_Height->SetFloat(h);
    m_SkeletonLines_Width->SetFloat(w);
    m_SkeletonLines_Height->SetFloat(h);
    m_pDiffX->SetFloat(w);
    m_pDiffY->SetFloat(h);
}

cv::Mat D3DApp::DrawSceneToCvMat(D3DXCOLOR backcolor, bool drawDiffusion)
{
    const int TexWidth = m_PicW * m_Scale;
    const int TexHeight = m_PicH * m_Scale;
    if(TexWidth > 0)
    {
        ID3D11RenderTargetView* old_pRTV = DXUTGetD3D11RenderTargetView();
        ID3D11DepthStencilView* old_pDSV = DXUTGetD3D11DepthStencilView();
        m_DeviceContext->OMGetRenderTargets(1, &old_pRTV, &old_pDSV);
        UINT NumViewports = 1;
        D3D11_VIEWPORT pViewports[10];
        m_DeviceContext->RSGetViewports(&NumViewports, &pViewports[0]);
        m_DeviceContext->OMSetRenderTargets(1, &m_distDirTextureTV,
                                            m_DrawTextureDepthStencilView);
        m_DeviceContext->ClearDepthStencilView(m_DrawTextureDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        m_DeviceContext->ClearRenderTargetView(m_distDirTextureTV, backcolor);
        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp.Width    = (float)TexWidth;
        vp.Height   = (float)TexHeight;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        m_DeviceContext->RSSetViewports(1, &vp);
        InterSetScale(m_Scale);
        InterSetLookCenter(0, 0);
        InterSetSize(TexWidth, TexHeight);
        InterDraw(drawDiffusion);
        ID3D11Texture2D* pTextureRead;
        D3D11_TEXTURE2D_DESC texDescCV;
        ZeroMemory(&texDescCV, sizeof(texDescCV));
        texDescCV.Width     = TexWidth;
        texDescCV.Height    = TexHeight;
        texDescCV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        texDescCV.MipLevels = 1;
        texDescCV.ArraySize = 1;
        texDescCV.SampleDesc.Quality = 0;
        texDescCV.SampleDesc.Count = 1;
        texDescCV.MiscFlags = 0;
        texDescCV.Usage = D3D11_USAGE_STAGING;
        texDescCV.BindFlags = 0;
        texDescCV.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        HR(m_pd3dDevice->CreateTexture2D(&texDescCV, 0, &pTextureRead));
        m_DeviceContext->CopyResource(pTextureRead, m_DrawTexture);
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        float* pimg;
        unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
        HR(m_DeviceContext->Map(pTextureRead, subresource, D3D11_MAP_READ, 0,
                                &MappedResource));
        pimg = (float*)MappedResource.pData;
        cv::Mat simg;
        simg.create(TexHeight, TexWidth, CV_8UC3);
        simg = cv::Scalar(0);
        int addoffset = (8 - (TexWidth - (TexWidth / 8 * 8))) % 8;
        for(int j = 0; j < simg.cols; ++j)
        {
            for(int i = 0; i < simg.rows; ++i)
            {
                int offset = (i * (simg.cols + addoffset) + j) * 4;
                cv::Vec3b& intensity = simg.at<cv::Vec3b>(i, j);
                intensity[2] = pimg[offset  ] * 255.0f;
                intensity[1] = pimg[offset + 1] * 255.0f;
                intensity[0] = pimg[offset + 2] * 255.0f;
            }
        }
        g_cvshowEX.AddShow("live", simg);
        m_DeviceContext->OMSetRenderTargets(1,  &old_pRTV,  old_pDSV);
        m_DeviceContext->RSSetViewports(NumViewports, &pViewports[0]);
        InterSetRenderTransparencyDefault();
        ReleaseCOM(pTextureRead);
        return simg;
    }
    else // no load any image
    {
        return cv::Mat(m_ClientHeight, m_ClientWidth, CV_8UC3);
    }
}

void D3DApp::InterSetRenderTransparencyOutput1()
{
    m_Triangle_Alpha->SetFloat(1);
    m_SelectPatch_Alpha->SetFloat(0);
    m_pAlpha->SetFloat(0);
    m_TriangleLine_Alpha->SetFloat(0);
    m_Lines_Alpha->SetFloat(1);
    m_Lines2w_CenterAlpha->SetFloat(1);
    m_SkeletonLines_Alpha->SetFloat(0.1);
    m_TransparencySV_Picture->SetFloat(1);
}

void D3DApp::InterSetRenderTransparencyOutput2()
{
    m_Triangle_Alpha->SetFloat(0);
    m_SelectPatch_Alpha->SetFloat(0);
    m_pAlpha->SetFloat(0);
    m_TriangleLine_Alpha->SetFloat(0);
    m_Lines_Alpha->SetFloat(0);
    m_Lines2w_CenterAlpha->SetFloat(0);
    m_SkeletonLines_Alpha->SetFloat(1);
    m_TransparencySV_Picture->SetFloat(0);
}

void D3DApp::InterSetRenderTransparencyOutput3()
{
    m_Triangle_Alpha->SetFloat(0);
    m_SelectPatch_Alpha->SetFloat(0);
    m_pAlpha->SetFloat(0);
    m_TriangleLine_Alpha->SetFloat(0);
    m_Lines_Alpha->SetFloat(0);
    m_Lines2w_CenterAlpha->SetFloat(1);
    m_SkeletonLines_Alpha->SetFloat(0);
    m_TransparencySV_Picture->SetFloat(0);
}

void D3DApp::InterSetRenderTransparencyDefault()
{
    m_Triangle_Alpha->SetFloat(m_Transparency_Triangle);
    m_SelectPatch_Alpha->SetFloat(m_Transparency_SelectPatch);
    m_pAlpha->SetFloat(m_Transparency_SelectPatch);
    m_TriangleLine_Alpha->SetFloat(m_Transparency_TriangleLine);
    m_Lines_Alpha->SetFloat(m_Transparency_Lines);
    m_Lines2w_CenterAlpha->SetFloat(m_Transparency_Lines);
    m_SkeletonLines_Alpha->SetFloat(m_Transparency_LineSkeleton);
    m_TransparencySV_Picture->SetFloat(m_Transparency_Picture);
}

void D3DApp::SetScaleTemporary(float s)
{
    m_ScaleSave = m_Scale;
    m_Scale = s;
    OnDrawToBimapResize();
}

void D3DApp::SetScaleRecovery()
{
    m_Scale = m_ScaleSave;
    OnDrawToBimapResize();
}

void D3DApp::AddLinesCover(const Lines& lines)
{
    for(int i = 0; i < lines.size(); ++i)
    {
        const Line& now_line = lines[i];
        if(now_line.size() < 2)
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
        for(int j = 1; j < now_line.size(); ++j)
        {
            slv.p1.x = now_line[j].x;
            slv.p1.y = m_PicH - now_line[j].y;
            slv.p2.x = now_line[j - 1].x;
            slv.p2.y = m_PicH - now_line[j - 1].y;
            m_SkeletonLinesVerticesCover.push_back(slv);
        }
    }
}

void D3DApp::ClearCovers()
{
    m_SkeletonLinesVerticesCover.clear();
}

void D3DApp::AddDiffusionLines(const Lines& lines)
{
    float scale = 1 / 255.0f;
    for(int i = 0; i < lines.size(); ++i)
    {
        CURVE_Vertexes curveline;
        const Line& now_line = lines[i];
        if(now_line.size() < 2)
        {
            continue;
        }
        float r, g, b;
        r = (rand() % 155 + 100) * scale;
        g = (rand() % 155 + 100) * scale;
        b = (rand() % 155 + 100) * scale;
        float r2, g2, b2;
        r2 = (rand() % 155 + 100) * scale;
        g2 = (rand() % 155 + 100) * scale;
        b2 = (rand() % 155 + 100) * scale;
        CURVE_Vertex vtx1, vtx2;
        vtx1.lcolor.x = r;
        vtx1.lcolor.y = g;
        vtx1.lcolor.z = b;
        vtx1.lcolor.w = 0;
        vtx1.rcolor.x = r2;
        vtx1.rcolor.y = g2;
        vtx1.rcolor.z = b2;
        vtx1.rcolor.w = 0;
        vtx2.lcolor.x = r;
        vtx2.lcolor.y = g;
        vtx2.lcolor.z = b;
        vtx2.lcolor.w = 0;
        vtx2.rcolor.x = r2;
        vtx2.rcolor.y = g2;
        vtx2.rcolor.z = b2;
        vtx2.rcolor.w = 0;
        for(int j = 1; j < now_line.size() - 1; ++j)
        {
            vtx1.pos.x = now_line[j].x;
            vtx1.pos.y = m_PicH - now_line[j].y;
            vtx2.pos.x = now_line[j + 1].x;
            vtx2.pos.y = m_PicH - now_line[j + 1].y;
            curveline.push_back(vtx1);
            curveline.push_back(vtx2);
        }
        curveline.front().nb = D3DXVECTOR2(10000.0f, 10000.0f);
        for(int j = 2; j < curveline.size(); j += 2)
        {
            curveline[j].nb = curveline[j - 2].pos;
            curveline[j - 1].nb = curveline[j + 1].pos;
        }
        (curveline.end() - 2)->nb = (curveline.end() - 4)->pos;
        curveline.back().nb = D3DXVECTOR2(10000.0f, 10000.0f);
        m_CurveVertexes.insert(m_CurveVertexes.end(), curveline.begin(), curveline.end());
    }
}

void D3DApp::AddDiffusionLines(const Lines& lines, const Vector3s2d& colors)
{
    float scale = 1 / 255.0f;
    for(int i = 0; i < lines.size(); ++i)
    {
        CURVE_Vertexes curveline;
        Line now_line = lines[i];
        const Vector3s& now_color = colors[i];
        CURVE_Vertex vtx1, vtx2;
        vtx1.lcolor.w = 0.0;
        vtx1.rcolor.w = 0.0;
        vtx2.lcolor.w = 0.0;
        vtx2.rcolor.w = 0.0;
        for(int j = 0; j < now_line.size() - 1; ++j)
        {
            if(now_line[j].distance(now_line[j + 1]) < 0.01)
            {
                now_line.erase(now_line.begin() + j);
                --j;
            }
        }
        if(now_line.size() > 3 && now_line.size() % 2 == 1)
        {
            Vector2 np = (now_line[0] + now_line[1]) / 2;
            now_line.insert(now_line.begin() + 1, np);
        }
        for(int j = 0; j < now_line.size() - 1; ++j)
        {
//          if (now_color[j].x == 0 && now_color[j].y == 0 && now_color[j].z == 0)
//          {
//              continue;
//          }
//          if (now_color[j + 1].x == 0 && now_color[j + 1].y == 0 && now_color[j + 1].z == 0)
//          {
//              continue;
//          }
            vtx1.lcolor.x = now_color[j].x * scale;
            vtx1.lcolor.y = now_color[j].y * scale;
            vtx1.lcolor.z = now_color[j].z * scale;
            vtx1.rcolor = vtx1.lcolor;
            vtx2.lcolor.x = now_color[j + 1].x * scale;
            vtx2.lcolor.y = now_color[j + 1].y * scale;
            vtx2.lcolor.z = now_color[j + 1].z * scale;
            vtx2.rcolor = vtx2.lcolor;
            vtx1.pos.x = now_line[j].x;
            vtx1.pos.y = m_PicH - now_line[j].y;
            vtx2.pos.x = now_line[j + 1].x;
            vtx2.pos.y = m_PicH - now_line[j + 1].y;
            curveline.push_back(vtx1);
            curveline.push_back(vtx2);
        }
        if(curveline.size() > 3)
        {
            curveline.front().nb = D3DXVECTOR2(10000.0f, 10000.0f);
            for(int j = 2; j < curveline.size(); j += 2)
            {
                curveline[j].nb = curveline[j - 2].pos;
                curveline[j - 1].nb = curveline[j + 1].pos;
            }
            (curveline.end() - 2)->nb = (curveline.end() - 4)->pos;
            curveline.back().nb = D3DXVECTOR2(10000.0f, 10000.0f);
            m_CurveVertexes.insert(m_CurveVertexes.end(), curveline.begin(), curveline.end());
        }
    }
}

void D3DApp::AddDiffusionLines(const Lines& lines, const Color2Side& colors)
{
//     m_RegionDiffusion.AddDiffusionLines(lines, colors);
//     return;
    float scale = 1 / 255.0f;
    for(int i = 0; i < lines.size(); ++i)
    {
        CURVE_Vertexes curveline;
        Line now_line = lines[i];
        const Vector3s& now_lcolor = colors.left[i];
        const Vector3s& now_rcolor = colors.right[i];
        if(now_line.size() < 2)
        {
            continue;
        }
        CURVE_Vertex vtx1, vtx2;
        vtx1.lcolor.w = 0;
        vtx1.rcolor.w = 0;
        vtx2.lcolor.w = 0;
        vtx2.rcolor.w = 0;
        for(int j = 0; j < now_line.size() - 1; ++j)
        {
            if(now_line[j].distance(now_line[j + 1]) < 0.01)
            {
                now_line.erase(now_line.begin() + j);
                --j;
            }
        }
        if(now_line.size() > 3)
        {
            Vector2 np = (now_line[0] + now_line[1]) / 2;
            now_line.insert(now_line.begin() + 1, np);
        }
        for(int j = 0; j < now_line.size() - 1; ++j)
        {
            vtx1.lcolor.x = now_lcolor[j].x * scale;
            vtx1.lcolor.y = now_lcolor[j].y * scale;
            vtx1.lcolor.z = now_lcolor[j].z * scale;
            vtx1.rcolor.x = now_rcolor[j].x * scale;
            vtx1.rcolor.y = now_rcolor[j].y * scale;
            vtx1.rcolor.z = now_rcolor[j].z * scale;
            vtx2.lcolor.x = now_lcolor[j].x * scale;
            vtx2.lcolor.y = now_lcolor[j].y * scale;
            vtx2.lcolor.z = now_lcolor[j].z * scale;
            vtx2.rcolor.x = now_rcolor[j].x * scale;
            vtx2.rcolor.y = now_rcolor[j].y * scale;
            vtx2.rcolor.z = now_rcolor[j].z * scale;
            vtx1.pos.x = now_line[j].x;
            vtx1.pos.y = m_PicH - now_line[j].y;
            vtx2.pos.x = now_line[j + 1].x;
            vtx2.pos.y = m_PicH - now_line[j + 1].y;
            curveline.push_back(vtx1);
            curveline.push_back(vtx2);
        }
        if(curveline.size() < 3)
        {
            continue;
        }
        curveline.front().nb = D3DXVECTOR2(10000.0f, 10000.0f);
        for(int j = 2; j < curveline.size() - 2; j += 2)
        {
            curveline[j].nb = curveline[j - 2].pos;
            curveline[j - 1].nb = curveline[j + 1].pos;
        }
        (curveline.end() - 2)->nb = (curveline.end() - 4)->pos;
        (curveline.end() - 3)->nb = (curveline.end() - 1)->pos;
        curveline.back().nb = D3DXVECTOR2(10000.0f, 10000.0f);
        m_CurveVertexes.insert(m_CurveVertexes.end(), curveline.begin(), curveline.end());
    }
}

void D3DApp::ClearSkeletonLines()
{
    m_SkeletonLinesVertices.clear();
}

int D3DApp::GetWidth()
{
    return m_ClientWidth;
}

int D3DApp::GetHeight()
{
    return m_ClientHeight;
}

ID3D11Device* D3DApp::GetDevice()
{
    return m_pd3dDevice;
}

ID3D11DeviceContext* D3DApp::GetDeviceContext()
{
    return m_DeviceContext;
}

void D3DApp::AddDiffusionLine(const Lines& lines, const Color2Side& colors, int idx)
{
    float scale = 1 / 255.0f;
    int i = idx;
    {
        CURVE_Vertexes curveline;
        const Line& now_line = lines[i];
        const Vector3s& now_lcolor = colors.left[i];
        const Vector3s& now_rcolor = colors.right[i];
        CURVE_Vertex vtx1, vtx2;
        vtx1.lcolor.w = 0;
        vtx1.rcolor.w = 0;
        vtx2.lcolor.w = 0;
        vtx2.rcolor.w = 0;
        for(int j = 0; j < now_line.size() - 1; ++j)
            //for(int j = 2; j < now_line.size() -3; ++j)
        {
            vtx1.lcolor.x = now_lcolor[j].x * scale;
            vtx1.lcolor.y = now_lcolor[j].y * scale;
            vtx1.lcolor.z = now_lcolor[j].z * scale;
            vtx1.rcolor.x = now_rcolor[j].x * scale;
            vtx1.rcolor.y = now_rcolor[j].y * scale;
            vtx1.rcolor.z = now_rcolor[j].z * scale;
            vtx2.lcolor.x = now_lcolor[j + 1].x * scale;
            vtx2.lcolor.y = now_lcolor[j + 1].y * scale;
            vtx2.lcolor.z = now_lcolor[j + 1].z * scale;
            vtx2.rcolor.x = now_rcolor[j + 1].x * scale;
            vtx2.rcolor.y = now_rcolor[j + 1].y * scale;
            vtx2.rcolor.z = now_rcolor[j + 1].z * scale;
            vtx1.pos.x = now_line[j].x;
            vtx1.pos.y = m_PicH - now_line[j].y;
            vtx2.pos.x = now_line[j + 1].x;
            vtx2.pos.y = m_PicH - now_line[j + 1].y;
            curveline.push_back(vtx1);
            curveline.push_back(vtx2);
        }
        curveline.front().nb = D3DXVECTOR2(10000.0f, 10000.0f);
        for(int j = 2; j < curveline.size(); j += 2)
        {
            curveline[j].nb = curveline[j - 2].pos;
            curveline[j - 1].nb = curveline[j + 1].pos;
        }
        (curveline.end() - 2)->nb = (curveline.end() - 4)->pos;
        curveline.back().nb = D3DXVECTOR2(10000.0f, 10000.0f);
        m_CurveVertexes.insert(m_CurveVertexes.end(), curveline.begin(), curveline.end());
    }
}
