#include "CEnemyStorage.h"
#include "SceneTransition.h"
#include "CPolygonMath.h"
#include <stdio.h>
#include "CCreatureDataIO.h"
#include "CTextLoad.h"

#define TREE_WIDTH (24.0f)
#define TREE_HEIGHT (24.0f)

#define DEFAULT_ENEMY_COFFI (0.2f)

void CCreatureDataIO::ExportEnemyObjData(void)
{
	//�t�@�C�����J��
	FILE *fp;
	fp = fopen("data/CREATURE_DATA/EnemyDataOutput.txt","w");

	//�K�v�ȃf�[�^�ϐ�
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;
	CEnemy::ENEMY_TYPE type;

	//���R�����g
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "#****************�G�̃f�[�^***************\n");
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "\n");

	//�r���{�[�h�̏����o��
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for(; it != itEnd; it++)
	{
		//�|�C���^�擾
		if (*it == nullptr) {
			continue;
		}

		//�f�[�^�擾
		POS = (*it) -> GetPos();
		ROT = (*it) ->GetRot();
		type = (*it) ->GetEnemyType();

		//�f�[�^�������o��
		switch (type) {
		case CEnemy::TYPE_DOG:
			fprintf(fp, "#-----------------------------------------\n");
			fprintf(fp, "#��\n");
			fprintf(fp, "#-----------------------------------------\n");
			break;
		}
		fprintf(fp, "ENEMY_BEGIN\n");

		fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);	//�ʒu�����o��
		fprintf(fp, "\tROT= %.2f %.2f %.2f\n", ROT.x, ROT.y, ROT.z);	//�ʒu
		fprintf(fp, "\tTYPE= %d\n\n", (int)type);						//�G�̃^�C�v
		
		//����E�F�C�|�C���g
		auto itPoint = (*it)->GetPathPointListBegin();
		auto itPointEnd = (*it)->GetPathPointListEnd();

		fprintf(fp, "\tPATHPOINT_LIST_BEGIN\n\n");

		for (; itPoint != itPointEnd; itPoint++) {
			if (*itPoint == nullptr) {
				continue;
			}

			D3DXVECTOR3 PointPos = (*itPoint) ->GetPos();
			int StopFrame = (*itPoint) ->GetStopFrame();

			fprintf(fp, "\t\tPOINT_BEGIN\n");
			fprintf(fp, "\t\t\tPOS= %.2f %.2f %.2f\n", PointPos.x, PointPos.y, PointPos.z);	//�ʒu�����o��
			fprintf(fp, "\t\t\tWAITTIME= %d\n", StopFrame);									//�҂����Ԃ�`���o��
			fprintf(fp, "\t\tPOINT_END\n\n");
		}

		fprintf(fp, "\tPATHPOINT_LIST_END\n");
		fprintf(fp, "ENEMY_END\n\n");
	}

	//�t�@�C�������
	fclose(fp);
}

void CCreatureDataIO::ImportEnemyObjData(const char* pObjFilePass)
{
	//�t�@�C�����J��
	FILE *fp;
	fp = fopen(pObjFilePass, "r");

	//nullptr�`�F�b�N
	if (nullptr == fp)           //nullptr�`�F�b�N
	{
		fclose(fp);
		MessageBox(nullptr, "���b�V���f�[�^�̃e�L�X�g�����݂��܂���", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}

	//�f�[�^�̓ǂݍ���
	char buf[1024];
	while(CTextLoad::GetStrToken(fp,TOKEN,buf) != -1)
	{
		if (strcmp("ENEMY_BEGIN", buf) == 0)
		{
			ReadEnemyObjs(fp);
		}
	}

	//�t�@�C�������
	fclose(fp);
}

void CCreatureDataIO::ExportPlayerObjData(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) {
		_RPT0(0,"[CCreatureDataIO.cpp][ExportPlayerObjData]����pPlayer�̓k��\n");
		return;
	}

	//�t�@�C�����J��
	FILE *fp;
	fp = fopen("data/CREATURE_DATA/PlayerDataOutput.txt", "w");

	//�K�v�ȃf�[�^�ϐ�
	D3DXVECTOR3 POS = pPlayer->GetPos();
	D3DXVECTOR3 ROT = pPlayer->GetRot();

	//���R�����g
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "#************�v���[���[�̃f�[�^***********\n");
	fprintf(fp, "#-----------------------------------------\n");

	//�ʒu�Ɖ�]����`���o��
	fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);	//�ʒu�����o��
	fprintf(fp, "\tROT= %.2f %.2f %.2f\n", ROT.x, ROT.y, ROT.z);	//��]����`���o��

	//�t�@�C�������
	fclose(fp);
}

