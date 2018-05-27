//���� : �G��AI����̃C���^�t�F�[�X
#pragma once
#ifndef CAI_ACTION_INTERFACE_H_
#define CAI_ACTION_INTERFACE_H_

//�C���N���[�h�w�b�_
#include "main.h"
#include "CDebug.h"

//����錾
class CPlayer;
class CEnemy;

class CAIActionInterface 
{
public:
	virtual void AIModeAction(void) = 0;				//AI�̐U�镑��

	virtual void SetPlayer(CPlayer *pPlayer) = 0;
	virtual void SetAI(CEnemy *pPlayer) = 0;

	virtual CPlayer* GetPlayer(void) = 0;
	virtual CEnemy* GetAI(void) = 0;
};

class CNullAIAction : public CAIActionInterface
{
	virtual void AIModeAction(void) {
		CDebug::AddText("�֐�AIModeAction,AI�̃C���^�t�F�[�X��nullptr�ł�\n");
	}

	virtual void SetPlayer(CPlayer *pPlayer) { pPlayer; CDebug::AddText("�֐�SetPlayer,AI�̃C���^�t�F�[�X��nullptr�ł�\n"); }
	virtual void SetAI(CEnemy *pPlayer) { pPlayer; CDebug::AddText("�֐�SetAI,AI�̃C���^�t�F�[�X��nullptr�ł�\n"); }

	virtual CPlayer* GetPlayer(void) { CDebug::AddText("�֐�GetPlayer,AI�̃C���^�t�F�[�X��nullptr�ł�\n"); return nullptr; }
	virtual CEnemy* GetAI(void) { CDebug::AddText("�֐�GetAI,AI�̃C���^�t�F�[�X��nullptr�ł�\n"); return nullptr;}
};

#endif