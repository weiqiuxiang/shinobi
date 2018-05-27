#include "CScene3DStorage.h"
#include "CCameraManager.h"

CScene3D *CScene3DStorage::m_Obj[CSCENE3D_OBJ_MAX] = {};
CScene3DStorage::Z_SORT CScene3DStorage::m_ZSort[CSCENE3D_OBJ_MAX] = {};             //Z�\�[�h�̏��Ԋi�[�s��
int CScene3DStorage::m_TotalObj = 0;

#define ID_IS_nullptr (-1)

void CScene3DStorage::InitAll(void)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
		}
		m_Obj[ID] = nullptr;
	}

	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}
}

void CScene3DStorage::UninitAll(void)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
			m_Obj[ID] = nullptr;
		}
	}
}

void CScene3DStorage::UpdateAll(void)
{

	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr) m_Obj[ID]->Update();
	}
}

void CScene3DStorage::DrawAll(void)
{
	ZSort();             //�I�u�W�F�N�g�S��Z�\�[�g����

	for (int ID = 0; ID < m_TotalObj; ID++)
	{
		int k = m_TotalObj - 1 - ID;
		if (m_Obj[m_ZSort[k].ID] != nullptr)
		{
			m_Obj[m_ZSort[k].ID]->Draw();
		}
	}
}

CScene3D* CScene3DStorage::CreateObject(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const CScene3D::CSCENE3D_TYPE CScene3DType, const char* Texfilepass)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] == nullptr)
		{
			m_Obj[ID] = new CScene3D();
			m_Obj[ID]->Init(worldMtx, pSize, Texfilepass);
			m_Obj[ID]->SetDrawType(CScene3DType);
			return m_Obj[ID];
		}
	}
	return nullptr;
}

void CScene3DStorage::Delete(CScene3D * pScene3D)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			if (m_Obj[ID] != pScene3D) continue;
			m_Obj[ID]->Uninit();
			m_Obj[ID] = nullptr;
		}
	}
}

CScene3D* CScene3DStorage::GetObj(int ID)
{
	bool OverRange = (ID < 0) || (ID > CSCENE3D_OBJ_MAX);
	if (OverRange) {
		MessageBox(nullptr, "CScene3D OBJ ID���͈͊O,CScene3D OBJ�擾�s�\", "�G���[", MB_OK | MB_ICONHAND);
		return nullptr;
	}
	return m_Obj[ID];
}
void CScene3DStorage::ZSort(void)
{
	D3DXMATRIX ViewMtx = CCameraManager::GetActiveCameraViewMtx();
	D3DXMATRIX ProjeMtx = CCameraManager::GetActiveCameraProjeMtx();
	D3DXMATRIX ViewProje = ViewMtx* ProjeMtx;                       //�r���[�ˉe�s�������
	m_TotalObj = 0;                                                   //�I�u�W�F�N�g����

	//�z��i�[�ɏ�����
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}

	//�i�[�J�n
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (nullptr != m_Obj[ID])                    //nullptr�`�F�b�N
		{
			D3DXVECTOR3 POS = D3DXVECTOR3(0.0f,0.0f,0.0f);            //���[�J�����W
			const D3DXMATRIX* pWorldMtx = m_Obj[ID]->GetWorldMtx();   //���[���h�s��擾
			D3DXMATRIX WorldViewProje = (*pWorldMtx) * ViewProje;     //���[���h�r���[�ˉe�s������߂�
			D3DXVec3TransformCoord(&POS,&POS, &WorldViewProje);       //�ϊ���̒��_������

			//Z�o�b�t�@�̒l��ID����
			for (int SortNum = 0; SortNum < CSCENE3D_OBJ_MAX; SortNum++)
			{
				if (ID_IS_nullptr != m_ZSort[SortNum].ID) continue;
				m_ZSort[SortNum].ID = ID;
				m_ZSort[SortNum].ZValue = POS.z;
				m_TotalObj++;
				break;
			}
		}
	}

	//�\�[�g�J�n
	QuickSort(0, m_TotalObj -1);
}

