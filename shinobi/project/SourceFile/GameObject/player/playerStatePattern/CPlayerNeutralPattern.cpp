#include "CPlayer.h"
#include "CPlayerNeutralPattern.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"

//マクロ
#define STICK_WALK_ZONE (0.5f)		//スティック歩く動作のゾーン
#define STICK_RUN_ZONE (0.8f)		//スティック走る動作のゾーン
#define STAMINA_RECOVERY (5)		//スタミナ回復値
#define STAMINA_DOWN_WHEN_RUN (-5)			//スタミナ減少値

#define WALK_POWER (0.02f)			//移動時の歩く力
#define RUN_POWER (0.05f)			//移動時の走る力
#define DASH_POWER (0.08f)			//移動時のダッシュの力
#define ROLLING_POWER (0.6f)		//ローリングの力
#define ROLLING_POWERY (0.4f)		//ローリング時のジャンプ力
#define STEP_POWER (0.6f)			//ステップの力

#define RELEASE_BUTTON_TO_ROLLING_FRAME (10)	//ローリング/ステップアクションのボタン押し判定時間(押しフレーム数はこのマクロより小さい場合、アクション発生)

CPlayerNeutralPattern::CPlayerNeutralPattern()
{
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	m_IsRun = false;
	m_MotionNumOld = CPlayer::MOTION_WSWORD_NEUTRAL;
}

CPlayerNeutralPattern::CPlayerNeutralPattern(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	m_IsRun = false;

	if (pPlayer != nullptr) {
		//武器によるモーション設定
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_NEUTRAL);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_NEUTRAL);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_NEUTRAL_PATTERN);
	}

	m_PlayerPatternManager = pManager;
	m_PushRunButtonTime = 0;
}

bool CPlayerNeutralPattern::InitParameter(CPlayer* pPlayer , CPlayerPatternManager*pManager)
{
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	m_IsRun = false;

	if (pPlayer != nullptr) {
		//武器によるニュートラルモーション設定
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_NEUTRAL);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_NEUTRAL);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_NEUTRAL_PATTERN);
	}

	m_PlayerPatternManager = pManager;
	m_PushRunButtonTime = 0;

	return true;
}

void CPlayerNeutralPattern::Update(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//入力されたスティックの値計算
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	bool IsInputFromStick = false;
	IsInputFromStick = CalcStickValueFromKeyboard();
	if (!IsInputFromStick) {
		IsInputFromStick = CalcStickValueFromController();
	}

	//走っているか
	m_IsRun = IsRun();

	//体力の増減
	if (m_IsRun) {
		pPlayer->AddStamina(STAMINA_DOWN_WHEN_RUN);
	}
	else {
		pPlayer->AddStamina(STAMINA_RECOVERY);
	}

	//移動
	float MovePower = CalcMovePower();
	if (pPlayer->GetLockOnFlag()) {
		MoveLockOn(pPlayer,MovePower);
	}
	else {
		MoveNoLockOn(pPlayer, MovePower);
	}

	//面の向きを計算
	if (pPlayer->GetLockOnFlag()) {
		CalcFaceGoalDirLockOn(pPlayer);
	}
	else {
		CalcFaceGoalDirNoLockOn(pPlayer);
	}
	CalcFaceDir(pPlayer);

	//モーション設定
	ChangeMotion(pPlayer,MovePower);

	//パターン変更
	if (ChangePattern(pPlayer) == true) {
		return;
	}

	//走るボタンを押しっぱなしの時間を累積
	PushRunButtonTime();
}

bool CPlayerNeutralPattern::CalcStickValueFromKeyboard(void)
{
	bool PressW = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_W);
	bool PressA = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_A);
	bool PressS = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_S);
	bool PressD = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_D);

	//コントローラスティック方向の算出
	if (PressW) m_StickValue = D3DXVECTOR2(0.0f, 1.0f);
	if (PressA) m_StickValue = D3DXVECTOR2(-1.0f, 0.0f);
	if (PressS) m_StickValue = D3DXVECTOR2(0.0f, -1.0f);
	if (PressD) m_StickValue = D3DXVECTOR2(1.0f, 0.0f);
	if (PressW && PressA) m_StickValue = D3DXVECTOR2(-0.7071f, 0.7071f);
	if (PressW && PressD) m_StickValue = D3DXVECTOR2(0.7071f, 0.7071f);
	if (PressS && PressA) m_StickValue = D3DXVECTOR2(-0.7071f, -0.7071f);
	if (PressS && PressD) m_StickValue = D3DXVECTOR2(0.7071f, -0.7071f);

	//ボタン押されたフラグ
	bool bBottonPush = false;
	if (PressW || PressA || PressS || PressD) bBottonPush = true;

	return bBottonPush;
}

