#pragma once
#ifndef CMOTION_H_
#define CMOTION_H_

#include "main.h"

#define MAX_PART (20)
#define MAX_KEY_FRAME (20)
#define MOTION_NAME_MAX (64)

class CKeyFrame
{
public:
	CKeyFrame();
	~CKeyFrame();

	//�����o�ϐ�
	SRP			m_SRPData[MAX_PART];	//���[���h�}�g���N�X�|�C���^(�p�[�c���Ɠ���)
	int			m_TotalFrame;			//�L�[�t���[���̃t���[����
	D3DXVECTOR2	m_HermitianStartDir;	//�G���~�[�g�Ȑ��n�_�ڐ��x�N�g��
	D3DXVECTOR2	m_HermitianEndDir;		//�G���~�[�g�Ȑ��I�_�ڐ��x�N�g��
};

//����:
//class CMotion�̓��[�V�����̃L�[�t���[������ۑ���,�L�[�t���[�����Ƃ̃f�[�^��ǂݍ���
class CMotion
{
public:
	CMotion();
	~CMotion();

	//�Z�b�^�[

	//�Q�b�^�[
	int GetTotalKeyFrame(void) { return (int)m_TotalKeyFrame; }			//���L�[�t���[�������Q�b�g
	CKeyFrame* GetKeyFrameData(int KeyFrameNum) { return &m_KeyFrame[KeyFrameNum]; }
	//���̊֐�
	
	//�����o�ϐ�
	bool			m_bLoop;					//���[�v�t���O
	KEY_FRAME_NUM	m_TotalKeyFrame;			//�L�[�t���[����
	CKeyFrame		m_KeyFrame[MAX_KEY_FRAME];	//�L�[�t���[���̃|�C���^
	char			m_Name[MOTION_NAME_MAX];	//���[�V�����̖��O
};

#endif