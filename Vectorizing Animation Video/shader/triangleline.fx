
cbuffer cbPerFrame
{
	float width;
	float height;
	float centerX;
	float centerY;
	float scale;
	float transparency;
};

struct VS_IN
{
	float2 p1: POSA;
	float2 p2: POSB;
	float2 p3: POSC;
	float3 c1: COLORA;
	float3 c2: COLORB;
	float3 c3: COLORC;
};

struct VS_OUT
{
	float2 p1: POSA;
	float2 p2: POSB;
	float2 p3: POSC;
	float3 c1: COLORA;
	float3 c2: COLORB;
	float3 c3: COLORC;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 color: COLOR4;
};
 
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.p1 = (vIn.p1/float2(width,height)*scale)*2-1;
	vOut.p2 = (vIn.p2/float2(width,height)*scale)*2-1;
	vOut.p3 = (vIn.p3/float2(width,height)*scale)*2-1;
	vOut.p1.x += centerX/width*2;
	vOut.p1.y -= centerY/height*2;
	vOut.p2.x += centerX/width*2;
	vOut.p2.y -= centerY/height*2;
	vOut.p3.x += centerX/width*2;
	vOut.p3.y -= centerY/height*2;
	vOut.c1 = vIn.c1;
	vOut.c2 = vIn.c2;
	vOut.c3 = vIn.c3;
	return vOut;
}


[maxvertexcount (4)]
void gs_main(point VS_OUT input[1], inout LineStream<GS_OUT> triStream)
{
	GS_OUT out3;
	
	out3.pos.z = 0;
	out3.pos.w = 1;
	out3.color = float4(input[0].c1, 1);
	out3.pos.xy = input[0].p1;
	triStream.Append( out3 );
	out3.color = float4(input[0].c2, 1);
	out3.pos.xy = input[0].p2;
	triStream.Append( out3 );
	out3.color = float4(input[0].c3, 1);
	out3.pos.xy = input[0].p3;
	triStream.Append( out3 );
	out3.color = float4(input[0].c1, 1);
	out3.pos.xy = input[0].p1;
	triStream.Append( out3 );

	triStream.RestartStrip( );
}

float4 PS(GS_OUT pIn) : SV_Target
{
//discard;
	return float4(pIn.color.x, pIn.color.y, pIn.color.z, transparency);
}

RasterizerState NoCull
{
	CullMode = None;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	DepthFunc = LESS_EQUAL;
};

technique11 PointTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, gs_main() ) );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
		SetRasterizerState(NoCull);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}
