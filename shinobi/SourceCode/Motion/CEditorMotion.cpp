#include "CEditorMotion.h"
#include "CTextLoad.h"
#include "CPolygonMath.h"
#include "CHermitian.h"
#include <stdio.h>

CEditorMotion::CEditorMotion()
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_WorldRotMtx);
	D3DXMatrixIdentity(&m_OtherParentMtx);
	m_MotionNow = 0;
	m_MotionOld = 0;
}

CEditorMotion::~CEditorMotion()
{

}

void CEditorMotion::UpdateOffset(void)		//�I�t�Z�b�g�\��
{
	ShowOffset();							//�S�p�[�c���̈ʒu���v�Z
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::UpdateKeyFrame(void)	//�L�[�t���[���\��
{
	ShowKeyFrame();							//�S�p�[�c���̈ʒu���v�Z
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::UpdateKeyFramePreview(float Percent)
{
	bool OverRange = Percent < 0 || Percent> 1;	//�p�[�Z���e�[�W��0�`1�͈̔͊O
	if (OverRange) {
		MessageBox(nullptr, "����Percent�̒l��0�`1�͈̔͊O", "�֐�CEditorMotion::UpdateKeyFramePreview�G���[", MB_ICONHAND);
		Percent = 0;
	}

	ShowKeyFramePreview(Percent);					//�S�p�[�c���̈ʒu���v�Z
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::UpdateMotion(void)		//���[�V�����Đ�
{
	KeyFrameChange();						//�L�[�t���[���̃C���N�������g����
	ShowMotion();							//�S�p�[�c���̈ʒu���v�Z
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::Draw(void)
{
	for(int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Draw();
	}
}

void CEditorMotion::Uninit(void)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Uninit();
	}
}

void CEditorMotion::LoadMotionDataToInit(const char* MotionPass)
{
	FILE *fp;
	fp = fopen(MotionPass,"r");

	if(nullptr == fp)
	{
		MessageBox(nullptr,"���[�V�����f�[�^�̃e�L�X�g�����݂��Ȃ�","�G���[",MB_OK | MB_ICONHAND);
		fclose(fp);
		return;
	}

	//���f�����ƃ��[�V�������ǂݍ���
	char buf[1024];				//�����o�b�t�@
	CTextLoad::FindFont(fp,TOKEN, "NUM_MODEL=");            //���f���t�H���g��T��
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	
	m_PartNum = atoi(buf);		//�p�[�c���擾

	LoadPartData(fp);						//�p�[�c�̃f�[�^�ǂݍ���
	LoadMotionData(fp);						//���[�V�����̃f�[�^�ǂݍ���

	fclose(fp);

	LinkParentAndChildren();				//�e�Ǝq���̊֌W�����т�

	//������
	m_MotionNow = 0;
	m_MotionOld = 0;
	m_KeyFrameNow = 0;
	m_FrameNow = 0;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_WorldRotMtx);

	//�������[�V�������[�h
	for (int i = 0;i < m_Motion[0].m_KeyFrame[0].m_TotalFrame;i++)
	{
		KeyFrameChange();
		ShowOffset();						//���f���̃I�t�Z�b�g�Z�b�g
	}
}

/*==============================================================================
	���݂̃��[�V�����ɃL�[�t���[���𑝂₷
===============================================================================*/
void CEditorMotion::AddKeyFrame(void)
{
	//���݃��[�V�����̃L�[�t���[������
	int TotalKeyFrame = m_Motion[m_MotionNow].m_TotalKeyFrame;

	//�g�p�\�ȃL�[�t���[���`�F�b�N
	if (TotalKeyFrame >= MAX_PART) 
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddKeyFrame]�g�p�\�ȃL�[�t���[���͂Ȃ�", "WARNING", MB_ICONERROR);
		return;
	}

	//�V�����L�[�t���[�������
	for (int PartNum = 0; PartNum < m_PartNum; PartNum++)
	{
		//�g��k���A��]�A�ړ�������
		SRP *pSRP = &m_Motion[m_MotionNow].m_KeyFrame[TotalKeyFrame].m_SRPData[PartNum];
		pSRP->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		//�Ȑ��f�[�^������
		m_Motion[m_MotionNow].m_KeyFrame[TotalKeyFrame].m_HermitianStartDir = D3DXVECTOR2(50, -50);
		m_Motion[m_MotionNow].m_KeyFrame[TotalKeyFrame].m_HermitianEndDir = D3DXVECTOR2(-50, 50);
	}

	//�L�[�t���[����1���₷
	m_Motion[m_MotionNow].m_TotalKeyFrame++;
}

/*==============================================================================
	�w�肷�郂�[�V�����ɃL�[�t���[���𑝂₷
===============================================================================*/
void CEditorMotion::AddKeyFrame(int MotionNum)
{
	//�͈͊O�`�F�b�N
	if (MotionNum < 0 || MotionNum >= m_TotalMotion)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddKeyFrame]����MotionNum�͔͈͊O", "WARNING", MB_ICONERROR);
		return;
	}

	//���݃��[�V�����̃L�[�t���[������
	int TotalKeyFrame = m_Motion[MotionNum].m_TotalKeyFrame;

	//�g�p�\�ȃL�[�t���[���`�F�b�N
	if (TotalKeyFrame >= MAX_PART)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddKeyFrame]�g�p�\�ȃL�[�t���[���͂Ȃ�", "WARNING", MB_ICONERROR);
		return;
	}

	//�V�����L�[�t���[�������
	for (int PartNum = 0; PartNum < m_PartNum; PartNum++)
	{
		//�g��k���A��]�A�ړ�������
		SRP *pSRP = &m_Motion[MotionNum].m_KeyFrame[TotalKeyFrame].m_SRPData[PartNum];
		pSRP->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		//�Ȑ��f�[�^������
		m_Motion[MotionNum].m_KeyFrame[TotalKeyFrame].m_HermitianStartDir = D3DXVECTOR2(50, -50);
		m_Motion[MotionNum].m_KeyFrame[TotalKeyFrame].m_HermitianEndDir = D3DXVECTOR2(-50, 50);
	}

	//�L�[�t���[����1���₷
	m_Motion[MotionNum].m_TotalKeyFrame++;
}

