#pragma once
#include <vector>

struct VSO_Vertex
{
	D3DXVECTOR3 pos; // Position
	D3DXVECTOR2 tex; // Texturecoords
};
typedef std::vector<VSO_Vertex> VSO_Vertexes;

struct CURVE_Vertex
{
	D3DXVECTOR2 pos;        // Position and Blurr
	D3DXVECTOR4 col[2];     // Color: left, right
	D3DXVECTOR2     nb;     // previous vertex and next vertex
};


struct COLORPOINT
{
	D3DXVECTOR3 col;
	int                     off;
};

struct BLURRPOINT
{
	float           blurr;
	int                     off;
};
typedef std::vector<D3DXVECTOR2> D3DXVECTOR2s;
typedef std::vector<COLORPOINT> COLORPOINTs;
typedef std::vector<BLURRPOINT> BLURRPOINTs;

struct CURVE
{
	int             pNum;
	D3DXVECTOR2s p;
	int             clNum;
	COLORPOINTs     cl;
	int             crNum;
	COLORPOINTs  cr;
	int             bNum;
	BLURRPOINTs b;
};
typedef std::vector<CURVE> CURVEs;

class VSObject
{
public:
	CComPtr<ID3DX10Mesh> m_pMeshDiff;
	CComPtr<ID3DX10Mesh> m_pMeshTexture;
	CComPtr<ID3DX10Mesh> m_pMeshCurves;

	ID3D10Texture2D*
	m_diffuseTexture[2];     // two textures used interleavedly for diffusion
	ID3D10Texture2D*
	m_distDirTexture;    // two textures used interleavedly for diffusion (blurr texture)
	ID3D10Texture2D* m_Texture;
	ID3D10Texture2D* m_pDepthStencil;         // for z culling
	ID3D10Texture2D*
	m_otherTexture;                // texture that keeps the color on the other side of a curve

public:
	ID3D10ShaderResourceView* m_diffuseTextureRV[2];
	ID3D10RenderTargetView*   m_diffuseTextureTV[2];
	ID3D10ShaderResourceView* m_distDirTextureRV;
	ID3D10RenderTargetView*   m_distDirTextureTV;
	ID3D10ShaderResourceView* m_TextureRV;
	ID3D10RenderTargetView*   m_TextureTV;
	ID3D10DepthStencilView*   m_pDepthStencilView;
	ID3D10ShaderResourceView* m_otherTextureRV;
	ID3D10RenderTargetView*   m_otherTextureTV;

	ID3D10EffectTechnique* m_pDrawVectorsTechnique;
	ID3D10EffectTechnique* m_pDiffuseTechnique;
	ID3D10EffectTechnique* m_pLineAntiAliasTechnique;
	ID3D10EffectTechnique* m_pDisplayImage;

	ID3D10EffectShaderResourceVariable* m_pInTex[3];
	ID3D10EffectShaderResourceVariable* m_pDiffTex;
	ID3D10EffectScalarVariable* m_pDiffX;
	ID3D10EffectScalarVariable* m_pDiffY;
	ID3D10EffectScalarVariable* g_pScale;
	ID3D10EffectVectorVariable* g_pPan;
	ID3D10EffectScalarVariable* m_pPolySize;

	D3D10_VIEWPORT m_vp;

	float m_sizeX;
	float m_sizeY;
	int diffSteps;
	float m_scale;
	D3DXVECTOR2 m_pan;
	float m_polySize;
	int     m_cNum;
	int m_cSegNum;
	CURVEs m_curve;
	float m_fWidth;
	float m_fHeight;
	int diffTex;
	int diff2Tex;

	static D3D10_INPUT_ELEMENT_DESC InputElements[];
	static D3D10_INPUT_ELEMENT_DESC InputCurveElements[];
	static int InputElementCount;
	static int InputCurveElementCount;
	ID3D10Buffer* m_pVertexBuffer;
	ID3D10InputLayout* m_pVertexLayout;

	VSObject(ID3D10Device* pd3dDevice);
	~VSObject(void);

	ID3D10ShaderResourceView* GetDisplayTextureRV(void) { return m_diffuseTextureRV[diffTex]; }

	BOOL CreateVertexBuffer(ID3D10Device* pd3dDevice);
	BOOL CreateIndexBuffer(ID3D10Device* pd3dDevice);
	void Render(ID3D10Device* pd3dDevice);
	void RenderDiffusion(ID3D10Device* pd3dDevice);
	void SetupTextures(ID3D10Device* pd3dDevice, ID3D10Effect* g_pEffect10,
	                   int sizeX, int sizeY);
	void ReadVectorFile(char* s);
	void ConstructCurves(ID3D10Device* pd3dDevice);
};
