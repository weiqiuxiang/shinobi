#include "CCoordinate.h"
#include "CRenderer.h"

CCoordinate::CCoordinate() 
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_MoveDist = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_DrawUse = false;
}

CCoordinate::~CCoordinate() 
{
	for (int i = 0; i < OBJ_MAX; i++) {
		m_Obj[i].Uninit();
	}
}

void CCoordinate::Init(const D3DXVECTOR3& pos)  
{
	SRP SRP;
	SRP.pos = pos;
	SRP.rot = D3DXVECTOR3(0.0f,0.0f,0.0f);
	SRP.scl = D3DXVECTOR3(1.0f,1.0f,1.0f);
	m_Obj[OBJ_X_AXIS].Init(SRP, COORDINATE_XAXIS_MODEL);
	m_Obj[OBJ_Y_AXIS].Init(SRP, COORDINATE_YAXIS_MODEL);
	m_Obj[OBJ_Z_AXIS].Init(SRP, COORDINATE_ZAXIS_MODEL);
	m_Obj[OBJ_BOX].Init(SRP, COORDINATE_BOX_MODEL);

	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_MoveDist = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_DrawUse = false;
}

void CCoordinate::Uninit(void) 
{
	for (int i = 0; i < OBJ_MAX; i++) {
		m_Obj[i].Uninit();
	}
}

void CCoordinate::Update(void) 
{
	m_MoveDist = m_pos - m_posOld;
	m_posOld = m_pos;

	for (int i = 0; i < OBJ_MAX; i++) {
		m_Obj[i].SetPosition(m_pos);
		m_Obj[i].SetScl(m_scl);
		m_Obj[i].Update();
	}
}

void CCoordinate::Draw(void) 
{
	if (false == m_DrawUse) {
		return;
	}

	//pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);				//ライトオフ
	for (int i = 0; i < OBJ_MAX; i++) {
		m_Obj[i].SetMtxSetting();
		m_Obj[i].Draw();
	}
	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);				//ライトオン
}

void CCoordinate::SetDrawUse(bool DrawUse)
{
	m_DrawUse = DrawUse;
	m_posOld = m_pos;
	m_MoveDist = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}