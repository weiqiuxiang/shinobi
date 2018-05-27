#include "CScene2D.h"
#include "TexManager.h"
#include "CRenderer.h"

CScene2D::CScene2D() :
	m_size(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_pVtxBuffer(nullptr),
	m_TexID(nullptr)
{
	m_RGBA = {255,255,255,255};
}

CScene2D::CScene2D(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize) :
	m_size(pSize),
	m_pVtxBuffer(nullptr),
	m_TexID(nullptr)
{
	m_pos = Pos;
	m_RGBA = { 255,255,255,255 };
}

CScene2D::~CScene2D()
{
	if (m_TexID != nullptr)
	{
		delete m_TexID;
		m_TexID = nullptr;
	}

	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}


HRESULT CScene2D::Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize, const char* filePass)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	if (m_TexID == nullptr) m_TexID = new TEX_ID;   //�e�N�X�`��ID�̎��̉�

	//���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D)*4, D3DUSAGE_WRITEONLY, FVF_VERTEX2D, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
	{
		MessageBox(nullptr, "���_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	//�e�N�X�`���ǂݍ���
	*m_TexID = CTexManager::LoadTex(filePass);    //�e�N�X�`�������[�h����

	m_pos = Pos;
	m_size = pSize;
	m_RGBA = { 255,255,255,255 };

	SetRenderStates();                  //�����_�[�X�e�[�g�Ɋւ��鏔�ݒ�
	VertexBufferLock();                 //���_���b�N
	
	return S_OK;
}

void CScene2D::Update(void)
{
	SetRenderStates();
	VertexBufferLock();
}

void CScene2D::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));
	pDevice->SetFVF(FVF_VERTEX2D);
	pDevice->SetTexture(0, CTexManager::UseTex(*m_TexID));
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);                    //�`��
}

void CScene2D::Uninit(void)
{
	if (m_TexID != nullptr)
	{
		delete m_TexID;
		m_TexID = nullptr;
	}

	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}

void CScene2D::SetRenderStates(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//�����_�X�e�[�g�ݒ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //����1:�ݒ薼�J�����O ����2:�ݒ�l(D3DCULL_CCW:�t���v���𗠖ʂ�����)
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CScene2D::VertexBufferLock(void)
{
	VERTEX_2D *pVtx = nullptr;

	D3DXVECTOR3 HalfSize = m_size * 0.5;

	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(m_pos.x - HalfSize.x, m_pos.y - HalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + HalfSize.x, m_pos.y - HalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - HalfSize.x, m_pos.y + HalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + HalfSize.x, m_pos.y + HalfSize.y, 0.0f);

	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	pVtx[0].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);
	pVtx[1].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);
	pVtx[2].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);
	pVtx[3].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	m_pVtxBuffer->Unlock();
}

void CScene2D::VertexBufferLockUV(void)
{
	VERTEX_2D *pVtx = nullptr;

	D3DXVECTOR3 HalfSize = m_size * 0.5;

	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(m_pos.x - HalfSize.x, m_pos.y - HalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + HalfSize.x, m_pos.y - HalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - HalfSize.x, m_pos.y + HalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + HalfSize.x, m_pos.y + HalfSize.y, 0.0f);

	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	pVtx[0].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);
	pVtx[1].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);
	pVtx[2].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);
	pVtx[3].color = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, m_RGBA.a);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtx[0].tex = D3DXVECTOR2(m_UV.x, m_UV.y);
	pVtx[1].tex = D3DXVECTOR2(m_UV.x + m_UVWH.x, m_UV.y);
	pVtx[2].tex = D3DXVECTOR2(m_UV.x, m_UV.y + m_UVWH.y);
	pVtx[3].tex = D3DXVECTOR2(m_UV.x + m_UVWH.x, m_UV.y + m_UVWH.y);

	m_pVtxBuffer->Unlock();
}

//�Z�b�^�[
void CScene2D::SetLayerR(int Value)
{
	if (Value < 0) { m_RGBA.r = 0; return; }
	if (Value > 255){ m_RGBA.r = 255; return; }
	m_RGBA.r = Value;
}
void CScene2D::SetLayerG(int Value)
{
	if (Value < 0) { m_RGBA.g = 0; return; }
	if (Value > 255) { m_RGBA.g = 255; return; }
	m_RGBA.g = Value;
}
void CScene2D::SetLayerB(int Value) 
{
	if (Value < 0) { m_RGBA.b = 0; return; }
	if (Value > 255) { m_RGBA.b = 255; return; }
	m_RGBA.b = Value;
}
void CScene2D::SetLayerA(int Value) 
{
	if (Value < 0) { m_RGBA.a = 0; return; }
	if (Value > 255) { m_RGBA.a = 255; return; }
	m_RGBA.a = Value;
}

void CScene2D::SetRGBA(const RGBA& rgba)
{
	SetLayerR(rgba.r);
	SetLayerG(rgba.g);
	SetLayerB(rgba.b);
	SetLayerA(rgba.a);
}
void CScene2D::ChangeAlpha(int ChangeValue) 
{
	m_RGBA.a += ChangeValue;
	if (m_RGBA.a < 0) { m_RGBA.a = 0; return; }
	if (m_RGBA.a > 255) { m_RGBA.a = 255; return; }
}