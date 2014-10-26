cbuffer cbChangesEveryFrame
{
    float g_scale;              // scale factor
    float2 g_pan;               // panning
    float g_diffX;              // x size of diffusion texture
    float g_diffY;              // y size of diffusion texture
    float g_polySize;           // size of the curve polygons (2 is screen width/height)
    float g_blurOn;
	float g_alpha;
};

Texture2D g_inTex[3];   // input texture (containing lines with north east south west colors)
Texture2D g_diffTex;    // diffusion texture


SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};


SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
float4 VS(float4 Pos : POSITION) : SV_POSITION
{
    return Pos;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);      // Yellow, with Alpha = 1
}



struct VS_INPUT
{
float3 Pos      :
    POSITION;
float2 Tex      :
    TEXTURE0;
};

struct PS_INPUT
{
float4 oPosition :
    SV_POSITION;
float4 worldPos  :
    TEXTURE0;
float2 Tex       :
    TEXTURE1;
};

struct PS_OUTPUT
{
float4 oColor   :
    SV_Target0;
};


struct PS_OUTPUT_MRT_3
{
float4 oColor1  :
    SV_Target0;
float4 oColor2  :
    SV_Target1;
float4 oColor3  :
    SV_Target2;
};


struct VS_CURVE_INPUT
{
float2 pos      :
    POSITION;
float4 col0     :
    TEXTURE0;
float4 col1     :
    TEXTURE1;
float2 nb       :
    TEXTURE2;
};


struct PS_CURVE_INPUT
{
float4 oPosition :
    SV_POSITION;
float4 col       :
    TEXTURE0;    //contains blurr in w component
float4 distdir   :
    TEXTURE1;    //distance(x) and direction(yz) to the closest border, will be used for diffusion and anti-aliasing
float4 oCol      :
    TEXTURE2;    //color on the other side of of the curve (used for antialiasing)
};



// ================================================================================
//
// Vertex Shader
//
// ================================================================================

PS_INPUT TetraVS(VS_INPUT In)
{
    PS_INPUT Out;
    Out.oPosition = float4(In.Pos.x, -In.Pos.y, +0.5, 1.0);
    Out.Tex = In.Tex;
    return Out;
}

PS_INPUT TetraVS2(VS_INPUT In)
{
    PS_INPUT Out;
    float2 offset = float2(g_pan.x / g_diffX, -g_pan.y / g_diffY) * 2;
    Out.oPosition.xy = (In.Pos.xy / float2(g_diffX, g_diffY) * g_scale) * 2 - 1 +
                       float2(g_pan.x / g_diffX, -g_pan.y / g_diffY) * g_scale;
    Out.oPosition.xy += offset;
    Out.oPosition.z = 0.5;
    Out.oPosition.w = 1.0;
    offset = float2(g_pan.x / g_diffX*(g_scale*0.5), g_pan.y / g_diffY*(2+g_scale*0.5));
    Out.Tex = (In.Tex.xy / float2(g_diffX, -g_diffY) * g_scale) + float2(0, 1) +
              float2(g_pan.x / g_diffX, -g_pan.y / g_diffY);
    Out.Tex += offset;
    return Out;
}

VS_CURVE_INPUT CurveVS(VS_CURVE_INPUT In)
{
    VS_CURVE_INPUT Out;
    float2 offset = float2(g_pan.x / g_diffX * 2, -g_pan.y / g_diffY * 2);
    Out.pos.xy = (In.pos.xy / float2(g_diffX, g_diffY) * g_scale) * 2 - 1 +
                 float2(g_pan.x / g_diffX, -g_pan.y / g_diffY) * g_scale;
    Out.pos.xy += offset;
    //Out.pos.xy = In.pos.xy*g_scale+g_pan;
    Out.col0 = In.col0;
    Out.col1 = In.col1;
    if(In.nb.x < 9999.0)
    {
        Out.nb = (In.nb / float2(g_diffX, g_diffY) * g_scale) * 2 - 1 +
                 float2(g_pan.x / g_diffX, -g_pan.y / g_diffY) * g_scale;
        Out.nb += offset;
    }
    else
    {
        Out.nb = float2(10000.0, 10000.0);
    }
    return Out;
}


// ================================================================================
//
// Geometry Shader
//
// ================================================================================



const float zMax = 1.0;

