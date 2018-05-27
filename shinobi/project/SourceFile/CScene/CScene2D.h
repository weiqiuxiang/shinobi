#pragma once
#ifndef _CSCENE2D_H_
#define _CSCENE2D_H_

#include "main.h"
#include "CScene.h"

class CScene2D : public CScene
{
public:
	//�����o�֐�
	CScene2D();
	CScene2D(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize);
	~CScene2D();

	//Init,Update,Draw,Uninit�֐�
	virtual HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize, const char *filePass);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Uninit(void);

	//�Z�b�^�[
	void SetSize(const D3DXVECTOR3& size) { m_size = size; }
	void SetLayerR(int Value);                     //RED���C���[�̒l�ݒ�
	void SetLayerG(int Value);                     //GREEN���C���[�̒l�ݒ�
	void SetLayerB(int Value);                     //BLUE���C���[�̒l�ݒ�
	void SetLayerA(int Value);                     //ALPHA���C���[�̒l�ݒ�
	void SetRGBA(const RGBA& rgba);                       //RGBA�ꊇ�ݒ�
	void ChangeAlpha(int ChangeValue);             //ALPHA���C���[�̒l�����X�ύX������
	void SetUV(const D3DXVECTOR2& UV) { m_UV = UV; }
	void SetUVWH(const D3DXVECTOR2& UVWH) { m_UVWH = UVWH; }

	//�Q�b�^�[
	int GetLayerA(void) { return m_RGBA.a; }       //ALPHA���C���[�̒l�擾
	D3DXVECTOR3 GetSize(void) { return m_size;}

	//���̊֐�
	void AddLayerA(int Value) { 
		m_RGBA.a += Value; 
		m_RGBA.a = min(255, m_RGBA.a);
		m_RGBA.a = max(0,m_RGBA.a);
	}
	void AddPosition(const D3DXVECTOR3& AddPos) { m_pos += AddPos; }

protected:
	//�����o�֐�
	void SetRenderStates(void);				//�����_�[�X�e�[�g���ݒ�
	void VertexBufferLock(void);			//���_���b�N
	void VertexBufferLockUV(void);			//���_���b�N

	//�����o�ϐ�
	D3DXVECTOR2 m_UV;
	D3DXVECTOR2 m_UVWH;
	D3DXVECTOR3 m_size;
	RGBA m_RGBA;
	TEX_ID *m_TexID;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;
};

#endif