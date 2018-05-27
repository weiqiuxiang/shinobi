#include "C_Obj3D_DataIO.h"
#include "CScene3DStorage.h"
#include "CModelStorage.h"
#include "SceneTransition.h"
#include "MousePosition.h"
#include "TexManager.h"
#include "ModelManager.h"
#include "CTextLoad.h"
#include "CMeshFieldManager.h"
#include "CPolygonMath.h"
#include <stdio.h>

#define TREE_WIDTH (24.0f)
#define TREE_HEIGHT (24.0f)

#define MODEL_ROTATION_SPEED (D3DX_PI * 0.05)

void C_Obj3D_DataIO::ExportObj3DData(void)
{
	//�t�@�C�����J��
	FILE *fp;
	fp = fopen("data/3D_OBJ_DATA/3D_OBJ_SAVE_DATA.txt","w");

	//�K�v�ȃf�[�^�ϐ�
	D3DXVECTOR3 POS;
	D3DXVECTOR3 SIZE;
	CScene3D::CSCENE3D_TYPE type;
	TEX_ID TexID;

	//���R�����g
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "#3D�I�u�W�F�N�g�f�[�^\n");
	fprintf(fp, "#-----------------------------------------\n");

	//�r���{�[�h�̏����o��
	for(int i = 0; i < CSCENE3D_OBJ_MAX; i++)
	{
		//�|�C���^�擾
		CScene3D *pScene3D = CScene3DStorage::GetObj(i);
		if (nullptr == pScene3D) continue;

		//�f�[�^�擾
		POS = pScene3D->GetPosition();
		SIZE = pScene3D->GetSize();
		type = pScene3D->GetType();
		TexID = pScene3D->GetTexID();
		const char* TEXTURE_PASS = CTexManager::GetTexturePass(TexID);

		//�f�[�^�������o��
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "#�r���{�[�h�I�u�W�F�̖��O\n");//���O�������o��
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "\tDATATYPE CSCENE3D\n");                               //�I�u�W�F�N�g�̃^�C�v��CScene3D
		fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);      //�ʒu�����o��
		fprintf(fp, "\tSIZE= %.2f %.2f %.2f\n", SIZE.x, SIZE.y, SIZE.z);  //�ʒu
		//�^�C�v�i���o�[����
		int TypeNumber = 0;
		if (type == CScene3D::TYPE_NORMAL) TypeNumber = 0;
		if (type == CScene3D::TYPE_BILLBOARD) TypeNumber = 1;
		if (type == CScene3D::TYPE_BILLBOARD_BOTTOM) TypeNumber = 2;
		fprintf(fp, "\tTYPE= %d\n", TypeNumber);                          //�^�C�v�����o��
		fprintf(fp, "\tTEXTURE_PASS= %s\n\n", TEXTURE_PASS);              //�e�N�X�`���p�X�����o��
	}

	D3DXVECTOR3 ROT;                          //���f���̉�]
	MODEL_ID MODEL_ID;                        //���f��ID

	//3D���f���̏����o��
	for (int i = 0; i < MODEL_OBJ_MAX; i++)
	{
		//�|�C���^�擾
		CModel *pModel = CModelStorage::GetObj(i);
		if (nullptr == pModel) continue;

		//�f�[�^�擾
		POS = pModel->GetPosition();
		ROT = pModel->GetRot();
		MODEL_ID = pModel->GetModelID();
		const char* MODEL_PASS = CModelManager::GetModelPass(MODEL_ID);

		//�f�[�^�����o��
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "#���f���I�u�W�F�̖��O\n");//���O�������o��
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "\tDATATYPE MODEL\n");                               //�I�u�W�F�N�g�̃^�C�v��CScene3D
		fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);      //�ʒu�����o��
		fprintf(fp, "\tROT= %.2f %.2f %.2f\n", ROT.x, ROT.y, ROT.z);      //��]��񏑂��o��
		fprintf(fp, "\tMODEL_PASS= %s\n\n", MODEL_PASS);              //�e�N�X�`���p�X�����o��
	}

	//�t�@�C�������
	fclose(fp);
}

void C_Obj3D_DataIO::ImportObj3DData(const char* pMeshFilePass)
{
	//�t�@�C�����J��
	FILE *fp;
	fp = fopen(pMeshFilePass, "r");

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
		if (strcmp("DATATYPE", buf) == 0)
		{
			CTextLoad::GetStrToken(fp, TOKEN, buf);      //�^�C�v�̓ǂݍ���

			if(strcmp("CSCENE3D", buf) == 0)             //�^�C�v��CSCENE3D�̏ꍇ
			{
				//�ǂݍ��ݕ���
				ReadBillBoard(fp);
			}

			if(strcmp("MODEL", buf) == 0)
			{
				//�ǂݍ��ݕ���
				ReadModel(fp);
			}
		}
	}

	//�t�@�C�������
	fclose(fp);
}

void C_Obj3D_DataIO::ReadBillBoard(FILE *fp)
{
	//�K�v�ȃf�[�^�ϐ�
	D3DXVECTOR3 POS;
	D3DXVECTOR3 SIZE;
	int ObjType;
	char TexPass[1024];

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
	CTextLoad::FindFont(fp, TOKEN, "SIZE=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	SIZE.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	SIZE.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	SIZE.z = (float)atof(buf);
	//�^�C�v�̓ǂݍ���
	CTextLoad::FindFont(fp, TOKEN, "TYPE=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ObjType = atoi(buf);
	//�e�N�X�`���p�X�̓ǂݍ���
	CTextLoad::FindFont(fp, TOKEN, "TEXTURE_PASS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	strcpy(TexPass, buf);

	//�N���G�C�g����
	SRP PosRotScl;
	PosRotScl.pos = POS;
	PosRotScl.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	PosRotScl.scl = D3DXVECTOR3(1.0f,1.0f,1.0f);
	CScene3DStorage::CreateObject(PosRotScl, SIZE, (CScene3D::CSCENE3D_TYPE)ObjType, TexPass);
}

void C_Obj3D_DataIO::ReadModel(FILE *fp)
{
	//�K�v�ȃf�[�^�ϐ�
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;
	char ModelPass[1024];

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

	CTextLoad::FindFont(fp, TOKEN, "MODEL_PASS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	strcpy(ModelPass, buf);

	//�N���G�C�g����
	SRP PosRotScl;
	PosRotScl.pos = POS;
	PosRotScl.rot = ROT;
	PosRotScl.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	CModelStorage::CreateObject(PosRotScl, ModelPass);
}