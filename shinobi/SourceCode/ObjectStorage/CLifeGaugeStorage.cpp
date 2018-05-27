#include "CLifeGaugeStorage.h"
#include "CCameraManager.h"
#include "CEnemyStorage.h"
#include <crtdbg.h>

//�}�N��
#define DOG_LIFE_GAUGE_POSY_FROM_BODY (3.0f)		//���C�t�Q�[�W�̈ʒu�́A�^�C�v�͌��̏ꍇ�A�{�f�B�̈ʒu���痣��鋗��

//static�ϐ�
CLifeGauge *CLifeGaugeStorage::m_Obj[CLIFE_GAUGE_NUM_MAX] = {};
CLifeGaugeStorage::Z_SORT CLifeGaugeStorage::m_ZSort[CLIFE_GAUGE_NUM_MAX] = {};             //Z�\�[�h�̏��Ԋi�[�s��
int CLifeGaugeStorage::m_TotalObj = 0;

#define ID_IS_nullptr (-1)

void CLifeGaugeStorage::InitAll(void)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
		}
		m_Obj[ID] = nullptr;
	}

	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}
}

void CLifeGaugeStorage::UninitAll(void)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
			m_Obj[ID] = nullptr;
		}
	}
}

void CLifeGaugeStorage::UpdateAll(void)
{
	//���C�t�Q�[�W�̍X�V
	int Count = 0;
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for (; it != itEnd; ++it) {
		if (*it != nullptr) {
			D3DXVECTOR3 BodyPos = (*it)->GetBodyPos();
			CEnemy::ENEMY_TYPE EnemyType = (*it)->GetEnemyType();
			switch (EnemyType) {
			case CEnemy::TYPE_DOG:
				BodyPos.y = BodyPos.y + DOG_LIFE_GAUGE_POSY_FROM_BODY;
				break;
			}
			CLifeGaugeStorage::SetWorldPos(Count, BodyPos);
			CLifeGaugeStorage::GetEnemyLife(Count, (float)(*it)->GetLife());
		}
		Count++;
	}

	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr) m_Obj[ID]->Update();
	}
}

void CLifeGaugeStorage::DrawAll(void)
{
	ZSort();

	for (int ID = 0; ID < m_TotalObj; ID++)
	{
		int k = m_TotalObj - 1 - ID;
		if (m_Obj[m_ZSort[k].ID] != nullptr)
		{
			m_Obj[m_ZSort[k].ID]->Draw();
		}
	}
}

CLifeGauge* CLifeGaugeStorage::CreateObject(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, int TotalLife)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] == nullptr)
		{
			m_Obj[ID] = new CLifeGauge();
			m_Obj[ID]->Init(pos, size, (float)TotalLife);
			return m_Obj[ID];
		}
	}
	return nullptr;
}

void CLifeGaugeStorage::Delete(CLifeGauge * pScene3D)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			if (m_Obj[ID] != pScene3D) continue;
			m_Obj[ID]->Uninit();
			m_Obj[ID] = nullptr;
		}
	}
}

CLifeGauge* CLifeGaugeStorage::GetObj(int ID)
{
	bool OverRange = (ID < 0) || (ID > CLIFE_GAUGE_NUM_MAX);
	if (OverRange) {
		_RPT0(0,"[CLifeGaugeStorage.cpp][GetObj]����ID���͈͊O");
		return nullptr;
	}
	return m_Obj[ID];
}

void CLifeGaugeStorage::SetWorldPos(int ID,const D3DXVECTOR3& pos)
{
	bool OverRange = (ID < 0) || (ID > CLIFE_GAUGE_NUM_MAX);
	if (OverRange) {
		_RPT0(0, "[CLifeGaugeStorage.cpp][SetWorldPos]����ID���͈͊O");
		return;
	}

	m_Obj[ID]->SetWorldPos(pos);
}

void CLifeGaugeStorage::GetEnemyLife(int ID, float Life)
{
	bool OverRange = (ID < 0) || (ID > CLIFE_GAUGE_NUM_MAX);
	if (OverRange) {
		_RPT0(0, "[CLifeGaugeStorage.cpp][AddDamageLife]����ID���͈͊O");
		return;
	}

	m_Obj[ID]->GetEnemyLife(Life);
}

void CLifeGaugeStorage::ZSort(void)
{
	D3DXMATRIX ViewMtx = CCameraManager::GetActiveCameraViewMtx();
	D3DXMATRIX ProjeMtx = CCameraManager::GetActiveCameraProjeMtx();
	D3DXMATRIX ViewProje = ViewMtx* ProjeMtx;                       //�r���[�ˉe�s�������
	m_TotalObj = 0;                                                   //�I�u�W�F�N�g����

	//�z��i�[�ɏ�����
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}

	//�i�[�J�n
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (nullptr != m_Obj[ID])                    //nullptr�`�F�b�N
		{
			D3DXVECTOR3 POS = D3DXVECTOR3(0.0f,0.0f,0.0f);				//���[�J�����W
			D3DXMATRIX WorldMtx = m_Obj[ID]->GetWorldMtx();				//���[���h�s��擾
			D3DXMATRIX WorldViewProje = (WorldMtx) * ViewProje;			//���[���h�r���[�ˉe�s������߂�
			D3DXVec3TransformCoord(&POS,&POS, &WorldViewProje);			//�ϊ���̒��_������

			//Z�o�b�t�@�̒l��ID����
			for (int SortNum = 0; SortNum < CLIFE_GAUGE_NUM_MAX; SortNum++)
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

void CLifeGaugeStorage::QuickSort(int nLeftID, int nRightID)
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

	int aQuickSortIDL[CLIFE_GAUGE_NUM_MAX];                 //�N�C�b�N�\�[�gID���z��
	int aQuickSortIDR[CLIFE_GAUGE_NUM_MAX];                 //�N�C�b�N�\�[�gID�E�z��
	float aDistanceL[CLIFE_GAUGE_NUM_MAX];                    //�p�[�e�B�N������J�����܂ł̋������z��
	float aDistanceR[CLIFE_GAUGE_NUM_MAX];                    //�p�[�e�B�N������J�����܂ł̋����E�z��

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