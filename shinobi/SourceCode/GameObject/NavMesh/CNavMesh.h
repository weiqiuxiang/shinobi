#ifndef CNAV_MESH
#define CNAV_MESH

#include "main.h"
#include "CScene3D.h"
#include <list>

typedef enum 
{
	NAVMESH_NO_CHECK= 0,
	NAVMESH_OPEN,
	NAVMESH_CLOSE
}NAVMESH_STATE_ENUM;

class CNavMesh 
{
public:
	CNavMesh();
	~CNavMesh();
	
	bool Init(const D3DXVECTOR3& pos,const D3DXVECTOR3& size,bool CanWalkFlag,bool IsUse);
	void Update(void);
	void DrawInEditor(void);						//エディター内の描画
	void Uninit(void);

	//ゲッター
	D3DXVECTOR3 GetCenterPosition(void) { return m_CenterPos; }							//メッシュの中心座標取得
	D3DXVECTOR3 GetLeftTopVertexPosition(void) { return m_VertexPosition[0]; }			//左上の頂点座標取得
	D3DXVECTOR3 GetRightBottomVertexPosition(void) { return m_VertexPosition[3]; }		//右下の頂点座標取得
	NAVMESH_STATE_ENUM GetState(void) { return m_State; }
	bool GetIsUse(void) { return m_IsUse; }

	//セッター
	void SetCommingMesh(CNavMesh* pNavMesh) { m_CommingMesh = pNavMesh; }
	void SetState(NAVMESH_STATE_ENUM state) { m_State = state; }
	void SetIsCanWalk(bool flag) { m_IsCanWalk = flag; }
	void SetIsUse(bool flag) { m_IsUse = flag; }

	//ポインタリセット
	void ResetCommingMesh(void) { m_CommingMesh = nullptr; }
	void ResetConnectMesh(void) { m_NavMeshPointerList.clear(); }

	//他のメッシュと繋がる時使用
	void AddConnectDoubleDir(CNavMesh*);			//双方向繋がり
	void AddConnectSingleDir(CNavMesh*);			//単方向繋がり

private:
	CScene3D				m_Polygon3D;			//描画ユニット
	D3DXVECTOR3				m_CenterPos;			//メッシュ中心座標
	D3DXVECTOR3				m_VertexPosition[4];	//頂点座標
	CNavMesh*				m_CommingMesh;			//どこから来たを記録するためのポインタ(経路探索時,使用)
	std::list<CNavMesh*>	m_NavMeshPointerList;	//繋がり用のポインタリスト
	NAVMESH_STATE_ENUM		m_State;				//メッシュの状態(未チェック、open、closeの3つの状態)
	bool					m_IsCanWalk;			//歩けるメッシュフラグ
	bool					m_IsUse;				//使用フラグ
	RGBA					m_RGBA;					//色
};

#endif