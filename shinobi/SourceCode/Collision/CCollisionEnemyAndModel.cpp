#include "CCollisionEnemyAndModel.h"
#include "CEnemy.h"
#include "CModelStorage.h"
#include "CPolygonMath.h"

#define PUSH_VALUE (0.01f)

void CCollisionEnemyAndModel::Collision(CEnemy* pEnemy, D3DXVECTOR3* OldPos)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		_RPT0(0, "[CCollisionPlayerAndModel.cpp][Update]����pPlayer�̒l�̓k��\n");
		return;
	}
	if (OldPos == nullptr) {
		_RPT0(0, "[CCollisionPlayerAndModel.cpp][Update]����OldPos�̒l�̓k��\n");
		return;
	}

	//�v���[���[���W�擾
	D3DXVECTOR3 PlayerBodyPos = pEnemy->GetBodyPos();

	//���f����AABB�ƃv���[���[�̓��蔻��
	for (int i = 0; i < MODEL_OBJ_MAX; i++) {
		//���f���|�C���^�擾
		CModel* pModel = CModelStorage::GetObj(i);
		if (pModel == nullptr) {
			continue;
		}

		//�v���[���[���W�̓��f����AABB�͈͓̔������v�Z
		D3DXVECTOR3 ModelPos = pModel->GetPosition();
		D3DXVECTOR2 XValue, YValue, ZValue;
		pModel->GetAABBValue(&XValue, &YValue, &ZValue);

		float XMinValue = XValue.x + ModelPos.x;
		float XMaxValue = XValue.y + ModelPos.x;
		float YMinValue = YValue.x + ModelPos.y;
		float YMaxValue = YValue.y + ModelPos.y;
		float ZMinValue = ZValue.x + ModelPos.z;
		float ZMaxValue = ZValue.y + ModelPos.z;

		bool IsPlayerInModelAABB_XDir = (PlayerBodyPos.x >= XMinValue) && (PlayerBodyPos.x <= XMaxValue);
		bool IsPlayerInModelAABB_YDir = (PlayerBodyPos.y >= YMinValue) && (PlayerBodyPos.y <= YMaxValue);
		bool IsPlayerInModelAABB_ZDir = (PlayerBodyPos.z >= ZMinValue) && (PlayerBodyPos.z <= ZMaxValue);
		bool IsPlayerInModel = IsPlayerInModelAABB_XDir && IsPlayerInModelAABB_YDir && IsPlayerInModelAABB_ZDir;

		//�͈͓����Ȃ��Ȃ玟�̃��f���ɍs��
		if (IsPlayerInModel == false) {
			continue;
		}

		//�Â����W�����f�����ɂ��邩
		bool IsPlayerOldInModelAABB_XDir = (OldPos->x >= XMinValue) && (OldPos->x <= XMaxValue);
		bool IsPlayerOldInModelAABB_YDir = (OldPos->y >= YMinValue) && (OldPos->y <= YMaxValue);
		bool IsPlayerOldInModelAABB_ZDir = (OldPos->z >= ZMinValue) && (OldPos->z <= ZMaxValue);
		bool IsPlayerOldInModel = IsPlayerOldInModelAABB_XDir && IsPlayerOldInModelAABB_YDir && IsPlayerOldInModelAABB_ZDir;

		//�Â����W�͔͈͓�
		if (IsPlayerOldInModel == true) {
			float DistanceFromXMin = fabs(PlayerBodyPos.x - XValue.x - ModelPos.x);
			float DistanceFromXMax = fabs(PlayerBodyPos.x - XValue.y - ModelPos.x);
			float DistanceFromZMin = fabs(PlayerBodyPos.z - ZValue.x - ModelPos.z);
			float DistanceFromZMax = fabs(PlayerBodyPos.z - ZValue.y - ModelPos.z);

			//�ȏ�̎l�̒l�̍ŏ��l���擾
			float Dist[4] = { DistanceFromXMin ,DistanceFromXMax ,DistanceFromZMin ,DistanceFromZMax };
			int Number = MinDistance(Dist);

			//�v���[���[�̍��W�����炷
			switch (Number) {
			case 0:
				pEnemy->SetPosX(XValue.x + ModelPos.x - PUSH_VALUE);
				break;
			case 1:
				pEnemy->SetPosX(XValue.y + ModelPos.x + PUSH_VALUE);
				break;
			case 2:
				pEnemy->SetPosZ(ZValue.x + ModelPos.z - PUSH_VALUE);
				break;
			case 3:
				pEnemy->SetPosZ(ZValue.y + ModelPos.z + PUSH_VALUE);
				break;
			}

			//���[�v���甲����
			break;
		}

		else {
			bool IsHit = false;
			float Dummyt1, Dummyt2;
			D3DXVECTOR2 CrossPoint = D3DXVECTOR2(0, 0);		//�����_

			//XZ���ʏ�̃v���[���[����
			D3DXVECTOR2 PlayerXZVec = D3DXVECTOR2(PlayerBodyPos.x, PlayerBodyPos.z) - D3DXVECTOR2(OldPos->x, OldPos->z);
			Segment SegPlayer;
			SegPlayer.s = D3DXVECTOR2(OldPos->x, OldPos->z);
			SegPlayer.v = PlayerXZVec;

			//XZ���ʏ�AABB�̎l�̓_
			D3DXVECTOR2 PointXMinZMin = D3DXVECTOR2(XMinValue, ZMinValue);
			D3DXVECTOR2 PointXMaxZMin = D3DXVECTOR2(XMaxValue, ZMinValue);
			D3DXVECTOR2 PointXMaxZMax = D3DXVECTOR2(XMaxValue, ZMaxValue);
			D3DXVECTOR2 PointXMinZMax = D3DXVECTOR2(XMinValue, ZMaxValue);

			//XMinZMin����XMaxZMin�܂ł̐����ƃv���[���[�����̓��蔻������߂�
			Segment XMinZMinToXMaxZMin;
			XMinZMinToXMaxZMin.s = PointXMinZMin;
			XMinZMinToXMaxZMin.v = PointXMaxZMin - PointXMinZMin;

			//���蔻�肪��������v���[���[�̍��W�𒲐�
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMinZMinToXMaxZMin, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pEnemy->SetPosZ(ZMinValue - PUSH_VALUE);
				break;
			}

			//XMaxZMin����XMaxZMax�܂ł̐����ƃv���[���[�����̓��蔻������߂�
			Segment XMaxZMinToXMaxZMax;
			XMaxZMinToXMaxZMax.s = PointXMaxZMin;
			XMaxZMinToXMaxZMax.v = PointXMaxZMax - PointXMaxZMin;

			//���蔻�肪��������v���[���[�̍��W�𒲐�
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMaxZMinToXMaxZMax, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pEnemy->SetPosX(XMaxValue + PUSH_VALUE);
				break;
			}

			//XMaxZMax����XMinZMax�܂ł̐����ƃv���[���[�����̓��蔻������߂�
			Segment XMaxZMaxToXMinZMax;
			XMaxZMaxToXMinZMax.s = PointXMaxZMax;
			XMaxZMaxToXMinZMax.v = PointXMinZMax - PointXMaxZMax;

			//���蔻�肪��������v���[���[�̍��W�𒲐�
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMaxZMaxToXMinZMax, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pEnemy->SetPosZ(ZMaxValue + PUSH_VALUE);
				break;
			}

			//XMinZMax����XMinZMin�܂ł̐����ƃv���[���[�����̓��蔻������߂�
			Segment XMinZMaxToXMinZMin;
			XMinZMaxToXMinZMin.s = PointXMinZMax;
			XMinZMaxToXMinZMin.v = PointXMinZMin - PointXMinZMax;

			//���蔻�肪��������v���[���[�̍��W�𒲐�
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMinZMaxToXMinZMin, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pEnemy->SetPosX(XMinValue - PUSH_VALUE);
				break;
			}
		}
	}

	//�Â����W�̏㏑��
	*OldPos = PlayerBodyPos;
}

//�l�̋����̒��̍ŏ��l�����߂�
//�߂�l : �ŏ��l�̗v�f�ԍ�
int CCollisionEnemyAndModel::MinDistance(float Dist[4])
{
	float Min = Dist[0];
	int Number = 0;
	for (int i = 0; i < 3; i++) {
		if (Min > Dist[i + 1]) {
			Min = Dist[i + 1];
			Number = i + 1;
		}
	}
	return Number;
}