float4x4 mtxWVP;		//ワールドビュープロジェクションの合成マトリクス
float4 DiffuseColor;	//マテリアルディフェンスの色
float4 AmbientColor;	//マテリアルディフェンスの色
float4 EmissiveColor;	//マテリアルディフェンスの色

//深度シャドウ部分
float4x4 mtxLightWVP;

struct IN_VERTEX
{
	float3 posL: POSITION0;
	float3 normalL:NORMAL0;
	float2 uv : TEXCOORD0;
};

struct OUT_VERTEX
{
	float4 posH: POSITION0;
	float2 uv : TEXCOORD0;
	float3 normalL : TEXCOORD1;
	float3 posL : TEXCOORD2;
	float4 colorH : COLOR0;
};

OUT_VERTEX main(IN_VERTEX InputData)
{
	//初期化しないとエラー出るよ
	OUT_VERTEX OutputData = (OUT_VERTEX)0;

	OutputData.posH = mul(float4(InputData.posL, 1.0f), mtxWVP);

	OutputData.posL = InputData.posL;

	OutputData.normalL = InputData.normalL;

	OutputData.colorH = DiffuseColor + EmissiveColor;
	OutputData.uv = InputData.uv;

	return OutputData;
}