bool CPlayerNeutralPattern::CalcStickValueFromController(void)
{
	D3DXVECTOR2 AnalogInputValue = CSceneTransition::GetXInput()->GetLeftThumbStickValueAnalog();
	m_StickValue = AnalogInputValue;

	bool bBottonPush = false;
	if (D3DXVec2LengthSq(&AnalogInputValue) > 0.0f) {
		bBottonPush = true;
	}

	return bBottonPush;
}

bool CPlayerNeutralPattern::IsRun(void)
{
	bool bKeyboardRun = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_SPACE);
	bool bControllerRun = CSceneTransition::GetXInput()->GetHoldButton(XINPUT_GAMEPAD_A);
	if (bKeyboardRun || bControllerRun) {
		return true;
	}
	return false;
}

//戻り値 : 移動力
float CPlayerNeutralPattern::CalcMovePower(void)
{
	float MovePower = 0;

	float PushStickValue = D3DXVec2Length(&m_StickValue);
	if (PushStickValue < STICK_WALK_ZONE) {
		MovePower = 0;
	}
	else if (PushStickValue < STICK_RUN_ZONE) {
		MovePower = WALK_POWER;
	}
	else {
		MovePower = RUN_POWER;
	}

	if (PushStickValue >= STICK_WALK_ZONE && m_IsRun == true) {
		MovePower = DASH_POWER;
	}

	return MovePower;
}

//
void CPlayerNeutralPattern::PushRunButtonTime(void)
{
	bool IsPushKeyboardRunButton = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_SPACE);
	bool IsPushControllerRunButton = CSceneTransition::GetXInput()->GetHoldButton(XINPUT_GAMEPAD_A);
	if (IsPushKeyboardRunButton || IsPushControllerRunButton) {
		m_PushRunButtonTime++;
		m_PushRunButtonTime = min(10000, m_PushRunButtonTime);	//オーバーフロー防止
	}
	else {
		m_PushRunButtonTime = 0;
	}
}

void CPlayerNeutralPattern::MoveNoLockOn(CPlayer* pPlayer, float MovePower)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//カメラの水平方向の向きを取得
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);

	//カメラの向いてるXZ平面の方向ベクトルを算出
	D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
	CameraLookatVec.y = 0;
	D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

	float StickXPercent = 0.0f;
	float StickYPercent = 0.0f;
	if (D3DXVec2Length(&m_StickValue) != 0.0f) {
		StickXPercent = m_StickValue.x / D3DXVec2Length(&m_StickValue);
		StickYPercent = m_StickValue.y / D3DXVec2Length(&m_StickValue);
	}

	//プレーヤーに与えた外力計算(スティック縦倒し)
	pPlayer->AddPower(CameraLookatVec * MovePower * StickYPercent);

	//プレーヤーに与えた外力計算(スティック横倒し)
	D3DXVECTOR3 PlayerSpeed = pPlayer->GetSpeed();							//プレーヤー速度取得
	D3DXVECTOR3 CameraPlayerVec = pPlayer->GetPos() - CameraPara.pos;
	CameraPlayerVec.y = 0;													//Y成分カット
	float SpeedValue = D3DXVec3Length(&PlayerSpeed);
	if (0 == SpeedValue) SpeedValue = 0.1f;				//例外処理 : 速度は0の時
	float Distance = D3DXVec3Length(&CameraPlayerVec);
	float TopAngle = atan2f(SpeedValue * 0.5f, Distance) * 2;
	D3DXMATRIX MtxRot;
	D3DXMatrixRotationY(&MtxRot, TopAngle);
	D3DXVECTOR3 CameraPlayerNextVec;					//次のXZ平面のEnemyPlayerベクトル
	D3DXVec3TransformCoord(&CameraPlayerNextVec, &CameraPlayerVec, &MtxRot);
	D3DXVECTOR3 NextPlayerPos = CameraPlayerNextVec + CameraPara.pos;
	D3DXVECTOR3 MoveDir = NextPlayerPos - pPlayer->GetPos();
	MoveDir.y = 0;									//Y成分をカット
													//例外処理 : 方向ベクトルの値は0の時
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == MoveDir) {
		D3DXVec3Cross(&MoveDir, &D3DXVECTOR3(0.0f, 1.0f, 1.0f), &CameraPlayerVec);
	}
	D3DXVec3Normalize(&MoveDir, &MoveDir);					//正規化
	pPlayer->AddPower(MoveDir * MovePower * StickXPercent);  //外力を与える
}