[maxvertexcount(4)]
void CurveGS1(line VS_CURVE_INPUT input[2], inout TriangleStream<PS_CURVE_INPUT> tStream)
{
    PS_CURVE_INPUT p0;
    float2 nL = input[1].pos.xy - input[0].pos.xy;
    nL = normalize(float2(-nL.y, nL.x));
    // left side
    p0.oPosition = float4(input[0].pos.xy + g_polySize * nL, zMax, 1.0);
    p0.col = input[0].col0;
    p0.oCol = input[0].col1;
    p0.distdir = float4(0.5 * g_polySize, -nL, 0);
    tStream.Append(p0);
    p0.oPosition = float4(input[1].pos.xy + g_polySize * nL, zMax, 1.0);
    p0.col = input[1].col0;
    p0.oCol = input[1].col1;
    p0.distdir = float4(0.5 * g_polySize, -nL, 0);
    tStream.Append(p0);
    p0.oPosition = float4(input[0].pos.xy, 0.0, 1.0);
    p0.col = input[0].col0;
    p0.oCol = input[0].col1;
    p0.distdir = float4(0, -nL, 0);
    tStream.Append(p0);
    p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
    p0.col = input[1].col0;
    p0.oCol = input[1].col1;
    p0.distdir = float4(0, -nL, 0);
    tStream.Append(p0);
    tStream.RestartStrip();
}



[maxvertexcount(4)]
void CurveGS2(line VS_CURVE_INPUT input[2], inout TriangleStream<PS_CURVE_INPUT> tStream)
{
    PS_CURVE_INPUT p0;
    float2 nL = input[1].pos.xy - input[0].pos.xy;
    nL = normalize(float2(-nL.y, nL.x));
    //right side
    p0.oPosition = float4(input[0].pos.xy, 0.0, 1.0);
    p0.col = input[0].col1;
    p0.oCol = input[0].col0;
    p0.distdir = float4(0, nL, 0);
    tStream.Append(p0);
    p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
    p0.col = input[1].col1;
    p0.oCol = input[1].col0;
    p0.distdir = float4(0, nL, 0);
    tStream.Append(p0);
    p0.oPosition = float4(input[0].pos.xy - g_polySize * nL, zMax, 1.0);
    p0.col = input[0].col1;
    p0.oCol = input[0].col0;
    p0.distdir = float4(0.5 * g_polySize, nL, 0);
    tStream.Append(p0);
    p0.oPosition = float4(input[1].pos.xy - g_polySize * nL, zMax, 1.0);
    p0.col = input[1].col1;
    p0.oCol = input[1].col0;
    p0.distdir = float4(0.5 * g_polySize, nL, 0);
    tStream.Append(p0);
    tStream.RestartStrip();
}



