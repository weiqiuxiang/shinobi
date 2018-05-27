#include "CCube.h"
#include "CRenderer.h"

CCube::CCube()
{
	m_pMesh = nullptr;
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
	D3DXMatrixIdentity(&m_WorldMtx);
}

CCube::~CCube()
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}

HRESULT CCube::Init(float X, float Y, float Z)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	if (nullptr != m_pMesh)               //nullptr�`�F�b�N
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	if (FAILED(D3DXCreateBox(pDevice,X,Y,Z,&m_pMesh,nullptr)))
	{
		MessageBox(nullptr, "Cube��Create�����s", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	m_X = X;
	m_Y = Y;
	m_Z = Z;

	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//�}�e���A���̏�����(�D�F)
	m_Mtrl.Diffuse = { 0.0f,0.0f,0.8f,0.5f };
	m_Mtrl.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_Mtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_Mtrl.Emissive = { 0.0f,0.0f,0.8f,0.5f };
	m_Mtrl.Power = 6.0f;

	m_MtrlSelectedByEditor.Diffuse = { 0.0f,0.0f,0.0f,0.0f };
	m_MtrlSelectedByEditor.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_MtrlSelectedByEditor.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_MtrlSelectedByEditor.Emissive = { 0.0f,0.6f,0.0f,0.8f };
	m_MtrlSelectedByEditor.Power = 6.0f;

	return S_OK;
}

void CCube::MtxWorldSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale/*�g��s��*/, mtxTran/*�ړ��s��*/, mtxResult/*���ʍs��*/;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixTranslation(&mtxTran,m_pos.x,m_pos.y+ m_Y * 0.5f,m_pos.z);
	mtxResult = mtxScale*mtxTran;								//���ʂ̕ϊ��s��̌v�Z
	D3DXMatrixIdentity(&m_WorldMtx);							//���[���h�s���P�ʍs��ŏ�����
	D3DXMatrixMultiply(&m_WorldMtx, &m_WorldMtx, &mtxResult);	//��]�s��ƃ��[���h�s��̊|���Z
	pDevice->SetTransform(D3DTS_WORLD, &m_WorldMtx);			//���[���h�ϊ��s��
}

void CCube::Update(void)
{
	
}

void CCube::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	MtxWorldSetting();										//���[���h�s��ݒ�
	pDevice->SetMaterial(&m_Mtrl);
	m_pMesh->DrawSubset(0);
	pDevice->SetMaterial(&matDef);							//�}�e���A����߂�
}

//�G�f�B�^�[�p�`��
void CCube::DrawSelectedByEditor(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	MtxWorldSetting();										//���[���h�s��ݒ�
	pDevice->SetMaterial(&m_MtrlSelectedByEditor);
	m_pMesh->DrawSubset(0);
	pDevice->SetMaterial(&matDef);							//�}�e���A����߂�
}

void CCube::Uninit(void)
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}