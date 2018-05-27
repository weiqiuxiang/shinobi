#pragma once
#ifndef _CSCALE_UI_H_
#define _CSCALE_UI_H_

#include "main.h"
#include "CScene2D.h"

//�O���錾
class CScene2D;

class CFontUIScale : public CScene2D
{
public:
	CFontUIScale();
	~CFontUIScale();
	HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize, const char *filePass) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	//�Q�b�^�[

	//�Z�b�^�[
	void SetScale(float fScale) { m_Scale = D3DXVECTOR2(max(0.01f, fScale), max(0.01f, fScale)); }
	void SetScaleX(float fScale) { m_Scale.x = max(0.01f, fScale); }
	void SetScaleY(float fScale) { m_Scale.y = max(0.01f, fScale); }
	void SetOriginPos(const D3DXVECTOR3& pos) { m_OriginPos = pos; }
	void SetScalePoint(const D3DXVECTOR3 &ScalePoint) { m_ScalePoint = ScalePoint; }
	void SetAddBlending(bool bAddBlending) { m_bAddBlending = bAddBlending; }

private:
	//�X�P�[���̂��߂̕ϐ�
	D3DXVECTOR3 m_ScalePoint;				//�X�P�[�����S
	D3DXVECTOR3 m_OriginPos;				//���̈ʒu
	D3DXVECTOR3 m_OriginSize;				//���̃T�C�Y
	D3DXVECTOR2 m_Scale;							//�X�P�[���{��
	bool m_bAddBlending;						//�A���t�@���Z
};

#endif