#ifndef CMOUSE_WINDOW_H_
#define CMOUSE_WINDOW_H_

#include "main.h"

class CMouseWindow 
{
public:
	CMouseWindow();
	~CMouseWindow();

	bool Init(const RGBA& color,float FrameWidht);
	void UpdateDragWindow(bool IsMousePress,const D3DXVECTOR2& MousePos);
	void Uninit(void);
	void DrawWindow(void);

	//�Q�b�^�[
	D3DXVECTOR2 GetStartPoint(void) { return m_StartPoint; }
	D3DXVECTOR2 GetEndPoint(void) { return m_EndPoint; }
	bool GetUsingFlag(void) { return m_UsingMouseOld; }

private:
	void DrawLine(void);				//�g���`��
	void LockVertex(void);				//���_���b�N
	void CalcStartEndPoint(void);		

private:
	bool			m_DrawFlag;				//�`��t���O
	bool			m_UsingMouseOld;		//�}�E�X�g�p�Â��t���O
	D3DXVECTOR2		m_PosStart;				//�}�E�X�̊J�n���W
	D3DXVECTOR2		m_PosEnd;				//�}�E�X�̏I�����W
	D3DXVECTOR2		m_StartPoint;			//�J�n���W
	D3DXVECTOR2		m_EndPoint;				//�I�����W

	//�E�B���h�E�p�����[�^
	RGBA			m_RGBA;					//�E�B���h�E�̐F
	float			m_FrameWidht;			//�g�̑���

	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuffer;	//���_�o�b�t�@(�E�B���h�E�`��p)
};

#endif