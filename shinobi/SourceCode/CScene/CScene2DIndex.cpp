#include "CScene2DIndex.h"
#include "CRenderer.h"
#include "TexManager.h"

HRESULT CScene2DIndex::Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pTotalSize, const D3DXVECTOR2& PieceNum, const char *filePass)
{
	/********************CScene2D�̕����̏�����************************/
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	if (m_TexID == nullptr) m_TexID = new TEX_ID;   //�e�N�X�`��ID�̎��̉�

	//���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX2D, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
	{
		MessageBox(nullptr, "���_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	//�e�N�X�`���ǂݍ���
	*m_TexID = CTexManager::LoadTex(filePass);    //�e�N�X�`�������[�h����

	m_pos = Pos;
	m_size = pTotalSize;
	m_RGBA = { 255,255,255,255 };
	m_IndexSize.m_PieceNum = PieceNum;			//�s�[�X�T�C�Y�̒l��ۑ�

	//piece�T�C�Y�v�Z
	m_IndexSize.m_PieceSize.x = pTotalSize.x / m_IndexSize.m_PieceNum.x;
	m_IndexSize.m_PieceSize.y = pTotalSize.y / m_IndexSize.m_PieceNum.y;

	return S_OK;
}