/*==============================================================================
	�L�[�t���[���̑}��
===============================================================================*/
void CEditorMotion::InsertKeyFrame(void)
{
	//���݃��[�V�����̃L�[�t���[������
	int TotalKeyFrame = m_Motion[m_MotionNow].m_TotalKeyFrame;

	//�g�p�\�ȃL�[�t���[���`�F�b�N
	if (TotalKeyFrame >= MAX_PART)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:InsertKeyFrame]�g�p�\�ȃL�[�t���[���͂Ȃ�", "WARNING", MB_ICONERROR);
		return;
	}

	//�L�[�t���[���̃f�[�^�����ɂ����
	for (int KeyframeNum = TotalKeyFrame - 1; KeyframeNum >= m_KeyFrameNow; KeyframeNum--) {
		m_Motion[m_MotionNow].m_KeyFrame[KeyframeNum + 1] = m_Motion[m_MotionNow].m_KeyFrame[KeyframeNum];
	}

	//���̃L�[�t���[���̃f�[�^��������
	for (int PartNum = 0; PartNum < m_PartNum; PartNum++)
	{
		//�g��k���A��]�A�ړ�������
		SRP *pSRP = &m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[PartNum];
		pSRP->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		//�Ȑ��f�[�^������
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir = D3DXVECTOR2(50, -50);
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir = D3DXVECTOR2(-50, 50);
	}

	//�L�[�t���[����1���₷
	m_Motion[m_MotionNow].m_TotalKeyFrame++;
}

/*==============================================================================
	���[�V�����𑝂₷(�L�[�t���[���̃f�t�H���g��2��)
===============================================================================*/
void CEditorMotion::AddMotion(void)
{
	if (m_TotalMotion >= EDITOR_MOTION_MAX)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddMotion]�g�p�\�ȃ��[�V�����͂Ȃ�", "WARNING", MB_ICONERROR);
		return;
	}

	m_Motion[m_TotalMotion].m_bLoop = false;
	m_Motion[m_TotalMotion].m_TotalKeyFrame = 0;

	//���[�V�������𑝂₷
	m_TotalMotion++;

	//�L�[�t���[��2�����
	AddKeyFrame(m_TotalMotion - 1);
	AddKeyFrame(m_TotalMotion - 1);
}

