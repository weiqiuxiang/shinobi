#include "CPlayerControl.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CMeshFieldManager.h"
#include "CPolygonMath.h"
#include "CVectorMath.h"
#include "CDebug.h"
#include "CLinkPlayerToUI.h"
#include "CGame.h"
#include "XInputControl.h"
#include "CLockOnEnemy.h"
#include "CCharacterCamera.h"

#define STAMINA_DOWN_SPEED (5)					//プレーヤーのスタミナ消耗速度
#define HOLD_BUTTON_TO_RUN_FRAME (10)			//走るモーションに切り替えため,ボタン押し続ける必要フレーム数

//移動系マクロ
#define WALK_POWER (0.02f)			//歩く力
#define RUN_POWER (0.05f)			//走る力
#define DASH_POWER (0.08f)			//ダッシュの力
#define STEP_POWER (0.6f)			//ステップの力
#define ROLLING_POWER (0.6f)		//ローリングの力
#define ROLLING_POWERY (0.4f)		//ローリング時のジャンプ力

#define STICK_WALK_ZONE (0.5f)
#define STICK_RUN_ZONE (0.8f)

#define DEFAULT_COFFI (0.2)					//デフォルト地面摩擦係数
#define SKY_COFFI (0.02)					//空中摩擦係数

//初期化
void CPlayerControl::Init(void)
{
	m_Enemy = nullptr;
	m_Player = nullptr;
	m_MotionMode = CPlayer::MOTION_WSWORD_NEUTRAL;
	m_MotionModeOld = CPlayer::MOTION_WSWORD_NEUTRAL;
	m_bMotionChange = false;
	m_nMotionTime = 0;
	m_ControlerStickDir = D3DXVECTOR2(0.0f, 0.0f);
	m_ControlerRightStickDirOld = D3DXVECTOR2(0.0f, 0.0f);

	//カメラオブジェクト初期化
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);
	m_PlayerCamera = new CCharacterCamera(
		CameraPara.pos, CameraPara.lookat,
		CCameraManager::GetHoriAngle(CCameraManager::CAMERA_PLAYER),
		CCameraManager::GetVertiAngle(CCameraManager::CAMERA_PLAYER));
	
	m_MovePower = WALK_POWER;											//移動パワー
	m_ActionMoveDir = D3DXVECTOR3(0.0f, 0.0f,0.0f);
	m_KeyFrameBefore = 0;
}

//プレーヤーをコントロールする
void CPlayerControl::Update(void)
{
	m_MotionModeOld = m_MotionMode;
	if (nullptr != m_Enemy) {
		if (m_Enemy->GetDeadFlag()) {
			m_Enemy = nullptr;
		}

		if (m_Player != nullptr) {
			m_Player->SetLockOnFlag(true);
			m_Player->SetLockOnPosition(GetLockOnEnemyPos());
		}
	}
	else {
		m_Player->SetLockOnFlag(false);
	}

	PlayerLockOnEnemy();		//敵にロックオン
	LockOnTargetChange();		//ロックオンしている標的チェンジ
	PlayerHitAllMesh();			//プレーヤーとメッシュの当り判定
	PlayerFaceDir();			//プレーヤーの姿勢計算
	PlayerCameraControl();		//プレーヤーのカメラ追従

	//外部関数
	CLinkPlayerToUI::SetPlayerLifeNow(m_Player->GetPlayerLife());
	CLinkPlayerToUI::SetPlayerStaminaNow(m_Player->GetPlayerStamina());
	m_ControlerRightStickDirOld = CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog(); //右スティックステータス取得
	
}

void CPlayerControl::SceneTraDecision(CGame::SCENE_TRA* pTraFlag)
{
	if (nullptr == m_Player) return;
	if (m_Player->GetPlayerDeadFlag() == true) *pTraFlag = CGame::TRA_TO_RESULT;
}

/*==============================================================================
	終了処理
===============================================================================*/
void CPlayerControl::Uninit(void)
{
	if (m_PlayerCamera != nullptr) {
		delete m_PlayerCamera;
		m_PlayerCamera = nullptr;
	}
}

/*==============================================================================
	ロックオンしている敵の座標
===============================================================================*/
D3DXVECTOR3 CPlayerControl::GetLockOnEnemyPos(void)
{
	if (nullptr != m_Enemy) {
		return m_Enemy->GetBodyPos();
	}
	return D3DXVECTOR3(0.0f,0.0f,0.0f);
}

void CPlayerControl::SetPlayer(CPlayer *Player)
{
	if(nullptr == Player)
	{
		MessageBox(nullptr, "CPlayerControlに渡したプレーヤーポインタはnullptr!", "ポインタの値はnullptr", MB_OK | MB_ICONHAND);
	}
	m_Player = Player;
}