void CPlayerNeutralPattern::MoveLockOn(CPlayer* pPlayer, float MovePower)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 EnemyPos = pPlayer->GetLockOnPosition();
	D3DXVECTOR3 EnemyPlayerVec = PlayerPos - EnemyPos;
	D3DXVECTOR3 EnemyPlayerXZVec = D3DXVECTOR3(EnemyPlayerVec.x, 0.0f, EnemyPlayerVec.z);
	D3DXVECTOR3 EnemyPlayerXZVecNor;
	D3DXVec3Normalize(&EnemyPlayerXZVecNor, &EnemyPlayerXZVec);				//XZ平面の方向ベクトル算出

	//プレーヤーに与えた外力計算(スティック縦倒し)
	float StickXPercent = 0.0f;
	float StickYPercent = 0.0f;
	if (D3DXVec2Length(&m_StickValue) != 0.0f) {
		StickXPercent = m_StickValue.x / D3DXVec2Length(&m_StickValue);
		StickYPercent = m_StickValue.y / D3DXVec2Length(&m_StickValue);
	}

	pPlayer->AddPower(StickYPercent * MovePower * (-EnemyPlayerXZVecNor));	//スティックのY方向の外力

	//プレーヤーに与えた外力計算(スティック横倒し)
	D3DXVECTOR3 PlayerSpeed = pPlayer->GetSpeed();
	float SpeedValue = D3DXVec3Length(&PlayerSpeed);
	if (0 == SpeedValue) SpeedValue = 0.1f;
	float Distance = D3DXVec3Length(&EnemyPlayerXZVec);
	float TopAngle = atan2f(SpeedValue * 0.5f, Distance) * 2;
	D3DXMATRIX MtxRot;
	D3DXMatrixRotationY(&MtxRot, TopAngle);
	D3DXVECTOR3 EnemyPlayerNextVec;					//次のXZ平面のEnemyPlayerベクトル
	D3DXVec3TransformCoord(&EnemyPlayerNextVec, &EnemyPlayerVec, &MtxRot);
	D3DXVECTOR3 NextPlayerPos = EnemyPlayerNextVec + EnemyPos;
	D3DXVECTOR3 MoveDir = NextPlayerPos - PlayerPos;
	MoveDir.y = 0;									//Y成分をカット

	//例外処理 : 方向ベクトルの値は0の時
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == MoveDir) {
		D3DXVec3Cross(&MoveDir, &D3DXVECTOR3(0.0f, 1.0f, 1.0f), &EnemyPlayerNextVec);
	}
	D3DXVec3Normalize(&MoveDir, &MoveDir);

	pPlayer->AddPower(MoveDir * MovePower * -StickXPercent);					//外力を与える
}

void CPlayerNeutralPattern::CalcFaceGoalDirNoLockOn(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	if (D3DXVec2Length(&m_StickValue) < 0.2f) {
		return;
	}

	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);       //カメラの水平方向の向きを取得

	//カメラの向いてるXZ平面の方向ベクトルを算出
	D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
	CameraLookatVec.y = 0;

	D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

	//プレーヤーの向き算出
	float AngleDir = CPolygonMath::Vec2AngleAndAngleDir(D3DXVECTOR2(0.0f, 1.0f), m_StickValue);		//コントローラの成す角度を算出
	D3DXMATRIX MtxRot;
	D3DXMatrixRotationY(&MtxRot, -AngleDir);
	D3DXVECTOR3 PlayerDir;
	D3DXVec3TransformCoord(&PlayerDir, &CameraLookatVec, &MtxRot);
	D3DXVec3Normalize(&PlayerDir, &PlayerDir);
	pPlayer->SetGoalDir(PlayerDir);
}

