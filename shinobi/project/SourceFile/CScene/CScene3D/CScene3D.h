#pragma once
#ifndef _CSCENE3D_H_
#define _CSCENE3D_H_

#include "main.h"
#include "CScene.h"
#include "CScene3D_Draw.h"

class CScene3D : public CScene
{
public:
	typedef enum
	{
		TYPE_NORMAL = 0,
		TYPE_BILLBOARD,
		TYPE_BILLBOARD_BOTTOM
	}CSCENE3D_TYPE;

	//�����o�֐�
	CScene3D();
	CScene3D(CScene3D_Draw *pDrawType);
	CScene3D(const SRP &worldMtx, const D3DXVECTOR3 &pSize);
	~CScene3D();

	//Init,Update,Draw,Uninit�֐�
	virtual HRESULT Init(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const char *filePass);
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
	void SetDrawType(CSCENE3D_TYPE drawType);

	//�Q�b�^�[
	int GetLayerA(void) { return m_RGBA.a; }       //ALPHA���C���[�̒l�擾
	D3DXVECTOR3 GetSize(void) { return m_size; }
	CSCENE3D_TYPE GetType(void) { return m_Type; }
	TEX_ID GetTexID(void) { return *m_TexID; }
	SRP GetWorldSRP(void) { SRP SclRotPos = {m_pos,m_rot,m_scl}; return SclRotPos; }
	const D3DXMATRIX* GetWorldMtx(void) { return &m_WorldMtx; }

	//���̊֐�
	void AddPosition(const D3DXVECTOR3& Speed) { m_pos += Speed; }           //pos�����X�ω�������

protected:
	//�����o�֐�
	void SetRenderStates(void);          //�����_�[�X�e�[�g���ݒ�

	//�����o�ϐ�
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_scl;
	D3DXVECTOR3 m_size;
	RGBA		m_RGBA;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;
	CScene3D_Draw *m_DrawType;
	TEX_ID *m_TexID;
	CSCENE3D_TYPE m_Type;
	D3DXMATRIX m_WorldMtx;                         //���[���h�}�g���N�X
};

#endif