/*
//プレーヤーの向きの目標を決める
void CPlayerControl::PlayerGoalDir(void)
{
	if (nullptr == m_Player) {
		return;
	}

	//敵をロックオンしていない状態
	if (false == m_bLockEnemy)
	{
		if (D3DXVec2Length(&m_ControlerStickDir) < 0.2f) return;

		CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);		//カメラの水平方向の向きを取得

		//カメラの向いてるXZ平面の方向ベクトルを算出
		D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
		CameraLookatVec.y = 0;
			
		D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

		//プレーヤーの向き算出
		float AngleDir = CPolygonMath::Vec2AngleAndAngleDir(D3DXVECTOR2(0.0f, 1.0f), m_ControlerStickDir);		//コントローラの成す角度を算出
		D3DXMATRIX MtxRot;
		D3DXMatrixRotationY(&MtxRot, -AngleDir);
		D3DXVECTOR3 PlayerDir;
		D3DXVec3TransformCoord(&PlayerDir, &CameraLookatVec, &MtxRot);
		D3DXVec3Normalize(&PlayerDir, &PlayerDir);
		m_Player->SetGoalDir(PlayerDir);
	}

	else
	{
		if (nullptr == m_Enemy) return;
		D3DXVECTOR3 PlayerPos = m_Player->GetPos();
		D3DXVECTOR3 EnemyPos = m_Enemy->GetPos();
		D3DXVECTOR3 PlayerEnemyVec = EnemyPos - PlayerPos;
		D3DXVec3Normalize(&PlayerEnemyVec, &PlayerEnemyVec);
		m_Player->SetGoalDir(PlayerEnemyVec);
	}
}
*/

//プレーヤーの面の向きの計算
void CPlayerControl::PlayerFaceDir(void)
{
	if (nullptr == m_Player) {
		return;
	}

	//プレーヤー向きベクトルでプレーヤーの姿勢を計算
	D3DXMATRIX PoseMtx;
	D3DXVECTOR3 X,Y,Z;
	Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 PlayerDir = m_Player->GetFaceDir();
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == PlayerDir) return;
	D3DXVec3Normalize(&Z, &PlayerDir);
	D3DXVec3Cross(&X, &Y,&Z);
	D3DXVec3Normalize(&X, &X);
	
	PoseMtx._11 = X.x; PoseMtx._12 = X.y; PoseMtx._13 = X.z; PoseMtx._14 = 0.0f;
	PoseMtx._21 = Y.x; PoseMtx._22 = Y.y; PoseMtx._23 = Y.z; PoseMtx._24 = 0.0f;
	PoseMtx._31 = Z.x; PoseMtx._32 = Z.y; PoseMtx._33 = Z.z; PoseMtx._34 = 0.0f;
	PoseMtx._41 = 0.0f; PoseMtx._42 = 0.0f; PoseMtx._43 = 0.0f; PoseMtx._44 = 1.0f;

	m_Player->SetPoseMtx(PoseMtx);
}

//プレーヤーと地面の当り判定
void CPlayerControl::PlayerHitAllMesh(void)
{
	if (nullptr == m_Player) return;

	D3DXVECTOR3 PlayerPos = m_Player->GetPos();
	CMeshField *pMesh = nullptr;

	//プレーヤーと全てのメッシュの当り判定
	for(int MeshNum = 0; MeshNum < MESH_MAX; MeshNum++)
	{
		pMesh = CMeshFieldManager::GetMesh(MeshNum);
		if (nullptr == pMesh) continue;

		const MESH_FACE *pFace = pMesh->GetMeshFace();			//面データ取得
		D3DXVECTOR2 PieceNum = pMesh->GetPieceNum();			//マス数取得

		//メッシュの全ての面とプレーヤーの当り判定
		for (int PieceY = 0; PieceY < PieceNum.y; PieceY++)
		{
			for (int PieceX = 0; PieceX < (int)PieceNum.x * 2; PieceX++)
			{
				//面番号計算
				int k = PieceY * ((int)PieceNum.x * 2) + PieceX;
				if (nullptr == &pFace[k]) continue;

				//プレーヤーは平面の範囲内にあるかどうか
				bool InMesh = CPolygonMath::PointInXZMeshFace(PlayerPos, pFace[k]);
				if (InMesh == false) {
					continue;
				}

				//プレーヤーのXZ座標を渡し,その位置の面のY座標を求める
				float fElevation = CPolygonMath::PointElevationInFace(PlayerPos, pFace[k]);

				//プレーヤーのY座標は標高より低い時
				if (m_Player->GetPos().y < fElevation) {
					m_Player->SetPosY(fElevation);
					m_Player->SetSpeedY(0);
				}
			}
		}
	}
}

