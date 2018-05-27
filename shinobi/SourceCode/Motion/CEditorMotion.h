//���� : ���[�V�����G�f�B�^�[�p���[�V����
#pragma once
#ifndef CEDITOR_MOTION_H_
#define CEDITOR_MOTION_H_

#include "main.h"
#include "CMotion.h"
#include "CPart.h"
#include <stdio.h>
#include <string>

#define EDITOR_MOTION_MAX (40)

class CEditorMotion
{
public:
	CEditorMotion();
	~CEditorMotion();

	//Init,Uninit,Update,Draw�֐�
	void LoadMotionDataToInit(const char* MotionPass);		//���[�V�����f�[�^��ǂݍ���ŏ���������
	void UpdateOffset(void);
	void UpdateKeyFrame(void);
	void UpdateKeyFramePreview(float Percent);
	void UpdateMotion(void);
	void Draw(void);
	void Uninit(void);

	//�Q�b�^�[
	D3DXMATRIX GetPartMtx (int PartID) {
		if (-1 == PartID) return m_WorldMtx;
		return m_Part[PartID].m_MtxNow; 
	}
	int GetTotalFrame(int MotionID) { return m_MotionTotalFrame[MotionID]; }				//���[�V�����̑��t���[�����擾
	int GetFrameOfKeyFrame(int MotionID, int KeyFrameID) { return m_Motion[MotionID].m_KeyFrame[KeyFrameID].m_TotalFrame; }
	int GetTotalKeyFrame(int MotionID) { return m_Motion[MotionID].GetTotalKeyFrame(); }	//���̃��[�V�����̃L�[�t���[���������擾
	int GetKeyFrameNow(void){ return m_KeyFrameNow; }
	int GetMotionNow(void) { return m_MotionNow; }
	D3DXVECTOR3 GetPartPosInWorld(int PartID) { return D3DXVECTOR3(m_Part[PartID].m_MtxNow._41, m_Part[PartID].m_MtxNow._42, m_Part[PartID].m_MtxNow._43); }
	int GetPartTotalNum(void) { return m_PartNum; }
	CPart* GetPartPointer(int PartID) {
		if (nullptr == m_Part) return nullptr;
		return &m_Part[PartID];
	}
	int GetTotalMotionNum(void) { return m_TotalMotion; }
	CMotion* GetMotion(int MotionNum) { return &m_Motion[MotionNum]; }
	inline void GetKeyFrameHerminateDirNow(D3DXVECTOR2* pOutStartDir, D3DXVECTOR2 *pOutEndDir) {
		*pOutStartDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir;
		*pOutEndDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir;
	}
	inline float GetKeyFrameChangeSpeed(void) {	//�L�[�t���[���̕ύX���x
		return (1.0f / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame);
	}
	const char* GetPartFilePass(int PartID) { return m_PartFilePass[PartID].c_str(); }

	//�Z�b�^�[
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetWorldRotMtx(const D3DXMATRIX& worldMtx) { m_WorldRotMtx = worldMtx; }
	void SetMotionNow(int MotionNumber);
	void SetKeyFrameNow(int KeyFrame);
	void SetFrameOfKeyFrame(int MotionID, int KeyFrameID,int Value) { m_Motion[MotionID].m_KeyFrame[KeyFrameID].m_TotalFrame = Value; }
	void SetOtherParentMtx(const D3DXMATRIX& worldMtx) {
		m_OtherParentMtx = worldMtx;
	}
	inline void SetKeyFrameHerminateDir(const D3DXVECTOR2& StartDir, const D3DXVECTOR2 &EndDir)
	{
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir = StartDir;
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir = EndDir;
	}

	//���̊֐�
	void AddKeyFrame(void);			//���݂̃��[�V�����ɃL�[�t���[���𑝂₷
	void AddKeyFrame(int MotionNum);//�w�肷�郂�[�V�����ɃL�[�t���[���𑝂₷
	void AddMotion(void);			//���[�V�����𑝂₷(�L�[�t���[���̃f�t�H���g��2��)
	void InsertKeyFrame(void);		//�L�[�t���[���̑}��
	void InsertMotion(void);		//���[�V�����̑}��
	void DeleteMotionNow(void);		//���̃��[�V�������폜���A�z��̌��̕������l�߂�
	void DeleteKeyFrameNow(void);	//���̃��[�V�����̍��̃L�[�t���[�����폜���A�z��̌��̕������l�߂�
	void ResetOtherParentMtx(void) { D3DXMatrixIdentity(&m_OtherParentMtx); }
private:
	//�����o�֐�
	void LoadPartData(FILE *fp);
	void LoadMotionData(FILE *fp);
	void KeyFrameChange(void);				//�L�[�t���[���ς��
	int CalcNextKeyFrame(void);				//���̃L�[�t���[��
	void LinkParentAndChildren(void);		//�e�Ǝq���̊֌W���т�

	//�\���n�֐�
	void ShowOffset(void);					//�I�t�Z�b�g�̍Đ�
	void ShowKeyFrame(void);				//�w��L�[�t���[���̍Đ�
	void ShowKeyFramePreview(float Percent);//�w��L�[�t���[���̃v���r���[
	void ShowMotion(void);					//�w�胂�[�V�����̍Đ�
	void CalcOffset(CPart*);				//�S�ăp�[�c�̃I�t�Z�b�g�̌v�Z
	void CalcKeyFrame(CPart*);				//�S�ăp�[�c�̃L�[�t���[���̌v�Z
	void CalcKeyFramePreview(CPart*,float Percent);
	void CalcPartWorldMtxRecursive(CPart*);	//���f���S�̂̃��[���h�}�g���N�X�ċA
private:
	CPart	m_Part[MAX_PART];
	std::string m_PartFilePass[MAX_PART];			//�p�[�c�̃t�@�C���p�X
	CMotion	m_Motion[EDITOR_MOTION_MAX];
	int		m_MotionTotalFrame[EDITOR_MOTION_MAX];	//���[�V�������Ƃ̑��t���[����
	int		m_PartNum;					//�p�[�c��
	int		m_TotalMotion;				//���[�V������
	int		m_MotionOld;				//�Â����[�V����
	D3DXVECTOR3	m_pos;					//���[���h�ʒu(�O���ݒ�)
	D3DXMATRIX	m_WorldMtx;				//���[���h�̃}�g���N�X
	D3DXMATRIX	m_WorldRotMtx;			//���݃��[���h��]�}�g���N�X(�O���ݒ�)
	D3DXMATRIX	m_OtherParentMtx;		//�ʂ̃I�u�W�F�N�g�͐e�Ƃ��Ă̏ꍇ,���̐e�̃}�g���N�X

	int		m_MotionNow;				//�����s���̃��[�V�����ԍ�
	int		m_KeyFrameNow;				//���s���̃L�[�t���[���ԍ�
	int		m_FrameNow;					//���s���̃t���[���ԍ�
};

#endif