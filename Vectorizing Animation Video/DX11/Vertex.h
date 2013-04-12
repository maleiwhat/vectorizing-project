#ifndef VERTEX_H
#define VERTEX_H
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

struct TriangleVertex
{
	D3DXVECTOR2 p1;
	D3DXVECTOR2 p2;
	D3DXVECTOR2 p3;
	D3DXVECTOR3 c1;
	D3DXVECTOR3 c2;
	D3DXVECTOR3 c3;
};
typedef std::vector<TriangleVertex> TriangleVertices;

//Point Vertex format
struct PointVertex
{
	D3DXVECTOR2 position;
	D3DXVECTOR2 size;
	D3DXVECTOR3 color;
};
typedef std::vector<PointVertex> PointVertices;

//Line Vertex format
struct LineVertex
{
	D3DXVECTOR2 p1;
	D3DXVECTOR2 p2;
	D3DXVECTOR2 p3;
	D3DXVECTOR2 p4;
	D3DXVECTOR2 width;
	D3DXVECTOR3 color;
};
typedef std::vector<LineVertex> LineVertices;

struct SkeletonLineVertex
{
	D3DXVECTOR2 p1;
	D3DXVECTOR2 p2;
	D3DXVECTOR3 color;
};
typedef std::vector<SkeletonLineVertex> SkeletonLineVertexes;

extern D3D11_INPUT_ELEMENT_DESC VertexDesc_PointVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_PICVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_TRIVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_LineVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_SkeletonLineVertex[];

#endif // VERTEX_H
