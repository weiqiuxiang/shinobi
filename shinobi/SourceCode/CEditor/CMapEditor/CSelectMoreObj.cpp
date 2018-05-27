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
	//マウスウィンドウ初期化
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

	//マウスが押し続ける状態なら、範囲内のモデルオブジェクトを数える
	if (IsPressMouseLeft == true) {
		//数をクリア
		m_SelectObjAmount = 0;

		//必要データ取得
		D3DXMATRIX MtxVPVP = CCameraManager::GetActiveCameraMtxVPVP();
		D3DXVECTOR2 StartPoint = m_bMouseWindow.GetStartPoint();
		D3DXVECTOR2 EndPoint = m_bMouseWindow.GetEndPoint();

		//選択範囲内のオブジェクトをリストに書き込む
		for (int i = 0; i < MODEL_OBJ_MAX; i++) {
			CModel* pModel = CModelStorage::GetObj(i);
			if (pModel == nullptr) {
				continue;
			}

			D3DXVECTOR3 ScreenPos;
			D3DXVec3TransformCoord(&ScreenPos, &pModel->GetPosition(), &MtxVPVP);

			bool HitXDir = (ScreenPos.x > StartPoint.x) && (ScreenPos.x < EndPoint.x);
			bool HitYDir = (ScreenPos.y > StartPoint.y) && (ScreenPos.y < EndPoint.y);

			//選択範囲内のオブジェクトは記録される
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
		_RPT0(0,"[CSelectMoreObj.cpp][GetSelectObjNum]引数IDは範囲外\n");
		return 0;
	}

	return m_SelectObjNumber[ID];
}