/*==============================================================================
	�t�@�C���� : CAfterimageEffect.cpp
===============================================================================*/
#include "CAfterimageEffect.h"
#include "CRenderer.h"
#include "TexManager.h"
#include "CCameraManager.h"

/*==============================================================================
	�R���X�g���N�^
===============================================================================*/
CAfterimageEffect::CAfterimageEffect()
{
	m_pVtxBuffer = nullptr;
	m_pIdxBuffer = nullptr;
	m_pIdxBufferBackFace = nullptr;
	m_IndexData = { 0,0,0 };
	m_TexID = -1;

	for (int i = 0; i < AFTERIMAGE_EF_VERTEX_MAX; i++)
	{
		m_VertexPos[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_TextureUValue[i] = 0.0f;
		m_UseVertex[i] = false;
	}
}

/*==============================================================================
	�f�X�g���N�^
===============================================================================*/
CAfterimageEffect::~CAfterimageEffect()
{
	
}

/*==============================================================================
	����������
	��������:
		Pos			:	�}�g���N�X�����p�̈ʒu���
		IndexSize	:	�C���f�b�N�X���(���c�}�X���Ɖ��c�Ԋu)
		TexFilePass	:	�e�N�X�`���̃t�@�C���p�X
===============================================================================*/
HRESULT CAfterimageEffect::Init( const char *TexFilePass) 
{
	//�}�g���N�X������񏉊���
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//�e�N�X�`���ǂݍ���
	m_TexID = CTexManager::LoadTex(TexFilePass);

	//�C���f�b�N�X�f�[�^������
	m_IndexData.nVertexNum = AFTERIMAGE_EF_VERTEX_MAX;
	m_IndexData.nIndexNum = AFTERIMAGE_EF_VERTEX_MAX;
	m_IndexData.nPolygonNum = m_IndexData.nIndexNum - 2;

	//�f�o�C�X�擾
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�m��
	if (m_pVtxBuffer == nullptr) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*m_IndexData.nVertexNum, D3DUSAGE_WRITEONLY, FVF_VERTEX3D, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
		{
			MessageBox(nullptr, "���b�V���t�B�[���h�̒��_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
			return E_FAIL;
		}
	}
	if (m_pIdxBuffer == nullptr)				//�\��
	{
		HRESULT hr;
		hr = pDevice->CreateIndexBuffer(
			sizeof(WORD)*m_IndexData.nIndexNum,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_pIdxBuffer,
			nullptr);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "���b�V���t�B�[���h�̃C���f�b�N�X�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
			return E_FAIL;
		}
	}
	if (m_pIdxBufferBackFace == nullptr)		//����
	{
		HRESULT hr;
		hr = pDevice->CreateIndexBuffer(
			sizeof(WORD)*m_IndexData.nIndexNum,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_pIdxBufferBackFace,
			nullptr);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "���b�V���t�B�[���h�̃C���f�b�N�X�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
			return E_FAIL;
		}
	}

	//���_�̍��W�ƃA���t�@�l������
	for (int i = 0; i < AFTERIMAGE_EF_VERTEX_MAX; i++) 
	{
		m_VertexPos[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_TextureUValue[i] = 0.0f;
		m_UseVertex[i] = false;
	}

	m_IndexData.nVertexNum = 0;			//�g�p���Ă钸�_����0��

	return S_OK;
}

/*==============================================================================
	�I������
===============================================================================*/
void CAfterimageEffect::Uninit(void)
{
	if (m_pVtxBuffer != nullptr) 
	{
		m_pVtxBuffer->Release(); 
		m_pVtxBuffer = nullptr; 
	}
	if (m_pIdxBuffer != nullptr) 
	{ 
		m_pIdxBuffer->Release(); 
		m_pIdxBuffer = nullptr; 
	}
	if (m_pIdxBufferBackFace != nullptr)
	{
		m_pIdxBufferBackFace->Release();
		m_pIdxBufferBackFace = nullptr;
	}
}

/*==============================================================================
	�X�V����
===============================================================================*/
void CAfterimageEffect::Update(void)
{
	LockVertexBuf();		//���_�o�b�t�@���b�N
	LockIndexBuf();			//�C���f�b�N�X���b�N
}

