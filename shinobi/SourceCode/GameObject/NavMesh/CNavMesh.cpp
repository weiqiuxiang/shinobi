#include "CNavMesh.h"
#include "CRenderer.h"
#include <crtdbg.h>

CNavMesh::CNavMesh()
{
	m_CenterPos = D3DXVECTOR3(0,0,0);
	m_NavMeshPointerList.clear();
	m_CommingMesh = nullptr;

	m_State = NAVMESH_NO_CHECK;
	m_IsCanWalk = false;
	m_IsUse = false;

	//頂点座標初期化
	for (int i = 0; i < 4; i++) {
		m_VertexPosition[i] = D3DXVECTOR3(0, 0, 0);
	}

	m_RGBA = { 50,50,200,150 };
}

CNavMesh::~CNavMesh()
{
	
}


bool CNavMesh::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, bool CanWalkFlag, bool IsUse)
{
	//座標代入
	m_CenterPos = pos;
	D3DXVECTOR3 halfSize = size * 0.5f;
	m_VertexPosition[0] = D3DXVECTOR3(pos.x - halfSize.x, pos.y - halfSize.y, 0.0f);
	m_VertexPosition[1] = D3DXVECTOR3(pos.x + halfSize.x, pos.y - halfSize.y, 0.0f);
	m_VertexPosition[2] = D3DXVECTOR3(pos.x - halfSize.x, pos.y + halfSize.y, 0.0f);
	m_VertexPosition[3] = D3DXVECTOR3(pos.x + halfSize.x, pos.y + halfSize.y, 0.0f);

	SRP srp;
	srp.pos = m_CenterPos;
	srp.rot = D3DXVECTOR3(D3DX_PI * 0.5,0.0f,0.0f);
	srp.scl = D3DXVECTOR3(1.0f,1.0f,1.0f);

	//描画ユニットの設定
	m_Polygon3D.Init(srp,size, nullptr);
	m_Polygon3D.SetDrawType(CScene3D::TYPE_NORMAL);
	m_Polygon3D.SetRGBA(m_RGBA);

	//歩けるメッシュかをチェック
	m_IsCanWalk = CanWalkFlag;
	m_State = NAVMESH_NO_CHECK;
	m_IsUse = IsUse;

	return true;
}

void CNavMesh::Update(void)
{
	if (m_IsUse == false) {
		return;
	}

	//色設定
	if (m_IsCanWalk == false) {
		m_RGBA = { 50,50,50,150 };
	}
	else {
		m_RGBA = { 50,200,50,150 };
	}
	m_Polygon3D.SetRGBA(m_RGBA);

	//描画ユニット更新
	m_Polygon3D.Update();
}

void CNavMesh::DrawInEditor(void)
{
	if (m_IsUse == false) {
		return;
	}

	m_Polygon3D.Draw();
}

void CNavMesh::Uninit(void)
{
	m_NavMeshPointerList.clear();
	m_CommingMesh = nullptr;
	m_Polygon3D.Uninit();
}

void CNavMesh::AddConnectDoubleDir(CNavMesh* pNavMesh)
{
	//ヌルチェック
	if (pNavMesh == nullptr) {
		_RPT0(0,"[CNavMesh.cpp][AddConnectDoubleDir]引数pNavMeshはヌル\n");
		return;
	}

	m_NavMeshPointerList.push_back(pNavMesh);
	pNavMesh->AddConnectSingleDir(this);
}

void CNavMesh::AddConnectSingleDir(CNavMesh* pNavMesh)
{
	//ヌルチェック
	if (pNavMesh == nullptr) {
		_RPT0(0, "[CNavMesh.cpp][AddConnectSingleDir]引数pNavMeshはヌル\n");
		return;
	}

	m_NavMeshPointerList.push_back(pNavMesh);
}