#include "CEnemyDogRorateToPlayerState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CPlayer.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"

#define CAN_SEE_PLAYER_DISTANCE (50.0f)

CEnemyDogRorateToPlayerState::CEnemyDogRorateToPlayerState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//���[�V�����ݒ�
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
	}
}

CEnemyDogRorateToPlayerState::~CEnemyDogRorateToPlayerState() 
{
	
}

bool CEnemyDogRorateToPlayerState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//���[�V�����ݒ�
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
	}

	return true;
}

int CEnemyDogRorateToPlayerState::Update(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	if (m_EnemyStateManager == nullptr) {
		return ENEMY_NULL_STATE;
	}

	//�q���X�e�[�g�̍X�V
	if (m_ChildState != nullptr) {
		m_TransitionStateNum = m_ChildState->Update(pEnemy);
	}

	//�v���[���[������������
	bool FlagIsMissPlayer = IsMissPlayer(pEnemy);
	if (FlagIsMissPlayer == true) {
		return ENEMY_DOG_PATROL_STATE;
	}

	//�G�̉�]��������A���S�Ƀv���[���[�Ɍ�������A
	bool IsFaceToPlayer = RorateToPlayer(pEnemy);
	if(IsFaceToPlayer == true){
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_RUN_STATE));
			return ENEMY_NULL_STATE;
		}
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogRorateToPlayerState::Draw(void)
{
	
}

bool CEnemyDogRorateToPlayerState::IsMissPlayer(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}
	
	CPlayer* pPlayer= pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		return false;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 Vec3AIToPlayer = PlayerPos - AIPos;
	float Distance = D3DXVec3Length(&Vec3AIToPlayer);
	if (Distance > CAN_SEE_PLAYER_DISTANCE) {
		return true;
	}

	return false;
}

//�v���[���[�̕����Ɍ����ĉ�]����
bool CEnemyDogRorateToPlayerState::RorateToPlayer(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}

	CPlayer *pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		return false;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 MyPos = pEnemy->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - MyPos;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);

	float Angle = acos(D3DXVec3Dot(&MoveDirGoal, &pEnemy->GetFaceDir() ) );

	if (Angle < D3DX_PI*0.1) {
		return true;
	}
	else {
		pEnemy->SetGoalDir(MoveDirGoal);
	}

	return false;
}