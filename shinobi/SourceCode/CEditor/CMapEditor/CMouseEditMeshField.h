//ファイル名 : CMouseInMeshField
//説明 : 画面上のマウス座標を3Dメッシュ上のマウス座標に変換
#ifndef CMOUSE_IN_MESHFIELD_H_
#define CMOUSE_IN_MESHFIELD_H_

#include "main.h"
#include "CScene3D.h"
#include "CMeshField.h"

class CMouseEditMeshField
{
public:
	CMouseEditMeshField();
	~CMouseEditMeshField() {}

	HRESULT Init(float fRadius,RGBA rgba);
	void Uninit(void);
	void Draw(void);

	//ゲッター
	D3DXVECTOR3 GetMousePosInMesh(void) { return m_Cscene3D.GetPosition(); }           //マウスがメッシュでの座標を返す
	D3DXVECTOR3 GetMouseRayStartPoint(void) { return m_StartPoint; }
	D3DXVECTOR3 GetMouseRayEndPoint(void) { return m_EndPoint; }

	//他の関数
	void CalcMousePositionInMesh(void);
	void EditHitFloor(void);

private:
	//メンバ関数
	void CalcLine(D3DXVECTOR3* pStartPoint, D3DXVECTOR3* pEndPoint);						//ライン計算
	D3DXVECTOR3 HitAllMesh(const D3DXVECTOR3& StartPoint,const D3DXVECTOR3& pEndPoint);		//マウスとメッシュのすべての当り判定
	D3DXVECTOR3 HitOneMesh(CMeshField *pMesh,const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint);
	void ChangeRange(void);               //メッシュに対する影響半径の変更

	//メンバ変数
	CScene3D		m_Cscene3D;
	float			m_fRadius;
	int				m_nHitMeshID;
	D3DXVECTOR3		m_StartPoint;
	D3DXVECTOR3		m_EndPoint;
};

#endif