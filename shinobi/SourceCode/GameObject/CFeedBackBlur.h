//����: �t�B�[�h�o�b�N�u���[
#ifndef CFEEDBACKBLUR_H_
#define CFEEDBACKBLUR_H_

#include "CRenderTarget2D.h"

//����錾
class CGame;

class CFeedBackBlur
{
public:
	CFeedBackBlur();
	~CFeedBackBlur() {}

	bool Init(void);
	void DrawBlurOnDrawTarget(CGame*);	//�u���[�^�[�Q�b�g�ɕ`��
	void Draw(void);
	void Uninit(void);

	LPDIRECT3DSURFACE9 GetDrawGameSurfacePointer(void) { return m_RenderTargetDrawInGame.GetSurfacePointer(); }

private:
	D3DXVECTOR3			m_TargetBlurPolygonSize;	//�u���[�T�C�Y
	int					m_BlurCount;				//�J�E���g
	CRenderTarget2D		m_RenderTargetBlur;			//�u���[�p
	CRenderTarget2D		m_RenderTargetDrawInGame;	//�Q�[���̒��ɕ`��p
};

#endif