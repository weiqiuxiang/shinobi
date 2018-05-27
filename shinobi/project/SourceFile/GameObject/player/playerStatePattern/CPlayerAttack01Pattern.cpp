#include "CPlayerAttack01Pattern.h"
#include "CPlayer.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"
#include "CPlayerPatternManager.h"
#include "common.h"

#define DEAD_ZONE (0.1f)

//コンストラクタ
CPlayerAttack01Pattern::CPlayerAttack01Pattern(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	//武器によるモーション初期化
	if (pPlayer != nullptr) {
		//武器によるニュートラルモーション設定
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ATTACK_ONE);
			pPlayer->SetAttackType(WSWORD_ATTACK_01);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			pPlayer->SetAttackType(CLAYMORE_ATTACK_01);
			break;
		}
		
		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ATTACK01_PATTERN);
	}

	//武器残像エフェクト初期化
	m_LeftWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_RightWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_LeftWeaponATKEffect.NoUseAllVertex();
	m_RightWeaponATKEffect.NoUseAllVertex();
	if (pPlayer != nullptr) {
		UpdateAttackEffectPosition(pPlayer);
	}

	ChangeDirWhenAttack(pPlayer);					//方向決め

	m_PlayerPatternManager = pManager;
}

//デストラクタ
CPlayerAttack01Pattern::~CPlayerAttack01Pattern()
{
	m_LeftWeaponATKEffect.Uninit();
	m_RightWeaponATKEffect.Uninit();
}

bool CPlayerAttack01Pattern::InitParameter(CPlayer* pPlayer , CPlayerPatternManager* pManager)
{
	//武器によるモーション初期化
	if (pPlayer != nullptr) {
		//武器によるニュートラルモーション設定
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ATTACK_ONE);
			pPlayer->SetAttackType(WSWORD_ATTACK_01);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			pPlayer->SetAttackType(CLAYMORE_ATTACK_01);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ATTACK01_PATTERN);
	}

	//武器残像エフェクト初期化
	m_LeftWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_RightWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_LeftWeaponATKEffect.NoUseAllVertex();
	m_RightWeaponATKEffect.NoUseAllVertex();
	if (pPlayer != nullptr) {
		UpdateAttackEffectPosition(pPlayer);
	}

	ChangeDirWhenAttack(pPlayer);					//方向決め

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerAttack01Pattern::Update(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//攻撃軌跡エフェクト位置更新
	UpdateAttackEffectPosition(pPlayer);

	//武器エフェクトの更新
	m_LeftWeaponATKEffect.Update();
	m_RightWeaponATKEffect.Update();

	//武器ごとの攻撃モーションの具体動作
	AttackActionDetail(pPlayer);

	//パターンの変更
	if(ChangePattern(pPlayer) == true){
		return;
	}
}

//描画処理
void CPlayerAttack01Pattern::Draw(void)
{
	m_LeftWeaponATKEffect.Draw();
	m_RightWeaponATKEffect.Draw();
}

void CPlayerAttack01Pattern::UpdateAttackEffectPosition(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//武器によるエフェクト
	CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
	switch (WeaponMode) {
	case CPlayer::WEAPON_WSWORD:
		UpdateWSwordAttackEffect(pPlayer);
		break;
	case CPlayer::WEAPON_CLAYMORE:
		UpdateClaymoreAttackEffect(pPlayer);
		break;
	}
}