/*==============================================================================
	�`�揈��
===============================================================================*/
void CAfterimageEffect::Draw(void)
{
	//���_��4�ȉ��Ȃ���s���Ȃ�
	if (m_IndexData.nVertexNum < 4)
	{
		return;
	}

	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	SRP PosRotScl = { m_pos,m_rot,m_scl };

	//���[���h�}�g���N�X�ݒ�
	MtxSetting(PosRotScl);

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);			//���C�g�I�t

	//�\�ʕ`��
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));	//VRAM����GPU�Ƀf�[�^���X�g���[��
	pDevice->SetIndices(m_pIdxBuffer);									//�C���f�b�N�X�o�b�t�@�̃Z�b�g
	pDevice->SetFVF(FVF_VERTEX3D);										//�`��t�H�[�}�b�g�Z�b�g
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID));				//�e�N�X�`���̐ݒ�
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,			//�`��
		m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	//���ʕ`��
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));	//VRAM����GPU�Ƀf�[�^���X�g���[��
	pDevice->SetIndices(m_pIdxBufferBackFace);							//�C���f�b�N�X�o�b�t�@�̃Z�b�g
	pDevice->SetFVF(FVF_VERTEX3D);										//�`��t�H�[�}�b�g�Z�b�g
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID));				//�e�N�X�`���̐ݒ�
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,			//�`��
		m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);			//���C�g�I��
}

/*==============================================================================
	�G�t�F�N�g�ʒu�̈ړ�
		TopPos : �G�t�F�N�g�������ʒu
		BottomPos : �G�t�F�N�g�����扺�ʒu
===============================================================================*/
void CAfterimageEffect::MoveEffectPosition(const D3DXVECTOR3& TopPos, const D3DXVECTOR3& BottomPos)
{
	int HoriNum = (int)(AFTERIMAGE_EF_VERTEX_MAX * 0.5f);	//���}�X��

	//���_���W�̓���ւ����s��
	for (int i = 0; i < HoriNum - 1; i++)
	{
		int K = i * 2;
		m_VertexPos[K] = m_VertexPos[K + 2];
		m_VertexPos[K + 1] = m_VertexPos[(K + 1) + 2];
	}

	//�Ō��2���_�ɍ����̃f�[�^����
	m_VertexPos[AFTERIMAGE_EF_VERTEX_MAX - 2] = TopPos;
	m_VertexPos[AFTERIMAGE_EF_VERTEX_MAX - 1] = BottomPos;
}
/*==============================================================================
�G�t�F�N�g�̒��_�̎g�p
��������:
	TopPos : �G�t�F�N�g�������ʒu
	BottomPos : �G�t�F�N�g�����扺�ʒu
�߂�l����:
	false : �g�p�ł��钸�_���݂��Ȃ�
	true : �g�p�ł��钸�_���݂���
===============================================================================*/
bool CAfterimageEffect::UseEffectVertex(const D3DXVECTOR3& TopPos, const D3DXVECTOR3& BottomPos)
{
	if (m_UseVertex[AFTERIMAGE_EF_VERTEX_MAX - 1] == true)
	{
		return false;
	}

	//�g�p���Ă��Ȃ����_��T��
	int HoriNum = (int)(AFTERIMAGE_EF_VERTEX_MAX * 0.5f);	//���}�X��
	int UseVertexNum = 0;									//�g�p���ꂽ���_��
	for (int i = 0; i < HoriNum; i++)
	{
		int K = i * 2;
		if (m_UseVertex[K] == false) 
		{
			m_UseVertex[K] = true;
			m_VertexPos[K] = TopPos;
			m_UseVertex[K+1] = true;
			m_VertexPos[K + 1] = BottomPos;
			UseVertexNum = K + 2;
			break;
		}
	}

	m_IndexData.nVertexNum = UseVertexNum;
	m_IndexData.nIndexNum = UseVertexNum;
	m_IndexData.nPolygonNum = m_IndexData.nIndexNum - 2;

	//�A���t�@�l�̌v�Z(���_4�ȏ�g�p�����Ȃ�)
	if (m_IndexData.nVertexNum >= 4)
	{
		HoriNum = (int)(m_IndexData.nVertexNum * 0.5f);		//���}�X��
		float AlphaChangeValue = 1.0f / (HoriNum - 1);	//�A���t�@�ω��l
		for (int i = 0; i < HoriNum; i++)
		{
			int K = i * 2;
			m_TextureUValue[K] = i * AlphaChangeValue;
			m_TextureUValue[K + 1] = i * AlphaChangeValue;
		}
	}

	return true;
}