void CPlayerControl::PlayerLockOnEnemy(void)           //敵にロックオン
{
	if (nullptr == m_Player) {
		return;
	}
	
	//敵にロックオンしている状態
	if (m_Enemy != nullptr) {
		//プレーヤーと敵の距離を求める
		D3DXVECTOR3 PlayerEnemyVec = m_Enemy->GetPos() - m_Player->GetPos();
		float Distance = D3DXVec3Length(&PlayerEnemyVec);

		//ロックオンボタン押されたフラグ
		bool IsPushLockOnButton = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RETURN) || 
			CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_THUMB);

		//ロックオンの解除判定
		if (Distance > LOCKON_DISTANCE || IsPushLockOnButton) {
			m_Player->SetLockOnFlag(false);
			m_Enemy = nullptr;			//Enemyポインタをnullptrに
			return; 
		}
	}

	else {
		//ロックオンボタン押されたフラグ
		bool IsPushLockOnButton = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RETURN) ||
			CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_THUMB);
		
		//ロックオンできる敵を探す
		if (IsPushLockOnButton) {
			CEnemy* pEnemy = CLockOnEnemy::LockOnEnemy(m_Player);

			//ロックオン敵はいない
			if (pEnemy == nullptr) {
				return;
			}
			else {
				m_Enemy = pEnemy;
				m_Player->SetLockOnFlag(true);
			}
		}
	}
}


//ロックオンのターゲットの変更
void CPlayerControl::LockOnTargetChange(void)
{
	//ヌルチェック
	if (m_Player == nullptr) {
		return;
	}
	if (m_Player->GetLockOnFlag() == false) {
		return;
	}

	//キーボードの←キー、→キーが押されたか
	bool bPushKeyLeft = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_LEFT);
	bool bPushKeyRight = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RIGHT);
	bool bPushDirKey = bPushKeyLeft || bPushKeyRight;

	//右スティックのを左か右に押したか
	bool bIsTiggerRightStick =
		(fabs(m_ControlerRightStickDirOld.x) < 0.5) && 
		(fabs(CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog().x) > 0.5);

	//キーボードもしくはスティックの操作はない場合、早期リターンする
	if (!bPushDirKey && !bIsTiggerRightStick) {
		return;
	}

	bool bDir = false;		//入力方向

	//スティックの入力はない場合、キーボードの入力を判定する
	if ( bIsTiggerRightStick == false) {
		if (bPushKeyLeft) {
			bDir = false;
		}
		if (bPushKeyRight) {
			bDir = true;
		}
	}
	else {
		if (CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog().x < 0.0f) {
			bDir = false;
		}
		else {
			bDir = true;
		}
	}

	//ロックオン標的の変更
	CEnemy* pEnemy = CLockOnEnemy::ChangeTarget(m_Player, m_Enemy, bDir);
	if (pEnemy != nullptr) {
		m_Enemy = pEnemy;
	}
}

void CPlayerControl::RotateCamera(void)
{
	//ヌルチェック
	if (m_Player == nullptr || m_PlayerCamera == nullptr) {
		return;
	}
	//カメラチェック
	if (CCameraManager::GetActiveCameraName() != CCameraManager::CAMERA_PLAYER) {
		return;
	}

	//コントローラのカメラ回転入力
	D3DXVECTOR2 AnalogInputValue = CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog();
	m_PlayerCamera->TpsCameraRotateStick(AnalogInputValue);

	//キーボードのカメラ回転入力
	float HorizonValue = 0;
	float VerticleValue = 0;
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_UP)) {
		VerticleValue = 1.0f;
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_DOWN)) {
		VerticleValue = -1.0f;
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_LEFT)) {
		HorizonValue = 1.0f;
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_RIGHT)) {
		HorizonValue = -1.0f;
	}
	m_PlayerCamera->TpsCameraRotateKeyboard(HorizonValue, VerticleValue);
}

void CPlayerControl::PlayerCameraControl(void)
{
	if (m_Player == nullptr) {
		return;
	}

	//ロックオンしていない時
	if (m_Player->GetLockOnFlag() == false) {
		if(m_PlayerCamera != nullptr && m_Player != nullptr){
			//目標位置の計算
			m_PlayerCamera->CalcCameraAndLookatPos(m_Player->GetPos());
			//補間する
			CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//カメラデータ取得
			m_PlayerCamera->CameraAndLookatToGoalLinearInterpolation(&CameraPara.pos, &CameraPara.lookat);
			//補間後座標を現カメラに代入
			CCameraManager::SetCameraAndLookat(CCameraManager::CAMERA_PLAYER, CameraPara.pos, CameraPara.lookat);
		}

		//カメラの回転コントロール
		RotateCamera();
	}
	else {
		if (m_PlayerCamera != nullptr && m_Player != nullptr && m_Enemy != nullptr) {
			//目標位置の計算
			m_PlayerCamera->CalcCameraAndLookatPosLockOn(m_Player->GetPos(), m_Enemy->GetPos() );
			//補間する
			CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//カメラデータ取得
			m_PlayerCamera->CameraAndLookatToGoalLinearInterpolation(&CameraPara.pos, &CameraPara.lookat);
			//補間後座標を現カメラに代入
			CCameraManager::SetCameraAndLookat(CCameraManager::CAMERA_PLAYER, CameraPara.pos, CameraPara.lookat);
		}
	}
}