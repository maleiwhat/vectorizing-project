
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
	float2 pos: POSITION;
	float2 size: SIZE;
	float3 color: COLOR;
};

struct VS_OUT
{
	float2 pos: POSITION;
	float2 size: SIZE;
	float3 color: COLOR;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 color: COLOR4;
};
 
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.pos = (vIn.pos/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.pos.x += centerX/width*2;
	vOut.pos.y -= centerY/height*2;
	vOut.size = (vIn.size/float2(width,height)*scale);
	vOut.color = vIn.color;
	return vOut;
}


[maxvertexcount (33)]
void gs_main(point VS_OUT input[1], inout LineStream<GS_OUT> triStream)
{
	GS_OUT out3;
	
	out3.pos.z = 0;
	out3.pos.w = 1;
	out3.color = float4(input[0].color, transparency);
	float angle = 12.25*3.14159/180;
	
	float2 len = float2(input[0].size.x, 0);
	for (int i = 0;i <= 32;++i)
	{
		float2x2 mat1 = {cos(angle*(i+1)), -sin(angle*(i+1)), sin(angle*(i+1)), cos(angle*(i+1))};
		out3.pos.xy = mul(len, mat1)/float2(width,height)*width + input[0].pos;
		triStream.Append( out3 );
	}
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
