cbuffer matrixbuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

struct VS_IN
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VS_OUT LineVertexShader(float3 position : POSITION, float4 color : COLOR)
{
    VS_OUT output;
    output.pos = mul(float4(position,1.0), mul(mul(worldMatrix, viewMatrix), projectionMatrix));
    output.color = color;
    return output;
}