#pragma once
#ifndef _CWEAPON_H_
#define _CWEAPON_H_

#include "CHuman.h"

class CWeapon : public CHuman
{
public:
	typedef enum
	{
		MOTION_DEFAULT,			//デフォルト状態
		MOTION_CHANGE			//変化後の状態
	}MOTION_MODE;

	CWeapon();
	~CWeapon();

	virtual void Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	
	//ゲッター
	int GetOtherParentID(void) { return m_OtherParentID; }
	D3DXVECTOR3 GetOffset(void) { return m_posOffset; }
	D3DXMATRIX GetPartMtx(int PartID) { return m_ModelMotion.GetPartMtx(PartID); }

	//セッター
	void SetParentID(int ID) { m_OtherParentID = ID; }
	void SetOffset(const D3DXVECTOR3 &Offset) { m_posOffset = Offset; }

	//他の関数
	void EnableHitCircle(void);
	void DisableHitCircle(void);

private:
	D3DXVECTOR3 m_posOffset;					//オフセット
	int m_OtherParentID;						//別のオブジェクトのパーツのID
};

#endif