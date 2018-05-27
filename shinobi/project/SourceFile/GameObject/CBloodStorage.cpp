#include "CBloodStorage.h"
#include "CScene3D.h"
#include "CRenderer.h"
#include <crtdbg.h>

CBlood CBloodStorage::m_Obj[BLOOD_AMOUNT_MAX];

void CBloodStorage::InitAll(void)
{
	for (int ID = 0; ID < BLOOD_AMOUNT_MAX; ID++){
		if (m_Obj[ID].GetIsUse() == true){
			m_Obj[ID].SetIsUse(false);
		}
	}
}

void CBloodStorage::UninitAll(void)
{
	for (int ID = 0; ID < BLOOD_AMOUNT_MAX; ID++) {
		m_Obj[ID].Uninit();
	}
}

void CBloodStorage::UpdateAll(void)
{
	for (int ID = 0; ID < BLOOD_AMOUNT_MAX; ID++){
		if (m_Obj[ID].GetIsUse() == true){
			m_Obj[ID].Update();
		}
	}
}

void CBloodStorage::DrawAll(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	for (int ID = 0; ID < BLOOD_AMOUNT_MAX; ID++) {
		if (m_Obj[ID].GetIsUse() == true) {
			m_Obj[ID].Draw();
		}
	}

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

CBlood* CBloodStorage::CreateObject(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &pSize, const D3DXVECTOR3& power,int Time, float fCoffi, const char* Texfilepass)
{
	for (int ID = 0; ID < BLOOD_AMOUNT_MAX; ID++)
	{
		if (m_Obj[ID].GetIsUse() == false)
		{
			SRP srp;
			srp.pos = pos;
			srp.rot = D3DXVECTOR3(0, 0, 0);
			srp.scl = D3DXVECTOR3(1, 1, 1);
			m_Obj[ID].Init(srp, pSize, Texfilepass);
			m_Obj[ID].SetIsUse(true);
			m_Obj[ID].SetDrawType(CScene3D::TYPE_BILLBOARD);
			m_Obj[ID].AddPower(power);
			m_Obj[ID].SetTime(Time);
			m_Obj[ID].SetCoffi(fCoffi);
			return &m_Obj[ID];
		}
	}

	_RPT0(0, "[CBloodStorage.cpp][CreateObject]CBloodStorage‚Í‚»‚êˆÈãCBlood‚ðì¬‚·‚é‚±‚Æ‚ª‚Å‚«‚È‚¢\n");
	return nullptr;
}