#include "CStencilShadow.h"
#include "CRenderer.h"

CStencilShadow::CStencilShadow() 
{
	
}

CStencilShadow::~CStencilShadow() 
{
	
}

HRESULT CStencilShadow::Init(const SRP& SclRotPos, const char* ModelFilepass)
{
	m_Model.Init(SclRotPos, ModelFilepass);
	m_Model.SetHitMeshFlag(false);

	//�����e�N�X�`��������
	m_CScene2D.Init(
		D3DXVECTOR3(SCREEN_WIDTH*0.5f,SCREEN_HEIGHT*0.5,0),
		D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0),
		"data/TEXTURE/StencilShadow.png"
	);

	m_CScene2D.SetLayerA(100);
	m_CScene2D.Update();

	return S_OK;
}

void CStencilShadow::Update(void)
{
	m_Model.Update();
	m_CScene2D.Update();
}

void CStencilShadow::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//���f���\�ʕ`��
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);		//�X�e���V���o�b�t�@�L��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		//Z�o�b�t�@����
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILREF, 1);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);	//�X�e���V���o�b�t�@�C���N�������g(�X�e���V���o�b�t�@�l1�ݒ肵�A�`��)
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);			//�\�ʂ����`��
	m_Model.SetMtxSetting();
	m_Model.Draw();

	//���f�����ʕ`��
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);	//�X�e���V���o�b�t�@�f�N�������g(�X�e���V���o�b�t�@�l-1�ݒ肵�A�`��)
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);			//���ʂ����`��
	m_Model.SetMtxSetting();
	m_Model.Draw();

	//�S�X�N���[�����Ȃ������`��
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		//�\�ʂ����`�惂�[�h�ɖ߂�
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);		//�������ޗL��
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);	//�X�e���V��1�̒l�̕��������`��
	m_CScene2D.Draw();

	//�ݒ��߂�
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);		//Z�o�b�t�@�L��
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);	//�X�e���V���o�b�t�@����

}

void CStencilShadow::Uninit(void)
{
	m_Model.Uninit();
	m_CScene2D.Uninit();
}