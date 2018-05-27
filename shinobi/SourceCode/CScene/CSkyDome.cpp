#include "CSkyDome.h"
#include "TexManager.h"
#include "CRenderer.h"

#define DEFAULT_START_ANGEL_VER (D3DX_PI*0.1f)            //���������̊J�n�p�x
#define DEFAULT_END_ANGEL_VER (D3DX_PI*0.9f)              //���������̏I���p�x

CSkyDome::CSkyDome()
{
	m_pVtxBuffer = nullptr;
	m_pIdxBuffer = nullptr;
	m_IndexData = { 0,0,0 };
	m_TexID = -1;
}

CSkyDome::~CSkyDome()
{
	
}

HRESULT CSkyDome::Init(float fRadius, int nHor_PieceNum, int nVer_PieceNum, const char *TexFilePass)
{
	//������h�~
	if (m_pVtxBuffer != nullptr) { m_pVtxBuffer->Release(); m_pVtxBuffer = nullptr; }
	if (m_pIdxBuffer != nullptr) { m_pIdxBuffer->Release(); m_pIdxBuffer = nullptr; }

	//�|�C���^������
	m_pVtxBuffer = nullptr;
	m_pIdxBuffer = nullptr;

	//�p�����[�^�ݒ�
	m_pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//�X�J�C�h�[���̃p�����[�^
	m_fRadius = fRadius;
	m_nHor_PieceNum = nHor_PieceNum;
	m_nVer_PieceNum =  nVer_PieceNum;
	m_fStart_Angel_Vertical = DEFAULT_START_ANGEL_VER;
	m_fEnd_Angel_Vertical = DEFAULT_END_ANGEL_VER;

	//�e�N�X�`���ǂݍ���
	m_TexID = -1;
	m_TexID = CTexManager::LoadTex(TexFilePass);       //�e�N�X�`�����[�h

	//���_���ƃC���f�b�N�X���ƎO�p�|���S���̌v�Z
	m_IndexData.nVertexNum = (m_nHor_PieceNum + 1) * (m_nVer_PieceNum + 1);
	m_IndexData.nIndexNum = (2 + 2 * m_nHor_PieceNum) * m_nVer_PieceNum + (m_nVer_PieceNum - 1) * 2;
	m_IndexData.nPolygonNum = m_IndexData.nIndexNum - 2;

	InitLockVertexBufAndIndexBuf();                //�o�b�t�@�m��

	return S_OK;
}

HRESULT CSkyDome::InitLockVertexBufAndIndexBuf(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_IndexData.nVertexNum, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
	{
		MessageBox(nullptr, "���b�V���t�B�[���h�̒��_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	//�C���f�b�N�X�o�b�t�@�̐���
	HRESULT hr;
	hr = pDevice->CreateIndexBuffer(
		sizeof(WORD)*m_IndexData.nIndexNum,	//�o�b�t�@��(2�o�C�g)
		D3DUSAGE_WRITEONLY,					//�g�p���@�t���O
		D3DFMT_INDEX16,						//�C���f�b�N�X�t�H�[�}�b�g(�d�����Ȃ����_����65536�͌��E)
		D3DPOOL_MANAGED,					//�������Ǘ����@
		&m_pIdxBuffer,						//�C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^�̃A�h���X
		nullptr);
	//���s�`�F�b�N
	if (FAILED(hr))
	{
		MessageBox(nullptr, "���b�V���t�B�[���h�̃C���f�b�N�X�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	return S_OK;
}


void CSkyDome::Uninit(void)
{
	if (m_pVtxBuffer != nullptr) { m_pVtxBuffer->Release(); m_pVtxBuffer = nullptr; }
	if (m_pIdxBuffer != nullptr) { m_pIdxBuffer->Release(); m_pIdxBuffer = nullptr; }
}

void CSkyDome::Update(void)
{
	LockSkyDomeVertexBuf();
	LockSkyDomeIndexBuf();
}

void CSkyDome::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	D3DXMATRIX mtxScale/*�g��s��*/, mtxRot/*��]�s��*/, mtxMove/*�ړ��s��*/, mtxResult/*���ʍs��*/, WorldMatrix;

	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;
	D3DXMatrixIdentity(&WorldMatrix);
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));	
	pDevice->SetIndices(m_pIdxBuffer);
	pDevice->SetFVF(FVF_VERTEX3D);
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID));
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);          //���C�g���I���ɂ���

}

void CSkyDome::LockSkyDomeVertexBuf(void)
{
	VERTEX_3D *pVtx = nullptr;
	float fHorizon_PerAngel = 2 * D3DX_PI / m_nHor_PieceNum;
	float fVertical_PerAngel = (m_fEnd_Angel_Vertical - m_fStart_Angel_Vertical) / m_nVer_PieceNum;
	float fHorizon_Angel;
	float fVertical_Angel;
	D3DXVECTOR3 pos;

	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < (m_nVer_PieceNum + 1); i++)
	{
		for (int j = 0; j < (m_nHor_PieceNum + 1); j++)
		{
			int k = i * (m_nHor_PieceNum + 1) + j;
			//pos�Ɋւ���v�Z
			fHorizon_Angel = -fHorizon_PerAngel*j;											//�����p�x
			fVertical_Angel = m_fStart_Angel_Vertical + fVertical_PerAngel * i;				//�����p�x
			pos.x = m_fRadius * sinf(fVertical_Angel) * cosf(fHorizon_Angel);		//���_��X���W�v�Z
			pos.y = m_fRadius * cosf(fVertical_Angel);								//���_��Y���W�v�Z
			pos.z = m_fRadius * sinf(fVertical_Angel) * sinf(fHorizon_Angel);		//���_��Z���W�v�Z

			pVtx[0 + k].pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
			pVtx[0 + k].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			pVtx[0 + k].tex = D3DXVECTOR2(j*(1.0f / m_nHor_PieceNum), i*(1.0f / m_nVer_PieceNum));
			pVtx[0 + k].color = D3DCOLOR_RGBA(255, 255, 255, 255);
		}
	}

	m_pVtxBuffer->Unlock();
}

void CSkyDome::LockSkyDomeIndexBuf(void)
{
	WORD *pIdx = nullptr;

	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	//�C���f�b�N�X�o�b�t�@�o�^
	for (int i = 0; i < m_nVer_PieceNum; i++)
	{
		//���ʕ���
		for (int j = 0; j < m_nHor_PieceNum + 1; j++)
		{
			int k = j * 2 + i*(m_nHor_PieceNum + 2) * 2;
			pIdx[0 + k] = (WORD)( (i + 1)*(m_nHor_PieceNum + 1) + j );
			pIdx[1 + k] = pIdx[0 + k] - (WORD)( (m_nHor_PieceNum + 1) );
		}

		int q = (2 + 2 * m_nHor_PieceNum)*(i + 1) + ((i + 1) - 1) * 2;

		//�k�ޕ���
		if (q < m_IndexData.nIndexNum)
		{
			pIdx[q] = pIdx[q - 1];         //�k��1�̒��_�ԍ�
			pIdx[q + 1] = pIdx[q] + (WORD)(m_nHor_PieceNum + 1) + 1;     //�k��2�̒��_�ԍ�
		}
	}

	m_pIdxBuffer->Unlock();
}