#include "CCollisionPlayerAndEnemy.h"
#include "CCollisionMath.h"
#include "CCollisionCircle.h"
#include "CBloodStorage.h"
#include "CGame.h"
#include <crtdbg.h>

//�}�N��
#define POWER_BODYHIT (0.8f)
#define PUSH_OUT_POWER (0.05f)					//�����o����

#define TEX_BLOOD_FILE_PASS "data/TEXTURE/DamageLifeColor.png"		//���̃p�[�e�B�N���̃e�N�X�`���̃t�@�C���p�X
#define BLOOD_PIECE_AMOUNT (5)
#define BLOOD_DEFAULT_CALC_POSITION_TIME (4)		//�U���q�b�g���猌���Ԃ��̌v�Z�t���[����
#define BLOOD_POS_DIST_FROM_DAMAGE_CIRCLE (0.5f)	//�����˂̈ʒu�̓_���[�W�����痣��鋗��
#define BLOOD_SHOT_POWER (0.5f)						//�����˂̎��̗�

//�p�[�e�B�N�����ˊp�x�̏d��
#define WEIGHT_ATTACK_CIRCLE_DIR (2.0f)		//�U�����̈ړ������̏d��
#define WEIGHT_OTHER (0.3f)					//���̈ړ������̏d��(���̘a��WEIGHT_OTHER�̒l)

//�O���[�o���ϐ�
CPlayer* CColliPlayerEnemy::m_Player = nullptr;
CEnemy* CColliPlayerEnemy::m_Enemy = nullptr;
CEnemy* CColliPlayerEnemy::m_EnemyOtherEnemy = nullptr;
float CColliPlayerEnemy::m_CoffiPushOut = 0.0f;
BLOOD_POSITION CColliPlayerEnemy::m_BloodPosition[BLOOD_POSITION_MAX];

void CColliPlayerEnemy::Init(void)
{
	m_Player = nullptr;
	m_Enemy = nullptr;

	for (int i = 0; i < BLOOD_POSITION_MAX; i++) {
		m_BloodPosition[i].AttackCircleOldPos = D3DXVECTOR3(0,0,0);
		m_BloodPosition[i].DamageCircleOldPos = D3DXVECTOR3(0,0,0);
		m_BloodPosition[i].AttackCircle = nullptr;
		m_BloodPosition[i].DamageCircle = nullptr;
		m_BloodPosition[i].Time = BLOOD_DEFAULT_CALC_POSITION_TIME;
		m_BloodPosition[i].IsUse = false;
	}
}