void CPlayerNeutralPattern::CalcFaceGoalDirLockOn(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 EnemyPos = pPlayer->GetLockOnPosition();
	D3DXVECTOR3 PlayerEnemyVec = EnemyPos - PlayerPos;
	D3DXVec3Normalize(&PlayerEnemyVec, &PlayerEnemyVec);
	pPlayer->SetGoalDir(PlayerEnemyVec);
}

void CPlayerNeutralPattern::CalcFaceDir(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//プレーヤー向きベクトルでプレーヤーの姿勢を計算
	D3DXMATRIX PoseMtx;
	D3DXVECTOR3 X, Y, Z;
	Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 PlayerDir = pPlayer->GetFaceDir();
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == PlayerDir) return;
	D3DXVec3Normalize(&Z, &PlayerDir);
	D3DXVec3Cross(&X, &Y, &Z);
	D3DXVec3Normalize(&X, &X);

	PoseMtx._11 = X.x; PoseMtx._12 = X.y; PoseMtx._13 = X.z; PoseMtx._14 = 0.0f;
	PoseMtx._21 = Y.x; PoseMtx._22 = Y.y; PoseMtx._23 = Y.z; PoseMtx._24 = 0.0f;
	PoseMtx._31 = Z.x; PoseMtx._32 = Z.y; PoseMtx._33 = Z.z; PoseMtx._34 = 0.0f;
	PoseMtx._41 = 0.0f; PoseMtx._42 = 0.0f; PoseMtx._43 = 0.0f; PoseMtx._44 = 1.0f;

	pPlayer->SetPoseMtx(PoseMtx);
}

//モーションの変化
void CPlayerNeutralPattern::ChangeMotion(CPlayer* pPlayer, float MovePower)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//現在のモーションを保存する
	m_MotionNumOld = pPlayer->GetMotionNow();

	//武器状況取得
	CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();

	//状況に応じてモーション変更
	//0はニュートラル、1は歩く、2は走る、3はダッシュ
	int MoveModeNum = 0;
	if (MovePower == WALK_POWER) {
		MoveModeNum = 1;
	}
	if (MovePower == RUN_POWER) {
		MoveModeNum = 2;
	}
	if (MovePower == DASH_POWER) {
		MoveModeNum = 3;
	}

	int MotionNum = 0;
	switch(WeaponMode){
	case CPlayer::WEAPON_WSWORD:
		if (MoveModeNum == 0) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_NEUTRAL;
		}
		else if (MoveModeNum == 1) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_WALK;
		}
		else if (MoveModeNum == 2) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_RUN;
		}
		else if (MoveModeNum == 3) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_DASH;
		}
		break;
	case CPlayer::WEAPON_CLAYMORE:
		if (MoveModeNum == 0) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_NEUTRAL;
		}
		else if (MoveModeNum == 1) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_WALK;
		}
		else if (MoveModeNum == 2) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_RUN;
		}
		else if (MoveModeNum == 3) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_DASH;
		}
		break;
	}

	//前のモーションと変化はあれば
	if (m_MotionNumOld != MotionNum) {
		pPlayer->SetMotionMode(MotionNum);
	}
}

//別のパターンに遷移する判定
bool CPlayerNeutralPattern::ChangePattern(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
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

	//攻撃パターンに遷移
	if (ChangePatternToAttack(pPlayer) == true) {
		return true;
	}

	//ローリング / ステップパターンに遷移
	if (pPlayer->GetLockOnFlag() == false) {
		if (ChangePatternToRolling(pPlayer) == true) {
			return true;
		}
	}
	else {
		if (ChangePatternToStep(pPlayer) == true) {
			return true;
		}
	}

	//武器変形パターンに遷移
	if (ChangePatternToChangeWeaponShape(pPlayer) == true) {
		return true;
	}

	return false;
}

//攻撃パターンに変更
bool CPlayerNeutralPattern::ChangePatternToAttack(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	bool IsPushKeyboardAttack = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_J);
	bool IsPushControllerAttack = CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER);
	if (IsPushKeyboardAttack || IsPushControllerAttack) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_ATTACK01_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	return false;
}

