#include "stdafx.h"
#include "RegionDiffusion.h"
#include "DX11\d3dApp.h"

void RegionDiffusion::ReadD3DSetting()
{
#define READV(x) x = m_D3DApp->x
    READV(m_PicH);
    READV(m_PicW);
    READV(m_pd3dDevice);
    READV(m_SwapChain);
    READV(m_DepthStencilBuffer);
    READV(m_DrawTexture);
    READV(m_RenderTargetView);
    READV(m_distDirTextureTV);
    READV(m_distDirTextureRV);
    READV(m_DepthStencilView);
    READV(m_DrawTextureDepthStencilView);
    READV(m_ClientWidth);
    READV(m_ClientHeight);
    READV(m_pDepthStencil_ZWriteON);
    READV(m_Scale);
    READV(m_LookCenterX);
    READV(m_LookCenterY);
    READV(m_BackBuffer);
    READV(m_DeviceContext);
    READV(m_DrawTextureDepthStencilBuffer);
    READV(m_Transparency_LineSkeleton);
	READV(m_Transparency_SelectPatch);
	READV(m_pBlendState_BLEND);
	if (m_pAlpha)
	{
		m_pAlpha->SetFloat(m_Transparency_SelectPatch);
	}
	if (m_SkeletonLines_Alpha)
	{
		m_SkeletonLines_Alpha->SetFloat(m_Transparency_LineSkeleton);
	}
}

