#include "CCollisionPlayerAndEnemy.h"
#include "CCollisionMath.h"
#include "CCollisionCircle.h"
#include "CBloodStorage.h"
#include "CGame.h"
#include <crtdbg.h>

//マクロ
#define POWER_BODYHIT (0.8f)
#define PUSH_OUT_POWER (0.05f)					//押し出す力

#define TEX_BLOOD_FILE_PASS "data/TEXTURE/DamageLifeColor.png"		//血のパーティクルのテクスチャのファイルパス
#define BLOOD_PIECE_AMOUNT (5)
#define BLOOD_DEFAULT_CALC_POSITION_TIME (4)		//攻撃ヒットから血しぶきの計算フレーム数
#define BLOOD_POS_DIST_FROM_DAMAGE_CIRCLE (0.5f)	//血発射の位置はダメージ球から離れる距離
#define BLOOD_SHOT_POWER (0.5f)						//血発射の時の力

//パーティクル発射角度の重さ
#define WEIGHT_ATTACK_CIRCLE_DIR (2.0f)		//攻撃球の移動方向の重さ
#define WEIGHT_OTHER (0.3f)					//他の移動方向の重さ(その和はWEIGHT_OTHERの値)

//グローバル変数
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

//血しぶきの生成とその位置の計算
void CColliPlayerEnemy::UpdateBlood(void)
{
	for (int i = 0; i < BLOOD_POSITION_MAX; i++) {
		//使用チェック
		if (m_BloodPosition[i].IsUse == false) {
			return;
		}
		//例外処理
		if (m_BloodPosition[i].AttackCircle == nullptr || m_BloodPosition[i].DamageCircle == nullptr) {
			continue;
		}

		D3DXVECTOR3 AttackDir = m_BloodPosition[i].AttackCircle->GetPos() - m_BloodPosition[i].AttackCircleOldPos;
		D3DXVECTOR3 DamageDir = m_BloodPosition[i].DamageCircle->GetPos() - m_BloodPosition[i].DamageCircleOldPos;
		//ベクトルの長さを求める
		float AttackLength = D3DXVec3Length(&AttackDir);
		float DamageLength = D3DXVec3Length(&DamageDir);
		//ベクトルの方向を求める
		if (AttackDir != D3DXVECTOR3(0, 0, 0)) {
			D3DXVec3Normalize(&AttackDir, &AttackDir);
		}
		if (DamageDir != D3DXVECTOR3(0, 0, 0)) {
			D3DXVec3Normalize(&DamageDir, &DamageDir);
		}

		//軌跡に沿って、等間隔で(BLOOD_PIECE_AMOUNT + 1)個点の位置ででパーティクルを出す
		for (int PieceNum = 0; PieceNum <= BLOOD_PIECE_AMOUNT; PieceNum++) {
			//パーティクル位置までの長さ計算
			float AttackToPosLength = AttackLength * (float)PieceNum / BLOOD_PIECE_AMOUNT;
			float DamageToPosLength = DamageLength * (float)PieceNum / BLOOD_PIECE_AMOUNT;

			//ダメージと攻撃球の位置計算
			D3DXVECTOR3 AttackCirclePos = m_BloodPosition[i].AttackCircleOldPos + AttackToPosLength * AttackDir;
			D3DXVECTOR3 DamageCirclePos = m_BloodPosition[i].DamageCircleOldPos + DamageToPosLength * DamageDir;

			//ダメージ球->攻撃球のベクトル
			D3DXVECTOR3 DamageAttackVec = AttackCirclePos - DamageCirclePos;
			D3DXVec3Normalize(&DamageAttackVec, &DamageAttackVec);

			//パーティクル位置計算
			D3DXVECTOR3 BloodPosition = DamageCirclePos;

			//DamageAttackVecとAttackDirに垂直ベクトルを求め
			D3DXVECTOR3 YDir = D3DXVECTOR3(0, 0, 0);
			D3DXVec3Cross(&YDir, &DamageAttackVec, &AttackDir);
			D3DXVec3Normalize(&YDir, &YDir);

			//発射角度を算出
			float PercentWeight = (float)(rand() % 101);
			D3DXVECTOR3 ShootVec = AttackDir * WEIGHT_ATTACK_CIRCLE_DIR - DamageAttackVec * WEIGHT_OTHER * PercentWeight + YDir * WEIGHT_OTHER * (1.0f - PercentWeight);
			D3DXVec3Normalize(&ShootVec, &ShootVec);

			//血パーティクル生成
			CBloodStorage::CreateObject(BloodPosition, D3DXVECTOR3(0.2f, 0.2f, 0.0f), ShootVec * BLOOD_SHOT_POWER, 60, 0.1f, TEX_BLOOD_FILE_PASS);
		}

		//座標上書き
		m_BloodPosition[i].AttackCircleOldPos = m_BloodPosition[i].AttackCircle->GetPos();
		m_BloodPosition[i].DamageCircleOldPos = m_BloodPosition[i].DamageCircle->GetPos();

		//計算回数カウントダウン
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

	//ヒット判定
	bool bBodyHit = false;

	for(int i = 0; i < CircleMax;i++){
		if (pPlayerColliCircle[i].GetType() != CCollisionCircle::TYPE_BODY) continue;

		//敵との体の当り判定
		for(int j = 0; j < CircleMax; j++){
			if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
			bBodyHit = CCollisionCircle::HitCheckUseNormal(&pPlayerColliCircle[i], &pEnemyColliCircle[j]);    //ヒット確認
			if (true == bBodyHit) {
				PlayerColliCircleNumber = i;
				EnemyColliCircleNumber = j;
				break;
			}
		}
		if (true == bBodyHit) break;
	}
	
	if(true == bBodyHit){
		//データ取得
		D3DXVECTOR3 PlayerHirCirclePos = pPlayerColliCircle[PlayerColliCircleNumber].GetPos();
		D3DXVECTOR3 EnemyHirCirclePos = pEnemyColliCircle[EnemyColliCircleNumber].GetPos();
		float PlayerColliCircleRadius = pPlayerColliCircle[PlayerColliCircleNumber].GetRadius();
		float EnemyColliCircleRadius = pEnemyColliCircle[EnemyColliCircleNumber].GetRadius();

		//プレーヤーと敵の速度を取得し,球の接線方向に移動速度に分解する
		D3DXVECTOR3 PlayerSpeed = m_Player->GetSpeed();
		D3DXVECTOR3 EnemySpeed = m_Enemy->GetSpeed();
		D3DXVECTOR3 PlayerEnemyColliCircleVec = PlayerHirCirclePos - EnemyHirCirclePos;
		PlayerEnemyColliCircleVec.y = 0;
		D3DXMATRIX MtxRot;
		D3DXVECTOR3 TangentVec;									//接線
		D3DXMatrixRotationY(&MtxRot, D3DX_PI*0.5);
		D3DXVec3TransformCoord(&TangentVec, &PlayerEnemyColliCircleVec,&MtxRot);
		D3DXVec3Normalize(&TangentVec, &TangentVec);			//単位ベクトル化
		float SpeedPlayer = D3DXVec3Dot(&TangentVec,&PlayerSpeed);
		float SpeedEnemy = D3DXVec3Dot(&TangentVec, &EnemySpeed);

		//押し出す計算
		D3DXVECTOR3 PushDir = PlayerHirCirclePos - EnemyHirCirclePos;				//押し出す方向設定
		float fDistance = D3DXVec3Length(&PushDir);
		m_CoffiPushOut = max(0.0f, PlayerColliCircleRadius + EnemyColliCircleRadius - fDistance);
		PushDir.y = 0.0f;
		D3DXVec3Normalize(&PushDir, &PushDir);
		m_Player->AddPower(PUSH_OUT_POWER*fDistance*fDistance*PushDir);

		//古い座標に戻る
		m_Player->ReturnOldPosXZ();
		m_Enemy->ReturnOldPosXZ();

		//当り判定円間の接線方向に移動させる
		m_Player->AddPos(SpeedPlayer*TangentVec);
		m_Enemy->AddPos(SpeedEnemy*TangentVec);
	}
}

void CColliPlayerEnemy::PlayerHitEnemy(void)
{
	if (nullptr == m_Player) return;
	if (nullptr == m_Enemy) return;

	//無敵状態
	if (m_Enemy->GetNoDamageTime() > 0) {
		return;
	}

	if (m_Enemy->GetDeadFlag() == true) return;

	CCollisionCircle* pPlayerWeaponLeftColliCircle = m_Player->GetWeaponLeftColli(0);
	CCollisionCircle* pPlayerWeaponRightColliCircle = m_Player->GetWeaponRightColli(0);
	CCollisionCircle* pEnemyColliCircle = m_Enemy->GetColli(0);
	int CircleMaxWeapon01 = m_Player->GetWeaponLeftMaxColliCircle();
	int CircleMaxWeapon02 = m_Player->GetWeaponRightMaxColliCircle();

	//ヒット判定
	bool bBodyHit = false;

	//武器01の
	if (pPlayerWeaponLeftColliCircle != nullptr) {
		for (int i = 0; i < CircleMaxWeapon01; i++) {
			if (pPlayerWeaponLeftColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;
			if (false) continue;
			//敵との体の当り判定
			for (int j = 0; j < CircleMaxWeapon01; j++) {
				if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
				bBodyHit = CCollisionCircle::HitCheckUseSSV(&pPlayerWeaponLeftColliCircle[i], &pEnemyColliCircle[j]);    //ヒット確認
				if (true == bBodyHit) {
					CreateBloodPosition(&pPlayerWeaponLeftColliCircle[i], &pEnemyColliCircle[j]);
					break;
				}
			}
			if (true == bBodyHit) break;
		}
	}

	//武器02の
	if (bBodyHit == false) {		//武器01は当たらなげれば実行
		if (pPlayerWeaponRightColliCircle != nullptr) {
			for (int i = 0; i < CircleMaxWeapon02; i++) {
				if (pPlayerWeaponRightColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;

				//敵との体の当り判定
				for (int j = 0; j < CircleMaxWeapon02; j++) {
					if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
					bBodyHit = CCollisionCircle::HitCheckUseSSV(&pPlayerWeaponRightColliCircle[i], &pEnemyColliCircle[j]);    //ヒット確認
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

		m_Enemy->SetDamageType(m_Player->GetAttackType());			//プレーヤーの攻撃タイプ取得
		m_Enemy->SetStunDir(-EnemyPlayerVec);						//スタン方向設定
		m_Enemy->SetIsBeAttackedFlag(true);							//攻撃されたフラグを設定
	}
}

void CColliPlayerEnemy::EnemyHitPlayer(void)
{
	if (nullptr == m_Player) return;
	if (nullptr == m_Enemy) return;

	//無敵状態
	if (m_Player->GetNoDamageTime() > 0) {
		return;
	}

	if (m_Enemy->GetDeadFlag() == true) return;

	CCollisionCircle* pPlayerColliCircle = m_Player->GetColli(0);
	CCollisionCircle* pEnemyColliCircle = m_Enemy->GetColli(0);
	int CircleMax = m_Player->GetMaxColliCircleNum();

	//ヒット判定
	bool bBodyHit = false;
	for (int i = 0; i < CircleMax; i++) {
		if (pEnemyColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;

		//敵との体の当り判定
		for (int j = 0; j < CircleMax; j++) {
			if (pPlayerColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
			bBodyHit = CCollisionCircle::HitCheckUseSSV(&pPlayerColliCircle[j], &pEnemyColliCircle[i]);    //ヒット確認
			if(bBodyHit == false) bBodyHit = CCollisionCircle::HitCheckUseNormal(&pPlayerColliCircle[j], &pEnemyColliCircle[i]);    //ヒット確認
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

		m_Player->SetDamageType(m_Enemy->GetAttackType());			//敵の攻撃タイプ取得
		m_Player->SetStunDir(EnemyPlayerVec);						//スタン方向設定
		m_Player->SetIsBeAttackedFlag(true);						//攻撃されたフラグを設定
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

	//ヒット判定
	bool bBodyHit = false;
	for (int i = 0; i < CircleMax; i++) {
		if (pPlayerColliCircle[i].GetType() != CCollisionCircle::TYPE_BODY) continue;

		//敵との体の当り判定
		for (int j = 0; j < CircleMax; j++) {
			if (pEnemyColliCircle[j].GetType() != CCollisionCircle::TYPE_BODY) continue;
			bBodyHit = CCollisionCircle::HitCheckUseNormal(&pPlayerColliCircle[i], &pEnemyColliCircle[j]);    //ヒット確認
			if (true == bBodyHit) {
				PlayerColliCircleNumber = i;
				EnemyColliCircleNumber = j;
				break;
			}
		}
		if (true == bBodyHit) break;
	}

	if (true == bBodyHit) {
		//データ取得
		D3DXVECTOR3 PlayerHirCirclePos = pPlayerColliCircle[PlayerColliCircleNumber].GetPos();
		D3DXVECTOR3 EnemyHirCirclePos = pEnemyColliCircle[EnemyColliCircleNumber].GetPos();
		float PlayerColliCircleRadius = pPlayerColliCircle[PlayerColliCircleNumber].GetRadius();
		float EnemyColliCircleRadius = pEnemyColliCircle[EnemyColliCircleNumber].GetRadius();

		//プレーヤーと敵の速度を取得し,球の接線方向に移動速度に分解する
		D3DXVECTOR3 PlayerSpeed = m_EnemyOtherEnemy->GetSpeed();
		D3DXVECTOR3 EnemySpeed = m_Enemy->GetSpeed();
		D3DXVECTOR3 PlayerEnemyColliCircleVec = PlayerHirCirclePos - EnemyHirCirclePos;
		PlayerEnemyColliCircleVec.y = 0;
		D3DXMATRIX MtxRot;
		D3DXVECTOR3 TangentVec;									//接線
		D3DXMatrixRotationY(&MtxRot, D3DX_PI*0.5);
		D3DXVec3TransformCoord(&TangentVec, &PlayerEnemyColliCircleVec, &MtxRot);
		D3DXVec3Normalize(&TangentVec, &TangentVec);			//単位ベクトル化
		float SpeedPlayer = D3DXVec3Dot(&TangentVec, &PlayerSpeed);
		float SpeedEnemy = D3DXVec3Dot(&TangentVec, &EnemySpeed);

		//押し出す計算
		D3DXVECTOR3 PushDir = PlayerHirCirclePos - EnemyHirCirclePos;				//押し出す方向設定
		float fDistance = D3DXVec3Length(&PushDir);
		m_CoffiPushOut = max(0.0f, PlayerColliCircleRadius + EnemyColliCircleRadius - fDistance);
		PushDir.y = 0.0f;
		D3DXVec3Normalize(&PushDir, &PushDir);
		m_EnemyOtherEnemy->AddPower(PUSH_OUT_POWER*fDistance*fDistance*PushDir);

		//古い座標に戻る
		m_EnemyOtherEnemy->ReturnOldPosXZ();
		m_Enemy->ReturnOldPosXZ();

		//当り判定円間の接線方向に移動させる
		m_EnemyOtherEnemy->AddPos(SpeedPlayer*TangentVec);
		m_Enemy->AddPos(SpeedEnemy*TangentVec);
	}
}

void CColliPlayerEnemy::CreateBloodPosition(CCollisionCircle* Attack, CCollisionCircle* Damage)
{
	//例外処理
	if (Attack == nullptr || Damage == nullptr) {
		_RPT0(0,"[CCollisionPlayerAndEnemy.cpp][CreateBloodPosition]引数AttackかDamageはヌル\n");
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
		MessageBox(nullptr, "CColliPlayerEnemyに渡したCPlayerポインタはnullptr!", "ポインタの値はnullptr", MB_OK | MB_ICONHAND);
	}
	m_Player = Player;
}

void CColliPlayerEnemy::SetEnemy(CEnemy *Enemy)
{
	if (nullptr == Enemy)
	{
		MessageBox(nullptr, "CColliPlayerEnemyに渡したCEnemyポインタはnullptr!", "ポインタの値はnullptr", MB_OK | MB_ICONHAND);
	}
	m_Enemy = Enemy;
}

void CColliPlayerEnemy::SetEnemyOther(CEnemy *Enemy)
{
	if (nullptr == Enemy)
	{
		MessageBox(nullptr, "CColliPlayerEnemyに渡したCEnemyポインタはnullptr!", "ポインタの値はnullptr", MB_OK | MB_ICONHAND);
	}
	m_EnemyOtherEnemy = Enemy;
}
/*test*/