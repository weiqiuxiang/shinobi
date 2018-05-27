//�G�̈ʒu�Ə���|�C���g����o�̓N���X
#ifndef CCRETURE_DATA_IO_H_
#define CCRETURE_DATA_IO_H_

#include "main.h"
#include <stdio.h>

//����錾
class CPlayer;

//�N���X��`
class CCreatureDataIO
{
public:
	void ExportEnemyObjData(void);								//�G�f�[�^�̏����o��
	void ImportEnemyObjData(const char* pObjFilePass);			//�G�f�[�^�̓ǂݍ���

	void ExportPlayerObjData(CPlayer*);							//�v���[���[�f�[�^�̏����o��
	CPlayer* ImportPlayerObjData(const char* pObjFilePass);		//�v���[���[�f�[�^�̓ǂݍ���

private:
	//�ǂݍ��݊֐�
	CPlayer* ReadPlayerObjs(FILE *fp);
	void ReadEnemyObjs(FILE *fp);
};

#endif