void CPlayerAttack01Pattern::UpdateWSwordAttackEffect(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	/***************左手武器**************/
	//必要変数
	D3DXMATRIX LeftWeaponMtx;
	D3DXMatrixIdentity(&LeftWeaponMtx);
	D3DXMATRIX OffsetMtx, ResultMtx;

	//エフェクト上位置計算
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 1);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 2.0f, 0.0f);	//オフセット計算
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//リザルトマトリクス計算
	D3DXVECTOR3 LeftTopPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);

	//エフェクト下位置計算
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 0);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 0.0f, 0.0f);	//オフセット計算
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//リザルトマトリクス計算
	D3DXVECTOR3 LeftBottomPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);

	//軌跡エフェクトの頂点設定
	if (m_LeftWeaponATKEffect.UseEffectVertex(LeftTopPos, LeftBottomPos) == false)
	{
		m_LeftWeaponATKEffect.MoveEffectPosition(LeftTopPos, LeftBottomPos);
	}

	/***************右手武器**************/
	D3DXMATRIX RightWeaponMtx;
	D3DXMatrixIdentity(&RightWeaponMtx);

	//エフェクト上位置計算
	pPlayer->GetWeaponRightPartMtx(&RightWeaponMtx, 1);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 2.0f, 0.0f);	//オフセット計算
	ResultMtx = OffsetMtx * RightWeaponMtx;				//リザルトマトリクス計算
	D3DXVECTOR3 RightTopPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//エフェクト下位置計算
	pPlayer->GetWeaponRightPartMtx(&RightWeaponMtx, 0);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 0.0f, 0.0f);	//オフセット計算
	ResultMtx = OffsetMtx * RightWeaponMtx;				//リザルトマトリクス計算
	D3DXVECTOR3 RightBottomPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//軌跡エフェクトの頂点設定
	if (m_RightWeaponATKEffect.UseEffectVertex(RightTopPos, RightBottomPos) == false)
	{
		m_RightWeaponATKEffect.MoveEffectPosition(RightTopPos, RightBottomPos);
	}
}

void CPlayerAttack01Pattern::UpdateClaymoreAttackEffect(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//必要変数
	D3DXMATRIX LeftWeaponMtx;
	D3DXMatrixIdentity(&LeftWeaponMtx);
	D3DXMATRIX OffsetMtx, ResultMtx;

	//エフェクト上位置計算
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 1);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 2.0f, 0.0f);	//オフセット計算
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//リザルトマトリクス計算
	D3DXVECTOR3 LeftTopPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//エフェクト下位置計算
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 0);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 0.0f, 0.0f);	//オフセット計算
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//リザルトマトリクス計算
	D3DXVECTOR3 LeftBottomPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//軌跡エフェクトの頂点設定
	if (m_LeftWeaponATKEffect.UseEffectVertex(LeftTopPos, LeftBottomPos) == false)
	{
		m_LeftWeaponATKEffect.MoveEffectPosition(LeftTopPos, LeftBottomPos);
	}
}

//武器のエフェクト停止
void CPlayerAttack01Pattern::StopWeaponEffect(void)
{
	m_LeftWeaponATKEffect.NoUseAllVertex();
	m_RightWeaponATKEffect.NoUseAllVertex();
}

//武器ごとの攻撃モーションの具体動作
void CPlayerAttack01Pattern::AttackActionDetail(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	D3DXVECTOR3 PlayerFaceDir = pPlayer->GetFaceDir();		//今のプレーヤー向き取得
	int KeyFrameNow = pPlayer->GetKeyFrameNow();			//今のキーフレーム取得
	bool HitColliEnable = false;							//攻撃当り判定円の使用可否

	switch (pPlayer->GetWeaponMode())
	{
	case CPlayer::WEAPON_WSWORD:
		//キーフレームによる移動部分
		if (KeyFrameNow == 0) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.2f);
		}
		if (KeyFrameNow == 1) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.2f);
		}
		if (KeyFrameNow == 2) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.4f);
		}

		//当り判定円の有効無効設定部分
		HitColliEnable = KeyFrameNow != 0;
		if (HitColliEnable) {
			pPlayer->EnableWeaponHitCircle(false, true);
		}
		else {
			pPlayer->DisableWeaponHitCircle(true, true);
		}

		//攻撃力設定
		pPlayer->SetAttackPower(40);
		break;

	case CPlayer::WEAPON_CLAYMORE:
		//キーフレームによる移動部分
		if (KeyFrameNow == 1) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * -0.1f);
		}
		if (KeyFrameNow == 3) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.2f);
		}
		if (KeyFrameNow == 4) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.4f);
		}

		//当り判定円の有効無効設定部分
		HitColliEnable = KeyFrameNow != 0 && KeyFrameNow != 1 && KeyFrameNow != 2;
		if (HitColliEnable) {
			pPlayer->EnableWeaponHitCircle(true, true);
		}
		else {
			pPlayer->DisableWeaponHitCircle(true, true);
		}

		//攻撃力設定
		pPlayer->SetAttackPower(70);
		break;
	}
}

