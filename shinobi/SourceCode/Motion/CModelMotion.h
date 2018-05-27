#pragma once
#ifndef CMODEL_MOTION_H_
#define CMODEL_MOTION_H_

#include "main.h"
#include "CMotion.h"
#include "CPart.h"
#include <stdio.h>

#define MOTION_MAX (40)

class CModelMotion
{
public:
	CModelMotion();
	~CModelMotion();

	//Init,Uninit,Update,Draw�֐�
	void LoadMotionDataToInit(const char* MotionPass);        //���[�V�����f�[�^��ǂݍ���ŏ���������
	void Update(void);
	void Draw(void);
	void DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(void);
	void DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture);
	void Uninit(void);

	//�Q�b�^�[
	D3DXMATRIX GetPartMtx (int PartID) {
		if (-1 == PartID) return m_WorldMtx;
		return m_Part[PartID].m_MtxNow; 
	}
	LPD3DXMESH GetMesh(int PartID) {
		return m_Part[PartID].GetMesh();
	}
	int GetTotalFrame(int MotionID) { return m_MotionTotalFrame[MotionID]; }				//���[�V�����̑��t���[�����擾
	int GetFrameOfKeyFrame(int MotionID, int KeyFrameID) { return m_Motion[MotionID].m_KeyFrame[KeyFrameID].m_TotalFrame; }
	int GetTotalKeyFrame(int MotionID) { return m_Motion[MotionID].GetTotalKeyFrame(); }	//���̃��[�V�����̃L�[�t���[���������擾
	int GetKeyFrameNow(void){ return m_KeyFrameNow; }
	int GetMotionNow(void) { return m_MotionNow; }
	D3DXVECTOR3 GetPartPosInWorld(int PartID) { return D3DXVECTOR3(m_Part[PartID].m_MtxNow._41, m_Part[PartID].m_MtxNow._42, m_Part[PartID].m_MtxNow._43); }
	void GetHerminateDir(D3DXVECTOR2* pOutStartDir, D3DXVECTOR2 *pOutEndDir) {
		*pOutStartDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir;
		*pOutEndDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir;
	}

	//�Z�b�^�[
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetWorldRotMtx(const D3DXMATRIX& worldMtx) { m_WorldRotMtx = worldMtx; }
	void SetMotionNow(int MotionNumber);
	void SetOtherParentMtx(const D3DXMATRIX& worldMtx) {
		m_OtherParentMtx = worldMtx;
	}
	void SetDrawType(CModel::DRAW_TYPE type);
	void SetMaterial(const D3DMATERIAL9& Mtrl);

	//���̊֐�
	void AddMtxWorld() {}
	void ResetOtherParentMtx(void) { D3DXMatrixIdentity(&m_OtherParentMtx); }
private:
	//�����o�֐�
	void LoadPartData(FILE *fp);
	void LoadMotionData(FILE *fp);
	void CalcPartWorldMtx(void);			//���f���S�̂̃��[���h�}�g���N�X�v�Z
	void CalcWorldMtx(void);				//���[���h�}�g���N�X���v�Z
	void CalcPartWorldMtxRecursive(CPart*);	//���f���S�̂̃��[���h�}�g���N�X�ċA
	void KeyFrameChange(void);				//�L�[�t���[���ς��
	int CalcNextKeyFrame(void);				//���̃L�[�t���[��
	void LinkParentAndChildren(void);		//�e�Ǝq���̊֌W���т�

	//�����o�ϐ�
	CPart m_Part[MAX_PART];
	CMotion m_Motion[MOTION_MAX];
	int   m_MotionTotalFrame[MOTION_MAX];		//���[�V�������Ƃ̑��t���[����
	int   m_PartNum;					//�p�[�c��
	int   m_TotalMotion;				//���[�V������
	int   m_MotionOld;					//�Â����[�V����
	D3DXVECTOR3 m_pos;					//���[���h�ʒu(�O���ݒ�)
	D3DXMATRIX m_WorldMtx;				//���[���h�̃}�g���N�X
	D3DXMATRIX m_WorldRotMtx;			//���݃��[���h��]�}�g���N�X(�O���ݒ�)
	D3DXMATRIX m_OtherParentMtx;		//�ʂ̃I�u�W�F�N�g�͐e�Ƃ��Ă̏ꍇ,���̐e�̃}�g���N�X

	int   m_MotionNow;					//�����s���̃��[�V�����ԍ�
	int   m_KeyFrameNow;				//���s���̃L�[�t���[���ԍ�
	int   m_FrameNow;					//���s���̃t���[���ԍ�
};

#endif