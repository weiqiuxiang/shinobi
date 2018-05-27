#include "CMouseWindow.h"
#include "CRenderer.h"

CMouseWindow::CMouseWindow()
{
	m_UsingMouseOld = false;
	m_DrawFlag = false;
	m_FrameWidht = 0;
	m_pVtxBuffer = nullptr;

	m_PosStart = D3DXVECTOR2(0,0);
	m_PosEnd = D3DXVECTOR2(0, 0);
	m_StartPoint = D3DXVECTOR2(0, 0);
	m_EndPoint = D3DXVECTOR2(0, 0);
}

CMouseWindow::~CMouseWindow()
{

}

bool CMouseWindow::Init(const RGBA& color, float FrameWidht)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX2D, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
	{
		MessageBox(nullptr, "���_�o�b�t�@�m�ۂ����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return false;
	}

	m_RGBA = color;
	m_FrameWidht = FrameWidht;

	return true;
}

//��������:
//IsMousePress	: �h���b�N���Ȃ�true
//MousePos		: ���}�E�X���E�B���h�E��ɂ�����W
void CMouseWindow::UpdateDragWindow( bool IsMousePress, const D3DXVECTOR2& MousePos)
{
	//�ʂ̃I�u�W�F�N�g�̓}�E�X�g�p���̎�
	if (IsMousePress == false){
		m_DrawFlag = false;
	}
	else {
		m_DrawFlag = true;
	}

	//�}�E�X�n�_�L�^
	if (m_UsingMouseOld == false && IsMousePress == true) {
		m_PosStart = MousePos;
	}

	m_PosEnd = MousePos;

	//�g�p�t���O�̋L�^
	m_UsingMouseOld = IsMousePress;

	//�J�n�ƏI���|�C���g�v�Z
	CalcStartEndPoint();

	//���_���b�N
	LockVertex();
}

//�g�`��
void CMouseWindow::DrawWindow(void)
{
	//�`�悵�Ȃ��Ȃ�A�֐��I��
	if (m_DrawFlag == false) {
		return;
	}

	//�f�o�C�X�擾
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//�E�B���h�E�`��
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));
	pDevice->SetFVF(FVF_VERTEX2D);
	pDevice->SetTexture(0, nullptr);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//�E�B���h�E�̃t���[���`��
	DrawLine();
}

//�������
void CMouseWindow::Uninit(void)
{
	if (m_pVtxBuffer != nullptr) {
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}

//�g���`��
void CMouseWindow::DrawLine(void)
{
	//�l�̃��C���̃o�b�t�@��ݒ�
	float halfWidth = m_FrameWidht * 0.5f;
	D3DXVECTOR2 Top[] = {
		D3DXVECTOR2(m_StartPoint.x - halfWidth,m_StartPoint.y),
		D3DXVECTOR2(m_EndPoint.x + halfWidth,m_StartPoint.y)
	};
	D3DXVECTOR2 Bottom[] = {
		D3DXVECTOR2(m_StartPoint.x - halfWidth,m_EndPoint.y),
		D3DXVECTOR2(m_EndPoint.x + halfWidth,m_EndPoint.y)
	};
	D3DXVECTOR2 Left[] = {
		D3DXVECTOR2(m_StartPoint.x,m_StartPoint.y - halfWidth),
		D3DXVECTOR2(m_StartPoint.x ,m_EndPoint.y + halfWidth)
	};
	D3DXVECTOR2 Right[] = {
		D3DXVECTOR2(m_EndPoint.x,m_StartPoint.y - halfWidth),
		D3DXVECTOR2(m_EndPoint.x ,m_EndPoint.y + halfWidth)
	};

	//���𐶐�����
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	LPD3DXLINE Line;
	D3DXCreateLine(pDevice, &Line);

	//���̐F
	D3DCOLOR LineColor = D3DCOLOR_RGBA(m_RGBA.r, m_RGBA.g, m_RGBA.b, 255);

	Line->SetAntialias(TRUE);
	Line->SetWidth(m_FrameWidht);
	Line->Begin();
	Line->Draw(Top, 2, LineColor);
	Line->Draw(Bottom, 2, LineColor);
	Line->Draw(Left, 2, LineColor);
	Line->Draw(Right, 2, LineColor);
	Line->End();

	Line->Release();
}

//���_���b�N
void CMouseWindow::LockVertex(void)
{
	//�`�悵�Ȃ��Ȃ�A���_���b�N���Ȃ�
	if (m_DrawFlag == false) {
		return;
	}

	if (m_pVtxBuffer == nullptr) {
		return;
	}

	//���_���b�N
	VERTEX_2D *pVtx = nullptr;

	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(m_StartPoint.x, m_StartPoint.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_EndPoint.x, m_StartPoint.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_StartPoint.x, m_EndPoint.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_EndPoint.x, m_EndPoint.y, 0.0f);

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

void CMouseWindow::CalcStartEndPoint(void)
{
	//X��������
	if (m_PosStart.x < m_PosEnd.x) {
		m_StartPoint.x = m_PosStart.x;
		m_EndPoint.x = m_PosEnd.x;
	}
	else {
		m_StartPoint.x = m_PosEnd.x;
		m_EndPoint.x = m_PosStart.x;
	}

	//Y��������
	if (m_PosStart.y < m_PosEnd.y) {
		m_StartPoint.y = m_PosStart.y;
		m_EndPoint.y = m_PosEnd.y;
	}
	else {
		m_StartPoint.y = m_PosEnd.y;
		m_EndPoint.y = m_PosStart.y;
	}
}