sampler Sampler0;

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
	return tex2D(Sampler0,uv);
}