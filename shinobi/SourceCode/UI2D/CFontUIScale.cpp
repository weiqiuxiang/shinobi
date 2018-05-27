#include "CFontUIScale.h"
#include "SceneTransition.h"
#include "CRenderer.h"

#define SCROLL_SPEED (0.005f)

CFontUIScale::CFontUIScale()
{

}

CFontUIScale::~CFontUIScale()
{

}

//�v���[���[�̏�����
HRESULT CFontUIScale::Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize, const char *filePass)
{
	CScene2D::Init(Pos, pSize, filePass);

	//�X�P�[���֌W
	m_OriginPos = Pos;
	m_OriginSize = pSize;
	m_Scale = D3DXVECTOR2(1.0f,1.0f);

	return S_OK;
}

//�X�V����
void CFontUIScale::Update(void)
{
	D3DXVECTOR3 DistanceFromSP = m_OriginPos - m_ScalePoint;	//�����_�ƃX�P�[�����S�̍���
	//�X�P�[���̌�̍����T�C�Y
	DistanceFromSP.x = DistanceFromSP.x * m_Scale.x;
	DistanceFromSP.y = DistanceFromSP.y * m_Scale.y;
	m_pos = m_ScalePoint + DistanceFromSP;						//�X�P�[��Center���W�����߂�
	//�X�P�[����̃T�C�Y
	m_size.x = m_OriginSize.x * m_Scale.x;
	m_size.y = m_OriginSize.y * m_Scale.y;

	CScene2D::VertexBufferLockUV();
}

//�`�揈��
void CFontUIScale::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	if (m_bAddBlending == true){
		//���Z�L��
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		CScene2D::Draw();

		//���Z����
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	}

	else {
		CScene2D::Draw();
	}
}

//�I������
void CFontUIScale::Uninit(void)
{
	CScene2D::Uninit();
}