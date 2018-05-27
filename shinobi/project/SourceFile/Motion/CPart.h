#pragma once
#ifndef CPART_H_
#define CPART_H_

#include "main.h"
#include "CModel.h"
#include <list>
#include <string.h>

#define PART_NAME_FONT_MAX_NUM (128)		//パーツのフォントの最大数

class CPart
{
public:
	CPart();
	~CPart();

	//Init,Uninit,Update,Draw関数
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(void);
	void DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture);

	//セッター
	void SetNowMtx(D3DXMATRIX& resultMtx) { m_MtxNow = resultMtx; }
	inline void SetParent(CPart *pParent) {
		if (this != pParent) {
			m_pParent = pParent;
		}
	}
	void SetName(const char* Name) { strcpy_s(m_Name, PART_NAME_FONT_MAX_NUM, Name); }
	void AddChildren(CPart *pChild) {m_pChildrenList.push_back(pChild); }

	//ゲッター
	CPart* GetParent(void) const { return m_pParent; }
	auto GetChildrenListBegin(void) const { return m_pChildrenList.begin(); }
	auto GetChildrenListEnd(void) const { return m_pChildrenList.end(); }
	LPD3DXMESH GetMesh(void) const { return m_Model.GetMesh(); }
	D3DXMATRIX GetNowMtx(void) const { return m_MtxNow; }
	SRP GetSRPNow(void) const { return m_SRPNow; }
	const char* GetName(void) const { return m_Name; }

	//他の関数
	bool IsParent(CPart*);					//親かどうか
	bool IsChild(CPart*);					//子供かどうか
	bool IsLowerHierarchy(CPart*);			//自分より下の階層かどうか

	//メンバ変数
	CModel				m_Model;			//描画ユニット
	SRP					m_SRPOffset;		//オフセット
	D3DXMATRIX			m_MtxNow;			//今のマトリクス
	D3DXMATRIX			m_MtxOld;			//以前のマトリクス
	SRP					m_SRPNow;			//今のフレームのSRP
	SRP					m_SRPBlendOld;		//フレームの実行状況を保存,別のモーションを実行する際,blendingする
	int					m_PartID;			//自分BのID
	int					m_ParentID;			//親のID
	char				m_Name[PART_NAME_FONT_MAX_NUM];	//名前
private:
	CPart*				m_pParent;			//親ポインタ
	std::list<CPart*>	m_pChildrenList;	//子供ポインタ
};

#endif