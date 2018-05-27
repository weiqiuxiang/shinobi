//���� : �R���W�����p�̐��w�֐�
//
#pragma once
#ifndef CCOLLISION_MATH_H_
#define CCOLLISION_MATH_H_

#include "main.h"

class CCollisionMath
{
public:
	static bool CircleAndCircleSSV(const D3DXVECTOR3& APosOld, const D3DXVECTOR3& APos,float ARadius, 
		const D3DXVECTOR3& BPosOld, const D3DXVECTOR3& BPos, float BRadius,float *t_Out
	);                                                                                             //���Ƌ��̏��ˎ����擾
	static bool CirclesCollision(const D3DXVECTOR3& APos, float ARadius, const D3DXVECTOR3& BPos, float BRadius);
	static bool PointInRect2D(const D3DXVECTOR2& Point, const D3DXVECTOR2& LeftUp, const D3DXVECTOR2& RightBottom);
	static bool PointInRect2DCenter(const D3DXVECTOR2& Point, const D3DXVECTOR2& CenterPos, const D3DXVECTOR2& Size);
};

#endif