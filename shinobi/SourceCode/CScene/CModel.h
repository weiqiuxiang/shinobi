#pragma once
#ifndef _CMODEL_H_
#define _CMODEL_H_

#include "main.h"
#include "CScene.h"
#include "CCube.h"

class CModel : public CScene
{
public:
	typedef enum {
		DRAW_USE_MTRL_DEFALUT = 0,			//読み込みの時モデル自身のマテリアルで描画
		DRAW_USE_MTRL_CUSTOMIZE,			//設定したのカスタマイズのマテリアルで描画
		DRAW_USE_MTRL_MAX
	}DRAW_TYPE;

	//メンバ関数
	CModel();
	~CModel();

	HRESULT Init(const SRP &PosRotScl,const char* filePass);
	void Update(void);
	void Draw(void);
	void DrawAABBTest(void);
	void DrawDefault(void);
	void DrawMtrl(void);
	void DrawUseMtrl(const D3DMATERIAL9& Mtrl);
	void DrawOnDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(const D3DXMATRIX& world);
	void DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture);
	void Uninit(void);

	//ゲッター
	D3DXMATRIX GetPoseMtx(void) const { return m_PoseMtx; }
	D3DXMATRIX GetResultMtx(void) const;
	D3DXMATRIX GetWorldPoseMtx(void) const;
	D3DXVECTOR3 GetRot(void) const { return m_rot; }
	MODEL_ID GetModelID(void) const { return m_ModelID; }
	SRP GetWorldSRP(void) const {
		SRP SclRotPos = { m_pos,m_rot,m_scl };
		return SclRotPos; }
	D3DXVECTOR3 GetDirVecXZ(void) const { return m_DirVecXZ; }
	LPD3DXMESH GetMesh(void) const;
	bool GetHitMeshFlag(void) const { return m_IsHitMesh; }
	int GetUseID(void) { return m_UseID; }

	//セッター
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }
	void SetPoseMtx(const D3DXMATRIX& PoseMtx) { m_PoseMtx = PoseMtx; }
	void SetMtxSetting(void);
	void SetMtxSettingPose(void);
	void SetMaterial(const D3DMATERIAL9& Mtrl) { m_Mtrl = Mtrl; }		//マテリアル指定描画の時のマテリアルの設定
	void SetDrawType(DRAW_TYPE);
	void SetScl(const D3DXVECTOR3& scl) { m_scl = scl; }
	void SetHitMeshFlag(bool flag) { m_IsHitMesh = flag; }
	void SetIsAlwaysCalcAABBFlag(bool flag) { m_IsAlwaysCalcAABB = flag; }
	void SetAlpha(float Alpha);
	void SetUseID(int ID) { m_UseID = ID; }

	//他の関数
	void AddRotateY(float AddAngle) { m_rot.y += AddAngle; }
	void AddPosX(float AddValue) { m_pos.x += AddValue; }
	void AddPosZ(float AddValue) { m_pos.z += AddValue; }

	void CalcAABB(void);			//AABBを計算する
	void GetAABBValue(D3DXVECTOR2 *OutMinXMaxX, D3DXVECTOR2 *OutMinYMaxY, D3DXVECTOR2 *OutMinZMaxZ);

private:
	//メンバ変数
	D3DXVECTOR3		m_rot;
	D3DXVECTOR3		m_scl;
	MODEL_ID		m_ModelID;
	D3DXVECTOR3		m_DirVecXZ;				//XZ方向の向きベクトル
	D3DXMATRIX		m_PoseMtx;				//姿勢マトリクス(回転行列である,拡大縮小と移動は入っていない)
	D3DXMATRIX		m_Result;				//リザルト
	D3DMATERIAL9	m_Mtrl;					//マテリアル
	DRAW_TYPE		m_DrawType;				//描画の仕方
	bool			m_IsHitMesh;			//trueだったらメッシュとの当り判定あり
	int				m_UseID;				//汎用ID

	//AABB変数
	bool			m_IsAlwaysCalcAABB;		//AABBの計算する常に行うフラグ
	D3DXVECTOR2		m_AABBMinXMaxX;			//AABBのX方向の最小値と最大値(m_MinXMaxX.xは最小値)
	D3DXVECTOR2		m_AABBMinYMaxY;			//AABBのY方向の最小値と最大値(m_MinYMaxY.xは最小値)
	D3DXVECTOR2		m_AABBMinZMaxZ;			//AABBのZ方向の最小値と最大値(m_MinZMaxZ.xは最小値)

	//AABBの描画用キューブ
	CCube			m_Cube;
};

#endif