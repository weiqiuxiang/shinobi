#include "CNavMeshManager.h"
#include "CPolygonMath.h"
#include "CModelStorage.h"
#include "CMeshFieldManager.h"

CNavMeshManager::CNavMeshManager()
{
	m_PlayerPositionInMeshNumber = -1;
}

CNavMeshManager::~CNavMeshManager() 
{
	
}

void CNavMeshManager:: UpdateAll(void)
{
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		m_NavMeshArray[i].Update();
	}
}

void CNavMeshManager::DrawAllNavMeshAndConnect(void)
{
	//�i�r���b�V���̕`��
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		if (m_NavMeshArray[i].GetIsUse() == false) {
			continue;
		}
		
		m_NavMeshArray[i].DrawInEditor();
	}

	//�q����̕`��

}

void CNavMeshManager::UninitAll(void)
{
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		m_NavMeshArray[i].Uninit();
	}
}

CNavMesh* CNavMeshManager::GetPointer(int NavMeshID)
{
	bool IsOverRange = NavMeshID < 0 || NavMeshID >= NAVMESH_MAX_NUM;
	if (IsOverRange) {
		_RPT0(0, "[CNavMeshManager.cpp][GetPointer]����NavMeshID�̒l�͔͈͊O\n");
		return nullptr;
	}

	return &m_NavMeshArray[NavMeshID];
}

void CNavMeshManager::SetAllNavMeshNoCheck(void)
{
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		m_NavMeshArray[i].SetState(NAVMESH_NO_CHECK);
	}
}

void CNavMeshManager::NoUseAllNavMesh(void)
{
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		m_NavMeshArray[i].SetIsUse(false);
	}
}

int CNavMeshManager::CalcPlayerInMeshNumber(const D3DXVECTOR3& PlayerPosition)
{
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		if (m_NavMeshArray[i].GetIsUse() == false) {
			continue;
		}

		D3DXVECTOR3 LeftTopPos = m_NavMeshArray[i].GetLeftTopVertexPosition();
		D3DXVECTOR3 RightBottomPos = m_NavMeshArray[i].GetRightBottomVertexPosition();

		bool HitX = PlayerPosition.x >= LeftTopPos.x && PlayerPosition.x <= RightBottomPos.x;
		bool HitZ = PlayerPosition.z >= LeftTopPos.z && PlayerPosition.z <= RightBottomPos.z;

		if(HitX && HitZ){
			m_PlayerPositionInMeshNumber = i;
			return m_PlayerPositionInMeshNumber;
		}
	}

	return -1;
}