void CScene3DStorage::QuickSort(int nLeftID, int nRightID)
{
	//---------------------------
	//�ϐ��錾
	//---------------------------
	int nCnt = 0;           //�J�E���g
	int nMiddleID = (nLeftID + nRightID) / 2;
	int nLeftNum_BE = nMiddleID - nLeftID;           //�����O�̒��Ԓl�̍����̔z�񃁃��o��
	int nRightNum_BE = nRightID - nMiddleID;         //�����O�̒��Ԓl�̉E���̔z�񃁃��o��
	int nLeftNum_AF = 0; //������̒��Ԓl�̍����̔z�񃁃��o��
	int nRightNum_AF = 0; //������̒��Ԓl�̉E���̔z�񃁃��o��

	int aQuickSortIDL[CSCENE3D_OBJ_MAX];                 //�N�C�b�N�\�[�gID���z��
	int aQuickSortIDR[CSCENE3D_OBJ_MAX];                 //�N�C�b�N�\�[�gID�E�z��
	float aDistanceL[CSCENE3D_OBJ_MAX];                    //�p�[�e�B�N������J�����܂ł̋������z��
	float aDistanceR[CSCENE3D_OBJ_MAX];                    //�p�[�e�B�N������J�����܂ł̋����E�z��

	//---------------------------
	//���ƉE����
	//---------------------------
	for (int i = 1; i <= nLeftNum_BE; i++)
	{
		//---------------------------
		//���Ԓl�O�̃����o�l�`�F�b�N
		//---------------------------
		//���z��ɓ����
		if (m_ZSort[nMiddleID - i].ZValue < m_ZSort[nMiddleID].ZValue)    //������r
		{
			aDistanceL[nLeftNum_AF] = (float)m_ZSort[nMiddleID - i].ZValue;          //�����z��ɓ����
			aQuickSortIDL[nLeftNum_AF] = m_ZSort[nMiddleID - i].ID;    //ID�z��ɓ����
			nLeftNum_AF++;                                         //�����o���J�E���g�A�b�v
		}
		//�E�z��ɓ����
		else
		{
			aDistanceR[nRightNum_AF] = (float)m_ZSort[nMiddleID - i].ZValue;  //�z��ɓ����
			aQuickSortIDR[nRightNum_AF] = m_ZSort[nMiddleID - i].ID;    //ID�z��ɓ����
			nRightNum_AF++;                                         //�����o���J�E���g�A�b�v
		}

		//---------------------------
		//���Ԓl���̃����o�l�`�F�b�N
		//---------------------------
		//���z��ɓ����
		if (m_ZSort[nMiddleID + i].ZValue < m_ZSort[nMiddleID].ZValue)
		{
			aDistanceL[nLeftNum_AF] = (float)m_ZSort[nMiddleID + i].ZValue;          //�����z��ɓ����
			aQuickSortIDL[nLeftNum_AF] = m_ZSort[nMiddleID + i].ID;    //ID�z��ɓ����
			nLeftNum_AF++;                                         //�����o���J�E���g�A�b�v
		}
		//�E�z��ɓ����
		else
		{
			aDistanceR[nRightNum_AF] = (float)m_ZSort[nMiddleID + i].ZValue;  //�z��ɓ����
			aQuickSortIDR[nRightNum_AF] = m_ZSort[nMiddleID + i].ID;    //ID�z��ɓ����
			nRightNum_AF++;                                         //�����o���J�E���g�A�b�v
		}
	}

	//�����������̏ꍇ
	if (nRightNum_BE > nLeftNum_BE)
	{
		//���z��ɓ����
		if (m_ZSort[nRightID].ZValue < m_ZSort[nMiddleID].ZValue)
		{
			aDistanceL[nLeftNum_AF] = (float)m_ZSort[nRightID].ZValue;          //�����z��ɓ����
			aQuickSortIDL[nLeftNum_AF] = m_ZSort[nRightID].ID;    //ID�z��ɓ����
			nLeftNum_AF++;                                         //�����o���J�E���g�A�b�v
		}
		//�E�z��ɓ����
		else
		{
			aDistanceR[nRightNum_AF] = (float)m_ZSort[nRightID].ZValue;  //�z��ɓ����
			aQuickSortIDR[nRightNum_AF] = m_ZSort[nRightID].ID;    //ID�z��ɓ����
			nRightNum_AF++;                                         //�����o���J�E���g�A�b�v
		}
	}

	//----------------------------
	//�����̌�̔z���z��ɔ��f����
	//----------------------------
	nCnt = nLeftID;              //�J�E���g������
	float MIDistance = (float)m_ZSort[nMiddleID].ZValue;      //���Ԓl�ۑ�
	int MIID = m_ZSort[nMiddleID].ID;      //���Ԓl�ۑ�

											   //������
	for (int j = 0; j < nLeftNum_AF; j++)
	{
		m_ZSort[nCnt].ZValue = aDistanceL[j];        //�������
		m_ZSort[nCnt].ID = aQuickSortIDL[j];     //ID���
		nCnt++;            //�J�E���g�A�b�v
	}

	//���Ԓl
	m_ZSort[nCnt].ZValue = MIDistance;        //�������
	m_ZSort[nCnt].ID = MIID;  //ID���
	nCnt++;       //�J�E���g�A�b�v

	//�E����
	for (int j = 0; j < nRightNum_AF; j++)
	{
		m_ZSort[nCnt].ZValue = aDistanceR[j];        //�������
		m_ZSort[nCnt].ID = aQuickSortIDR[j];         //ID���
		nCnt++;            //�J�E���g�A�b�v
	}

	//------------------
	//�ċA����
	//------------------
	//������
	if (nLeftNum_AF > 1)
	{
		QuickSort(nLeftID, nLeftID + nLeftNum_AF - 1);
	}

	//�E����
	if (nRightNum_AF > 1)
	{
		QuickSort(nRightID - nRightNum_AF + 1, nRightID);
	}
}