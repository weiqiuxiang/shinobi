#include "CLifeGauge.h"
#include "TexManager.h"
#include "CRenderer.h"
#include "CCameraManager.h"

#define WINDOW_FRAME_WIDTH (0.1f)
#define WINDOW_FRAME_HEIGHT (0.1f)
#define GAUGE_WIDTH_MAX (10.0f)
#define GAUGE_HEIGHT_MAX (5.0f)
#define LIFE_MAX (10000)
#define LIFE_SHOW_TIME (60)

#define TEX_PASS_GAUGE "data/TEXTURE/LifeGaugeWindow.png"
#define TEX_PASS_LEFT_LIFE "data/TEXTURE/LeftLifeColor.png"
#define TEX_PASS_DAMAGE_LIFE "data/TEXTURE/DamageLifeColor.png"

CLifeGauge::CLifeGauge()
{
	for (int i = 0; i < TEX_ID_MAX; i++) {
		m_TexID[i] = -1;
	}

	//���C�t�֌W
	m_TotalLife = 0;
	m_LeftLife = 0;
	m_DamageLife = 0;
	m_DamageShowTime = LIFE_SHOW_TIME;

	m_Alpha = 255;
	m_LifeGaugeSize = D3DXVECTOR3(0, 0, 0);
	m_LifeWindowSize = D3DXVECTOR3(0, 0, 0);

	D3DXMatrixIdentity(&m_WorldMtx);
}

CLifeGauge::~CLifeGauge()
{

}

bool CLifeGauge::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float TotalLife)
{
	//��O����
	if (TotalLife < 1 || TotalLife >= LIFE_MAX) {
		MessageBox(nullptr, "[CLifeGauge.cpp][Init]����TotalLife�̒l�͔͈͊O", "", MB_ICONHAND);
		return false;
	}
	if (size.x < 0 || size.x >= GAUGE_WIDTH_MAX) {
		MessageBox(nullptr, "[CLifeGauge.cpp][Init]����size.x�̒l�͔͈͊O", "", MB_ICONHAND);
		return false;
	}
	if (size.y < 0 || size.y >= GAUGE_HEIGHT_MAX) {
		MessageBox(nullptr, "[CLifeGauge.cpp][Init]����size.y�̒l�͔͈͊O", "", MB_ICONHAND);
		return false;
	}

	m_pos = pos;

	m_Alpha = 255;
	m_DamageShowTime = LIFE_SHOW_TIME;
	m_LifeWindowSize = size;
	m_LifeGaugeSize = size - D3DXVECTOR3(WINDOW_FRAME_WIDTH, WINDOW_FRAME_HEIGHT,0) * 2;

	m_TotalLife = TotalLife;
	m_LeftLife = TotalLife;
	m_DamageLife = 0;

	//�e�N�X�`���ǂݍ���
	m_TexID[TEX_WINDOW] = CTexManager::LoadTex(TEX_PASS_GAUGE);
	m_TexID[TEX_GAUGE] = CTexManager::LoadTex(TEX_PASS_LEFT_LIFE);
	m_TexID[TEX_DAMAGE_GAUGE] = CTexManager::LoadTex(TEX_PASS_DAMAGE_LIFE);

	for (int i = 0; i < TEX_ID_MAX; i++) {
		if (m_TexID[i] == -1) {
			MessageBox(nullptr, "[CLifeGauge.cpp][Init]�e�N�X�`���ǂݍ��ݎ��s", "", MB_ICONHAND);
			return false;
		}
	}

	//���_�o�b�t�@����镔��
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX3D, D3DPOOL_MANAGED, &m_pVtxBufferWindow, nullptr))){
		MessageBox(nullptr, "[CLifeGauge.cpp][Init]���_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return false;
	}
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX3D, D3DPOOL_MANAGED, &m_pVtxBufferLeftGauge, nullptr))) {
		MessageBox(nullptr, "[CLifeGauge.cpp][Init]���_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return false;
	}
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX3D, D3DPOOL_MANAGED, &m_pVtxBufferDamageGauge, nullptr))) {
		MessageBox(nullptr, "[CLifeGauge.cpp][Init]���_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return false;
	}

	D3DXMatrixIdentity(&m_WorldMtx);

	return true;
}