/*==============================================================================
	���[�V�����̑}��
===============================================================================*/
void CEditorMotion::InsertMotion(void)
{
	if (m_TotalMotion >= EDITOR_MOTION_MAX)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:InsertMotion]�g�p�\�ȃ��[�V�����͂Ȃ�", "WARNING", MB_ICONERROR);
		return;
	}

	//���݃��[�V�����ԍ��Ƃ��̌��̗v�f�����Ɉ�v�f�������
	for (int MotionNum = m_TotalMotion - 1; MotionNum >= m_MotionNow; MotionNum--)
	{
		m_Motion[MotionNum + 1] = m_Motion[MotionNum];
	}

	strcpy_s(m_Motion[m_MotionNow].m_Name, "NewMotion");
	m_Motion[m_MotionNow].m_bLoop = false;
	m_Motion[m_MotionNow].m_TotalKeyFrame = 0;

	//���[�V�������𑝂₷
	m_TotalMotion++;

	//�L�[�t���[��2�����
	AddKeyFrame(m_MotionNow);
	AddKeyFrame(m_MotionNow);
}

/*==============================================================================
	���̃��[�V�������폜���A�z��̌��̕������l�߂�
===============================================================================*/
void CEditorMotion::DeleteMotionNow(void)
{
	//���[�V��������1�ȏ�ێ�
	if (m_TotalMotion <= 1)
	{
		return;
	}

	//���݃��[�V�����ԍ�����ɁA���̔z���O�ɋl�߂�
	for(int MotionNum = m_MotionNow; MotionNum < m_TotalMotion - 1; MotionNum++)
	{
		m_Motion[MotionNum] = m_Motion[MotionNum + 1];
	}

	//���̃��[�V�����ԍ�����O���炷
	if (m_MotionNow == (m_TotalMotion - 1))
	{
		m_MotionNow--;
	}

	//���[�V������������
	m_TotalMotion--;
}

/*==============================================================================
	���̃��[�V�����̍��̃L�[�t���[�����폜���A�z��̌��̕������l�߂�
===============================================================================*/
void CEditorMotion::DeleteKeyFrameNow(void)
{
	//���݃��[�V�����̃L�[�t���[������
	int TotalKeyFrame = m_Motion[m_MotionNow].m_TotalKeyFrame;

	//�L�[�t���[������1�ȏ�ێ�
	if (TotalKeyFrame <= 1) 
	{
		return;
	}

	//���݃L�[�t���[���ԍ�����ɁA���̔z���O�ɋl�߂�
	for (int KeyFrameNum = m_KeyFrameNow; KeyFrameNum < TotalKeyFrame - 1; KeyFrameNum++)
	{
		m_Motion[m_MotionNow].m_KeyFrame[KeyFrameNum] = 
			m_Motion[m_MotionNow].m_KeyFrame[KeyFrameNum + 1];
	}

	//���̃L�[�t���[���ԍ�����O���炷
	if (m_KeyFrameNow == (m_Motion[m_MotionNow].m_TotalKeyFrame - 1) )
	{
		m_KeyFrameNow--;
	}

	//�L�[�t���[����������
	m_Motion[m_MotionNow].m_TotalKeyFrame--;
}

