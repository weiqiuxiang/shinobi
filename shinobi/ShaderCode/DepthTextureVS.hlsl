struct VS_OUTPUT
{
	float4 posH : POSITION;				// �ˉe�ϊ����W
	float4 UsePosH : TEXCOORD0;		//Z�o�b�t�@�e�N�X�`��
};

float4x4 mtxLightWVP;

VS_OUTPUT main(float3 posL : POSITION0)
{
	VS_OUTPUT outVS = (VS_OUTPUT)0;
	outVS.posH = mul(float4(posL, 1.0f), mtxLightWVP);
	outVS.UsePosH = outVS.posH;

	return outVS;
}