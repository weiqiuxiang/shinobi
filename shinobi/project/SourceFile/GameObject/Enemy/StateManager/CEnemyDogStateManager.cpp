#include <algorithm>
#include "CEnemyDogStateManager.h"
#include "CEnemyState.h"
#include "CEnemyDogAttackModeState.h"
#include "CEnemyDogBiteAttackState.h"
#include "CEnemyDogDeadState.h"
#include "CEnemyDogRorateToPlayerState.h"
#include "CEnemyDogJumpAttackState.h"
#include "CEnemyDogPatrolState.h"
#include "CEnemyDogRunState.h"
#include "CEnemyDogStunState.h"
#include "CEnemyDogTopState.h"
#include "CEnemyDogWaitState.h"
#include "CEnemyDogWalkState.h"
#include "CEnemyDogAttackRecoveryState.h"

CEnemyDogStateManager::CEnemyDogStateManager()
{
	for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
		m_StateArray[i] = nullptr;
	}
}

CEnemyDogStateManager::~CEnemyDogStateManager() 
{
	
}

//すべてのパターンインスタンス化
bool CEnemyDogStateManager::InitAllInstance(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}

	//配列のクリア
	int ArrayNum = m_StateArray.size();
	if(ArrayNum != 0){
		for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
			if (m_StateArray[i] != nullptr) {
				delete m_StateArray[i];
				m_StateArray[i] = nullptr;
			}
		}
	}

	CEnemyState* pEnemyState = nullptr;

	//topステート初期化
	pEnemyState = new CEnemyDogTopState(pEnemy,this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_TOP_STATE] = pEnemyState;

	//attackステート初期化
	pEnemyState = new CEnemyDogAttackModeState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE] = pEnemyState;

	//biteAttackステート初期化
	pEnemyState = new CEnemyDogBiteAttackState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_BITE_ATTACK_STATE] = pEnemyState;

	//deadステート初期化
	pEnemyState = new CEnemyDogDeadState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_DEAD_STATE] = pEnemyState;

	//RorateToPlayerステート初期化
	pEnemyState = new CEnemyDogRorateToPlayerState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_RORATE_TO_PLAYER_STATE] = pEnemyState;

	//JumpAttackステート初期化
	pEnemyState = new CEnemyDogJumpAttackState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_JUMPATTACK_STATE] = pEnemyState;

	//AttackRecoveryステート初期化
	pEnemyState = new CEnemyDogAttackRecoveryState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_ATTACK_RECOVERY_STATE] = pEnemyState;
	
	//Patrolステート初期化
	pEnemyState = new CEnemyDogPatrolState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_PATROL_STATE] = pEnemyState;

	//Runステート初期化
	pEnemyState = new CEnemyDogRunState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_RUN_STATE] = pEnemyState;

	//Stunステート初期化
	pEnemyState = new CEnemyDogStunState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_STUN_STATE] = pEnemyState;

	//Waitステート初期化
	pEnemyState = new CEnemyDogWaitState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_WAIT_STATE] = pEnemyState;

	//Walkステート初期化
	pEnemyState = new CEnemyDogWalkState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_WALK_STATE] = pEnemyState;

	//全パターン初期化する
	for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
		if (m_StateArray[i] != nullptr) {
			m_StateArray[i]->InitParameter(pEnemy,this);
		}
	}

	//親子関係を結びつく
	LinkParentAndChild();

	return true;
}

CEnemyState* CEnemyDogStateManager::GetState(int PatternNum)
{
	if(PatternNum < 0 || PatternNum >= ENEMY_DOG_STATE_MAX){
		_RPT0(0,"[CEnemyDogStateManager.cpp][GetPattern]引数PatternNumの値は範囲外\n");
		return nullptr;
	}

	return m_StateArray[PatternNum];
}

void CEnemyDogStateManager::UninitAllInstance(void)
{
	int ArrayNum = m_StateArray.size();
	if (ArrayNum != 0) {
		for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
			if (m_StateArray[i] != nullptr) {
				delete m_StateArray[i];
				m_StateArray[i] = nullptr;
			}
		}
	}
}

void CEnemyDogStateManager::LinkParentAndChild(void)
{
	//「巡回」ステートの親ステートは「TOP」ステート
	if (m_StateArray[ENEMY_DOG_PATROL_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_PATROL_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//「攻撃モード」ステートの親ステートは「TOP」ステート
	if (m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//「スタン」ステートの親ステートは「TOP」ステート
	if (m_StateArray[ENEMY_DOG_STUN_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_STUN_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//「死亡」ステートの親ステートは「TOP」ステート
	if (m_StateArray[ENEMY_DOG_DEAD_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_DEAD_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//「待つ」ステートの親ステートは「巡回」ステート
	if (m_StateArray[ENEMY_DOG_WAIT_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_WAIT_STATE]->SetParentState(m_StateArray[ENEMY_DOG_PATROL_STATE]);
	}

	//「移動」ステートの親ステートは「巡回」ステート
	if (m_StateArray[ENEMY_DOG_WALK_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_WALK_STATE]->SetParentState(m_StateArray[ENEMY_DOG_PATROL_STATE]);
	}

	//「プレーヤーに向く」ステートの親ステートは「攻撃モード」ステート
	if (m_StateArray[ENEMY_DOG_RORATE_TO_PLAYER_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_RORATE_TO_PLAYER_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//「走る」ステートの親ステートは「攻撃モード」ステート
	if (m_StateArray[ENEMY_DOG_RUN_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_RUN_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//「噛み攻撃」ステートの親ステートは「攻撃モード」ステート
	if (m_StateArray[ENEMY_DOG_BITE_ATTACK_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_BITE_ATTACK_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//「ジャンプ攻撃」ステートの親ステートは「攻撃モード」ステート
	if (m_StateArray[ENEMY_DOG_JUMPATTACK_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_JUMPATTACK_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//「攻撃硬直」ステートの親ステートは「攻撃モード」ステート
	if (m_StateArray[ENEMY_DOG_ATTACK_RECOVERY_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_ATTACK_RECOVERY_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}
}