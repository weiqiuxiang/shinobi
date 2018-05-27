sampler OriginalSampler;

//�s�N�Z���f�[�^
struct IN_VERTEX
{
	float2 uv : TEXCOORD0;				//uv
	float2 blurSpeed : TEXCOORD1;		//�u���[���x
	float2 ScreenUV : TEXCOORD2;		//�X�N���[����UV
};

float4 main(IN_VERTEX InputData) : COLOR0
{
	float4 color = (float4)0;
	for (int i = 0; i < 10; i++) {
		color += tex2D(OriginalSampler, InputData.ScreenUV + InputData.blurSpeed * -0.1f * i) *0.1f;
	}
	
	return color;
}