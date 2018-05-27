sampler Sampler0;
sampler DepthMapSampler;

float3 lightDirL;

//�s�N�Z���f�[�^
struct IN_VERTEX
{
	float2 uv : TEXCOORD0;
	float3 normalL : TEXCOORD1;
	float4 colorH : COLOR0;

	//�[�x�V���h�E����
	float4 lightPosH : TEXCOORD3;
};

float4 main(IN_VERTEX InputData) : COLOR0
{
	//�@���P�ʉ�
	InputData.normalL = normalize(InputData.normalL);
	
	//Diffuse�̌v�Z
	float diffBrightness = (dot(InputData.normalL, -lightDirL) + 1) * 0.5f;
	
	//���v�Z
	float3 diffuseColor = diffBrightness * float3(0.7f,0.7f,0.7f);
	float3 LightColor = (diffuseColor);
	
	float4 ResultColor = float4(LightColor, 1.0f) * InputData.colorH * tex2D(Sampler0, InputData.uv);
	
	//�ˉe�ϊ����I�����Ńs�N�Z���̃��C�g�����̈ʒu���A�r���[�|�[�g�ϊ����A�V���h�E�}�b�v�ɑΉ�����UV���W���Z�o
	InputData.lightPosH.x = 0.5 + InputData.lightPosH.x / InputData.lightPosH.w * 0.5f;
	InputData.lightPosH.y = 0.5f - InputData.lightPosH.y / InputData.lightPosH.w * 0.5f;

	//�Z�o���ꂽUV���g���āA�V���h�E�}�b�v����[�x�f�[�^���擾����
	float lightDepthWV = tex2D(DepthMapSampler, InputData.lightPosH.xy).r;
	float modelDepthWV = InputData.lightPosH.z / InputData.lightPosH.w;
	float ColorRate = 1.0f;
	if ((lightDepthWV + 0.003f) < modelDepthWV) {
		ColorRate = 0.5f;
	}
	
	return float4(ResultColor.rgb * ColorRate,1.0f);
	//return tex2D(DepthMapSampler, InputData.uv);
}