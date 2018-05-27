#pragma once
#ifndef CCOLLISION_CIRCLE_H_
#define CCOLLISION_CIRCLE_H_

#include "main.h"

class CCollisionCircle
{
public:
	CCollisionCircle();
	~CCollisionCircle();

	typedef enum
	{
		TYPE_NONE,						//�_�~�[
		TYPE_BODY,						//���̂ɓ�����Ɛi�܂Ȃ��Ƃ�,TYPE_HIT�Ƃ̓��蔻��ōU�����󂯂����ǂ����𔻒肷��^�C�v
		TYPE_HIT_PREVIEW,				//���蔻��~���g�p���
		TYPE_HIT,						//�U���p�̃^�C�v
		TYPE_HIT_ENABLE,				//���蔻��v�Z���L���ɂȂ�
	}TYPE;

	HRESULT Init(float Radius);
	HRESULT Init(float Radius , int ParentID, TYPE type);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//�Q�b�^�[
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXMATRIX GetMtxResultOld(void){ return m_MtxResultOld; }
	D3DXMATRIX GetMtxResult(void) { return m_MtxResult; }
	float GetRadius(void) { return m_Radius; }
	int GetParentID(void) { return m_ParentID; }
	TYPE GetType(void) { return m_type; }
	D3DXVECTOR3 GetOffset(void) { return m_posOffset; }
	bool GetUse(void) { return m_bUse; }

	//�Z�b�^�[
	void SetType(TYPE type) { m_type = type; }                      //�^�C�v�Z�b�g(�ύX)
	void SetRadius(float Radius) { m_Radius = max(0.01f, Radius); }
	void SetOffset(const D3DXVECTOR3& Pos) { m_posOffset = Pos; }
	void SetUse(bool bUse) { m_bUse = bUse; }
	void SetMtxResult(const D3DXMATRIX& Mtx) { m_MtxResult = Mtx; }

	//���̊֐�
	static bool HitCheckUseNormal(CCollisionCircle* pA, CCollisionCircle* pB);
	static bool HitCheckUseSSV(CCollisionCircle* pA, CCollisionCircle* pB);
	static bool HitCheckUseSSV(CCollisionCircle* pA, CCollisionCircle* pB,float *t);

private:
	//�����o�֐�
	void MtxWorldSetting(void);
	void InitMaterialBaseOnType(void);
	void SetMaterialBaseOnType(void);

	//�����o�ϐ�
	D3DXVECTOR3 m_pos;					//�ʒu
	D3DXMATRIX m_MtxResult;				//���ʃ}�g���N�X
	D3DXMATRIX m_MtxResultOld;			//���ʃ}�g���N�X
	D3DXVECTOR3 m_posOffset;			//�I�t�Z�b�g
	D3DXVECTOR3 m_scl;					//�X�P�[��
	TYPE m_type;						//�^�C�v
	TYPE m_typeOld;						//�Â��^�C�v
	int m_ParentID;						//�e�ɂȂ郂�f���̃��[�V�����̃p�[�c��ID
	float m_Radius;						//���a
	LPD3DXMESH m_pMesh;					//���b�V���|�C���^
	D3DMATERIAL9 m_Mtrl;				//�}�e���A��
	bool m_bUse;						//�O���pUse�t���O
};

#endif