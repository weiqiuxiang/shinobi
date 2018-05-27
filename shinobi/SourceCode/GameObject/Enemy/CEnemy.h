#pragma once
#ifndef _CENEMY_H_
#define _CENEMY_H_

//村人(敵)クラス
#include "CHuman.h"
#include "CPathPoint.h"
#include <list>
#include <algorithm>
#include "common.h"

class CPlayer;
class CEnemyControl;
class CEnemyState;
class CEnemyStateManager;

class CEnemy : public CHuman
{
public:
	typedef enum 
	{
		TYPE_DOG = 0,			//犬
		TYPE_VILLAGER,			//村人
		TYPE_BIGMAN,			//大男
	}ENEMY_TYPE;

	CEnemy();
	~CEnemy();

	virtual void Init(ENEMY_TYPE type,const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void UpdateEditor(void);
	virtual void Draw(void);
	virtual void DrawPath(void);
	virtual void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawLineBetweenPathPoint(void);

	//セッター
	void SetPlayer(CPlayer* pPlayer);
	void SetPathNumberNow(CPathPoint* pPathPoint) { m_PathNumberNow = pPathPoint; }
	void SetFlag_IsPathPointIsAscendingOrder(bool Flag) { m_IsPathPointIsAscendingOrder = Flag; }	//trueなら昇順を表す

	//セッター(スタン関連)
	void SetNoDamageTime(int Time) { m_NoDamageTime = Time; }
	void SetStunDir(const D3DXVECTOR3& Stundir) { m_StunDir = Stundir; }
	void SetAttackType(ATTACK_TYPE_ENEMY attackType) { m_AttackType = attackType; }
	void SetDamageType(ATTACK_TYPE_PLAYER DamageType) { m_DamageType = DamageType; }
	void SetIsBeAttackedFlag(bool flag) { m_IsBeAttacked = flag; }
	void SetUseID(int ID) { m_UseID = ID; }

	//ゲッター
	int GetLife(void) { return m_nLife; }
	bool GetDeadFlag(void) { return m_DeadFlag; }
	CPathPoint* GetPathNumberNow(void) { return m_PathNumberNow; }
	CPlayer* GetPlayer(void);
	std::list<CPathPoint*>::iterator GetPathPointListBegin(void) { return m_PathPoints.begin(); }
	std::list<CPathPoint*>::iterator GetPathPointListEnd(void) { return m_PathPoints.end(); }
	std::list<CPathPoint*>::iterator GetPathPointListNumber(int Number);
	
	int GetPathPointListSize(void) { return m_PathPoints.size(); }
	std::list<CPathPoint*>::iterator GetPathPointListPrev(CPathPoint* pPoint, int Num);			//今のポインタの前
	std::list<CPathPoint*>::iterator GetPathPointListNext(CPathPoint* pPoint, int Num);			//今のポインタの後ろ
	bool GetFlag_IsPathPointIsAscendingOrder(void) { return m_IsPathPointIsAscendingOrder; }

	//ゲッター(スタン関連)
	int GetNoDamageTime(void) { return m_NoDamageTime; }
	D3DXVECTOR3 GetStunDir(void) { return m_StunDir; }
	ATTACK_TYPE_ENEMY GetAttackType(void) { return m_AttackType; }
	ATTACK_TYPE_PLAYER GetDamageType(void) { return m_DamageType; }
	bool GetIsBeAttackedFlag(void) { return m_IsBeAttacked; }
	ENEMY_TYPE GetEnemyType(void) { return m_EnemyType; }
	int GetUseID(void) { return m_UseID; }

	//他の関数
	void AddLife(int nLife) {
		m_nLife += nLife; 
		m_nLife = max(0, m_nLife);
	}
	void AddPathPoint(const D3DXVECTOR3& pos, int StopFrame);	//巡回ルートポイントの追加
	void DeletePathPointUsePointer(CPathPoint* pPath);			//渡されたポインタに基づく削除
	void InsertPathPointUsePointer(const D3DXVECTOR3& pos, int StopFrame, CPathPoint* pPath);
	void DeletePathPoint(void);									//巡回ルートポイントリストの末尾から要素を削除
	
	bool FindElementInPathPointList(CPathPoint* pPathPoint) {
		auto Found = std::find(m_PathPoints.begin(), m_PathPoints.end(), pPathPoint);
		if (Found == m_PathPoints.end()) return false;
		return true;
	}

	void EnableHitCircle(void);			//攻撃当り判定球を有効に
	void DisableHitCircle(void);		//攻撃当り判定球を無効に

private:
	//メンバ関数
	void InitPathPoint(void);
	void UpdatePathPoint(void);
	void DrawPathPoint(void);
	void UninitPathPoint(void);

	void EnemyTypeToSetColliCircle(void);		//敵のタイプによって当り判定球の設置

	int						m_nLife;
	bool					m_DeadFlag;
	CEnemyControl *			m_Controller;
	std::list<CPathPoint*>	m_PathPoints;					//巡回ルートのポイント
	CPathPoint*				m_PathNumberNow;
	bool					m_IsPathPointIsAscendingOrder;	//巡回ポイントは昇順ならtrue
	ENEMY_TYPE				m_EnemyType;					//敵タイプ
	D3DXVECTOR3				m_InitPos;						//初期座標

	//ダメージ系変数
	int						m_NoDamageTime;					//ダメージを通らない時間
	D3DXVECTOR3				m_StunDir;						//攻撃を受ける時のスタン時間
	ATTACK_TYPE_PLAYER		m_DamageType;					//相手の攻撃タイプ
	ATTACK_TYPE_ENEMY		m_AttackType;					//自分の攻撃タイプ
	bool					m_IsBeAttacked;					//攻撃されたかのフラグ

	CEnemyState*			m_pEnemyState;					//ステート
	CEnemyStateManager*		m_pEnemyStateManager;			//ステートマネジャー
	int						m_UseID;						//使用されるためのID
};

#endif