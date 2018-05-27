#include "CLockOnEnemy.h"
#include "CEnemyStorage.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"
#include "CVectorMath.h"

#define LOCKON_ANGLE_HORI (D3DX_PI * 0.4)		//���b�N�I���ł���p�x(����)
#define LOCKON_ANGLE_VERTI (D3DX_PI * 0.3)		//���b�N�I���ł���p�x(����)

//��������:
//�v���[���[�|�C���^�[
//�����b�N�I�����Ă���G
//���b�N�I���ύX�̕���(�� : false,�E : true)
CEnemy* CLockOnEnemy::ChangeTarget(CPlayer* pPlayer, CEnemy *pEnemy, bool bDir)
{
	//�������^�[��
	if (nullptr == pPlayer) {
		return nullptr;
	}
	if (nullptr == pEnemy) {
		return nullptr;
	}

	//�ϐ��錾
	CEnemy* pEnemyReturn = nullptr;						//CEnemy�|�C���^
	float HoriAngleSabunMin = D3DX_PI*10;				//�����p�x�̍����̍ŏ��l

	//�f�[�^�擾
	auto itEnemy = CEnemyStorage::GetEnemiesListBegin();	//���X�g��Begin�C�e���[�^�[
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();		//���X�g��End�C�e���[�^�[

	//�p�x�v�Z
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//�J�����f�[�^�擾
	D3DXVECTOR2 AngleCL = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, CameraPara.lookat);	//�J�����ƒ����_�̐����A�����p�x�v�Z

	for (; itEnemy != itEnd; ++itEnemy) {
		if ((*itEnemy) == pEnemy) continue;
		if ((*itEnemy)->GetDeadFlag()) continue;

		//�v���[���[�ƓG�Ƃ̋����v�Z
		D3DXVECTOR3 PlayerEnemyVec = (*itEnemy)->GetPos() - pPlayer->GetPos();
		float Distance = D3DXVec3Length(&PlayerEnemyVec);
		if (Distance > LOCKON_DISTANCE)  continue; 						//���b�N�I�������ɖ������ĂȂ��Ȃ�

		//�p�x���v�Z
		D3DXVECTOR2 AngleCE = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, (*itEnemy)->GetPos());	//�J�����ƓG�̐����A�����p�x�v�Z
		float HoriAngleSabun = fabs(AngleCL.x - AngleCE.x);					//�����p�x���v�Z
		float VertiAngleSabun = fabs(AngleCL.y - AngleCE.y);				//�����p�x���v�Z
		bool bAngleCanLockon = (HoriAngleSabun < LOCKON_ANGLE_HORI) && (VertiAngleSabun < LOCKON_ANGLE_VERTI);					//�����Ɛ����p�x���͈͓���
		if (!bAngleCanLockon) continue;

		//�����`�F�b�N
		bool bDirCheck = false;
		D3DXVECTOR3 Cross;
		D3DXVec3Cross(
			&Cross,
			&(D3DXVECTOR3(CameraPara.lookat.x - CameraPara.pos.x,0.0f, CameraPara.lookat.z - CameraPara.pos.z)),
			&(D3DXVECTOR3((*itEnemy)->GetPos().x - CameraPara.pos.x, 0.0f, (*itEnemy)->GetPos().z - CameraPara.pos.z))
			);
		if ((false == bDir) && (Cross.y <= 0.0f)) bDirCheck = true;		//�G�����ɂ���
		if ((true == bDir) && (Cross.y > 0.0f)) bDirCheck = true;		//�G���E�ɂ���
		if (!bDirCheck) continue;

		//�����p�x���͍ŏ��l��
		bool bIsHoriAngleSabunMin = HoriAngleSabun < HoriAngleSabunMin;															//�����p�x���l�̍ŏ��l��
		if (bIsHoriAngleSabunMin) {
			pEnemyReturn = (*itEnemy);				//�|�C���^���
			HoriAngleSabunMin = HoriAngleSabun;		//�ŏ��l�㏑��
		}
	}

	return pEnemyReturn;
}

CEnemy* CLockOnEnemy::LockOnEnemy(CPlayer* pPlayer)
{
	//�������^�[��
	if (nullptr == pPlayer) return nullptr;

	//�ϐ��錾
	CEnemy* pEnemy = nullptr;								//CEnemy�|�C���^
	float HoriAngleSabunMin = D3DX_PI * 10;				//�����p�x�̍����̍ŏ��l

	//�f�[�^�擾
	auto itEnemy = CEnemyStorage::GetEnemiesListBegin();		//���X�g��Begin�C�e���[�^�[
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();	//���X�g��End�C�e���[�^�[

	//�p�x�v�Z
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//�J�����f�[�^�擾
	D3DXVECTOR2 AngleCL = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, CameraPara.lookat);	//�J�����ƒ����_�̐����A�����p�x�v�Z

	for (; itEnemy != itEnd; ++itEnemy) {
		if ((*itEnemy)->GetDeadFlag()) continue;

		//�v���[���[�ƓG�Ƃ̋����v�Z
		D3DXVECTOR3 PlayerEnemyVec = (*itEnemy)->GetPos() - pPlayer->GetPos();
		float Distance = D3DXVec3Length(&PlayerEnemyVec);
		if (Distance > LOCKON_DISTANCE)  continue; 						//���b�N�I�������ɖ������ĂȂ��Ȃ�

		//�p�x���v�Z
		D3DXVECTOR2 AngleCE = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, (*itEnemy)->GetPos());	//�J�����ƓG�̐����A�����p�x�v�Z
		float HoriAngleSabun = fabs(AngleCL.x - AngleCE.x);					//�����p�x���v�Z
		float VertiAngleSabun = fabs(AngleCL.y - AngleCE.y);				//�����p�x���v�Z
		bool bAngleCanLockon = (HoriAngleSabun < LOCKON_ANGLE_HORI) && (VertiAngleSabun < LOCKON_ANGLE_VERTI);					//�����Ɛ����p�x���͈͓���
		bool bIsHoriAngleSabunMin = HoriAngleSabun < HoriAngleSabunMin;															//�����p�x���l�̍ŏ��l��

		if (bAngleCanLockon && bIsHoriAngleSabunMin) {
			pEnemy = (*itEnemy);					//�|�C���^���
			HoriAngleSabunMin = HoriAngleSabun;		//�ŏ��l�㏑��
		}
	}

	return pEnemy;
}