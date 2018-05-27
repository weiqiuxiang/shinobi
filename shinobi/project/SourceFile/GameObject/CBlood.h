//血しぶき用パーティクル
#ifndef CBLOOD_H_
#define CBLOOD_H_

#include "main.h"
#include "CScene3D.h"

class CBlood : public CScene3D
{
public:
	CBlood();
	~CBlood();

	HRESULT Init(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const char *filePass);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//セッター
	void SetGravityAcc(float acc) { m_GravityAcc = acc; }
	void SetTime(int Time) { m_Time = Time; }
	void SetIsUse(bool bUse) { m_IsUse = bUse; }
	void SetCoffi(float Coffi) { m_Coffi = Coffi; }

	//ゲッター
	bool GetIsUse(void) { return m_IsUse; }

	//他の関数
	void AddPower(const D3DXVECTOR3& power) { m_Power += power; }

private:
	D3DXVECTOR3		m_Power;		//外力
	D3DXVECTOR3		m_Speed;		//速度
	float			m_Coffi;		//抵抗
	int				m_Time;			//消える時間
	float			m_GravityAcc;	//重力加速度
	bool			m_IsUse;		//使用フラグ
};

#endif