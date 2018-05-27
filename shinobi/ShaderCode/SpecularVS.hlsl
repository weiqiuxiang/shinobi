float4x4 mtxWVP;		//���[���h�r���[�v���W�F�N�V�����̍����}�g���N�X
float4 DiffuseColor;	//�}�e���A���f�B�t�F���X�̐F
float4 AmbientColor;	//�}�e���A���f�B�t�F���X�̐F
float4 EmissiveColor;	//�}�e���A���f�B�t�F���X�̐F

//�[�x�V���h�E����
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
	//���������Ȃ��ƃG���[�o���
	OUT_VERTEX OutputData = (OUT_VERTEX)0;

	OutputData.posH = mul(float4(InputData.posL, 1.0f), mtxWVP);

	OutputData.posL = InputData.posL;

	OutputData.normalL = InputData.normalL;

	OutputData.colorH = DiffuseColor + EmissiveColor;
	OutputData.uv = InputData.uv;

	return OutputData;
}