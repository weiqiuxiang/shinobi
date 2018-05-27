#include "CMeshDataIO.h"
#include "TexManager.h"
#include "CTextLoad.h"
#include "CMeshFieldManager.h"
#include <stdio.h>

void CMeshDataIO::ExportMeshData(void)
{
	if (m_pMesh == nullptr) return;        //���b�V���f�[�^���݂��Ȃ�

	//�f�[�^�擾(����������)
	D3DXVECTOR3 POS = m_pMesh->GetPosition();
	D3DXVECTOR2 PIECE_SIZE = m_pMesh->GetPieceSize();
	D3DXVECTOR2 PIECE_NUM = m_pMesh->GetPieceNum();
	TEX_ID MeshTexID = m_pMesh->GetTexID();
	const char* TEXTURE_PASS = CTexManager::GetTexturePass(MeshTexID);

	//�f�[�^�擾(���ΕW������)
	int VERTEX_NUM = m_pMesh->GetVertexNum();
	const double *pMeshRelativeHeight = m_pMesh->GetHighRelative();

	//�f�[�^�̏����o��
	FILE *fp;
	fp = fopen("data/MESH_DATA/EDIT_MESH_SAVE.txt","w");    //�����o����w��

	//�擪�R�����g�����o��
	fprintf(fp, "#-------------------------------------------------\n");
	fprintf(fp, "#MESH_DATA\n");
	fprintf(fp, "#-------------------------------------------------\n");

	//���������������o��
	fprintf(fp, "MESH_INIT START\n");
	fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);              //�ʒu�̏����o��
	fprintf(fp, "\tPIECE_SIZE= %.2f %.2f\n", PIECE_SIZE.x, PIECE_SIZE.y);     //�}�X�T�C�Y�̏����o��
	fprintf(fp, "\tPIECE_NUM= %.0f %.0f\n", PIECE_NUM.x, PIECE_NUM.y);        //�}�X���̏����o��
	fprintf(fp, "\tTEXTURE_PASS= %s\n", TEXTURE_PASS);
	fprintf(fp, "MESH_INIT END\n\n");

	//���ΕW�������̏����o��
	fprintf(fp, "RELATIVE_HEIGHT START\n");
	fprintf(fp, "\tVERTEX_NUM= %d\n", VERTEX_NUM);                 //���_���̏����o��
	for (int i = 0; i < (PIECE_NUM.y + 1); i++)
	{
		fprintf(fp, "\t");
		fprintf(fp, "ELEVATION ");
		for (int j = 0; j < (PIECE_NUM.x + 1); j++)
		{
			int k = i * ((int)PIECE_NUM.x + 1) + j;
			fprintf(fp,"%.2f ",pMeshRelativeHeight[k]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "RELATIVE_HEIGHT END\n\n");
	fclose(fp);
}


CMeshField* CMeshDataIO::ImportMeshData(const char* pMeshFilePass)
{
	m_pMesh = nullptr; //���̃��b�V���������Ȃ��悤��

	//����������
	D3DXVECTOR3 POS;                  //�ʒu���
	D3DXVECTOR2 PIECE_SIZE;           //�}�X�T�C�Y���
	D3DXVECTOR2 PIECE_NUM;            //�}�X�����
	char TEXTURE_PASS[256];          //�e�N�X�`���p�X���

	//�W������
	int VERTEX_NUM;                   //���_���
	double *pMeshRelativeHeight;      //�W���|�C���^

	//�f�[�^�̓ǂݍ��݊J�n
	FILE *fp;
	fp = fopen(pMeshFilePass,"r");    //�ǂݍ��ݐ�w��
	if(nullptr == fp)           //nullptr�`�F�b�N
	{
		fclose(fp);
		MessageBox(nullptr, "���b�V���f�[�^�̃e�L�X�g�����݂��܂���", "�G���[", MB_OK | MB_ICONHAND);
		return nullptr;
	}

	char FontBuf[1024];     //�t�H���g�o�b�t�@�����

	//�ʒu��ǂݍ���
	CTextLoad::FindFont(fp,TOKEN, "POS=");     //�ʒu�f�[�^��������
	CTextLoad::GetStrToken(fp,TOKEN, FontBuf); 
	POS.x = (float)atof(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	POS.y = (float)atof(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	POS.z = (float)atof(FontBuf);

	//�}�X�T�C�Y��ǂݍ���
	CTextLoad::FindFont(fp, TOKEN, "PIECE_SIZE=");     //�}�X�T�C�Y�f�[�^��������
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_SIZE.x = (FLOAT)atof(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_SIZE.y = (FLOAT)atof(FontBuf);

	//�}�X����ǂݍ���
	CTextLoad::FindFont(fp, TOKEN, "PIECE_NUM=");
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_NUM.x = (FLOAT)atoi(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_NUM.y = (FLOAT)atoi(FontBuf);

	//�e�N�X�`���p�X���R�s�[
	CTextLoad::FindFont(fp, TOKEN, "TEXTURE_PASS=");
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	strcpy(TEXTURE_PASS,FontBuf);

	//���_�����R�s�[
	CTextLoad::FindFont(fp, TOKEN, "VERTEX_NUM=");
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	VERTEX_NUM = atoi(FontBuf);

	pMeshRelativeHeight = new double[VERTEX_NUM];    //���_���̕W���f�[�^���m�ۂ���

	//�W���f�[�^��ǂݍ���
	for (int i = 0; i < (PIECE_NUM.y + 1); i++)
	{
		CTextLoad::FindFont(fp, TOKEN, "ELEVATION");
		for (int j = 0; j < (PIECE_NUM.x + 1); j++)
		{
			CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
			int k = i * ((int)PIECE_NUM.x + 1) + j;
			pMeshRelativeHeight[k] = atof(FontBuf);
		}
	}

	//�ȏ�̃f�[�^�Ń��b�V���t�B�[���h�����
	CMeshField *pMesh = CMeshFieldManager::CreateMeshField(POS, CINDEX_SIZE(PIECE_NUM, PIECE_SIZE), TEXTURE_PASS);
	pMesh->LoadRelativeHigh(pMeshRelativeHeight);

	//���낢����
	delete[] pMeshRelativeHeight;
	fclose(fp);

	return pMesh;
}