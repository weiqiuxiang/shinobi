#include "CEnemyDogPatrolState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyDogStateManager.h"
#include "CPlayer.h"

//マクロ
#define EYE_HALF_HORIZON_ANGLE (D3DX_PI * 0.25)		//視覚の半分角度(水平)
#define EYE_HALF_VERTICAL_ANGLE (D3DX_PI * 0.25)	//視覚の半分角度(垂直)
#define EYE_DISTANCE (50.0f)						//視覚の距離
#define LISTEN_DISTANCE (20.0f)						//聞こえる範囲内の距離、その距離内でプレーヤーの走る、攻撃などの動作の時プレーヤーを発見

CEnemyDogPatrolState::CEnemyDogPatrolState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//子とものデフォルトステート設定
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

	//子とものデフォルトステート設定
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_WALK_STATE));
	}

	return true;
}

int CEnemyDogPatrolState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	
	//自分の子ステートを更新する
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

//目探知
//戻り値 : false : プレーヤー未発見      true : プレーヤー発見
bool CEnemyDogPatrolState::EyeSence(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}

	//プレーヤーポインタ取得
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		return false;
	}

	//見える距離内プレーヤーいるかを判定
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 Vec3AIToPlayer = PlayerPos - AIPos;
	float Distance = D3DXVec3Length(&Vec3AIToPlayer);
	if (Distance > EYE_DISTANCE) {
		return false;
	}

	//視野角の範囲内にいるかを判定
	D3DXVec3Normalize(&Vec3AIToPlayer, &Vec3AIToPlayer);	//方向ベクトル単位化
	D3DXVECTOR3 AIFaceDir = pEnemy->GetFaceDir();			//AIの向きベクトル
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

//耳探知
//戻り値 : false : プレーヤー未発見      true : プレーヤー発見
bool CEnemyDogPatrolState::EarSence(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}

	//プレーヤーポインタ取得
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		return false;
	}

	//聞こえる範囲内にいるか
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 Vec3AIToPlayer = PlayerPos - AIPos;
	float Distance = D3DXVec3Length(&Vec3AIToPlayer);
	if (Distance <= LISTEN_DISTANCE) {	//聞こえる範囲外なら
		return true;
	}

	return false;
}

bool CEnemyDogPatrolState::ChangeState(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//ダメージ受けるとスタン状態に遷移
	if (pEnemy->GetIsBeAttackedFlag() == true) {
		if (m_ParentState != nullptr) {
			pEnemy->SetIsBeAttackedFlag(false);
			pEnemy->SetNoDamageTime(3);
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_STUN_STATE));
			return true;
		}
	}

	//プレーヤー探知部分
	bool IsSeePlayer = EyeSence(pEnemy);
	bool IsHearPlayer = EarSence(pEnemy);
	bool IsFindPlayer = IsSeePlayer || IsHearPlayer;

	//プレーヤー探知できたら、戦闘ステートに遷移
	if (IsFindPlayer == true) {
		if (m_ParentState != nullptr) {
			CEnemyState* pState = m_EnemyStateManager->GetState(ENEMY_DOG_ATTACK_MODE_STATE);
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, pState);
			return true;
		}
	}

	//子供の帰り値で遷移
	if (m_TransitionStateNum != ENEMY_NULL_STATE) {
		if (m_ParentState != nullptr) {
			CEnemyState* pState = m_EnemyStateManager->GetState(m_TransitionStateNum);
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, pState);
			return true;
		}
	}

	return false;
}