[maxvertexcount(8)]
void CurveGS3(line VS_CURVE_INPUT input[2], inout TriangleStream<PS_CURVE_INPUT> tStream)
{
    // close the gaps to the following line
    if(input[1].nb.x < 9999.0)
    {
        PS_CURVE_INPUT p0;
        float2 nL = input[1].pos.xy - input[0].pos.xy;
        nL = normalize(float2(-nL.y, nL.x));
        float2 nLN = input[1].nb.xy - input[1].pos.xy;
        nLN = normalize(float2(-nLN.y, nLN.x));
        //larger than 90 degree angle -> form 4 polygons
        if(dot(input[0].pos.xy - input[1].pos.xy, input[1].nb.xy - input[1].pos.xy) >= 0)
        {
            float2 lP = -normalize(normalize(input[0].pos.xy - input[1].pos.xy) + normalize(input[1].nb.xy - input[1].pos.xy));
            float4 col = input[1].col0;
            float4 oCol = input[1].col1;
            float2 lH1, lH2;
            if(dot(nL, input[1].nb.xy - input[1].pos.xy) > 0)
            {
                col = input[1].col1;
                oCol = input[1].col0;
                nL = -nL;
                nLN = -nLN;
                lH1 = normalize(nL + lP);
                lH2 = normalize(nLN + lP);
                //return;
            }
            else
            {
                lH1 = normalize(nL + lP);
                lH2 = normalize(nLN + lP);
            }
            p0.col = col;
            p0.oCol = oCol;
            p0.oPosition = float4(input[1].pos + g_polySize * nL, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * nL, 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
            p0.distdir = float4(0, float2(0, 0), 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos + g_polySize * lH1, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * lH1, 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy + g_polySize * lP, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * lP, 1);
            tStream.Append(p0);
            tStream.RestartStrip();
            p0.oPosition = float4(input[1].pos.xy + g_polySize * lP, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * lP, 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
            p0.distdir = float4(0, float2(0, 0), 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos + g_polySize * lH2, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * lH2, 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy + g_polySize * nLN, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * nLN, 1);
            tStream.Append(p0);
            tStream.RestartStrip();
        }
        else if(dot(nL, nLN) <= 0.7071) //between 45 and 90 degree angle -> form 2 polygons
        {
            float4 col = input[1].col0;
            float4 oCol = input[1].col1;
            if(dot(input[1].nb.xy - input[1].pos.xy, nL) >= 0)
            {
                //return;
                col = input[1].col1;
                oCol = input[1].col0;
                nL = - nL;
                nLN = -nLN;
            }
            float2 lH = normalize(nL + nLN);
            p0.col = col;
            p0.oCol = oCol;
            p0.oPosition = float4(input[1].pos.xy + g_polySize * nL, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * nL, 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
            p0.distdir = float4(0, float2(0, 0), 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy + g_polySize * lH, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * lH, 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy + g_polySize * nLN, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * nLN, 1);
            tStream.Append(p0);
            tStream.RestartStrip();
        }
        else //smaller than 45 degrees angle -> form 1 polygon
        {
            float4 col = input[1].col0;
            float4 oCol = input[1].col1;
            if(dot(input[1].nb.xy - input[1].pos.xy, nL) >= 0)
            {
                //return;
                col = input[1].col1;
                oCol = input[1].col0;
                nL = - nL;
                nLN = -nLN;
            }
            p0.col = col;
            p0.oCol = oCol;
            p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
            p0.distdir = float4(0, float2(0, 0), 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy + g_polySize * nL, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * nL, 1);
            tStream.Append(p0);
            p0.oPosition = float4(input[1].pos.xy + g_polySize * nLN, zMax, 1.0);
            p0.distdir = float4(0, -g_polySize * nLN, 1);
            tStream.Append(p0);
            tStream.RestartStrip();
        }
    }
}



[maxvertexcount(4)]
void CurveGS4(line VS_CURVE_INPUT input[2], inout TriangleStream<PS_CURVE_INPUT> tStream)
{
    if(input[0].nb.x > 9999.0)
    {
        // fill the beginning of the line if we are at a starting point
        PS_CURVE_INPUT p0;
        float2 nL = input[1].pos.xy - input[0].pos.xy;
        nL = normalize(float2(-nL.y, nL.x));
        float2 lP = -normalize(input[1].pos.xy - input[0].pos.xy);
        float2 lH = normalize(nL + lP);
        p0.col = input[0].col0;
        p0.oCol = input[0].col0;
        p0.oPosition = float4(input[0].pos.xy + g_polySize * nL, zMax, 1.0);
        p0.distdir = float4(0, -g_polySize * nL, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[0].pos.xy, 0.0, 1.0);
        p0.distdir = float4(0, float2(0, 0), 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[0].pos.xy + g_polySize * lH, zMax + 0.1, 1.0);
        p0.distdir = float4(0, -g_polySize * lH, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[0].pos.xy + g_polySize * lP, zMax + 0.2, 1.0);
        p0.distdir = float4(0, -g_polySize * lP, 1);
        tStream.Append(p0);
        tStream.RestartStrip();
    }
    else if(input[1].nb.x > 9999.0)
    {
        // fill the end of the line if we are at an end point
        PS_CURVE_INPUT p0;
        float2 nL = input[1].pos.xy - input[0].pos.xy;
        nL = normalize(float2(-nL.y, nL.x));
        float2 lP = normalize(input[1].pos.xy - input[0].pos.xy);
        float2 lH = normalize(nL + lP);
        p0.col = input[1].col0;
        p0.oCol = input[1].col0;
        p0.oPosition = float4(input[1].pos.xy + g_polySize * nL, zMax, 1.0);
        p0.distdir = float4(0, -g_polySize * nL, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
        p0.distdir = float4(0, float2(0, 0), 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[1].pos.xy + g_polySize * lH, zMax + 0.1, 1.0);
        p0.distdir = float4(0, -g_polySize * lH, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[1].pos.xy + g_polySize * lP, zMax + 0.2, 1.0);
        p0.distdir = float4(0, -g_polySize * lP, 1);
        tStream.Append(p0);
        tStream.RestartStrip();
    }
}



[maxvertexcount(4)]
void CurveGS5(line VS_CURVE_INPUT input[2], inout TriangleStream<PS_CURVE_INPUT> tStream)
{
    if(input[0].nb.x > 9999.0)
    {
        // fill the beginning of the line if we are at a starting point
        PS_CURVE_INPUT p0;
        float2 nL = input[1].pos.xy - input[0].pos.xy;
        nL = normalize(float2(-nL.y, nL.x));
        float2 lP = -normalize(input[1].pos.xy - input[0].pos.xy);
        float2 lH = normalize(-nL + lP);
        p0.col = input[0].col1;
        p0.oCol = input[0].col1;
        p0.oPosition = float4(input[0].pos.xy + g_polySize * lP, zMax + 0.2, 1.0);
        p0.distdir = float4(0, -g_polySize * lP, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[0].pos.xy, 0.0, 1.0);
        p0.distdir = float4(0, float2(0, 0), 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[0].pos.xy + g_polySize * lH, zMax + 0.1, 1.0);
        p0.distdir = float4(0, -g_polySize * lH, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[0].pos.xy - g_polySize * nL, zMax, 1.0);
        p0.distdir = float4(0, g_polySize * nL, 1);
        tStream.Append(p0);
        tStream.RestartStrip();
    }
    else if(input[1].nb.x > 9999.0)
    {
        // fill the end of the line if we are at an end point
        PS_CURVE_INPUT p0;
        float2 nL = input[1].pos.xy - input[0].pos.xy;
        nL = normalize(float2(-nL.y, nL.x));
        float2 lP = normalize(input[1].pos.xy - input[0].pos.xy);
        float2 lH = normalize(-nL + lP);
        p0.col = input[1].col1;
        p0.oCol = input[1].col1;
        p0.oPosition = float4(input[1].pos.xy + g_polySize * lP, zMax + 0.2, 1.0);
        p0.distdir = float4(0, -g_polySize * lP, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[1].pos.xy, 0.0, 1.0);
        p0.distdir = float4(0, float2(0, 0), 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[1].pos.xy + g_polySize * lH, zMax + 0.1, 1.0);
        p0.distdir = float4(0, -g_polySize * lH, 1);
        tStream.Append(p0);
        p0.oPosition = float4(input[1].pos.xy - g_polySize * nL, zMax, 1.0);
        p0.distdir = float4(0, g_polySize * nL, 1);
        tStream.Append(p0);
        tStream.RestartStrip();
    }
}



// ================================================================================
//
// The Pixel Shader.
//
// ================================================================================


PS_OUTPUT TetraUpPS(PS_INPUT In)
{
    PS_OUTPUT Out;
    Out.oColor = g_diffTex.Sample(LinearSampler, In.Tex.xy).xyzw;
	Out.oColor.w = g_alpha;
    return Out;
}



PS_OUTPUT_MRT_3 CurvePS(PS_CURVE_INPUT In)
{
    PS_OUTPUT_MRT_3 Out;
    Out.oColor1 = In.col;
    if(In.distdir.w < 0.5)
    {
        Out.oColor2 = float4(In.distdir.x, float2(In.distdir.y, -In.distdir.z), In.col.w);
    }
    else
    {
        Out.oColor2 = float4(0.5 * length(In.distdir.yz), normalize(float2(In.distdir.y, -In.distdir.z)), In.col.w);
    }
    Out.oColor3 = In.oCol;
    return Out;
}

PS_OUTPUT DiffusePS(PS_INPUT In)
{
    PS_OUTPUT Out;
    float4 ncolor = g_inTex[1].SampleLevel(PointSampler, In.Tex.xy, 0);
    float rawKernel = 0.92387 * ncolor.x;
    float kernel = rawKernel * g_diffX;
    if(g_blurOn)
    {
        kernel += ncolor.w / g_diffX * 3000;
    }
    kernel *= g_polySize;
    kernel -= 0.5;
    kernel = max(0, kernel);
    Out.oColor = g_inTex[0].SampleLevel(PointSampler, In.Tex.xy + float2(-kernel / g_diffX, 0), 0);
    Out.oColor += g_inTex[0].SampleLevel(PointSampler, In.Tex.xy + float2(kernel / g_diffX, 0), 0);
    kernel = rawKernel * g_diffY;
    if(g_blurOn)
    {
        kernel += ncolor.w / g_diffY * 3000;
    }
    kernel *= g_polySize;
    kernel -= 0.5;
    kernel = max(0, kernel);
    Out.oColor += g_inTex[0].SampleLevel(PointSampler, In.Tex.xy + float2(0, -kernel / g_diffY), 0);
    Out.oColor += g_inTex[0].SampleLevel(PointSampler, In.Tex.xy + float2(0, kernel / g_diffY), 0);
//  if (g_blurOn)
//      Out.oColor += g_inTex[0].SampleLevel(PointSampler, In.Tex.xy, 0)*2;
//  if (g_blurOn)
//      Out.oColor /= 6;
//  else
    if(Out.oColor.x < 0)
    {
        Out.oColor.x = 0;
    }
    if(Out.oColor.y < 0)
    {
        Out.oColor.y = 0;
    }
    if(Out.oColor.z < 0)
    {
        Out.oColor.z = 0;
    }
    Out.oColor /= 4;
    return Out;
}


PS_OUTPUT LineAntiAliasPS(PS_INPUT In)
{
    PS_OUTPUT Out;
    float4 ncolor = g_inTex[1].SampleLevel(PointSampler, In.Tex.xy, 0);
    float3 dd = g_diffTex.SampleLevel(PointSampler, In.Tex.xy, 0).xyz;
    dd.yz = normalize(dd.yz);
    dd.z = -dd.z;
    float4 thisColor = g_inTex[0].SampleLevel(PointSampler, In.Tex.xy, 0).xyzw;
    if(dd.x * g_diffX > 0.7)
    {
        Out.oColor = thisColor;
    }
    else if(ncolor.w < 0.0001)
    {
        float4 otherColor = g_inTex[1].SampleLevel(PointSampler, In.Tex.xy, 0).xyzw;
        Out.oColor = (1.0 - (dd.x * g_diffX / 0.7)) * 0.5 * (otherColor + thisColor) + (dd.x * g_diffX / 0.7) * thisColor;
    }
    return Out;
}



//--------------------------------------------------------------------------------------
// Techniques/states
//--------------------------------------------------------------------------------------

RasterizerState state
{
    FillMode = Solid;
    CullMode = none;
    MultisampleEnable = FALSE;
};


DepthStencilState soliddepth
{
    DepthEnable = FALSE;
};


DepthStencilState depthEnable
{
    DepthEnable = TRUE;
    Depthfunc = LESS_EQUAL;
    DepthWriteMask = ALL;
    StencilEnable = FALSE;
};

BlendState Blending
{
    AlphaToCoverageEnable = TRUE;
    BlendEnable[0] = TRUE;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

technique10 DisplayDiffusionImage2
{
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, TetraVS2()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TetraUpPS()));
        SetRasterizerState(state);
        SetBlendState(Blending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(soliddepth, 0);
    }
}


technique10 DisplayDiffusionImage
{
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, TetraVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TetraUpPS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(soliddepth, 0);
    }
}



technique10 DrawCurves
{
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, CurveVS()));
        SetGeometryShader(CompileShader(gs_4_0, CurveGS1()));
        SetPixelShader(CompileShader(ps_4_0, CurvePS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(depthEnable, 0);
    }
    pass P2
    {
        SetVertexShader(CompileShader(vs_4_0, CurveVS()));
        SetGeometryShader(CompileShader(gs_4_0, CurveGS2()));
        SetPixelShader(CompileShader(ps_4_0, CurvePS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(depthEnable, 0);
    }
    pass P3
    {
        SetVertexShader(CompileShader(vs_4_0, CurveVS()));
        SetGeometryShader(CompileShader(gs_4_0, CurveGS3()));
        SetPixelShader(CompileShader(ps_4_0, CurvePS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(depthEnable, 0);
    }
    pass P4
    {
        SetVertexShader(CompileShader(vs_4_0, CurveVS()));
        SetGeometryShader(CompileShader(gs_4_0, CurveGS4()));
        SetPixelShader(CompileShader(ps_4_0, CurvePS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(depthEnable, 0);
    }
    pass P5
    {
        SetVertexShader(CompileShader(vs_4_0, CurveVS()));
        SetGeometryShader(CompileShader(gs_4_0, CurveGS5()));
        SetPixelShader(CompileShader(ps_4_0, CurvePS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(depthEnable, 0);
    }
}



technique10 Diffuse
{
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, TetraVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, DiffusePS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(soliddepth, 0);
    }
}


technique10 LineAntiAlias
{
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, TetraVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, LineAntiAliasPS()));
        SetRasterizerState(state);
        SetBlendState(NoBlending, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xFFFFFFFF);
        SetDepthStencilState(soliddepth, 0);
    }
}


