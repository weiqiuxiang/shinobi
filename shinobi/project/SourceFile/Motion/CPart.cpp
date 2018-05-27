#include "CPart.h"
#include "CRenderer.h"
#include <algorithm>

CPart::CPart()
{
	D3DXMatrixIdentity(&m_MtxNow);
	D3DXMatrixIdentity(&m_MtxOld);
	m_pParent = nullptr;
	m_pChildrenList.clear();
	strcpy_s(m_Name, PART_NAME_FONT_MAX_NUM,"NO_NAME");

	m_SRPOffset.pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_SRPOffset.rot = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_SRPOffset.scl = D3DXVECTOR3(1.0f,1.0f,1.0f);
}

CPart::~CPart()
{
	m_Model.Uninit();
}

void CPart::Uninit(void)
{
	m_Model.Uninit();
}

void CPart::Update(void)
{
	m_Model.Update();
}

void CPart::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxNow);
	m_Model.Draw();
}

void CPart::DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	m_Model.DrawOnDepthMap(m_MtxNow, LightView, LightProje);
}

void CPart::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture) 
{
	m_Model.DrawUseDepthMap(m_MtxNow, LightView, LightProje, DepthTexture);
}

void CPart::DrawSpecular(void)
{
	m_Model.DrawSpecular(m_MtxNow);
}

void CPart::DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture)
{
	m_Model.DrawMotionBlur(m_MtxNow, m_MtxOld,OriginalTexture);
}

bool CPart::IsParent(CPart* pParent)
{
	if (pParent == m_pParent) return true;
	return false;
}

bool CPart::IsChild(CPart* pChild)
{
	auto result = std::find(m_pChildrenList.begin(), m_pChildrenList.end(), pChild);
	if (result == m_pChildrenList.end()) return false;
	return true;
}

bool CPart::IsLowerHierarchy(CPart* pChild)
{
	bool bLowerHierarchy = false;
	auto result = std::find(m_pChildrenList.begin(), m_pChildrenList.end(), pChild);
	if (result == m_pChildrenList.end()) {				//子供ではないなら子供の子供かどうか検索
		auto it = m_pChildrenList.begin();
		for (; it != m_pChildrenList.end(); ++it) {
			bLowerHierarchy = (*it)->IsLowerHierarchy(pChild);		//再帰
			if (true == bLowerHierarchy) return bLowerHierarchy;	//見つかったらtrueに返す
		}
	}
	else bLowerHierarchy = true;
	return bLowerHierarchy;
}