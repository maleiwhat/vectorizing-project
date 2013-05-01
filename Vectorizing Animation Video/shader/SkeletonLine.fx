
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
	float3 c1: COLOR;
};

struct VS_OUT
{
	float2 p1: POSA;
	float2 p2: POSB;
	float3 c1: COLOR;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR4;
};
 
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	float2 offset = float2(centerX/width*2, -centerY/height*2);
	vOut.p1 = (vIn.p1/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p2 = (vIn.p2/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p1 += offset;
	vOut.p2 += offset;
	vOut.c1 = vIn.c1;
	return vOut;
}


[maxvertexcount (2)]
void gs_main(point VS_OUT input[1], inout LineStream<GS_OUT> triStream)
{
	GS_OUT out3;
	
	out3.pos.z = 0;
	out3.pos.w = 1;
	out3.color = float4(input[0].c1, 1);
	out3.pos.xy = input[0].p1;
	triStream.Append( out3 );
	out3.pos.xy = input[0].p2;
	triStream.Append( out3 );
	triStream.RestartStrip();
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
