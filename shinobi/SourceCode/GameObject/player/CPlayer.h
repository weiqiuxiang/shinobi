#pragma once
#ifndef _CPLAYER_H_
#define _CPLAYER_H_

#include "CHuman.h"
#include "CWeapon.h"
#include "CCollisionCircle.h"
#include "CStencilShadow.h"
#include "CPlayerPatternManager.h"
#include "common.h"

#define PLAYER_LIFE_MAX (1000)
#define PLAYER_STAMINA_MAX (1000)

class CPlayerPattern;

class CPlayer : public CHuman
{
public:
	typedef enum
	{
		//両刀モードモーションリスト
		MOTION_WSWORD_NEUTRAL = 0,
		MOTION_WSWORD_WALK,
		MOTION_WSWORD_RUN,
		MOTION_WSWORD_DASH,
		MOTION_WSWORD_ROLLING,
		MOTION_WSWORD_STEP_FORWARD,
		MOTION_WSWORD_STEP_BACK,
		MOTION_WSWORD_STEP_LEFT,
		MOTION_WSWORD_STEP_RIGHT,
		MOTION_WSWORD_ATTACK_ONE,
		MOTION_WSWORD_ATTACK_TWO,
		MOTION_WSWORD_ATTACK_THREE,
		MOTION_WSWORD_ATTACK_FOUR,
		MOTION_WSWORD_STUN,
		MOTION_WSWORD_DEAD,

		//大剣モードモーションリスト
		MOTION_CLAYMORE_NEUTRAL,
		MOTION_CLAYMORE_WALK,
		MOTION_CLAYMORE_RUN,
		MOTION_CLAYMORE_DASH,
		MOTION_CLAYMORE_ROLLING,
		MOTION_CLAYMORE_STEP_FORWARD,
		MOTION_CLAYMORE_STEP_BACK,
		MOTION_CLAYMORE_STEP_LEFT,
		MOTION_CLAYMORE_STEP_RIGHT,
		MOTION_CLAYMORE_ATTACK_ONE,
		MOTION_CLAYMORE_ATTACK_TWO,
		MOTION_CLAYMORE_ATTACK_THREE,
		MOTION_CLAYMORE_STUN,
		MOTION_CLAYMORE_DEAD,
		
		//武器変形モーション
		MOTION_CHANGE_WSWORD_TO_CLAYMORE,		//両刀から大剣へ
		MOTION_CHANGE_CLAYMORE_TO_WSWORD		//大剣から両刀へ
	}MOTION_MODE;

	typedef enum 
	{
		WEAPON_WSWORD = 0,
		WEAPON_CLAYMORE
	}WEAPON_MODE;			//武器状態

	CPlayer();
	~CPlayer();

