float4 main(float4 UsePosH : TEXCOORD0) : COLOR0
{
	return float4(UsePosH.z / UsePosH.w,0,0,1);
}