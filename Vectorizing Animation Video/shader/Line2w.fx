
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
	float2 p4: POSD;
	float2 widthL: WA;
	float2 widthR: WB;
	float3 c1: COLOR;
};

struct VS_OUT
{
	float2 p1: POSA;
	float2 p2: POSB;
	float2 p3: POSC;
	float2 p4: POSD;
	float2 p22: POSE;
	float2 p33: POSF;
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
	float2 p1 = vIn.p1;
	float2 p2 = vIn.p2;
	float2 p3 = vIn.p3;
	float2 p4 = vIn.p4;

	float2 p13 = normalize(p3 - p1);
	float2 p24 = normalize(p4 - p2);
	
	float angle = 90*3.14159/180;
	float2x2 mat = {cos(angle), -sin(angle), sin(angle), cos(angle)};
	p13 = mul(p13, mat);
	p24 = mul(p24, mat);
	vOut.p1 = p2 + p13 * vIn.widthL.x;
	vOut.p2 = p2 - p13 * vIn.widthR.x;
	vOut.p3 = p3 + p24 * vIn.widthL.y;
	vOut.p4 = p3 - p24 * vIn.widthR.y;
	vOut.p22 = p2;
	vOut.p33 = p3;
	float2 offset = float2(centerX/width*2, -centerY/height*2);
	vOut.p1 = (vOut.p1/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p2 = (vOut.p2/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p3 = (vOut.p3/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p4 = (vOut.p4/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p22 = (vOut.p22/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p33 = (vOut.p33/float2(width,height)*scale)*2-1+float3(centerX/width,-centerY/height, 0)*scale;
	vOut.p1 += offset;
	vOut.p2 += offset;
	vOut.p3 += offset;
	vOut.p4 += offset;
	vOut.p22 += offset;
	vOut.p33 += offset;
	vOut.c1 = vIn.c1;
	return vOut;
}


[maxvertexcount (6)]
void gs_main(point VS_OUT input[1], inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT out3;
	
	out3.pos.z = 0;
	out3.pos.w = 1;
	out3.color = float4(input[0].c1, 1);
	out3.pos.xy = input[0].p1;
	triStream.Append( out3 );
	out3.pos.xy = input[0].p3;
	triStream.Append( out3 );
	out3.pos.xy = input[0].p22;
	triStream.Append( out3 );
	out3.pos.xy = input[0].p33;
	triStream.Append( out3 );
	out3.pos.xy = input[0].p2;
	triStream.Append( out3 );
	out3.pos.xy = input[0].p4;
	triStream.Append( out3 );
	triStream.RestartStrip();
}

float4 PS(GS_OUT pIn) : SV_Target
{
//discard;
	return float4(0,0,0,transparency);
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
