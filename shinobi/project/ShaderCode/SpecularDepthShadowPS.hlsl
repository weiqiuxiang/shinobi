sampler Sampler0;
sampler DepthMapSampler;

float3 lightDirL;
float3 posEyeL;
float specularPower;

//ピクセルデータ
struct IN_VERTEX
{
	float2 uv : TEXCOORD0;
	float3 normalL : TEXCOORD1;	
	float3 posL : TEXCOORD2;	
	float4 colorH : COLOR0;

	//深度シャドウ部分
	float4 lightPosH : TEXCOORD3;
};

float4 main(IN_VERTEX InputData) : COLOR0
{
	//法線単位化
	InputData.normalL = normalize(InputData.normalL);

	//カメラへのベクトル
	float3 toEyeL = normalize(posEyeL - InputData.posL);
	
	//反射ベクトル
	float3 reflectVector = reflect(lightDirL, InputData.normalL);
	
	//スペキュラの明るさ計算(スペキュラの光は何割目に入るか)
	float specBrightness = pow(max(dot(reflectVector, toEyeL), 0.0f), specularPower);
	
	//Diffuseの計算
	float diffBrightness = (dot(InputData.normalL, -lightDirL) + 1) * 0.5f;
	
	//光計算
	float3 specularColor = specBrightness * float3(1.0f,1.0f,1.0f);
	float3 diffuseColor = diffBrightness * float3(0.7f,0.7f,0.7f);
	float3 LightColor = (diffuseColor + specularColor);
	
	float4 ResultColor = float4(LightColor, 1.0f) * InputData.colorH * tex2D(Sampler0, InputData.uv);
	
	//射影変換を終えたでピクセルのライト方向の位置を、ビューポート変換し、シャドウマップに対応したUV座標を算出
	InputData.lightPosH.x = 0.5 + InputData.lightPosH.x / InputData.lightPosH.w * 0.5f;
	InputData.lightPosH.y = 0.5f - InputData.lightPosH.y / InputData.lightPosH.w * 0.5f;

	//算出されたUVを使って、シャドウマップから深度データを取得する
	float lightDepthWV = tex2D(DepthMapSampler, InputData.lightPosH.xy).r;
	float modelDepthWV = InputData.lightPosH.z / InputData.lightPosH.w;
	float ColorRate = 1.0f;
	if ((lightDepthWV + 0.003f) < modelDepthWV) {
		ColorRate = 0.5f;
	}
	
	return float4(ResultColor.rgb * ColorRate,1.0f);
	//return tex2D(DepthMapSampler, InputData.uv);
}