//�����Ԃ��̐����Ƃ��̈ʒu�̌v�Z
void CColliPlayerEnemy::UpdateBlood(void)
{
	for (int i = 0; i < BLOOD_POSITION_MAX; i++) {
		//�g�p�`�F�b�N
		if (m_BloodPosition[i].IsUse == false) {
			return;
		}
		//��O����
		if (m_BloodPosition[i].AttackCircle == nullptr || m_BloodPosition[i].DamageCircle == nullptr) {
			continue;
		}

		D3DXVECTOR3 AttackDir = m_BloodPosition[i].AttackCircle->GetPos() - m_BloodPosition[i].AttackCircleOldPos;
		D3DXVECTOR3 DamageDir = m_BloodPosition[i].DamageCircle->GetPos() - m_BloodPosition[i].DamageCircleOldPos;
		//�x�N�g���̒��������߂�
		float AttackLength = D3DXVec3Length(&AttackDir);
		float DamageLength = D3DXVec3Length(&DamageDir);
		//�x�N�g���̕��������߂�
		if (AttackDir != D3DXVECTOR3(0, 0, 0)) {
			D3DXVec3Normalize(&AttackDir, &AttackDir);
		}
		if (DamageDir != D3DXVECTOR3(0, 0, 0)) {
			D3DXVec3Normalize(&DamageDir, &DamageDir);
		}

		//�O�Ղɉ����āA���Ԋu��(BLOOD_PIECE_AMOUNT + 1)�_�̈ʒu�łŃp�[�e�B�N�����o��
		for (int PieceNum = 0; PieceNum <= BLOOD_PIECE_AMOUNT; PieceNum++) {
			//�p�[�e�B�N���ʒu�܂ł̒����v�Z
			float AttackToPosLength = AttackLength * (float)PieceNum / BLOOD_PIECE_AMOUNT;
			float DamageToPosLength = DamageLength * (float)PieceNum / BLOOD_PIECE_AMOUNT;

			//�_���[�W�ƍU�����̈ʒu�v�Z
			D3DXVECTOR3 AttackCirclePos = m_BloodPosition[i].AttackCircleOldPos + AttackToPosLength * AttackDir;
			D3DXVECTOR3 DamageCirclePos = m_BloodPosition[i].DamageCircleOldPos + DamageToPosLength * DamageDir;

			//�_���[�W��->�U�����̃x�N�g��
			D3DXVECTOR3 DamageAttackVec = AttackCirclePos - DamageCirclePos;
			D3DXVec3Normalize(&DamageAttackVec, &DamageAttackVec);

			//�p�[�e�B�N���ʒu�v�Z
			D3DXVECTOR3 BloodPosition = DamageCirclePos;

			//DamageAttackVec��AttackDir�ɐ����x�N�g��������
			D3DXVECTOR3 YDir = D3DXVECTOR3(0, 0, 0);
			D3DXVec3Cross(&YDir, &DamageAttackVec, &AttackDir);
			D3DXVec3Normalize(&YDir, &YDir);

			//���ˊp�x���Z�o
			float PercentWeight = (float)(rand() % 101);
			D3DXVECTOR3 ShootVec = AttackDir * WEIGHT_ATTACK_CIRCLE_DIR - DamageAttackVec * WEIGHT_OTHER * PercentWeight + YDir * WEIGHT_OTHER * (1.0f - PercentWeight);
			D3DXVec3Normalize(&ShootVec, &ShootVec);

			//���p�[�e�B�N������
			CBloodStorage::CreateObject(BloodPosition, D3DXVECTOR3(0.2f, 0.2f, 0.0f), ShootVec * BLOOD_SHOT_POWER, 60, 0.1f, TEX_BLOOD_FILE_PASS);
		}

		//���W�㏑��
		m_BloodPosition[i].AttackCircleOldPos = m_BloodPosition[i].AttackCircle->GetPos();
		m_BloodPosition[i].DamageCircleOldPos = m_BloodPosition[i].DamageCircle->GetPos();

		//�v�Z�񐔃J�E���g�_�E��
		m_BloodPosition[i].Time--;
		if (m_BloodPosition[i].Time <= 0) {
			m_BloodPosition[i].AttackCircle = nullptr;
			m_BloodPosition[i].DamageCircle = nullptr;
			m_BloodPosition[i].IsUse = false;
		}
	}
}

void CColliPlayerEnemy::GetPlayerEnemyDataAndColliCheck(void)
{
	if (CanCalcPlayerEnemy() == false) return;
	BodyCollisionPlayerEnemy();
	PlayerHitEnemy();
	EnemyHitPlayer();
}

bool CColliPlayerEnemy::CanCalcPlayerEnemy(void)
{
	if (nullptr == m_Player) return false;
	if (nullptr == m_Enemy) return false;

	CCollisionCircle* pPlayerColliCircle = m_Player->GetColliEnable();
	CCollisionCircle* pEnemyColliCircle = m_Enemy->GetColliEnable();

	bool bHit = CCollisionCircle::HitCheckUseNormal(pPlayerColliCircle, pEnemyColliCircle);
	return bHit;
}

