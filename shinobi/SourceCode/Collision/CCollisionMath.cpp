#include "CCollisionMath.h"

bool CCollisionMath::CircleAndCircleSSV(const D3DXVECTOR3& APosOld, const D3DXVECTOR3& APos, float ARadius, const D3DXVECTOR3& BPosOld, const D3DXVECTOR3& BPos, float BRadius,float *t_Out)
{
	//�񎟕������̊e�p�����[�^������
	D3DXVECTOR3 C1 = APos - BPos;
	D3DXVECTOR3 C0 = APosOld - BPosOld;
	D3DXVECTOR3 D = C1 - C0;
	float P = D3DXVec3LengthSq(&D);
	float Q = D3DXVec3Dot(&C0, &D);
	float R = D3DXVec3LengthSq(&C0);

	//��A�Ƌ�B�̔��a�̘a�̕���
	float Radius_SumSq = (ARadius + BRadius) * (ARadius + BRadius);

	//�񎟕������̉������݂���萔������(b^2 - 4*a*c)
	float K = Q*Q - P*(R - Radius_SumSq);
	if (K < 0) return false;                      //���݂��Ȃ��ꍇreturn false�I��

	//���̃t���[���̏��ˎ���������
	float t1 = (-Q - sqrtf(K)) / P;
	float t2 = (-Q + sqrtf(K)) / P;

	//�t���[�����ŏՓ˂��鎞������
	bool bt1Check = (t1 >= 0.0f) && (t1 <= 1.0f);
	bool bt2Check = (t2 >= 0.0f) && (t2 <= 1.0f);

	if(bt1Check || bt2Check)
	{
		if(bt1Check && bt2Check)
		{
			*t_Out = t1;                   //���ˎ������i�[
			return true;
		}

		else
		{
			if(bt1Check)
			{
				*t_Out = t1;                   //���ˎ������i�[
				return true;
			}

			else
			{
				*t_Out = t2;                   //���ˎ������i�[
				return true;
			}
		}
	}

	return false;
}

bool CCollisionMath::CirclesCollision(const D3DXVECTOR3& APos, float ARadius, const D3DXVECTOR3& BPos, float BRadius)
{
	D3DXVECTOR3 distanceVec = APos - BPos;
	float ABRadiusSQ = (ARadius + BRadius)*(ARadius + BRadius);				//���Ƌ��̘a�̕���
	float distanceSQ = D3DXVec3LengthSq(&distanceVec);						//�����S�Ƌ����S�����̕���
	if (distanceSQ < ABRadiusSQ) return true;
	return false;
}

bool CCollisionMath::PointInRect2D(const D3DXVECTOR2& Point, const D3DXVECTOR2& LeftUp, const D3DXVECTOR2& RightBottom)
{
	bool XAxisInRange = (Point.x >= LeftUp.x) && (Point.x <= RightBottom.x);
	bool YAxisInRange = (Point.y >= LeftUp.y) && (Point.x <= RightBottom.y);
	if (XAxisInRange && YAxisInRange) return true;
	return false;
}

bool CCollisionMath::PointInRect2DCenter(const D3DXVECTOR2& Point, const D3DXVECTOR2& CenterPos, const D3DXVECTOR2& Size)
{
	bool XAxisInRange = (Point.x >= (CenterPos.x - Size.x * 0.5)) && (Point.x <= (CenterPos.x + Size.x * 0.5));
	bool YAxisInRange = (Point.y >= (CenterPos.y - Size.y * 0.5)) && (Point.y <= (CenterPos.y + Size.y * 0.5));
	if (XAxisInRange && YAxisInRange) return true;
	return false;
}