struct VS_OUTPUT
{
	float4 posH : POSITION;				// 射影変換座標
	float4 UsePosH : TEXCOORD0;		//Zバッファテクスチャ
};

float4x4 mtxLightWVP;

VS_OUTPUT main(float3 posL : POSITION0)
{
	VS_OUTPUT outVS = (VS_OUTPUT)0;
	outVS.posH = mul(float4(posL, 1.0f), mtxLightWVP);
	outVS.UsePosH = outVS.posH;

	return outVS;
}