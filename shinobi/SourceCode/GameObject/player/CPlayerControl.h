#pragma once
#ifndef _CPLAYER_CONTROL_H_
#define _CPLAYER_CONTROL_H_

#include "CPlayer.h"
#include "CEnemy.h"
#include "CMeshField.h"
#include "CGame.h"
#include "CAfterimageEffect.h"

//先方宣言
class CCharacterCamera;

//クラス定義
class CPlayerControl
{
public:
	void Init(void);					//初期化
	void Update(void);					//プレーヤーをコントロールする(まとめクラス)
	void SceneTraDecision(CGame::SCENE_TRA* pTraFlag);		//シーン遷移
	void Uninit(void);					//解放

	//セッター
	void SetPlayer(CPlayer *Player);
	
	//ゲッター
	inline bool GetLockOnFlag(void) {
		if (m_Player != nullptr) {
			return m_Player->GetLockOnFlag();
		}
		return false; 
	}
	D3DXVECTOR3 GetLockOnEnemyPos(void);
private:
	void PlayerHitAllMesh(void);				//プレーヤーとメッシュフィールドとの当り判定
	void PlayerLockOnEnemy(void);				//敵にロックオン
	void LockOnTargetChange(void);				//ロックオンしている標的を変更
	void RotateCamera(void);					//カメラを回転する
	void PlayerCameraControl(void);				//プレーヤーのカメラ追従
	void PlayerFaceDir(void);					//プレーヤーの面の向きから姿勢を計算

private:
	CEnemy *m_Enemy;
	CPlayer *m_Player;
	bool m_bMotionChange;
	CPlayer::MOTION_MODE m_MotionMode;		//実行するモーション番号
	CPlayer::MOTION_MODE m_MotionModeOld;	//前回実行したモーション番号
	int m_nMotionTime;						//モーションの継続時間取得
	int m_nKeyFrameTime;					//1キーフレームの継続時間取得
	D3DXVECTOR2 m_ControlerStickDir;		//スティックの押し方向
	D3DXVECTOR2 m_ControlerRightStickDirOld;//スティックの押し方向
	D3DXVECTOR3 m_ActionMoveDir;			//プレーヤーが特定の動作をする時の移動方向(例 : ローリング、ステップ、敵の攻撃を受け時の移動方向)
	float		m_MovePower;				//プレーヤーの移動力
	int			m_KeyFrameBefore;			//前のキーフレーム
	CCharacterCamera *m_PlayerCamera;
};

#endif