#include "CEnemyDogRunState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "CPlayer.h"

#define RUN_POWER (0.10f)							//走る力
#define CAN_JUMP_ATTACK_DISTANCE (20.0f)			//攻撃可能距離

CEnemyDogRunState::CEnemyDogRunState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_IsCanDoJumpAttack = true;

	float DistFromPlayer = CalcDistFromPlayer(pEnemy);

	if (pEnemy != nullptr) {
		if (DistFromPlayer < CAN_JUMP_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_RUN);
		}
		else {
			m_IsCanDoJumpAttack = false;
		}
	}
}

CEnemyDogRunState::~CEnemyDogRunState() 
{
	
}

bool CEnemyDogRunState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_IsCanDoJumpAttack = true;

	float DistFromPlayer = CalcDistFromPlayer(pEnemy);

	if (pEnemy != nullptr) {
		if (DistFromPlayer < CAN_JUMP_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_RUN);
		}
		else {
			m_IsCanDoJumpAttack = false;		//一定距離離れないと、ジャンプ攻撃は不可
		}
	}

	return true;
}

int CEnemyDogRunState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//自分-->プレーヤーの方向ベクトルの計算
	D3DXVECTOR3 DirectionToPlayer = CalcDirectionToPlayer(pEnemy);

	//プレーヤーに向けて走る
	pEnemy->AddPower(DirectionToPlayer * RUN_POWER);

	//ステート変更
	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogRunState::Draw(void)
{
	
}

D3DXVECTOR3 CEnemyDogRunState::CalcDirectionToPlayer(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		_RPT0(0,"[CEnemyDogRunState.cpp][CalcEnemyFaceToPlayerDir]引数pEnemyはヌル\n");
		return D3DXVECTOR3(0,0,0);
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcEnemyFaceToPlayerDir]変数pPlayerはヌル\n");
		return D3DXVECTOR3(0, 0, 0);
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();            //プレーヤー位置取得
	D3DXVECTOR3 Enemy = pEnemy->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}

float CEnemyDogRunState::CalcDistFromPlayer(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcDistFromPlayer]引数pEnemyはヌル\n");
		return -1;
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcDistFromPlayer]変数pPlayerはヌル\n");
		return -1;
	}

	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	return D3DXVec3Length(&(AIPos - PlayerPos));
}

bool CEnemyDogRunState::ChangeState(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//ジャンプ攻撃可能なのかを判定
	if (m_IsCanDoJumpAttack == false) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_BITE_ATTACK_STATE));
			return true;
		}
	}

	else {
		//プレーヤーまでの距離の計算
		float DistFromPlayer = CalcDistFromPlayer(pEnemy);

		//ジャンプ攻撃可能の距離内なら
		if (DistFromPlayer <= CAN_JUMP_ATTACK_DISTANCE) {
			int RandomNum = rand() % 100 + 1;

			//25%の確率でジャンプ攻撃、75%の確率で走って接近し、噛み攻撃をする
			if (RandomNum <= 25) {
				if (m_ParentState != nullptr) {
					m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_JUMPATTACK_STATE));
					return true;
				}
			}
			else {
				if (m_ParentState != nullptr) {
					m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_BITE_ATTACK_STATE));
					return true;
				}
			}
		}
	}

	return false;
}