/*==============================================================================
	�S�Ă̒��_�𖢎g�p��Ԃɂ���
===============================================================================*/
void CAfterimageEffect::NoUseAllVertex(void)
{
	//�t���O���Z�b�g
	for (int i = 0; i < AFTERIMAGE_EF_VERTEX_MAX; i++)
	{
		m_UseVertex[i] = false;
	}

	m_IndexData.nVertexNum = 0;			//�g�p���Ă钸�_����0��
}

/*==============================================================================
	���_�o�b�t�@���b�N
===============================================================================*/
void CAfterimageEffect::LockVertexBuf(void)
{
	//���_��4�ȉ��Ȃ���s���Ȃ�
	if (m_IndexData.nVertexNum < 4) 
	{
		return;
	}

	VERTEX_3D *pVtx = nullptr;

	//���_���b�N(�@���ƐF���܂܂��Ȃ�)
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	int HoriNum = (int)(m_IndexData.nVertexNum * 0.5f);	//���������}�X��

	for (int X = 0; X < HoriNum; X++)
	{
		int K = X * 2;
		pVtx[K].pos = m_VertexPos[K];
		pVtx[K].tex = D3DXVECTOR2(m_TextureUValue[K], 0.0f);
		pVtx[K].color = D3DCOLOR_RGBA(255,255,255, 200);
		pVtx[K + 1].pos = m_VertexPos[K + 1];
		pVtx[K + 1].tex = D3DXVECTOR2(m_TextureUValue[K + 1], 1.0f);
		pVtx[K + 1].color = D3DCOLOR_RGBA(255, 255, 255, 200);
	}

	m_pVtxBuffer->Unlock();
}

/*==============================================================================
	�C���f�b�N�X�o�b�t�@���b�N
===============================================================================*/
void CAfterimageEffect::LockIndexBuf(void)
{
	//���_��4�ȉ��Ȃ���s���Ȃ�
	if (m_IndexData.nVertexNum < 4)
	{
		return;
	}

	WORD *pIdx = nullptr;

	//�\�ʃC���f�b�N�X�o�b�t�@�o�^
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	for (int X = 0; X < m_IndexData.nVertexNum; X++)
	{
		pIdx[X] = (WORD)X;
	}

	m_pIdxBuffer->Unlock();

	//���ʃC���f�b�N�X�o�b�t�@�o�^
	m_pIdxBufferBackFace->Lock(0, 0, (void**)&pIdx, 0);

	int HoriNum = (int)(m_IndexData.nVertexNum * 0.5f);	//���}�X��
	for (int X = 0; X < HoriNum; X++)
	{
		int K = X * 2;
		pIdx[K] = (WORD)( (m_IndexData.nVertexNum - 2) - K );
		pIdx[K + 1] = (WORD)( (m_IndexData.nVertexNum - 1) - K );

	}

	m_pIdxBufferBackFace->Unlock();
}

/*==============================================================================
	���[���h�}�g���N�X�̃Z�b�g
===============================================================================*/
void CAfterimageEffect::MtxSetting(const SRP& SclPosRot)
{
	//TODO: �G�t�F�N�g�̓r���{�[�h�̕`��Ȃ̂ŁA�r���{�[���̂悤�ɂ��Ȃ���
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult, WorldMatrix;

	//�g��k���A��]�A�ړ��Ȃǂ̍s��̍���
	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, SclPosRot.rot.y, SclPosRot.rot.x, SclPosRot.rot.z);
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);

	//�s��̍���
	mtxResult = mtxScale*mtxRot*mtxMove;
	D3DXMatrixIdentity(&WorldMatrix);
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);

	//���[���h�ɔ��f
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);
}