#include "CSelectMoreObj.h"
#include "MousePosition.h"
#include "SceneTransition.h"
#include "CModelStorage.h"
#include "CCameraManager.h"
#include <crtdbg.h>

CSelectMoreObj::CSelectMoreObj() 
{
	m_SelectObjAmount = 0;
}

CSelectMoreObj::~CSelectMoreObj() 
{

}

bool CSelectMoreObj::Init(void) 
{
	//�}�E�X�E�B���h�E������
	if (m_bMouseWindow.Init({ 156,167,226,150 }, 10.0f) == false) {
		return false;
	}

	m_SelectObjAmount = 0;

	return true;
}

void CSelectMoreObj::SelectObjs(void)
{
	bool IsPressMouseLeft = CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT);

	D3DXVECTOR3 MousePosInWindow = CMousePosition::GetMousePos();
	m_bMouseWindow.UpdateDragWindow(IsPressMouseLeft, D3DXVECTOR2(MousePosInWindow.x, MousePosInWindow.y));

	//�}�E�X�������������ԂȂ�A�͈͓��̃��f���I�u�W�F�N�g�𐔂���
	if (IsPressMouseLeft == true) {
		//�����N���A
		m_SelectObjAmount = 0;

		//�K�v�f�[�^�擾
		D3DXMATRIX MtxVPVP = CCameraManager::GetActiveCameraMtxVPVP();
		D3DXVECTOR2 StartPoint = m_bMouseWindow.GetStartPoint();
		D3DXVECTOR2 EndPoint = m_bMouseWindow.GetEndPoint();

		//�I��͈͓��̃I�u�W�F�N�g�����X�g�ɏ�������
		for (int i = 0; i < MODEL_OBJ_MAX; i++) {
			CModel* pModel = CModelStorage::GetObj(i);
			if (pModel == nullptr) {
				continue;
			}

			D3DXVECTOR3 ScreenPos;
			D3DXVec3TransformCoord(&ScreenPos, &pModel->GetPosition(), &MtxVPVP);

			bool HitXDir = (ScreenPos.x > StartPoint.x) && (ScreenPos.x < EndPoint.x);
			bool HitYDir = (ScreenPos.y > StartPoint.y) && (ScreenPos.y < EndPoint.y);

			//�I��͈͓��̃I�u�W�F�N�g�͋L�^�����
			if (HitXDir && HitYDir) {
				m_SelectObjNumber[m_SelectObjAmount] = i;
				m_SelectObjAmount++;
			}
		}
	}
}

void CSelectMoreObj::Draw(void) 
{
	m_bMouseWindow.DrawWindow();
}

void CSelectMoreObj::Uninit(void) 
{
	m_bMouseWindow.Uninit();
}

int CSelectMoreObj::GetSelectObjNum(int ID)
{
	if (ID < 0 || ID >= MODEL_OBJ_MAX) {
		_RPT0(0,"[CSelectMoreObj.cpp][GetSelectObjNum]����ID�͔͈͊O\n");
		return 0;
	}

	return m_SelectObjNumber[ID];
}