void CColliPlayerEnemy::BodyCollisionPlayerEnemy(void)
{
	if (nullptr == m_Player) return;
	if (nullptr == m_Enemy) return;

	if (m_Enemy->GetDeadFlag() == true) return;

	int PlayerColliCircleNumber = 0;
	int EnemyColliCircleNumber = 0;

	CCollisionCircle* pPlayerColliCircle = m_Player->GetColli(0);
	CCollisionCircle* pEnemyColliCircle = m_Enemy->GetColli(0);
	int CircleMax = m_Player->GetMaxColliCircleNum();

	//�q�b�g����
	bool bBodyHit = false;

	for(int i = 0; i < CircleMax;i++){
		if (pPlayerColliCircle[i].GetType() != CCollisionCircle::TYPE_BODY) continue;

		//�G�Ƃ̑̂̓��蔻��
		for(int j = 0; j < CircleMax; j++){
			if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
			bBodyHit = CCollisionCircle::HitCheckUseNormal(&pPlayerColliCircle[i], &pEnemyColliCircle[j]);    //�q�b�g�m�F
			if (true == bBodyHit) {
				PlayerColliCircleNumber = i;
				EnemyColliCircleNumber = j;
				break;
			}
		}
		if (true == bBodyHit) break;
	}
	
	if(true == bBodyHit){
		//�f�[�^�擾
		D3DXVECTOR3 PlayerHirCirclePos = pPlayerColliCircle[PlayerColliCircleNumber].GetPos();
		D3DXVECTOR3 EnemyHirCirclePos = pEnemyColliCircle[EnemyColliCircleNumber].GetPos();
		float PlayerColliCircleRadius = pPlayerColliCircle[PlayerColliCircleNumber].GetRadius();
		float EnemyColliCircleRadius = pEnemyColliCircle[EnemyColliCircleNumber].GetRadius();

		//�v���[���[�ƓG�̑��x���擾��,���̐ڐ������Ɉړ����x�ɕ�������
		D3DXVECTOR3 PlayerSpeed = m_Player->GetSpeed();
		D3DXVECTOR3 EnemySpeed = m_Enemy->GetSpeed();
		D3DXVECTOR3 PlayerEnemyColliCircleVec = PlayerHirCirclePos - EnemyHirCirclePos;
		PlayerEnemyColliCircleVec.y = 0;
		D3DXMATRIX MtxRot;
		D3DXVECTOR3 TangentVec;									//�ڐ�
		D3DXMatrixRotationY(&MtxRot, D3DX_PI*0.5);
		D3DXVec3TransformCoord(&TangentVec, &PlayerEnemyColliCircleVec,&MtxRot);
		D3DXVec3Normalize(&TangentVec, &TangentVec);			//�P�ʃx�N�g����
		float SpeedPlayer = D3DXVec3Dot(&TangentVec,&PlayerSpeed);
		float SpeedEnemy = D3DXVec3Dot(&TangentVec, &EnemySpeed);

		//�����o���v�Z
		D3DXVECTOR3 PushDir = PlayerHirCirclePos - EnemyHirCirclePos;				//�����o�������ݒ�
		float fDistance = D3DXVec3Length(&PushDir);
		m_CoffiPushOut = max(0.0f, PlayerColliCircleRadius + EnemyColliCircleRadius - fDistance);
		PushDir.y = 0.0f;
		D3DXVec3Normalize(&PushDir, &PushDir);
		m_Player->AddPower(PUSH_OUT_POWER*fDistance*fDistance*PushDir);

		//�Â����W�ɖ߂�
		m_Player->ReturnOldPosXZ();
		m_Enemy->ReturnOldPosXZ();

		//���蔻��~�Ԃ̐ڐ������Ɉړ�������
		m_Player->AddPos(SpeedPlayer*TangentVec);
		m_Enemy->AddPos(SpeedEnemy*TangentVec);
	}
}

