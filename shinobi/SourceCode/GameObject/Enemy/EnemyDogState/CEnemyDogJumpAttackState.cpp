#include "CEnemyDogJumpAttackState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "common.h"

#define JUMP_POWER_XZ (1.2f)		//XZ平面のジャンプパワー
#define JUMP_POWER_Y (0.8f)			//Y方向のジャンプパワー
#define ATTACK_RECOVERY_TIME (20)	//硬直時間

CEnemyDogJumpAttackState::CEnemyDogJumpAttackState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_MotionTime = 0;
	m_KeyFrameOld = 0;
	m_AttackRecovery = ATTACK_RECOVERY_TIME;

	//モーション設定
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_JUMPATTACK);
		m_MotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_JUMPATTACK);

		pEnemy->SetAttackType(DOG_JUMP_ATTACK);
	}
}

CEnemyDogJumpAttackState::~CEnemyDogJumpAttackState() 
{
	
}

bool CEnemyDogJumpAttackState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_MotionTime = 0;
	m_KeyFrameOld = 0;
	m_AttackRecovery = ATTACK_RECOVERY_TIME;

	//モーション設定
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_JUMPATTACK);
		m_MotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_JUMPATTACK);

		pEnemy->SetAttackType(DOG_JUMP_ATTACK);
	}

	return true;
}

int CEnemyDogJumpAttackState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	if (m_EnemyStateManager == nullptr) {
		return ENEMY_NULL_STATE;
	}
	
	//モーション時間減少
	m_MotionTime--;

	//ジャンプ攻撃中の具体動作
	AttackDetailAction(pEnemy);

	//状態遷移
	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogJumpAttackState::Draw(void)
{
	
}

void CEnemyDogJumpAttackState::AttackDetailAction(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return;
	}

	int AIKeyFrameNow = pEnemy->GetKeyFrameNow();				//今の進行中のキーフレーム取得
	bool bDiffKeyFrame = m_KeyFrameOld != AIKeyFrameNow;		//前のキーフレームと異なるか

	//キーフレームは2より小さい時、プレーヤーに方に向く
	if (AIKeyFrameNow < 2) {
		pEnemy->SetGoalDir(CalcDirectionToPlayer(pEnemy));
	}

	//キーフレーム変更時の処理
	if (bDiffKeyFrame) {
		m_KeyFrameOld = AIKeyFrameNow;			//前のキーフレームに今のキーフレームを代入

		if (AIKeyFrameNow == 2) {
			pEnemy->SetCoffi(0.05f);
			pEnemy->AddPower(CalcDirectionToPlayer(pEnemy) * JUMP_POWER_XZ);
			pEnemy->AddPower(D3DXVECTOR3(0.0f, JUMP_POWER_Y, 0.0f));
		}
	}

	//攻撃判定の有効/無効の切り替え
	bool HitColliEnable = (AIKeyFrameNow != 0) && (AIKeyFrameNow != 1) && (AIKeyFrameNow != 5);
	if (HitColliEnable) {
		pEnemy->EnableHitCircle();		//攻撃判定球を有効に
	}
	else {
		pEnemy->DisableHitCircle();		//攻撃判定球を無効に
	}
}

//自分->プレーヤーの方向ベクトルの計算
D3DXVECTOR3 CEnemyDogJumpAttackState::CalcDirectionToPlayer(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		_RPT0(0, "[CEnemyDogJumpAttackState.cpp][CalcEnemyFaceToPlayerDir]引数pEnemyはヌル\n");
		return D3DXVECTOR3(0, 0, 0);
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogJumpAttackState.cpp][CalcEnemyFaceToPlayerDir]変数pPlayerはヌル\n");
		return D3DXVECTOR3(0, 0, 0);
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();            //プレーヤー位置取得
	D3DXVECTOR3 Enemy = pEnemy->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}

bool CEnemyDogJumpAttackState::ChangeState(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//「プレーヤーに向く」ステートに遷移
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_JUMPATTACK) {
		if (m_MotionTime <= 0) {
			if (m_ParentState != nullptr) {
				pEnemy->SetCoffi(0.2f);
				pEnemy->DisableHitCircle();
				pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
			}
			return false;
		}
	}
	
	//ニュートラルモーション時硬直発生
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_NEUTRAL) {
		m_AttackRecovery--;

		if (m_AttackRecovery <= 0) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_ATTACK_RECOVERY_STATE));
			return true;
		}
	}

	return false;
}