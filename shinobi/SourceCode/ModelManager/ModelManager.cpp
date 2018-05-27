#include "ModelManager.h"
#include <crtdbg.h>

//�O���[�o��
CModelObj *CModelManager::m_ModelSlot[MODELSLOT_NUM] = {};

//���ׂẴ��f���X���b�g������
HRESULT CModelManager::InitAll(void)
{
	for (int nCnt = 0; nCnt < MODELSLOT_NUM; nCnt++)
	{
		if (m_ModelSlot[nCnt] != nullptr)
		{
			m_ModelSlot[nCnt]->Uninit();
			delete m_ModelSlot[nCnt];
			m_ModelSlot[nCnt] = nullptr;
		}

		m_ModelSlot[nCnt] = nullptr;
	}

	return S_OK;
}

//���ׂẴ��f���X���b�g���
void CModelManager::UninitAll(void)
{
	for (int nCnt = 0; nCnt < MODELSLOT_NUM; nCnt++)
	{
		if (m_ModelSlot[nCnt] != nullptr)
		{
			m_ModelSlot[nCnt]->Uninit();
			delete m_ModelSlot[nCnt];
			m_ModelSlot[nCnt] = nullptr;
		}
	}
}

//���f�������[�h��ID��Ԃ�
MODEL_ID CModelManager::LoadModel(const char *FilePass)
{
	for (MODEL_ID IDNum = 0; IDNum < MODELSLOT_NUM; IDNum++)
	{
		if (m_ModelSlot[IDNum] != nullptr)//�X���b�g���łɎg�p�����ꍇ
		{
			const char *SlotFilePass = m_ModelSlot[IDNum]->GetModelPass();   //�t�@�C���p�X�擾
			int ValueStrcmp = strcmp(SlotFilePass, FilePass);
			if (ValueStrcmp == 0) return IDNum;
			else continue;
		}

		if (m_ModelSlot[IDNum] == nullptr)//�X���b�g�󂪂������烂�f��������
		{
			CreateModelObj(IDNum, FilePass);
			return IDNum;
		}
	}

	//��O����
	MessageBox(nullptr, "�g�p�ł��郂�f���X���b�g���Ȃ�", "�G���[", MB_OK | MB_ICONHAND);
	return -1;
}

//���f���ԍ���n��,���f���|�C���^��Ԃ�
void CModelManager::DrawModel(MODEL_ID IDNum)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		MessageBox(nullptr, "���f�����݂��Ȃ�,�`��ł��Ȃ�", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}
	m_ModelSlot[IDNum]->Draw();
}

void CModelManager::DrawModelUseMtrl(MODEL_ID IDNum, const D3DMATERIAL9& Mtrl)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		MessageBox(nullptr, "���f�����݂��Ȃ�,�`��ł��Ȃ�", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}
	m_ModelSlot[IDNum]->DrawMtrl(Mtrl);
}

void CModelManager::DrawModelOnDepthMap(MODEL_ID IDNum, const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawModelOnDepthMap]���f�����݂��Ȃ�,�`��ł��Ȃ�\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawOnDepthMap(world, LightView,LightProje);
}

void CModelManager::DrawUseDepthMap(MODEL_ID IDNum, const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawUseDepthMap]���f�����݂��Ȃ�,�`��ł��Ȃ�\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawUseDepthMap(world, LightView, LightProje, DepthTexture);
}

void CModelManager::DrawSpecular(MODEL_ID IDNum, const D3DXMATRIX& world)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawUseDepthMap]���f�����݂��Ȃ�,�`��ł��Ȃ�\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawSpecular(world);
}

//���[�V�����u���[��`��
void CModelManager::DrawMotionBlur(MODEL_ID IDNum, const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawMotionBlur]���f�����݂��Ȃ�,�`��ł��Ȃ�\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawMotionBlur(worldMtx, worldMtxOld, OriginalTexture);
}

//���f���X���b�g�Ƀ��f��������
HRESULT CModelManager::CreateModelObj(MODEL_ID IDNum, const char *FilePass)
{
	if (m_ModelSlot[IDNum] != nullptr)
	{
		MessageBox(nullptr, "���̃��f���X���b�g�����łɎg�p����Ă���", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}
	m_ModelSlot[IDNum] = new CModelObj;//���̉�
	m_ModelSlot[IDNum]->SetModelPass(FilePass);//�t�@�C���p�X���Z�b�g
	m_ModelSlot[IDNum]->CreateModelObj();//���f���I�u�W�F�N�g����

	return S_OK;
}

const char* CModelManager::GetModelPass(MODEL_ID IDNum)
{
	if (nullptr == m_ModelSlot[IDNum]) return nullptr;
	return m_ModelSlot[IDNum]->GetModelPass();
}

LPD3DXMESH CModelManager::GetMesh(MODEL_ID IDNum)
{
	if (nullptr == m_ModelSlot[IDNum]) {
		return nullptr;
	}
	return m_ModelSlot[IDNum]->GetMesh();
}

void CModelManager::SetAlpha(MODEL_ID IDNum, float Alpha) {
	if (nullptr == m_ModelSlot[IDNum]) {
		return;
	}
	m_ModelSlot[IDNum]->SetAlpha(Alpha);
}