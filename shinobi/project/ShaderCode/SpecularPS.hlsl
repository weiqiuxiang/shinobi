sampler Sampler0;

float3 lightDirL;
float3 posEyeL;
float specularPower;

//�s�N�Z���f�[�^
struct IN_VERTEX
{
	float2 uv : TEXCOORD0;
	float3 normalL : TEXCOORD1;	
	float3 posL : TEXCOORD2;	
	float4 colorH : COLOR0;
};

float4 main(IN_VERTEX InputData) : COLOR0
{
	//�@���P�ʉ�
	InputData.normalL = normalize(InputData.normalL);

	//�J�����ւ̃x�N�g��
	float3 toEyeL = normalize(posEyeL - InputData.posL);
	
	//���˃x�N�g��
	float3 reflectVector = reflect(lightDirL, InputData.normalL);
	
	//�X�y�L�����̖��邳�v�Z(�X�y�L�����̌��͉����ڂɓ��邩)
	float specBrightness = pow(max(dot(reflectVector, toEyeL), 0.0f), 10);
	
	//Diffuse�̌v�Z
	float diffBrightness = (dot(InputData.normalL, -lightDirL) + 1) * 0.5f;
	
	//���v�Z
	float3 specularColor = specBrightness * float3(1.0f,1.0f,1.0f);
	float3 diffuseColor = diffBrightness * float3(0.7f,0.7f,0.7f);
	float3 LightColor = (diffuseColor + specularColor);
	
	float4 ResultColor = float4(LightColor, 1.0f) * InputData.colorH * tex2D(Sampler0, InputData.uv);
	
	return float4(ResultColor.rgb,1.0f);
}