CPlayer* CCreatureDataIO::ImportPlayerObjData(const char* pObjFilePass)
{
	CPlayer* pPlayer = nullptr;

	//�t�@�C�����J��
	FILE *fp;
	fp = fopen(pObjFilePass, "r");

	//nullptr�`�F�b�N
	if (nullptr == fp)           //nullptr�`�F�b�N
	{
		fclose(fp);
		MessageBox(nullptr, "���b�V���f�[�^�̃e�L�X�g�����݂��܂���", "�G���[", MB_OK | MB_ICONHAND);
		return nullptr;
	}

	char buf[1024];
	while (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1)
	{
		pPlayer = ReadPlayerObjs(fp);
	}

	//�t�@�C�������
	fclose(fp);

	return pPlayer;
}

CPlayer* CCreatureDataIO::ReadPlayerObjs(FILE *fp)
{
	//�K�v�ȃf�[�^�ϐ�
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;

	char buf[1024];

	//�ʒu�̓ǂݍ���
	CTextLoad::FindFont(fp, TOKEN, "POS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.z = (float)atof(buf);
	//�T�C�Y�̓ǂݍ���
	CTextLoad::FindFont(fp, TOKEN, "ROT=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.z = (float)atof(buf);

	CPlayer* pPlayer = new CPlayer();
	if (pPlayer == nullptr) {
		return nullptr;
	}

	pPlayer->Init(POS, ROT, MOTION_DATA_PLAYER);

	return pPlayer;
}

void CCreatureDataIO::ReadEnemyObjs(FILE *fp)
{
	//�K�v�ȃf�[�^�ϐ�
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;
	CEnemy::ENEMY_TYPE type;
	
	char buf[1024];

	//�ʒu�̓ǂݍ���
	CTextLoad::FindFont(fp, TOKEN, "POS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.z = (float)atof(buf);

	CTextLoad::FindFont(fp, TOKEN, "ROT=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.z = (float)atof(buf);

	CTextLoad::FindFont(fp, TOKEN, "TYPE=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	type = (CEnemy::ENEMY_TYPE)atoi(buf);

	CEnemy *pEnemy = CEnemyStorage::Create(type, POS, ROT, MOTION_DATA_ENEMYDOG, 1.0f, -0.1f, DEFAULT_ENEMY_COFFI);
	if (pEnemy == nullptr) {
		return;
	}

	//����p�X��ǂݍ���
	while (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1)
	{
		//���X�G���h�܂œǂݍ���
		if (strcmp("PATHPOINT_LIST_END", buf) == 0) {
			break;
		}
		if (strcmp("POINT_BEGIN", buf) != 0) {
			continue;
		}

		D3DXVECTOR3 POINTPOS;
		int WaitTime;

		CTextLoad::FindFont(fp, TOKEN, "POS=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		POINTPOS.x = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		POINTPOS.y = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		POINTPOS.z = (float)atof(buf);

		CTextLoad::FindFont(fp, TOKEN, "WAITTIME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		WaitTime = atoi(buf);

		pEnemy->AddPathPoint(POINTPOS, WaitTime);
	}
}