RegionDiffusion::RegionDiffusion(D3DApp* app)
{
    m_Init = false;
    m_D3DApp = app;
    m_SkeletonLines_Buffer = NULL;
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
    m_pDisplayImage2 = NULL;
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

RegionDiffusion::~RegionDiffusion(void)
{
}

void RegionDiffusion::AddLines(const Lines& lines)
{
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

void RegionDiffusion::BuildShaderFX()
{
    ID3DBlob* pCode;
    ID3DBlob* pError;
    //Picture
    HRESULT hr = 0;
    D3DX11_PASS_DESC PassDesc;
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
    m_pDisplayImage2 = pEffect11->GetTechniqueByName("DisplayDiffusionImage2");
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
    m_pPolySize = pEffect11->GetVariableByName("g_polySize")->AsScalar();
    m_pPan = pEffect11->GetVariableByName("g_pan")->AsVector();
	m_pAlpha = pEffect11->GetVariableByName("g_alpha")->AsScalar();
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

void RegionDiffusion::BuildPoint()
{
    if(!m_Init)
    {
        return;
    }
    ReleaseCOM(m_SkeletonLines_Buffer);
    ReleaseCOM(m_pCurveVertexBuffer);
    ReleaseCOM(m_pCurveVertex2Buffer);
    m_vbd.Usage = D3D11_USAGE_IMMUTABLE;
    m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_vbd.CPUAccessFlags = 0;
    m_vbd.MiscFlags = 0;
    if(!m_SkeletonLinesVertices.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(SkeletonLineVertex) *
                                 m_SkeletonLinesVertices.size());
        m_vbd.StructureByteStride = sizeof(SkeletonLineVertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_SkeletonLinesVertices[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_SkeletonLines_Buffer));
    }
    // diffusion curve part
    if(!m_CurveVertexes.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(CURVE_Vertex) * m_CurveVertexes.size());
        m_vbd.StructureByteStride = sizeof(CURVE_Vertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_CurveVertexes[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_pCurveVertexBuffer));
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
    if(!m_RegionTrangles.empty())
    {
        m_vbd.ByteWidth = (UINT)(sizeof(VSO_Vertex) * m_RegionTrangles.size());
        m_vbd.StructureByteStride = sizeof(VSO_Vertex);
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &m_RegionTrangles[0];
        HR(m_pd3dDevice->CreateBuffer(&m_vbd, &vinitData, &m_pCurveVertex3Buffer));
    }
}

void RegionDiffusion::DrawScene()
{
    if(!m_RenderTargetView || !m_DepthStencilView)
    {
        return;
    }
    InterSetSize(m_ClientWidth, m_ClientHeight);
    InterSetLookCenter(m_LookCenterX, m_LookCenterY);
    InterSetScale(m_Scale);
    InterDraw();
}

void RegionDiffusion::InterSetSize(float w, float h)
{
    if(!m_Init)
    {
        return;
    }
    m_SkeletonLines_Width->SetFloat(w);
    m_SkeletonLines_Height->SetFloat(h);
    m_pDiffX->SetFloat(w);
    m_pDiffY->SetFloat(h);
}

void RegionDiffusion::InterSetScale(float s)
{
    if(!m_Init)
    {
        return;
    }
    m_SkeletonLines_Scale->SetFloat(s);
    m_pScale->SetFloat(s);
}
void RegionDiffusion::InterSetLookCenter(float x, float y)
{
    if(!m_Init)
    {
        return;
    }
    m_SkeletonLines_CenterX->SetFloat(x);
    m_SkeletonLines_CenterY->SetFloat(y);
    m_pan.x = x;
    m_pan.y = y;
    m_pPan->SetFloatVector(m_pan);
}

void RegionDiffusion::OnResizeRelease()
{
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
    ReleaseCOM(m_pDepthStencilView);
}

void RegionDiffusion::OnResize(int w, int h)
{
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
}


void RegionDiffusion::InterDraw()
{
    if(!m_Init)
    {
        return;
    }
	float BlendFactor[4] = {0, 0, 0, 0};
	m_DeviceContext->OMSetBlendState(m_pBlendState_BLEND, BlendFactor, 0xffffffff);
    m_DeviceContext->OMSetDepthStencilState(m_pDepthStencil_ZWriteON, 0);
    //Draw Diffusion
    if(m_CurveVertexes.size() > 0)
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
        ID3D11RenderTargetView* old_pRTV = m_RenderTargetView;
        ID3D11DepthStencilView* old_pDSV = m_DrawTextureDepthStencilView;
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
        {
            m_DeviceContext->OMSetRenderTargets(1,  &old_pRTV, old_pDSV);
            offset = 0;
            m_DeviceContext->IASetVertexBuffers(0, 1, &m_pCurveVertex3Buffer, &stride, &offset);
            m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            HR(m_pDiffTex->SetResource(m_TextureRV));
            for(UINT p = 0; p < techDesc.Passes; ++p)
            {
                for(int np = 0; np < m_RegionOffset.size() - 1; np++)
                {
                    m_pDisplayImage2->GetPassByIndex(p)->Apply(0, m_DeviceContext);
                    if(m_RegionOffset[np + 1] - m_RegionOffset[np] > 0)
                    {
                        m_DeviceContext->Draw((m_RegionOffset[np + 1] - m_RegionOffset[np]) * 3, m_RegionOffset[np] * 3);
                    }
                }
            }
        }
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
        m_DeviceContext->Draw((UINT)m_SkeletonLinesVertices.size(), 0);
    }
}


void RegionDiffusion::AddDiffusionLines(const Lines& lines, const Vector3s2d& colors)
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

void RegionDiffusion::AddDiffusionLines(const Lines& lines, const Color2Side& colors)
{
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

void RegionDiffusion::AddColorTriangles(int r, const Triangles& ts)
{
    if(r < 0 || r >= m_Region.size())
    {
        return;
    }
    m_Region[r].insert(m_Region[r].end(), ts.begin(), ts.end());
}

void RegionDiffusion::SetDiffusionRegions(int n)
{
    m_Region.clear();
    m_Region.resize(n);
    m_RegionCurveVertexes.clear();
    m_RegionCurveVertexes.resize(n);
}

void RegionDiffusion::AddDiffusionLinesRegion(int r, const Lines& lines, const Color2Side& colors)
{
    if(r < 0 || r >= m_Region.size())
    {
        return;
    }
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
        m_RegionCurveVertexes[r].insert(m_RegionCurveVertexes[r].end(), curveline.begin(), curveline.end());
    }
}

void RegionDiffusion::SetRegions(Triangles2d& regions)
{
    int c = 0;
    m_RegionOffset.clear();
    for(int a = 0; a < regions.size(); ++a)
    {
        m_RegionOffset.push_back(c);
        c += regions[a].size();
    }
    m_RegionOffset.push_back(c);
    m_RegionTrangles.resize(c * 3);
    c = 0;
    for(int a = 0; a < regions.size(); ++a)
    {
        Triangles& now_tris = regions[a];
        for(int b = 0; b < now_tris.size(); ++b)
        {
            Triangle& now_tri = now_tris[b];
            m_RegionTrangles[c * 3 + 0].pos.x = now_tri.pts[0].x;
            m_RegionTrangles[c * 3 + 0].pos.y = m_PicH - now_tri.pts[0].y;
            m_RegionTrangles[c * 3 + 0].pos.z = 0.5f;
            m_RegionTrangles[c * 3 + 1].pos.x = now_tri.pts[1].x;
            m_RegionTrangles[c * 3 + 1].pos.y = m_PicH - now_tri.pts[1].y;
            m_RegionTrangles[c * 3 + 1].pos.z = 0.5f;
            m_RegionTrangles[c * 3 + 2].pos.x = now_tri.pts[2].x;
            m_RegionTrangles[c * 3 + 2].pos.y = m_PicH - now_tri.pts[2].y;
            m_RegionTrangles[c * 3 + 2].pos.z = 0.5f;
            // texture uv
            m_RegionTrangles[c * 3 + 0].tex.x = now_tri.pts[0].x;
            m_RegionTrangles[c * 3 + 0].tex.y = m_PicH - now_tri.pts[0].y;
            m_RegionTrangles[c * 3 + 1].tex.x = now_tri.pts[1].x;
            m_RegionTrangles[c * 3 + 1].tex.y = m_PicH - now_tri.pts[1].y;
            m_RegionTrangles[c * 3 + 2].tex.x = now_tri.pts[2].x;
            m_RegionTrangles[c * 3 + 2].tex.y = m_PicH - now_tri.pts[2].y;
            ++c;
        }
    }
}