void CLifeGauge::Update(void)
{
	//��O����
	m_LeftLife = max(0, m_LeftLife);

	//�_���[�W���C�t�̌���
	float UnitTotalLife = m_TotalLife / 100;

	if (m_DamageShowTime <= 0) {
		m_DamageLife -= UnitTotalLife;
		m_DamageLife = max(0, m_DamageLife);
	}
	else {
		m_DamageShowTime--;
	}

	//�p�[�Z���e�[�W�v�Z
	float leftLifePercent = m_LeftLife / m_TotalLife;
	float damageLifePercent = m_DamageLife / m_TotalLife;
	
	float GaugeLeftSize = -m_LifeGaugeSize.x * 0.5f;
	float LeftLifeRightSize = GaugeLeftSize + leftLifePercent * m_LifeGaugeSize.x;
	float DamageLifeWidth = damageLifePercent * m_LifeGaugeSize.x;

	LockVertexBuf(LeftLifeRightSize , DamageLifeWidth);
}

void CLifeGauge::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//���[���h�}�g���N�X�ݒ�
	SetWorldMtxBillBoard();

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//�Q�[�W�E�B���h�E�`��
	pDevice->SetStreamSource(0, m_pVtxBufferWindow, 0, sizeof(VERTEX_3D));
	pDevice->SetFVF(FVF_VERTEX3D);
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID[TEX_WINDOW]));
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//�c�胉�C�t�Q�[�W�Q�[�W�`��
	pDevice->SetStreamSource(0, m_pVtxBufferLeftGauge, 0, sizeof(VERTEX_3D));
	pDevice->SetFVF(FVF_VERTEX3D);
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID[TEX_GAUGE]));
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//�_���[�W�Q�[�W�`��
	pDevice->SetStreamSource(0, m_pVtxBufferDamageGauge, 0, sizeof(VERTEX_3D));
	pDevice->SetFVF(FVF_VERTEX3D);
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID[TEX_DAMAGE_GAUGE]));
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CLifeGauge::Uninit(void)
{
	if (m_pVtxBufferWindow != nullptr) {
		m_pVtxBufferWindow->Release();
		m_pVtxBufferWindow = nullptr;
	}
	if (m_pVtxBufferLeftGauge != nullptr) {
		m_pVtxBufferLeftGauge->Release();
		m_pVtxBufferLeftGauge = nullptr;
	}
	if (m_pVtxBufferDamageGauge != nullptr) {
		m_pVtxBufferDamageGauge->Release();
		m_pVtxBufferDamageGauge = nullptr;
	}
}

void CLifeGauge::GetEnemyLife(float Life)
{
	if (m_LeftLife != Life) {
		float Damage = m_LeftLife - Life;
		m_LeftLife = Life;
		m_DamageLife += Damage;
		m_DamageShowTime = LIFE_SHOW_TIME;
	}
}

void CLifeGauge::AddDamageLife(float life)
{
	m_DamageLife += life;
	m_LeftLife = -life;
	m_LeftLife = max(0, m_LeftLife);
}

