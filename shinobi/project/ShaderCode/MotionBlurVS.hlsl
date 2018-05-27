//�[�x�V���h�E����
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
	float2 blurSpeed : TEXCOORD1;		//�u���[���x
	float2 ScreenUV : TEXCOORD2;		//�X�N���[����UV
};

OUT_VERTEX main(IN_VERTEX InputData)
{
	//���������Ȃ��ƃG���[�o���
	OUT_VERTEX OutputData = (OUT_VERTEX)0;

	//�O��posH�ƍ�posH�����߂�
	float4 oldPosH = mul(float4(InputData.posL,1.0f), mtxWVPOld);
	OutputData.posH = mul(float4(InputData.posL, 1.0f), mtxWVP);

	//�u���[�̑��x�����߂�
	OutputData.blurSpeed = OutputData.posH.xy / OutputData.posH.w - oldPosH.xy / oldPosH.w;
	OutputData.blurSpeed *= 0.05f;		//���x����
	
	//���uv���Z�o
	OutputData.ScreenUV.x = (OutputData.posH.x / OutputData.posH.w  * 0.5f) + 0.5f;
	OutputData.ScreenUV.y = (OutputData.posH.y / OutputData.posH.w  * -0.5f) + 0.5f;

	//����uv���
	OutputData.uv = InputData.uv;

	//normalH�����߂�
	float2 normalH = mul(float4(InputData.normalL,0.0f), mtxWITVP).xy;

	//�u���[�Ώ۔���
	float dotSpeedAndNormal = dot(normalH, OutputData.blurSpeed);
	if (dotSpeedAndNormal <= 0) {
		OutputData.posH = oldPosH;
	}

	return OutputData;
}