#include "CEnemyDogWalkState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyControl.h"
#include "CEnemyDogStateManager.h"

#define WALK_POWER (0.02f)									//歩く力
#define DEAD_ZONE_DISTANCE_FROM_PATHPOINT (1.0f)			//巡回ポイントに着く判定距離

CEnemyDogWalkState::CEnemyDogWalkState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	m_IsGetGoal = false;

	//モーション設定
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_MOVE);
	}
}

CEnemyDogWalkState::~CEnemyDogWalkState() 
{
	
}

bool CEnemyDogWalkState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	m_IsGetGoal = false;

	//モーション設定
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_MOVE);
	}

	return true;
}

int CEnemyDogWalkState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//目標地点を取得する
	if (m_IsGetGoal == false) {
		m_GoalPos = FindGoalPosition(pEnemy);
		m_IsGetGoal = true;
	}

	//目標地点に向かって、歩く
	D3DXVECTOR3 Vec3AIToGoalXZ = m_GoalPos - pEnemy->GetPos();
	Vec3AIToGoalXZ.y = 0;					//Y値をカット

	bool IsGoal = D3DXVec3Length(&Vec3AIToGoalXZ) < DEAD_ZONE_DISTANCE_FROM_PATHPOINT;
	if (!IsGoal) {
		D3DXVec3Normalize(&Vec3AIToGoalXZ, &Vec3AIToGoalXZ);
		pEnemy->AddPower(Vec3AIToGoalXZ * WALK_POWER);
		pEnemy->SetGoalDir(Vec3AIToGoalXZ);
	}

	//状態変更
	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogWalkState::Draw(void)
{
	
}

//目標地点を探す
D3DXVECTOR3 CEnemyDogWalkState::FindGoalPosition(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		_RPT0(0,"[CEnemyDogWalkState.cpp][FindGoalPosition]引数pEnemyの値はヌル\n");
		return D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

	//巡回ポイント一個しかない場合
	if (pEnemy->GetPathPointListSize() == 1) {
		auto it = pEnemy->GetPathPointListBegin();
		if ((*it) != nullptr) {
			return (*it)->GetPos();
		}
	}

	//現在の巡回ポイントを取得
	if (pEnemy->GetFlag_IsPathPointIsAscendingOrder() == true) {
		//次の巡回ポイント取得
		auto NextPoint = pEnemy->GetPathPointListNext(pEnemy->GetPathNumberNow(), 1);
		if (NextPoint == pEnemy->GetPathPointListEnd()) {								//要素尾だったら
			NextPoint = pEnemy->GetPathPointListPrev(pEnemy->GetPathNumberNow(), 2);	//リストのEndの1個前のイテレーターに移行
			pEnemy->SetFlag_IsPathPointIsAscendingOrder(false);
		}

		//巡回ポイントなどのデータをpEnemyに保存
		pEnemy->SetPathNumberNow(*NextPoint);
		D3DXVECTOR3 GoalPosition = (*NextPoint)->GetPos();
		return GoalPosition;
	}
	else {
		auto itBegin = pEnemy->GetPathPointListBegin();
		auto NextPoint = itBegin;
		//今のポイントはリストのスタートポイントだったら
		if (*itBegin == pEnemy->GetPathNumberNow()) {
			NextPoint = pEnemy->GetPathPointListNext(*itBegin, 1);
			pEnemy->SetFlag_IsPathPointIsAscendingOrder(true);
		}
		else {
			NextPoint = pEnemy->GetPathPointListPrev(pEnemy->GetPathNumberNow(), 1);
		}

		//巡回ポイントなどのデータをpEnemyに保存
		pEnemy->SetPathNumberNow(*NextPoint);
		D3DXVECTOR3 GoalPosition = (*NextPoint)->GetPos();
		return GoalPosition;
	}
}

bool CEnemyDogWalkState::ChangeState(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//ゴールについたか
	D3DXVECTOR3 Vec3AIToGoalXZ = m_GoalPos - pEnemy->GetPos();
	Vec3AIToGoalXZ.y = 0;
	bool IsGoal = D3DXVec3Length(&Vec3AIToGoalXZ) < DEAD_ZONE_DISTANCE_FROM_PATHPOINT;
	if (IsGoal) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_WAIT_STATE));
			return true;
		}
	}

	return false;
}