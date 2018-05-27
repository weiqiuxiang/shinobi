#pragma once
#ifndef _CSKYDOME_H_
#define _CSKYDOME_H_

#include "main.h"
#include "CMeshField.h"

class CSkyDome
{
public:
	CSkyDome();
	~CSkyDome();

	//Init,Uninit,Update,Draw�֐�
	HRESULT Init(float fRadius, int nHor_PieceNum,int m_nVer_PieceNum, const char *TexFilePass);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//�Z�b�^�[
	void SetStartEndAngleVertical(float fStart_Angel, float fEnd_Angel) {
		m_fStart_Angel_Vertical = fStart_Angel;
		m_fEnd_Angel_Vertical = fEnd_Angel;
	}
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }
private:
	//�����o�֐�
	void LockSkyDomeVertexBuf(void);
	void LockSkyDomeIndexBuf(void);
	HRESULT InitLockVertexBufAndIndexBuf(void);

	//�����o�ϐ�
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_scl;
	int	m_TexID;
	float m_fRadius;							//�X�J�C�h�[���̔��a
	int   m_nHor_PieceNum;						//���������̕�����
	int   m_nVer_PieceNum;						//���������̕�����
	float m_fStart_Angel_Vertical;				//���������J�n�p�x
	float m_fEnd_Angel_Vertical;				//���������I���p�x
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;		//���_�o�b�t�@
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;		//�C���f�b�N�X�o�b�t�@
	INDEX_DATA m_IndexData;
};

#endif