//パターンの変更
bool CPlayerAttack01Pattern::ChangePattern(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}

	if(m_PlayerPatternManager == nullptr){
		return false;
	}

	//攻撃受けったらスタンパターンに入る
	if (pPlayer->GetIsBeAttackedFlag() == true && pPlayer->GetNoDamageTime() <= 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
	}

	m_nMotionTime--;			//モーション継続時間減少

	//ニュートラルパターンに戻る
	if(m_nMotionTime < 0){
		m_LeftWeaponATKEffect.NoUseAllVertex();
		m_RightWeaponATKEffect.NoUseAllVertex();

		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	int MotionNow = pPlayer->GetMotionNow();								//今実行中のモーション取得
	int MotionTotalKeyFrame = pPlayer->GetMotionTotalKeyFrame(MotionNow);		//キーフレーム総数取得
	int KeyFrameNow = pPlayer->GetKeyFrameNow();							//今実行中のキーフレーム

	//攻撃フレームは最後のキーフレーム
	if ((MotionTotalKeyFrame - 1) == KeyFrameNow) {
		//攻撃ボタン入力の有無確認
		bool bKeyboard = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_J);
		bool bController = CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER);

		//入力なしなら処理終了
		if (!bKeyboard && !bController) {
			return false;
		}

		//武器による攻撃パターンの変更
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_ATTACK02_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	return false;
}

void CPlayerAttack01Pattern::ChangeDirWhenAttack(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	if (pPlayer->GetLockOnFlag() == false) {
		D3DXVECTOR2 AnalogInputValue = KeyboardValue();
		if (D3DXVec2Length(&AnalogInputValue) < DEAD_ZONE) {
			AnalogInputValue = CSceneTransition::GetXInput()->GetLeftThumbStickValueAnalog();
		}
		if (D3DXVec2Length(&AnalogInputValue) < DEAD_ZONE) {
			return;
		}

		CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);		//カメラの水平方向の向きを取得

		//カメラの向いてるXZ平面の方向ベクトルを算出
		D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
		CameraLookatVec.y = 0;

		D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

		//プレーヤーの向き算出
		float AngleDir = CPolygonMath::Vec2AngleAndAngleDir(D3DXVECTOR2(0.0f, 1.0f), AnalogInputValue);		//コントローラの成す角度を算出
		D3DXMATRIX MtxRot;
		D3DXMatrixRotationY(&MtxRot, -AngleDir);
		D3DXVECTOR3 PlayerDir;
		D3DXVec3TransformCoord(&PlayerDir, &CameraLookatVec, &MtxRot);
		D3DXVec3Normalize(&PlayerDir, &PlayerDir);
		pPlayer->SetGoalDir(PlayerDir);
		pPlayer->SetFaceDir(PlayerDir);
	}

	else {
		D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
		D3DXVECTOR3 EnemyPos = pPlayer->GetLockOnPosition();
		D3DXVECTOR3 PlayerEnemyVec = EnemyPos - PlayerPos;
		D3DXVec3Normalize(&PlayerEnemyVec, &PlayerEnemyVec);
		pPlayer->SetGoalDir(PlayerEnemyVec);
		pPlayer->SetFaceDir(PlayerEnemyVec);
	}
}

D3DXVECTOR2 CPlayerAttack01Pattern::KeyboardValue(void)
{
	bool PressW = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_W);
	bool PressA = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_A);
	bool PressS = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_S);
	bool PressD = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_D);

	//コントローラスティック方向の算出
	D3DXVECTOR2 StickValue = D3DXVECTOR2(0.0f, 0.0f);
	if (PressW) StickValue = D3DXVECTOR2(0.0f, 1.0f);
	if (PressA) StickValue = D3DXVECTOR2(-1.0f, 0.0f);
	if (PressS) StickValue = D3DXVECTOR2(0.0f, -1.0f);
	if (PressD) StickValue = D3DXVECTOR2(1.0f, 0.0f);
	if (PressW && PressA) StickValue = D3DXVECTOR2(-0.7071f, 0.7071f);
	if (PressW && PressD) StickValue = D3DXVECTOR2(0.7071f, 0.7071f);
	if (PressS && PressA) StickValue = D3DXVECTOR2(-0.7071f, -0.7071f);
	if (PressS && PressD) StickValue = D3DXVECTOR2(0.7071f, -0.7071f);

	return StickValue;
}