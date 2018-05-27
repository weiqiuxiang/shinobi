//説明 : 敵にロックオンした時の表示
#pragma once
#ifndef LOCK_ON_EF_H_
#define LOCK_ON_EF_H_

#include "main.h"
#include "CScene2D.h"
#include "CModel.h"

class CLockOnEF
{
public:
	CLockOnEF();
	~CLockOnEF();

	bool Init(const D3DXVECTOR3& size,const char*TexFilePass);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//セッター
	void SetPos(const D3DXVECTOR3 &pos) { m_posWorld = pos; }
	void SetUse(bool Use) { m_bUse = Use; }

private:
	int			m_Alpha;		//アルファ
	float		m_scl;			//拡大倍数
	D3DXVECTOR3	m_size;			//サイズ
	D3DXVECTOR3	m_posWorld;		//世界座標位置
	CScene2D	m_CScene2D;		//描画ユニット
	bool		m_bUse;			//使用フラグ
};

#endif