void CEditorMotion::LoadPartData(FILE *fp)
{
	if (nullptr == fp) return;

	char buf[1024];            //�����o�b�t�@

	//���f���ǂݍ���
	SRP SRP;
	SRP.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SRP.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SRP.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < m_PartNum; i++)
	{
		CTextLoad::FindFont(fp, TOKEN, "MODEL_FILENAME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_PartFilePass[i].clear();
		m_PartFilePass[i] += buf;
		m_Part[i].m_Model.Init(SRP, buf);
	}

	//�p�[�c�̃I�t�Z�b�g��ID�ԍ���ǂݍ���
	for (int i = 0; i < m_PartNum; i++)
	{
		//���O�ǂݍ���
		CTextLoad::FindFont(fp, TOKEN, "NAME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		strcpy_s( m_Part[i].m_Name,PART_NAME_FONT_MAX_NUM, buf);

		//�����̃p�[�cID��ǂݍ���
		CTextLoad::FindFont(fp, TOKEN, "INDEX=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_PartID = atoi(buf);
		//�e�̃p�[�cID��ǂݍ���
		CTextLoad::FindFont(fp, TOKEN, "PARENT=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_ParentID = atoi(buf);
		//�ʒu�I�t�Z�b�g��ǂݍ���
		CTextLoad::FindFont(fp, TOKEN, "POS=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.pos.x = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.pos.y = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.pos.z = (float)atof(buf);
		//��]�I�t�Z�b�g��ǂݍ���
		CTextLoad::FindFont(fp, TOKEN, "ROT=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.rot.x = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.rot.y = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.rot.z = (float)atof(buf);

		m_Part[i].m_SRPOffset.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}
}

void CEditorMotion::LoadMotionData(FILE *fp)
{
	if (nullptr == fp) return;

	m_TotalMotion = 0;             //�����[�V��������0�ɂ���
	char buf[1024];            //�����o�b�t�@

	//���[�V�������̓ǂݍ���
	while ( (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1) && (m_TotalMotion < EDITOR_MOTION_MAX) )
	{
		//������MOTIONSET��������܂Ń��[�v
		if (strcmp("MOTIONSET", buf) != 0) {
			continue;	
		}

		CTextLoad::FindFont(fp, TOKEN, "NAME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		strcpy_s(m_Motion[m_TotalMotion].m_Name, MOTION_NAME_MAX, buf);

		//���[�v�t���O�̓ǂݍ���
		CTextLoad::FindFont(fp, TOKEN, "LOOP=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		int nLoop = atoi(buf);
		if (0 == nLoop) m_Motion[m_TotalMotion].m_bLoop = false;
		else m_Motion[m_TotalMotion].m_bLoop = true;

		//�L�[���̓ǂݍ���
		CTextLoad::FindFont(fp, TOKEN, "NUM_KEY=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Motion[m_TotalMotion].m_TotalKeyFrame = atoi(buf);
		m_MotionTotalFrame[m_TotalMotion] = 0;

		//�L�[�t���[���f�[�^�̓ǂݍ���
		for(int nKeyFrameNum = 0; nKeyFrameNum < m_Motion[m_TotalMotion].m_TotalKeyFrame; nKeyFrameNum++)
		{
			//�L�[�t���[���̃t���[������ǂݍ���
			CTextLoad::FindFont(fp, TOKEN, "FRAME=");
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_TotalFrame = atoi(buf);
			m_MotionTotalFrame[m_TotalMotion] += m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_TotalFrame;     //���̃��[�V�����̃t���[������(����)�̘a

			//�L�[�t���[���̃G���~�[�g�Ȑ��f�[�^�̓ǂݍ���
			CTextLoad::FindFont(fp, TOKEN, "START_DIR=");
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianStartDir.x = (FLOAT)atof(buf);
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianStartDir.y = (FLOAT)atof(buf);
			CTextLoad::FindFont(fp, TOKEN, "END_DIR=");
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianEndDir.x = (FLOAT)atof(buf);
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianEndDir.y = (FLOAT)atof(buf);

			//�L�[�t���[���̋��POS��ROT�̓ǂݍ���
			for (int k = 0; k < m_PartNum; k++)
			{
				SRP SRP;
				SRP.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				//�ʒu�f�[�^�̓ǂݍ���
				CTextLoad::FindFont(fp, TOKEN, "POS=");
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.pos.x = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.pos.y = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.pos.z = (float)atof(buf);
				//��]�f�[�^�̓ǂݍ���
				CTextLoad::FindFont(fp, TOKEN, "ROT=");
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.rot.x = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.rot.y = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.rot.z = (float)atof(buf);

				m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_SRPData[k] = SRP;
			}
		}
		m_TotalMotion++;                   //���[�V�����J�E���g�A�b�v
	}

	//���[�V�����ǂݎ��Ȃ�������
	if (m_TotalMotion == 0) {
		m_TotalMotion = 1;
		m_MotionNow = 0;
		AddKeyFrame();
	}
}

//�I�t�Z�b�g�̍Đ�
void CEditorMotion::ShowOffset(void)
{
	CPart* pTopNode = nullptr;

	//Top�̐e���擾
	for (int i = 0; i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//�e���
		}
	}

	//��Ԃ̐e�͂Ȃ�
	if (nullptr == pTopNode)
	{
		MessageBox(nullptr, "��ԏ�̐e�͂Ȃ��I", "�֐�CEditorMotion::CalcWorldMtx�G���[����", MB_ICONHAND);
		return;
	}

	CalcOffset(pTopNode);			//�ċA�I�ȃ��[�V�����v�Z
}

//�w��L�[�t���[���̍Đ�
void CEditorMotion::ShowKeyFrame(void)
{
	CPart* pTopNode = nullptr;

	//Top�̐e���擾
	for (int i = 0; i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//�e���
		}
	}

	//��Ԃ̐e�͂Ȃ�
	if (nullptr == pTopNode)
	{
		MessageBox(nullptr, "��ԏ�̐e�͂Ȃ��I", "�֐�CEditorMotion::CalcWorldMtx�G���[����", MB_ICONHAND);
		return;
	}

	CalcKeyFrame(pTopNode);			//�ċA�I�ȃ��[�V�����v�Z
}

void CEditorMotion::ShowKeyFramePreview(float Percent)
{
	CPart* pTopNode = nullptr;

	//Top�̐e���擾
	for (int i = 0; i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//�e���
		}
	}

	//��Ԃ̐e�͂Ȃ�
	if (nullptr == pTopNode)
	{
		MessageBox(nullptr, "��ԏ�̐e�͂Ȃ��I", "�֐�CEditorMotion::CalcWorldMtx�G���[����", MB_ICONHAND);
		return;
	}

	CalcKeyFramePreview(pTopNode, Percent);			//�ċA�I�ȃ��[�V�����v�Z
}

void CEditorMotion::ShowMotion(void)
{
	CPart* pTopNode = nullptr;

	//Top�̐e���擾
	for (int i = 0;i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//�e���
		}
	}

	//��Ԃ̐e�͂Ȃ�
	if(nullptr == pTopNode)
	{
		MessageBox(nullptr, "��ԏ�̐e�͂Ȃ��I", "�֐�CEditorMotion::CalcWorldMtx�G���[����", MB_ICONHAND);
		return;
	}

	CalcPartWorldMtxRecursive(pTopNode);			//�ċA�I�ȃ��[�V�����v�Z
}

void CEditorMotion::CalcOffset(CPart* Node)
{
	/*************************�e�̃}�g���N�X�v�Z************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//�e�}�g���N�X
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//���[���h�}�g���N�X�̕ۑ�
		MtxParent = MtxWorld * m_OtherParentMtx;							//�e�}�g���N�X�ɑ��
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//�I�t�Z�b�g�}�g���N�X
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//�I�t�Z�b�g�}�g���N�X�v�Z

	//���ʃ}�g���N�X�v�Z
	D3DXMATRIX MtxResult;
	MtxResult = MtxOffset*MtxParent;

	//���f���ɃZ�b�g
	Node->SetNowMtx(MtxResult);						//�}�g���N�X���f
	Node->m_SRPNow.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	Node->m_SRPNow.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	/*************************�q���̃}�g���N�X�v�Z************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcOffset(*itChild);					//�ċA
	}
}

void CEditorMotion::CalcKeyFrame(CPart* Node)
{
	/*************************�e�̃}�g���N�X�v�Z************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//�e�}�g���N�X
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//���[���h�}�g���N�X�̕ۑ�
		MtxParent = MtxWorld * m_OtherParentMtx;							//�e�}�g���N�X�ɑ��
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//�I�t�Z�b�g�}�g���N�X
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//�I�t�Z�b�g�}�g���N�X�v�Z

	int nPartID = Node->m_PartID;
	SRP SRPKeyFrame = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartID];	//���̃L�[�t���[����SRP������
	
	//���ʃ}�g���N�X�v�Z
	D3DXMATRIX MtxKeyFrame, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxKeyFrame, SRPKeyFrame);
	MtxResult = MtxKeyFrame*MtxOffset*MtxParent;                        //�����̉�]�}�g���N�X*�����I�t�Z�b�g�}�g���N�X*�e�}�g���N�X

	//���f���ɃZ�b�g
	Node->SetNowMtx(MtxResult);						//�}�g���N�X���f
	Node->m_SRPNow = SRPKeyFrame;					//SRPKeyFrame�L�^

	/*************************�q���̃}�g���N�X�v�Z************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcKeyFrame(*itChild);					//�ċA
	}
}

void CEditorMotion::CalcKeyFramePreview(CPart* Node, float Percent)
{
	int NextKeyFrame = CalcNextKeyFrame();       //���̃L�[�t���[�������߂�

	/*************************�e�̃}�g���N�X�v�Z************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//�e�}�g���N�X
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//���[���h�}�g���N�X�̕ۑ�
		MtxParent = MtxWorld * m_OtherParentMtx;							//�e�}�g���N�X�ɑ��
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//�I�t�Z�b�g�}�g���N�X
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//�I�t�Z�b�g�}�g���N�X�v�Z

	int nPartID = Node->m_PartID;
	SRP StartSRP = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartID];	//���̃L�[�t���[����SRP������
	SRP EndSRP = m_Motion[m_MotionNow].m_KeyFrame[NextKeyFrame].m_SRPData[nPartID];		//���̃L�[�t���[����SRP���߂�
	float t = Percent;			//���̃t���[���ڂ͑��t���[���̉���

	//���݂�SRP�v�Z
	SRP SRP_FrameNow;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_FrameNow.pos = StartSRP.pos * (1 - t) + EndSRP.pos * t;
	SRP_FrameNow.rot = StartSRP.rot * (1 - t) + EndSRP.rot * t;

	//���ʃ}�g���N�X�v�Z
	D3DXMATRIX MtxNow, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
	MtxResult = MtxNow*MtxOffset*MtxParent;                        //�����̉�]�}�g���N�X*�����I�t�Z�b�g�}�g���N�X*�e�}�g���N�X

	//���f���ɃZ�b�g
	Node->SetNowMtx(MtxResult);						//�}�g���N�X���f
	Node->m_SRPNow = SRP_FrameNow;					//SRPNow�L�^

	/*************************�q���̃}�g���N�X�v�Z************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcKeyFramePreview(*itChild,Percent);					//�ċA
	}
}

void CEditorMotion::CalcPartWorldMtxRecursive(CPart* Node)
{
	int NextKeyFrame = CalcNextKeyFrame();       //���̃L�[�t���[�������߂�

	/*************************�e�̃}�g���N�X�v�Z************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//�e�}�g���N�X
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//���[���h�}�g���N�X�̕ۑ�
		MtxParent = MtxWorld * m_OtherParentMtx;							//�e�}�g���N�X�ɑ��
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//�I�t�Z�b�g�}�g���N�X
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//�I�t�Z�b�g�}�g���N�X�v�Z

	int nPartID = Node->m_PartID;
	SRP StartSRP = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartID];	//���̃L�[�t���[����SRP������
	SRP EndSRP = m_Motion[m_MotionNow].m_KeyFrame[NextKeyFrame].m_SRPData[nPartID];		//���̃L�[�t���[����SRP���߂�
	float t = (float)m_FrameNow / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;		//���̃t���[���ڂ͑��t���[���̉���

	//�G���~�[�g�Ȑ����g���āA�ω��ʂ̃p�[�Z���e�[�W������
	float Percent = CHermitian::GetChangeValuePercent(
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir,
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir,
		t
		);

	//���݂�SRP�v�Z
	SRP SRP_FrameNow;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_FrameNow.pos = StartSRP.pos + (EndSRP.pos - StartSRP.pos) * Percent;
	SRP_FrameNow.rot = StartSRP.rot + (EndSRP.rot - StartSRP.rot) * Percent;

	//���ʃ}�g���N�X�v�Z
	D3DXMATRIX MtxNow, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
	MtxResult = MtxNow*MtxOffset*MtxParent;                        //�����̉�]�}�g���N�X*�����I�t�Z�b�g�}�g���N�X*�e�}�g���N�X

	//���f���ɃZ�b�g
	Node->SetNowMtx(MtxResult);						//�}�g���N�X���f
	Node->m_SRPNow = SRP_FrameNow;					//SRPNow�L�^

	/*************************�q���̃}�g���N�X�v�Z************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcPartWorldMtxRecursive(*itChild);					//�ċA
	}
}

void CEditorMotion::KeyFrameChange(void)
{
	int FrameTotal = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;

	if (m_FrameNow < FrameTotal) m_FrameNow++;
	else                                //�t���[�������t���[�����ɒB�����玟�̃L�[�t���[���ɍs������
	{
		int nNextKeyFrame = CalcNextKeyFrame();				//���̃L�[�t���[��
		if (nNextKeyFrame == m_KeyFrameNow) return;			//���[�v�ł͂Ȃ����[�V������������Ȍ�̃\�[�X���s���Ȃ�

		//���̃t���[���Ɉڍs������
		m_KeyFrameNow = nNextKeyFrame;						//���̃L�[�t���[���Ɉڍs
		m_FrameNow = 0;										//���݂̃t���[���Ԗڂ�0�Ԗڂɂ���

		//�p�[�c��
		for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
		{
			m_Part[nPartNow].m_SRPBlendOld = m_Part[nPartNow].m_SRPNow;      //�u�����f�B���O���W�ۑ�
		}
	}
}

void CEditorMotion::SetMotionNow(int MotionNumber)
{
	bool bOverRange = (MotionNumber < 0) || (MotionNumber >= m_TotalMotion);
	if(bOverRange)
	{
		MessageBox(nullptr, "���͂������[�V�����ԍ��͔͈͊O", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}
	m_MotionOld = m_MotionNow;

	//�ڍs������
	m_MotionNow = MotionNumber;
	m_KeyFrameNow = 0;
	m_FrameNow = 0;

	for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		m_Part[nPartNow].m_SRPBlendOld = m_Part[nPartNow].m_SRPNow;
	}
}

void CEditorMotion::SetKeyFrameNow(int KeyFrame)
{
	bool bOverRange = (KeyFrame < 0) || (KeyFrame >= m_Motion[m_MotionNow].m_TotalKeyFrame);
	if (bOverRange)
	{
		MessageBox(nullptr, "���͂����L�[�t���[���ԍ��͔͈͊O", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}

	//�ڍs������
	m_KeyFrameNow = KeyFrame;
	m_FrameNow = 0;
}

int CEditorMotion::CalcNextKeyFrame(void)
{
	int NextKeyFrame;
	if (m_KeyFrameNow < m_Motion[m_MotionNow].m_TotalKeyFrame - 1)      //���̃L�[�t���[���͂��̃��[�V�����̍Ō�̃L�[�t���[�����ǂ���
	{
		NextKeyFrame = m_KeyFrameNow + 1;
	}

	else
	{
		//���[�V�����̃��[�v����
		if (m_Motion[m_MotionNow].m_bLoop == false)
		{
			NextKeyFrame = m_KeyFrameNow;
		}

		else
		{
			NextKeyFrame = 0;
		}
	}
	return NextKeyFrame;
}

void CEditorMotion::LinkParentAndChildren(void)
{
	for (int i = 0; i < m_PartNum; i++) 
	{
		int ParentID = m_Part[i].m_ParentID;
		if(ParentID >= 0)m_Part[i].SetParent(&m_Part[ParentID]);		//�e�ݒ�
		CPart *pParent = nullptr;
		pParent = m_Part[i].GetParent();						//�e�擾
		if(pParent != nullptr) pParent->AddChildren(&m_Part[i]);	//�q���ݒ�
	}
}