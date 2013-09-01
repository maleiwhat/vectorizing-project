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

struct LineVertex2w
{
	D3DXVECTOR2 p1;
	D3DXVECTOR2 p2;
	D3DXVECTOR2 p3;
	D3DXVECTOR2 p4;
	D3DXVECTOR2 widthL;
	D3DXVECTOR2 widthR;
	D3DXVECTOR3 color;
};
typedef std::vector<LineVertex2w> Line2wVertices;

struct SkeletonLineVertex
{
	D3DXVECTOR2 p1;
	D3DXVECTOR2 p2;
	D3DXVECTOR3 color;
};
typedef std::vector<SkeletonLineVertex> SkeletonLineVertexes;

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


extern D3D11_INPUT_ELEMENT_DESC VertexDesc_PointVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_PICVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_TRIVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_LineVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_LineVertex2w[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_SkeletonLineVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_VSOVertex[];
extern D3D11_INPUT_ELEMENT_DESC VertexDesc_CurveVertex[];

#endif // VERTEX_H