//ローリングパターンに変更
bool CPlayerNeutralPattern::ChangePatternToRolling(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	if (m_PushRunButtonTime > RELEASE_BUTTON_TO_ROLLING_FRAME) {
		return false;
	}
	bool bReleaseLShift = CSceneTransition::GetInputKeyboard()->GetKeyRelease(DIK_SPACE);
	bool bReleaseAButton = CSceneTransition::GetXInput()->GetReleaseButton(XINPUT_GAMEPAD_A);
	if (!bReleaseAButton && !bReleaseLShift) {
		return false;
	}

	//進行方向求め
	D3DXVECTOR3 MoveDir;
	if (m_StickValue == D3DXVECTOR2(0.0f, 0.0f)) {		//スディック押していない時
		MoveDir = D3DXVECTOR3(pPlayer->GetFaceDir().x, 0.0f, pPlayer->GetFaceDir().z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);
	}
	else {
		D3DXVECTOR3 CameraLookatDir = CCameraManager::GetDirCameraToLookat(CCameraManager::CAMERA_PLAYER);
		float RotAngle = atan2f(m_StickValue.y, m_StickValue.x);
		MoveDir = D3DXVECTOR3(CameraLookatDir.x, 0.0f, CameraLookatDir.z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);
		CPolygonMath::DirVec3QuaternionRot(&MoveDir, &MoveDir, D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DX_PI * 0.5f - RotAngle);
	}

	pPlayer->AddPower(MoveDir * ROLLING_POWER);								//速度与える
	pPlayer->AddPower(D3DXVECTOR3(0.0f, ROLLING_POWERY, 0.0f));				//ローリング時のジャンプ力を与え
	pPlayer->SetGoalDir(MoveDir);												//プレーヤー向き設定
	
	//ローリングパターンに変更
	CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_ROLLING_PATTERN);
	if (pPlayerPattern != nullptr) {
		pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
	}
	pPlayer->SetState(pPlayerPattern);

	return true;
}

//ステップパターンに変更
bool CPlayerNeutralPattern::ChangePatternToStep(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	if (m_PushRunButtonTime > RELEASE_BUTTON_TO_ROLLING_FRAME) {
		return false;
	}
	bool bReleaseAButton = CSceneTransition::GetXInput()->GetReleaseButton(XINPUT_GAMEPAD_A);
	bool bReleaseLShift = CSceneTransition::GetInputKeyboard()->GetKeyRelease(DIK_SPACE);
	if (!bReleaseAButton && !bReleaseLShift) {
		return false;
	}

	//進行方向求め
	D3DXVECTOR3 MoveDir;
	if (m_StickValue == D3DXVECTOR2(0.0f, 0.0f)) {		//スディック押していない時
		MoveDir = D3DXVECTOR3(pPlayer->GetFaceDir().x, 0.0f, pPlayer->GetFaceDir().z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);
		MoveDir = -MoveDir;
	}
	else {
		D3DXVECTOR3 PlayerEnemyDir = pPlayer->GetLockOnPosition() - pPlayer->GetPos();
		D3DXVec3Normalize(&PlayerEnemyDir, &PlayerEnemyDir);
		float RotAngle = atan2f(m_StickValue.y, m_StickValue.x);
		MoveDir = D3DXVECTOR3(PlayerEnemyDir.x, 0.0f, PlayerEnemyDir.z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);						//単位化
		CPolygonMath::DirVec3QuaternionRot(&MoveDir, &MoveDir, D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DX_PI * 0.5f - RotAngle);
	}

	pPlayer->AddPower(MoveDir*STEP_POWER);

	//ステップパターンに変更
	CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STEP_PATTERN);
	if (pPlayerPattern != nullptr) {
		pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
	}
	pPlayer->SetState(pPlayerPattern);

	return true;
}

//武器変形パターンに変更
bool CPlayerNeutralPattern::ChangePatternToChangeWeaponShape(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	//武器変形パターンに遷移
	bool IsPushController = CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_LEFT_SHOULDER);
	bool IsPushKeyboard = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_K);
	if (IsPushController || IsPushKeyboard) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_CHANGE_WEAPON_SHAPE_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);

		return true;
	}

	return false;
}