void CColliPlayerEnemy::PlayerHitEnemy(void)
{
	if (nullptr == m_Player) return;
	if (nullptr == m_Enemy) return;

	//���G���
	if (m_Enemy->GetNoDamageTime() > 0) {
		return;
	}

	if (m_Enemy->GetDeadFlag() == true) return;

	CCollisionCircle* pPlayerWeaponLeftColliCircle = m_Player->GetWeaponLeftColli(0);
	CCollisionCircle* pPlayerWeaponRightColliCircle = m_Player->GetWeaponRightColli(0);
	CCollisionCircle* pEnemyColliCircle = m_Enemy->GetColli(0);
	int CircleMaxWeapon01 = m_Player->GetWeaponLeftMaxColliCircle();
	int CircleMaxWeapon02 = m_Player->GetWeaponRightMaxColliCircle();

	//�q�b�g����
	bool bBodyHit = false;

	//����01��
	if (pPlayerWeaponLeftColliCircle != nullptr) {
		for (int i = 0; i < CircleMaxWeapon01; i++) {
			if (pPlayerWeaponLeftColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;
			if (false) continue;
			//�G�Ƃ̑̂̓��蔻��
			for (int j = 0; j < CircleMaxWeapon01; j++) {
				if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
				bBodyHit = CCollisionCircle::HitCheckUseSSV(&pPlayerWeaponLeftColliCircle[i], &pEnemyColliCircle[j]);    //�q�b�g�m�F
				if (true == bBodyHit) {
					CreateBloodPosition(&pPlayerWeaponLeftColliCircle[i], &pEnemyColliCircle[j]);
					break;
				}
			}
			if (true == bBodyHit) break;
		}
	}

	//����02��
	if (bBodyHit == false) {		//����01�͓�����Ȃ���Ύ��s
		if (pPlayerWeaponRightColliCircle != nullptr) {
			for (int i = 0; i < CircleMaxWeapon02; i++) {
				if (pPlayerWeaponRightColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;

				//�G�Ƃ̑̂̓��蔻��
				for (int j = 0; j < CircleMaxWeapon02; j++) {
					if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
					bBodyHit = CCollisionCircle::HitCheckUseSSV(&pPlayerWeaponRightColliCircle[i], &pEnemyColliCircle[j]);    //�q�b�g�m�F
					if (true == bBodyHit) {
						CreateBloodPosition(&pPlayerWeaponRightColliCircle[i], &pEnemyColliCircle[j]);
						break;
					}
				}
				if (true == bBodyHit) break;
			}
		}
	}

	if (true == bBodyHit) {
		D3DXVECTOR3 PlayerPos = m_Player->GetPos();
		D3DXVECTOR3 EnemyPos = m_Enemy->GetPos();
		D3DXVECTOR3 EnemyPlayerVec = PlayerPos - EnemyPos;
		EnemyPlayerVec.y = 0;
		D3DXVec3Normalize(&EnemyPlayerVec, &EnemyPlayerVec);

		m_Enemy->SetDamageType(m_Player->GetAttackType());			//�v���[���[�̍U���^�C�v�擾
		m_Enemy->SetStunDir(-EnemyPlayerVec);						//�X�^�������ݒ�
		m_Enemy->SetIsBeAttackedFlag(true);							//�U�����ꂽ�t���O��ݒ�
	}
}

void CColliPlayerEnemy::EnemyHitPlayer(void)
{
	if (nullptr == m_Player) return;
	if (nullptr == m_Enemy) return;

	//���G���
	if (m_Player->GetNoDamageTime() > 0) {
		return;
	}

	if (m_Enemy->GetDeadFlag() == true) return;

	CCollisionCircle* pPlayerColliCircle = m_Player->GetColli(0);
	CCollisionCircle* pEnemyColliCircle = m_Enemy->GetColli(0);
	int CircleMax = m_Player->GetMaxColliCircleNum();

	//�q�b�g����
	bool bBodyHit = false;
	for (int i = 0; i < CircleMax; i++) {
		if (pEnemyColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;

		//�G�Ƃ̑̂̓��蔻��
		for (int j = 0; j < CircleMax; j++) {
			if (pPlayerColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
			bBodyHit = CCollisionCircle::HitCheckUseSSV(&pPlayerColliCircle[j], &pEnemyColliCircle[i]);    //�q�b�g�m�F
			if(bBodyHit == false) bBodyHit = CCollisionCircle::HitCheckUseNormal(&pPlayerColliCircle[j], &pEnemyColliCircle[i]);    //�q�b�g�m�F
			if (true == bBodyHit) {
				CreateBloodPosition(&pEnemyColliCircle[i], &pPlayerColliCircle[j]);
				break;
			}
		}
		if (true == bBodyHit) break;
	}

	if (true == bBodyHit) {
		D3DXVECTOR3 PlayerPos = m_Player->GetPos();
		D3DXVECTOR3 EnemyPos = m_Enemy->GetPos();
		D3DXVECTOR3 EnemyPlayerVec = PlayerPos - EnemyPos;
		EnemyPlayerVec.y = 0.0f;
		D3DXVec3Normalize(&EnemyPlayerVec, &EnemyPlayerVec);

		m_Player->SetDamageType(m_Enemy->GetAttackType());			//�G�̍U���^�C�v�擾
		m_Player->SetStunDir(EnemyPlayerVec);						//�X�^�������ݒ�
		m_Player->SetIsBeAttackedFlag(true);						//�U�����ꂽ�t���O��ݒ�
	}
}

void CColliPlayerEnemy::GetEnemiesDataAndColliCheck(void)
{
	BodyCollisionEnemies();
}

void CColliPlayerEnemy::BodyCollisionEnemies(void)
{
	if (nullptr == m_EnemyOtherEnemy) return;
	if (nullptr == m_Enemy) return;
	if (m_Enemy->GetDeadFlag()) return;
	if (m_EnemyOtherEnemy->GetDeadFlag()) return;

	int PlayerColliCircleNumber = 0;
	int EnemyColliCircleNumber = 0;

	CCollisionCircle* pPlayerColliCircle = m_EnemyOtherEnemy->GetColli(0);
	CCollisionCircle* pEnemyColliCircle = m_Enemy->GetColli(0);
	int CircleMax = m_EnemyOtherEnemy->GetMaxColliCircleNum();

	//�q�b�g����
	bool bBodyHit = false;
	for (int i = 0; i < CircleMax; i++) {
		if (pPlayerColliCircle[i].GetType() != CCollisionCircle::TYPE_BODY) continue;

		//�G�Ƃ̑̂̓��蔻��
		for (int j = 0; j < CircleMax; j++) {
			if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
			bBodyHit = CCollisionCircle::HitCheckUseNormal(&pPlayerColliCircle[i], &pEnemyColliCircle[j]);    //�q�b�g�m�F
			if (true == bBodyHit) {
				PlayerColliCircleNumber = i;
				EnemyColliCircleNumber = j;
				break;
			}
		}
		if (true == bBodyHit) break;
	}

	if (true == bBodyHit) {
		//�f�[�^�擾
		D3DXVECTOR3 PlayerHirCirclePos = pPlayerColliCircle[PlayerColliCircleNumber].GetPos();
		D3DXVECTOR3 EnemyHirCirclePos = pEnemyColliCircle[EnemyColliCircleNumber].GetPos();
		float PlayerColliCircleRadius = pPlayerColliCircle[PlayerColliCircleNumber].GetRadius();
		float EnemyColliCircleRadius = pEnemyColliCircle[EnemyColliCircleNumber].GetRadius();

		//�v���[���[�ƓG�̑��x���擾��,���̐ڐ������Ɉړ����x�ɕ�������
		D3DXVECTOR3 PlayerSpeed = m_EnemyOtherEnemy->GetSpeed();
		D3DXVECTOR3 EnemySpeed = m_Enemy->GetSpeed();
		D3DXVECTOR3 PlayerEnemyColliCircleVec = PlayerHirCirclePos - EnemyHirCirclePos;
		PlayerEnemyColliCircleVec.y = 0;
		D3DXMATRIX MtxRot;
		D3DXVECTOR3 TangentVec;									//�ڐ�
		D3DXMatrixRotationY(&MtxRot, D3DX_PI*0.5);
		D3DXVec3TransformCoord(&TangentVec, &PlayerEnemyColliCircleVec, &MtxRot);
		D3DXVec3Normalize(&TangentVec, &TangentVec);			//�P�ʃx�N�g����
		float SpeedPlayer = D3DXVec3Dot(&TangentVec, &PlayerSpeed);
		float SpeedEnemy = D3DXVec3Dot(&TangentVec, &EnemySpeed);

		//�����o���v�Z
		D3DXVECTOR3 PushDir = PlayerHirCirclePos - EnemyHirCirclePos;				//�����o�������ݒ�
		float fDistance = D3DXVec3Length(&PushDir);
		m_CoffiPushOut = max(0.0f, PlayerColliCircleRadius + EnemyColliCircleRadius - fDistance);
		PushDir.y = 0.0f;
		D3DXVec3Normalize(&PushDir, &PushDir);
		m_EnemyOtherEnemy->AddPower(PUSH_OUT_POWER*fDistance*fDistance*PushDir);

		//�Â����W�ɖ߂�
		m_EnemyOtherEnemy->ReturnOldPosXZ();
		m_Enemy->ReturnOldPosXZ();

		//���蔻��~�Ԃ̐ڐ������Ɉړ�������
		m_EnemyOtherEnemy->AddPos(SpeedPlayer*TangentVec);
		m_Enemy->AddPos(SpeedEnemy*TangentVec);
	}
}

void CColliPlayerEnemy::CreateBloodPosition(CCollisionCircle* Attack, CCollisionCircle* Damage)
{
	//��O����
	if (Attack == nullptr || Damage == nullptr) {
		_RPT0(0,"[CCollisionPlayerAndEnemy.cpp][CreateBloodPosition]����Attack��Damage�̓k��\n");
		return;
	}

	for (int i = 0; i < BLOOD_POSITION_MAX; i++) {
		if (m_BloodPosition[i].IsUse == false) {
			m_BloodPosition[i].AttackCircleOldPos = Attack->GetPos();
			m_BloodPosition[i].DamageCircleOldPos = Damage->GetPos();
			m_BloodPosition[i].AttackCircle = Attack;
			m_BloodPosition[i].DamageCircle = Damage;
			m_BloodPosition[i].Time = BLOOD_DEFAULT_CALC_POSITION_TIME;
			m_BloodPosition[i].IsUse = true;
			break;
		}
	}
}

/*test*/
void CColliPlayerEnemy::SetPlayer(CPlayer *Player)
{
	if (nullptr == Player)
	{
		MessageBox(nullptr, "CColliPlayerEnemy�ɓn����CPlayer�|�C���^��nullptr!", "�|�C���^�̒l��nullptr", MB_OK | MB_ICONHAND);
	}
	m_Player = Player;
}

void CColliPlayerEnemy::SetEnemy(CEnemy *Enemy)
{
	if (nullptr == Enemy)
	{
		MessageBox(nullptr, "CColliPlayerEnemy�ɓn����CEnemy�|�C���^��nullptr!", "�|�C���^�̒l��nullptr", MB_OK | MB_ICONHAND);
	}
	m_Enemy = Enemy;
}

void CColliPlayerEnemy::SetEnemyOther(CEnemy *Enemy)
{
	if (nullptr == Enemy)
	{
		MessageBox(nullptr, "CColliPlayerEnemy�ɓn����CEnemy�|�C���^��nullptr!", "�|�C���^�̒l��nullptr", MB_OK | MB_ICONHAND);
	}
	m_EnemyOtherEnemy = Enemy;
}
/*test*/