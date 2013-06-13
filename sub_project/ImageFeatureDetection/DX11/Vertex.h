#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#include <d3d11.h>
#include <windows.h>
#include <d3dx11.h>
#include <vector>

//Picture Vertex format
struct PictureVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 size;
};
typedef std::vector<PictureVertex> PictureVertices;
//Point Vertex format
struct PointVertex
{
	D3DXVECTOR2 position;
	D3DXVECTOR2 size;
	D3DXVECTOR3 color;
};
typedef std::vector<PointVertex> PointVertices;

extern D3D11_INPUT_ELEMENT_DESC VertexDesc_PICVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_PointVertex[];