//�z�u�����I�u�W�F�N�g��AABB�Ɠ��蔻������A�����i�r���b�V���𐶐�����
//��������:
//MasuNum : �}�X��,MasuNum.x��X�������̃}�X��,MasuNum.y��Z�������̃}�X��
//size : �i�r���b�V���̃T�C�Y
//Dist : �i�r���b�V���̊Ԋu,Dist.x��X�������̊Ԋu,Dist.y��Z�������̊Ԋu
void CNavMeshManager::AutoGeneratedNavMesh(const D3DXVECTOR2& MasuNum, const D3DXVECTOR3& size, const D3DXVECTOR2& Dist)
{
	const int XAxisNum = abs((int)MasuNum.x);
	const int ZAxisNum = abs((int)MasuNum.y);
	const float TotalWidth = XAxisNum * size.x + (XAxisNum - 1) * Dist.x;
	const float TotalHeight = ZAxisNum * size.y + (ZAxisNum - 1) * Dist.y;

	const D3DXVECTOR3 LeftTopPosition = D3DXVECTOR3(-TotalWidth * 0.5f, 0.0f, -TotalHeight * 0.5f);		//������W

	//��XAxisNum * ZAxisNum�𐶐�����
	for (int Z = 0; Z < ZAxisNum; Z++) {
		for (int X = 0; X < XAxisNum; X++) {
			D3DXVECTOR3 MeshPosition;
			MeshPosition.y = 0.0f;
			MeshPosition.x = LeftTopPosition.x + X * (size.x + Dist.x);
			MeshPosition.z = LeftTopPosition.z + Z * (size.y + Dist.y);
			MeshPosition.y = CMeshFieldManager::CalcPosYValueInMeshField(MeshPosition) + 1.0f;

			//���b�V������
			CreateNewNavMesh(MeshPosition, size, true);
		}
	}

	//AABB�Ɠ��蔻�葶�݂��郁�b�V���̕�����t���O��false�ɂ���
	for (int NavMeshNum = 0; NavMeshNum < NAVMESH_MAX_NUM; NavMeshNum++) {
		if (m_NavMeshArray[NavMeshNum].GetIsUse() == false) {
			continue;
		}

		D3DXVECTOR3 CenterPos = m_NavMeshArray[NavMeshNum].GetCenterPosition();
		D3DXVECTOR2 MeshPos = D3DXVECTOR2(CenterPos.x, CenterPos.z);
		D3DXVECTOR2 HitSize = D3DXVECTOR2(size.x, size.y);

		//�S�Ẵ��f���Ƃ�AABB�̓��蔻������
		for (int ModelNum = 0; ModelNum < MODEL_OBJ_MAX; ModelNum++) {
			//���f���|�C���^�擾
			CModel* pModel = CModelStorage::GetObj(ModelNum);
			if (pModel == nullptr) {
				continue;
			}

			//AABB�̏��擾
			D3DXVECTOR2 ModelPos = D3DXVECTOR2(pModel->GetPosition().x, pModel->GetPosition().z);
			D3DXVECTOR2 XValue, YValue, ZValue;
			pModel->GetAABBValue(&XValue, &YValue, &ZValue);
			D3DXVECTOR2 AABBSize = D3DXVECTOR2(fabs(XValue.y - XValue.x), fabs(ZValue.y - ZValue.x));

			bool IsHit = CPolygonMath::HitAABB2D(MeshPos, HitSize, ModelPos, AABBSize);

			//���蔻�肪��������
			if (IsHit == true) {
				m_NavMeshArray[NavMeshNum].SetIsCanWalk(false);
				break;
			}
		}
	}

	//�i�r���b�V���̌q����
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		if (m_NavMeshArray[i].GetIsUse() == false) {
			continue;
		}

		//��s�������Ȃ��ꍇ�A�������Ȃ�
		if (XAxisNum <= 1 || ZAxisNum <= 1) {
			break;
		}

		//�i�r���b�V��
		bool IsTop = (i / XAxisNum) == 0;
		bool IsBottom = (i / XAxisNum) == ZAxisNum - 1;

		//��[�ɂ��邩
		if (IsTop == true) {
			bool IsLeftSign = (i % XAxisNum) == 0;
			bool IsRightSign = (i % XAxisNum) == XAxisNum - 1;
			//���[
			if (IsLeftSign == true) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum + 1]);
			}
			//�E�[
			if (IsRightSign == true) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum - 1]);
			}
			//����
			if ((!IsLeftSign) && (!IsRightSign)) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum + 1]);
			}
			continue;
		}

		//���[�ɂ��邩
		if (IsBottom == true) {
			bool IsLeftSign = (i % XAxisNum) == 0;
			bool IsRightSign = (i % XAxisNum) == XAxisNum - 1;
			//���[
			if (IsLeftSign == true) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum + 1]);
			}
			//�E�[
			if (IsRightSign == true) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum - 1]);
			}
			//����
			if ((!IsLeftSign) && (!IsRightSign)) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum + 1]);
			}
			continue;
		}

		//���Ԃɂ��邩
		if ((!IsTop) && (!IsBottom)) {
			bool IsLeftSign = (i % XAxisNum) == 0;
			bool IsRightSign = (i % XAxisNum) == XAxisNum - 1;

			//���[
			if (IsLeftSign == true) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum ]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum + 1]);
			}
			//�E�[
			if (IsRightSign == true) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum - 1]);
			}
			//����
			if ((!IsLeftSign) && (!IsRightSign)) {
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i - XAxisNum + 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum - 1]);
				m_NavMeshArray[i].AddConnectDoubleDir(&m_NavMeshArray[i + XAxisNum + 1]);
			}
			continue;
		}
	}
}

CNavMesh* CNavMeshManager::CreateNewNavMesh(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, bool CanWalkFlag)
{
	for (int i = 0; i < NAVMESH_MAX_NUM; i++) {
		if (m_NavMeshArray[i].GetIsUse() == false) {
			m_NavMeshArray[i].Init(pos, size, CanWalkFlag,true);
			return &m_NavMeshArray[i];
		}
	}

	_RPT0(0,"[CNavMeshManager.cpp][CreateNewNavMesh]m_NavMeshArray�̗v�f���͏���ɒB����\n");
	return nullptr;
}