	virtual void Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(void);
	void DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture);
	void UpdateWeaponPosition(void);			//武器の位置更新

	//ゲッター
	int GetPlayerLife(void) { return m_nLife; }
	int GetPlayerStamina(void) { return m_nStamina; }
	bool GetPlayerDeadFlag(void) { return m_DeadFlag; }
	int GetWeaponLeftMotionNow(void) {
		if (nullptr == m_WeaponLeft) return -1;
		return m_WeaponLeft->GetMotionNow();
	}
	CCollisionCircle* GetWeaponLeftColli(int Num);
	CCollisionCircle* GetWeaponRightColli(int Num);
	int GetWeaponLeftMaxColliCircle(void) { if (nullptr == m_WeaponLeft) return -1; return m_WeaponLeft->GetMaxColliCircleNum(); }
	int GetWeaponRightMaxColliCircle(void) { if (nullptr == m_WeaponRight) return -1; return m_WeaponRight->GetMaxColliCircleNum(); }
	void GetWeaponLeftPartMtx(D3DXMATRIX *pOut,int PartID);
	void GetWeaponRightPartMtx(D3DXMATRIX *pOut,int PartID);
	int GetAttackPower(void) { return m_AttackPower; }
	WEAPON_MODE GetWeaponMode(void) { return m_WeaponMode; }
	const D3DXVECTOR3& GetLockOnPosition(void) const { return m_LockOnPos; }
	bool GetLockOnFlag(void) { return m_LockOnFlag; }
	
	//ゲッター(スタン関連)
	int GetNoDamageTime(void) { return m_NoDamageTime; }
	D3DXVECTOR3 GetStunDir(void) { return m_StunDir; }
	ATTACK_TYPE_PLAYER GetAttackType(void) { return m_AttackType; }
	ATTACK_TYPE_ENEMY GetDamageType(void) { return m_DamageType; }
	bool GetIsBeAttackedFlag(void) { return m_IsBeAttacked; }
	PLAYER_PATTERN GetStateNow(void) { return m_PlayerStateNow; }

	//セッター
	void SetWeaponLeft(CWeapon *pWeapon,const D3DXVECTOR3& offset, int ParentID);
	void SetWeaponRight(CWeapon *pWeapon,const D3DXVECTOR3& offset, int ParentID);
	void SetWeaponLeftMotion(CWeapon::MOTION_MODE mode) {if (nullptr != m_WeaponLeft) m_WeaponLeft->SetMotionMode(mode);}
	void SetWeaponRightMotion(CWeapon::MOTION_MODE mode) {if (nullptr != m_WeaponRight) m_WeaponRight->SetMotionMode(mode);}
	void SetWeaponLeftPara(CWeapon::MOTION_MODE mode, const D3DXVECTOR3 &offset, int ParentID);
	void SetWeaponRightPara(CWeapon::MOTION_MODE mode, const D3DXVECTOR3 &offset, int ParentID);
	void SetAttackPower(int nPower) { m_AttackPower = max(0, nPower); }
	void SetWeaponMode(WEAPON_MODE WeaponMode) { m_WeaponMode = WeaponMode; }
	void SetLockOnPosition(const D3DXVECTOR3& pos) { m_LockOnPos = pos; }
	void SetLockOnFlag(bool Flag) { m_LockOnFlag = Flag; }
	void SetState(CPlayerPattern*);
	void SetStateNow(PLAYER_PATTERN pStateNow) { m_PlayerStateNow = pStateNow; }

	//セッター(スタン関連)
	void SetNoDamageTime(int Time) { m_NoDamageTime = Time; }
	void SetStunDir(const D3DXVECTOR3& Stundir) { m_StunDir = Stundir; }
	void SetAttackType(ATTACK_TYPE_PLAYER attackType) { m_AttackType = attackType; }
	void SetDamageType(ATTACK_TYPE_ENEMY DamageType) { m_DamageType = DamageType; }
	void SetIsBeAttackedFlag(bool flag) { m_IsBeAttacked = flag; }

	//他の関数
	inline void AddLife(int Value) {
		m_nLife += Value;
		m_nLife = min(PLAYER_LIFE_MAX, m_nLife);
		if(m_nLife <= 0){
			m_nLife = 0;
		}
	}
	inline void AddStamina(int Value){
		m_nStamina += Value;
		m_nStamina = min(PLAYER_STAMINA_MAX, m_nStamina);
		m_nStamina = max(0, m_nStamina);
	}

	void EnableWeaponHitCircle(bool bWeaponLeft, bool bWeaponRight);
	void DisableWeaponHitCircle(bool bWeaponLeft, bool bWeaponRight);

private:
	int			m_nLife;
	int			m_nStamina;
	bool		m_DeadFlag;
	int			m_DeadTime;
	CWeapon*	m_WeaponLeft;
	CWeapon*	m_WeaponRight;
	int			m_AttackPower;				//攻撃力
	WEAPON_MODE	m_WeaponMode;				//今の武器状態
	bool		m_LockOnFlag;				//ロックオンフラグ
	D3DXVECTOR3	m_LockOnPos;				//ロックオン位置
	CStencilShadow m_Shadow;				//影
	CPlayerPattern* m_State;				//パターンポインター
	CPlayerPatternManager m_StateManager;	//パターンマネージャー
	PLAYER_PATTERN		m_PlayerStateNow;	//プレーヤー現在のステート

	int			m_NoDamageTime;				//無敵時間
	D3DXVECTOR3	m_StunDir;					//攻撃を受ける時のスタン時間
	ATTACK_TYPE_PLAYER		m_AttackType;	//自分の攻撃タイプ
	ATTACK_TYPE_ENEMY		m_DamageType;	//相手の攻撃タイプ
	bool					m_IsBeAttacked;	//攻撃されたかのフラグ
};

#endif