#include "CVectorMath.h"

float CVectorMath::Vec2Angle(const D3DXVECTOR2& A, const D3DXVECTOR2& B) 
{
	float Dot = A.x*B.x + A.y*B.y;
	float Cross = A.x*B.y - A.y*B.x;
	float Angle = atan2f(Dot, Cross);
	return Angle;
}