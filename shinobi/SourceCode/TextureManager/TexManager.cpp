#include "TexManager.h"

//�}�N��
#define TEXTURE_IS_nullptr (-1)

//�O���[�o��
TEX *CTexManager::m_TexTureSlot[TEXSLOT_NUM] = {};

//���ׂẴe�N�X�`���X���b�g������
HRESULT CTexManager::InitAll(void)
{
	for (int nCnt = 0; nCnt < TEXSLOT_NUM; nCnt++)
	{
		if (m_TexTureSlot[nCnt] != nullptr)
		{
			m_TexTureSlot[nCnt]->Uninit();
			delete m_TexTureSlot[nCnt];
			m_TexTureSlot[nCnt] = nullptr;
		}

		m_TexTureSlot[nCnt] = nullptr;
	}

	return S_OK;
}

//���ׂẴe�N�X�`���X���b�g���
void CTexManager::UninitAll(void)
{
	for (int nCnt = 0; nCnt < TEXSLOT_NUM; nCnt++)
	{
		if (m_TexTureSlot[nCnt] != nullptr)
		{
			m_TexTureSlot[nCnt]->Uninit();
			delete m_TexTureSlot[nCnt];
			m_TexTureSlot[nCnt] = nullptr;
		}
	}
}

//�e�N�X�`�������[�h��ID��Ԃ�
TEX_ID CTexManager::LoadTex(const char *FilePass)
{
	if (FilePass == nullptr) {
		return TEXTURE_IS_nullptr;
	}
	for (TEX_ID IDNum = 0; IDNum < TEXSLOT_NUM; IDNum++)
	{
		if (m_TexTureSlot[IDNum] != nullptr)//�X���b�g���łɎg�p�����ꍇ
		{
			const char *SlotFilePass = m_TexTureSlot[IDNum]->GetTexPass();   //�t�@�C���p�X�擾
			int ValueStrcmp = strcmp(SlotFilePass, FilePass);
			if (ValueStrcmp == 0) return IDNum;
			else continue;
		}

		if (m_TexTureSlot[IDNum] == nullptr)//�X���b�g�󂪂�������e�N�X�`��������
		{
			CreateTexObj(IDNum, FilePass);
			return IDNum;
		}
	}

	//��O����
	MessageBox(nullptr, "�g�p�ł���e�N�X�`���X���b�g���Ȃ�", "�G���[", MB_OK | MB_ICONHAND);
	return TEXTURE_IS_nullptr;
}

//�e�N�X�`���ԍ���n��,�e�N�X�`���|�C���^��Ԃ�
LPDIRECT3DTEXTURE9 CTexManager::UseTex(TEX_ID IDNum)
{
	if (IDNum == TEXTURE_IS_nullptr) return nullptr;

	if (m_TexTureSlot[IDNum] == nullptr)
	{
		return nullptr;
	}
	return m_TexTureSlot[IDNum]->GetTexturePoint();
}

//�e�N�X�`���X���b�g�Ƀe�N�X�`��������
HRESULT CTexManager::CreateTexObj(TEX_ID IDNum, const char *FilePass)
{
	if (m_TexTureSlot[IDNum] != nullptr)
	{
		MessageBox(nullptr, "���̃e�N�X�`���X���b�g�����łɎg�p����Ă���", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}
	m_TexTureSlot[IDNum] = new TEX;//���̉�
	m_TexTureSlot[IDNum]->SetTexPass(FilePass);//�t�@�C���p�X���Z�b�g
	m_TexTureSlot[IDNum]->CreateTexObj();//�e�N�X�`���I�u�W�F�N�g����

	return S_OK;
}

const char *CTexManager::GetTexturePass(TEX_ID IDNum)
{
	if (nullptr == m_TexTureSlot[IDNum]) return nullptr;
	return m_TexTureSlot[IDNum]->GetTexPass();
}