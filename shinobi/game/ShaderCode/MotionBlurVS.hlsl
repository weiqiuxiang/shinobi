//深度シャドウ部分
float4x4 mtxWVPOld;
float4x4 mtxWVP;
float4x4 mtxWITVP;

struct IN_VERTEX
{
	float3 posL: POSITION0;
	float3 normalL:NORMAL0;
	float2 uv : TEXCOORD0;
};

struct OUT_VERTEX
{
	float4 posH: POSITION0;				//posH
	float2 uv : TEXCOORD0;				//uv
	float2 blurSpeed : TEXCOORD1;		//ブラー速度
	float2 ScreenUV : TEXCOORD2;		//スクリーンのUV
};

OUT_VERTEX main(IN_VERTEX InputData)
{
	//初期化しないとエラー出るよ
	OUT_VERTEX OutputData = (OUT_VERTEX)0;

	//前のposHと今posHを求める
	float4 oldPosH = mul(float4(InputData.posL,1.0f), mtxWVPOld);
	OutputData.posH = mul(float4(InputData.posL, 1.0f), mtxWVP);

	//ブラーの速度を求める
	OutputData.blurSpeed = OutputData.posH.xy / OutputData.posH.w - oldPosH.xy / oldPosH.w;
	OutputData.blurSpeed *= 0.05f;		//速度調整
	
	//画面uvを算出
	OutputData.ScreenUV.x = (OutputData.posH.x / OutputData.posH.w  * 0.5f) + 0.5f;
	OutputData.ScreenUV.y = (OutputData.posH.y / OutputData.posH.w  * -0.5f) + 0.5f;

	//普通uv代入
	OutputData.uv = InputData.uv;

	//normalHを求める
	float2 normalH = mul(float4(InputData.normalL,0.0f), mtxWITVP).xy;

	//ブラー対象判定
	float dotSpeedAndNormal = dot(normalH, OutputData.blurSpeed);
	if (dotSpeedAndNormal <= 0) {
		OutputData.posH = oldPosH;
	}

	return OutputData;
}