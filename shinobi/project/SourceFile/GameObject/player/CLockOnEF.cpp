#include "CLockOnEF.h"
#include "CRenderer.h"
#include "CDebug.h"
#include "CCameraManager.h"

CLockOnEF::CLockOnEF()
{
	m_bUse = false;
	m_scl = 1.0f;
}

CLockOnEF::~CLockOnEF()
{
	
}

bool CLockOnEF::Init(const D3DXVECTOR3& size, const char*TexFilePass)
{
	m_bUse = false;

	m_CScene2D.Init(
		D3DXVECTOR3(SCREEN_WIDTH*0.5f, SCREEN_HEIGHT*0.5, 0),
		size,
		TexFilePass
	);
	m_size = size;
	m_scl = 4.0f;
	m_CScene2D.SetLayerA(0);
	return true;
}

void CLockOnEF::Update(void)
{
	if (m_bUse == false) {
		m_scl = 4.0f;
		m_Alpha = 0;
		return;
	}

	//�傫���X�V
	m_scl -= 0.2f;
	m_scl = max(1.0f, m_scl);

	//�A���t�@�X�V
	m_Alpha += 10;
	m_Alpha = min(m_Alpha, 200);

	//�r���[�|�[�g�s������߂�
	D3DXMATRIX MtxViewPort;
	D3DXMatrixIdentity(&MtxViewPort);
	MtxViewPort._11 = SCREEN_WIDTH * 0.5f;
	MtxViewPort._22 = -SCREEN_HEIGHT * 0.5f;
	MtxViewPort._41 = SCREEN_WIDTH * 0.5f;
	MtxViewPort._42 = SCREEN_HEIGHT * 0.5f;

	//�������W���X�N���[�����W�ɕϊ�
	D3DXMATRIX MtxViewProjectionViewport;
	MtxViewProjectionViewport = CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	MtxViewProjectionViewport = MtxViewProjectionViewport * MtxViewPort;
	D3DXVECTOR3 ScreenPos;
	D3DXVec3TransformCoord(&ScreenPos,&m_posWorld,&MtxViewProjectionViewport);

	//�`�惆�j�b�g�ݒ�
	m_CScene2D.SetLayerA(m_Alpha);
	m_CScene2D.SetPosition(ScreenPos);
	m_CScene2D.SetSize(m_size * m_scl);
	m_CScene2D.Update();
}

void CLockOnEF::Draw(void)
{
	if (m_bUse == false) {
		return;
	}

	m_CScene2D.Draw();
}

void CLockOnEF::Uninit(void)
{
	m_CScene2D.Uninit();
}