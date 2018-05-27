#include "CEnemyDogPatrolState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyDogStateManager.h"
#include "CPlayer.h"

//�}�N��
#define EYE_HALF_HORIZON_ANGLE (D3DX_PI * 0.25)		//���o�̔����p�x(����)
#define EYE_HALF_VERTICAL_ANGLE (D3DX_PI * 0.25)	//���o�̔����p�x(����)
#define EYE_DISTANCE (50.0f)						//���o�̋���
#define LISTEN_DISTANCE (20.0f)						//��������͈͓��̋����A���̋������Ńv���[���[�̑���A�U���Ȃǂ̓���̎��v���[���[�𔭌�

CEnemyDogPatrolState::CEnemyDogPatrolState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//�q�Ƃ��̃f�t�H���g�X�e�[�g�ݒ�
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_WAIT_STATE));
	}
}

CEnemyDogPatrolState::~CEnemyDogPatrolState() 
{
	
}

bool CEnemyDogPatrolState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//�q�Ƃ��̃f�t�H���g�X�e�[�g�ݒ�
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_WALK_STATE));
	}

	return true;
}

int CEnemyDogPatrolState::Update(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	
	//�����̎q�X�e�[�g���X�V����
	if (m_ChildState != nullptr) {
		m_TransitionStateNum = m_ChildState->Update(pEnemy);
	}

	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogPatrolState::Draw(void){
}

//�ڒT�m
//�߂�l : false : �v���[���[������      true : �v���[���[����
bool CEnemyDogPatrolState::EyeSence(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}

	//�v���[���[�|�C���^�擾
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		return false;
	}

	//�����鋗�����v���[���[���邩�𔻒�
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 Vec3AIToPlayer = PlayerPos - AIPos;
	float Distance = D3DXVec3Length(&Vec3AIToPlayer);
	if (Distance > EYE_DISTANCE) {
		return false;
	}

	//����p�͈͓̔��ɂ��邩�𔻒�
	D3DXVec3Normalize(&Vec3AIToPlayer, &Vec3AIToPlayer);	//�����x�N�g���P�ʉ�
	D3DXVECTOR3 AIFaceDir = pEnemy->GetFaceDir();			//AI�̌����x�N�g��
	D3DXVECTOR3 AIFaceDirXZ = D3DXVECTOR3(AIFaceDir.x, 0.0f, AIFaceDir.z);
	D3DXVECTOR3 Vec3AIToPlayerXZ = D3DXVECTOR3(Vec3AIToPlayer.x, 0.0f, Vec3AIToPlayer.z);
	D3DXVec3Normalize(&AIFaceDirXZ, &AIFaceDirXZ);
	D3DXVec3Normalize(&Vec3AIToPlayerXZ, &Vec3AIToPlayerXZ);
	float SabunHori = acos(
		D3DXVec3Dot(&AIFaceDirXZ, &Vec3AIToPlayerXZ)
	);

	bool IsPlayerInHorizonAngle = SabunHori <= EYE_HALF_HORIZON_ANGLE;

	float SabunVerti = fabs(asin(AIFaceDir.y) - asin(Vec3AIToPlayer.y));
	bool IsPlayerInVerticalAngle = SabunVerti <= EYE_HALF_VERTICAL_ANGLE;

	if (IsPlayerInHorizonAngle && IsPlayerInVerticalAngle) {
		return true;
	}

	return false;
}

//���T�m
//�߂�l : false : �v���[���[������      true : �v���[���[����
bool CEnemyDogPatrolState::EarSence(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}

	//�v���[���[�|�C���^�擾
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		return false;
	}

	//��������͈͓��ɂ��邩
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 Vec3AIToPlayer = PlayerPos - AIPos;
	float Distance = D3DXVec3Length(&Vec3AIToPlayer);
	if (Distance <= LISTEN_DISTANCE) {	//��������͈͊O�Ȃ�
		return true;
	}

	return false;
}

bool CEnemyDogPatrolState::ChangeState(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//�_���[�W�󂯂�ƃX�^����ԂɑJ��
	if (pEnemy->GetIsBeAttackedFlag() == true) {
		if (m_ParentState != nullptr) {
			pEnemy->SetIsBeAttackedFlag(false);
			pEnemy->SetNoDamageTime(3);
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_STUN_STATE));
			return true;
		}
	}

	//�v���[���[�T�m����
	bool IsSeePlayer = EyeSence(pEnemy);
	bool IsHearPlayer = EarSence(pEnemy);
	bool IsFindPlayer = IsSeePlayer || IsHearPlayer;

	//�v���[���[�T�m�ł�����A�퓬�X�e�[�g�ɑJ��
	if (IsFindPlayer == true) {
		if (m_ParentState != nullptr) {
			CEnemyState* pState = m_EnemyStateManager->GetState(ENEMY_DOG_ATTACK_MODE_STATE);
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, pState);
			return true;
		}
	}

	//�q���̋A��l�őJ��
	if (m_TransitionStateNum != ENEMY_NULL_STATE) {
		if (m_ParentState != nullptr) {
			CEnemyState* pState = m_EnemyStateManager->GetState(m_TransitionStateNum);
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, pState);
			return true;
		}
	}

	return false;
}