void CLifeGauge::LockVertexBuf(float LeftLifeRightSide, float DamageLifeWidth)
{
	VERTEX_3D *pVtx = nullptr;

	D3DXVECTOR3 HalfFrameSize = m_LifeWindowSize * 0.5f;

	//�E�B���h�E�t���[�������b�N�I��
	m_pVtxBufferWindow->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-HalfFrameSize.x, HalfFrameSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(HalfFrameSize.x, HalfFrameSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-HalfFrameSize.x, -HalfFrameSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(HalfFrameSize.x, -HalfFrameSize.y, 0.0f);

	pVtx[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVtx[0].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtx[1].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtx[2].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtx[3].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	m_pVtxBufferWindow->Unlock();

	float GaugeTop = HalfFrameSize.y - WINDOW_FRAME_HEIGHT;
	float GaugeBottom = -HalfFrameSize.y + WINDOW_FRAME_HEIGHT;

	//�c�胉�C�t�Q�[�W�����b�N�I��
	VERTEX_3D *pVtxLeftLife = nullptr;
	float GaugeLeftSide = -HalfFrameSize.x + WINDOW_FRAME_WIDTH;
	
	m_pVtxBufferLeftGauge->Lock(0, 0, (void**)&pVtxLeftLife, 0);

	pVtxLeftLife[0].pos = D3DXVECTOR3(GaugeLeftSide, GaugeTop, 0.0f);
	pVtxLeftLife[1].pos = D3DXVECTOR3(LeftLifeRightSide, GaugeTop, 0.0f);
	pVtxLeftLife[2].pos = D3DXVECTOR3(GaugeLeftSide, GaugeBottom, 0.0f);
	pVtxLeftLife[3].pos = D3DXVECTOR3(LeftLifeRightSide, GaugeBottom, 0.0f);

	pVtxLeftLife[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtxLeftLife[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtxLeftLife[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtxLeftLife[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVtxLeftLife[0].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtxLeftLife[1].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtxLeftLife[2].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtxLeftLife[3].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtxLeftLife[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtxLeftLife[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtxLeftLife[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtxLeftLife[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	m_pVtxBufferLeftGauge->Unlock();

	//�_���[�W���C�t�����b�N�I��
	VERTEX_3D *pVtxDamageLife = nullptr;
	m_pVtxBufferDamageGauge->Lock(0, 0, (void**)&pVtxDamageLife, 0);

	pVtxDamageLife[0].pos = D3DXVECTOR3(LeftLifeRightSide, GaugeTop, 0.0f);
	pVtxDamageLife[1].pos = D3DXVECTOR3(LeftLifeRightSide + DamageLifeWidth, GaugeTop, 0.0f);
	pVtxDamageLife[2].pos = D3DXVECTOR3(LeftLifeRightSide, GaugeBottom, 0.0f);
	pVtxDamageLife[3].pos = D3DXVECTOR3(LeftLifeRightSide + DamageLifeWidth, GaugeBottom, 0.0f);

	pVtxDamageLife[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtxDamageLife[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtxDamageLife[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtxDamageLife[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVtxDamageLife[0].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtxDamageLife[1].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtxDamageLife[2].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);
	pVtxDamageLife[3].color = D3DCOLOR_RGBA(255, 255, 255, m_Alpha);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtxDamageLife[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtxDamageLife[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtxDamageLife[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtxDamageLife[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	m_pVtxBufferDamageGauge->Unlock();
}

void CLifeGauge::SetWorldMtxBillBoard(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXMATRIX mtxMove , mtxView, mtxViewInverse;

	//�ړ��s��ݒ�
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);

	//�r���[�s��̋t�s��𐶐�
	CAMERA_PARA PosLookatUp = CCameraManager::GetCameraPara(CCameraManager::GetActiveCameraName());
	D3DXMatrixLookAtLH(&mtxView, &PosLookatUp.pos, &PosLookatUp.lookat, &PosLookatUp.up);
	D3DXMatrixTranspose(&mtxViewInverse, &mtxView);

	//���s�ړ������J�b�g
	mtxViewInverse._14 = 0.0f;
	mtxViewInverse._24 = 0.0f;
	mtxViewInverse._34 = 0.0f;
	mtxViewInverse._41 = 0.0f;
	mtxViewInverse._42 = 0.0f;
	mtxViewInverse._43 = 0.0f;

	//���[���h�}�g���N�X���Z�b�g
	m_WorldMtx = mtxViewInverse * mtxMove;
	pDevice->SetTransform(D3DTS_WORLD, &m_WorldMtx);
}