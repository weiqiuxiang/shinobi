#include "CEnemyDogStunState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "common.h"
#include "CEnemyControl.h"

#define STUN_COFFI (0.4f)	//スタン時の抵抗係数

CEnemyDogStunState::CEnemyDogStunState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//スタンパラメータ設定
	StunSetting(pEnemy);

	//モーション設定
	if (pEnemy != nullptr) {
		pEnemy->DisableHitCircle();
		pEnemy->SetCoffi(STUN_COFFI);
		pEnemy->SetMotionMode(CEnemyControl::MOTION_STUN);
		m_StunMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_STUN);
	}
}

CEnemyDogStunState::~CEnemyDogStunState() 
{
	
}

bool CEnemyDogStunState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//スタンパラメータ設定
	StunSetting(pEnemy);
	
	if (pEnemy != nullptr) {
		pEnemy->DisableHitCircle();
		pEnemy->SetCoffi(STUN_COFFI);
		pEnemy->SetMotionMode(CEnemyControl::MOTION_STUN);
		m_StunMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_STUN);
	}

	return true;
}

int CEnemyDogStunState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	if (m_EnemyStateManager == nullptr) {
		return ENEMY_NULL_STATE;
	}
	
	//例外処理
	if (pEnemy->GetDamageType() == PLAYER_ATTACK_TYPE_NONE) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_PATROL_STATE));
			return ENEMY_NULL_STATE;
		}
	}

	//モーション時間減少
	if (m_StunMotionTime >= 0) {
		m_StunMotionTime--;
	}
	else {
		m_DecoveryTime--;
	}

	//状態遷移,lifeは0だったら死亡状態に遷移、lifeは0以上だったら攻撃状態に遷移
	if (m_DecoveryTime < 0) {
		if (pEnemy->GetLife() <= 0) {
			if (m_ParentState != nullptr) {
				m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_DEAD_STATE));
				return ENEMY_NULL_STATE;
			}
		}
		else {
			if (m_ParentState != nullptr) {
				pEnemy->SetCoffi(0.2f);
				m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_ATTACK_MODE_STATE));
				return ENEMY_NULL_STATE;
			}
		}
	}

	if (pEnemy->GetIsBeAttackedFlag() == true) {
		if (pEnemy->GetNoDamageTime() <= 0) {
			if (m_ParentState != nullptr) {
				pEnemy->SetIsBeAttackedFlag(false);
				pEnemy->SetNoDamageTime(3);
				m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_STUN_STATE));
				return true;
			}
		}
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogStunState::Draw(void)
{
	
}

void CEnemyDogStunState::StunSetting(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return;
	}
	if (m_EnemyStateManager == nullptr) {
		return;
	}

	//スタン方向取得
	D3DXVECTOR3 StunDir = pEnemy->GetStunDir();

	//プレーヤーの攻撃タイプに応じてダメージを受け、一定距離に飛ばされる
	switch (pEnemy->GetDamageType()) 
	{
	case PLAYER_ATTACK_TYPE_NONE:
		break;
	case WSWORD_ATTACK_01:
		pEnemy->AddPower(0.8f * StunDir);		//スタン移動パワー設定
		pEnemy->SetNoDamageTime(3);				//無敵時間設定
		pEnemy->AddLife(-20);					//HP減少
		m_DecoveryTime = 10;					//スタン時間設定
		break;
	case WSWORD_ATTACK_02:
		pEnemy->AddPower(0.8f * StunDir);		//スタン移動パワー設定
		pEnemy->SetNoDamageTime(3);				//無敵時間設定
		pEnemy->AddLife(-20);					//HP減少
		m_DecoveryTime = 10;					//スタン時間設定
		break;
	case WSWORD_ATTACK_03:
		pEnemy->AddPower(0.8f * StunDir);		//スタン移動パワー設定
		pEnemy->SetNoDamageTime(3);				//無敵時間設定
		pEnemy->AddLife(-20);					//HP減少
		m_DecoveryTime = 10;					//スタン時間設定
		break;
	case WSWORD_ATTACK_04:
		pEnemy->AddPower(0.8f * StunDir);		//スタン移動パワー設定
		pEnemy->SetNoDamageTime(3);				//無敵時間設定
		pEnemy->AddLife(-20);					//HP減少
		m_DecoveryTime = 10;					//スタン時間設定
		break;
	case CLAYMORE_ATTACK_01:
		pEnemy->AddPower(2.5f * StunDir);		//スタン移動パワー設定
		pEnemy->SetNoDamageTime(6);				//無敵時間設定
		pEnemy->AddLife(-30);					//HP減少
		m_DecoveryTime = 20;					//スタン時間設定
		break;
	case CLAYMORE_ATTACK_02:
		pEnemy->AddPower(2.5f * StunDir);		//スタン移動パワー設定
		pEnemy->SetNoDamageTime(6);			//無敵時間設定
		pEnemy->AddLife(-40);					//HP減少
		m_DecoveryTime = 20;					//スタン時間設定
		break;
	case CLAYMORE_ATTACK_03:
		pEnemy->AddPower(2.5f * StunDir);		//スタン移動パワー設定
		pEnemy->SetNoDamageTime(6);			//無敵時間設定
		pEnemy->AddLife(-50);					//HP減少
		m_DecoveryTime = 20;					//スタン時間設定
		break;
	default:
		break;
	}
}