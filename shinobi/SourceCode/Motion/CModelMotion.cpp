#include "CModelMotion.h"
#include "CTextLoad.h"
#include "CPolygonMath.h"
#include <stdio.h>
#include "CHermitian.h"

CModelMotion::CModelMotion()
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_WorldRotMtx);
	D3DXMatrixIdentity(&m_OtherParentMtx);
	m_MotionNow = 0;
	m_MotionOld = 0;
}

CModelMotion::~CModelMotion()
{
	for (int i = 0; i < m_PartNum; i++) {
		m_Part[i].Uninit();
	}
}

void CModelMotion::Update(void)
{
	KeyFrameChange();
	CalcWorldMtx();
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CModelMotion::Draw(void)
{
	for(int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Draw();
	}
}

void CModelMotion::DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawOnDepthMap(LightView, LightProje);
	}
}

void CModelMotion::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawUseDepthMap(LightView, LightProje, DepthTexture);
	}
}

void CModelMotion::DrawSpecular(void)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawSpecular();
	}
}

void CModelMotion::DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawMotionBlur(OriginalTexture);
	}
}

void CModelMotion::Uninit(void)
{
	for (int i = 0; i < m_PartNum; i++) {
		m_Part[i].Uninit();
	}
}

void CModelMotion::LoadMotionDataToInit(const char* MotionPass)
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
	char buf[1024];            //�����o�b�t�@
	CTextLoad::FindFont(fp,TOKEN, "NUM_MODEL=");            //���f���t�H���g��T��
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	//�p�[�c���擾
	m_PartNum = atoi(buf);
	if (m_PartNum > MAX_PART) {
		MessageBox(nullptr,"�p�[�c��������ɒB����","[CModelMotion.cpp][LoadMotionDataToInit]",MB_OK);
	}

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
		CalcWorldMtx();						//���f���̃��[���h�}�g���N�X�v�Z
	}
}

void CModelMotion::LoadPartData(FILE *fp)
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
		m_Part[i].m_Model.Init(SRP, buf);
	}

	//�p�[�c�̃I�t�Z�b�g��ID�ԍ���ǂݍ���
	for (int i = 0; i < m_PartNum; i++)
	{
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

void CModelMotion::LoadMotionData(FILE *fp)
{
	if (nullptr == fp) return;

	m_TotalMotion = 0;             //�����[�V��������0�ɂ���
	char buf[1024];            //�����o�b�t�@

	//���[�V�������̓ǂݍ���
	while ( (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1) && (m_TotalMotion < MOTION_MAX) )
	{
		//���[�v�t���O�̓ǂݍ���
		if (strcmp("LOOP=", buf) != 0) continue;      //����LOOP=��������܂Ń��[�v
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
}

void CModelMotion::CalcPartWorldMtx(void)
{
	int NextKeyFrame = CalcNextKeyFrame();       //���̃L�[�t���[�������߂�

	//�K�v�ȕϐ�
	D3DXMATRIX MtxOffset, MtxParent,MtxResult,MtxWorld;
	SRP SRP_WorldMtx,SRP_FrameNow,SRP_OldStart,SRP_StartEnd;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//���E��SRP�}�g���N�X�v�Z
	SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_WorldMtx.pos = m_pos;

	//���[���h�}�g���N�X�̍쐬
	D3DXMATRIX PosMtx, SclMtx;
	D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
	D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
	MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
	m_WorldMtx = MtxWorld;												//���[���h�}�g���N�X�̕ۑ�

	//�p�[�c���Ƃ̃}�g���N�X�̌v�Z
	for(int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		//�e��SRP(scl,rot,pos)�擾
		int ParentID = m_Part[nPartNow].m_ParentID;          //�e��ID
		if (-1 == ParentID) //�e�̐e�͐��E�̏ꍇ
		{
			MtxParent = MtxWorld * m_OtherParentMtx;		//m_OtherParentMtx���Z�b�g���Ă���ꍇ����,m_OtherParentMtx�����f�����
		}
		else
		{
			MtxParent = m_Part[ParentID].m_MtxNow;
		}

		CPolygonMath::CalcMtxFromSRP(&MtxOffset, m_Part[nPartNow].m_SRPOffset);      //�I�t�Z�b�g�}�g���N�X�v�Z

		SRP StartSRP = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartNow];  //���̃L�[�t���[����SRP������
		SRP EndSRP = m_Motion[m_MotionNow].m_KeyFrame[NextKeyFrame].m_SRPData[nPartNow];     //���̃L�[�t���[����SRP���߂�
		SRP OldBlendSRP = m_Part[nPartNow].m_SRPBlendOld;                            //�u�����f�B���OSRP�����߂�
		float t = (float)m_FrameNow / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;      //���̃t���[���ڂ͑��t���[���̉���

		//���݂�SRP�v�Z
		SRP_OldStart.pos = OldBlendSRP.pos * (1 - t) + StartSRP.pos * t;
		SRP_OldStart.rot = OldBlendSRP.rot * (1 - t) + StartSRP.rot * t;
		SRP_StartEnd.pos = StartSRP.pos * (1 - t) + EndSRP.pos * t;
		SRP_StartEnd.rot = StartSRP.rot * (1 - t) + EndSRP.rot * t;
		SRP_FrameNow.pos = SRP_OldStart.pos * (1 - t) + SRP_StartEnd.pos * t;
		SRP_FrameNow.rot = SRP_OldStart.rot * (1 - t) + SRP_StartEnd.rot * t;

		D3DXMATRIX MtxNow;
		CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
		MtxResult = MtxNow*MtxOffset*MtxParent;                        //�����̉�]�}�g���N�X*�����I�t�Z�b�g�}�g���N�X*�e�}�g���N�X

		//���f���ɃZ�b�g
		m_Part[nPartNow].m_MtxOld = m_Part[nPartNow].m_MtxNow;		//�Â��̃}�g���N�X��ۑ�
		m_Part[nPartNow].SetNowMtx(MtxResult);						//�}�g���N�X���f
		m_Part[nPartNow].m_SRPNow = SRP_FrameNow;					//SRPNow�L�^
	}
}

void CModelMotion::CalcWorldMtx(void)
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
		MessageBox(nullptr, "��ԏ�̐e�͂Ȃ��I", "�֐�CModelMotion::CalcWorldMtx�G���[����", MB_ICONHAND);
		return;
	}

	CalcPartWorldMtxRecursive(pTopNode);			//�ċA�I�ȃ��[�V�����v�Z
}

void CModelMotion::CalcPartWorldMtxRecursive(CPart* Node)
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
	SRP OldBlendSRP = m_Part[nPartID].m_SRPBlendOld;										//�u�����f�B���OSRP�����߂�
	float t = (float)m_FrameNow / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;		//���̃t���[���ڂ͑��t���[���̉���

	//�G���~�[�g�Ȑ����g���āA�ω��ʂ̃p�[�Z���e�[�W������
	float Percent = CHermitian::GetChangeValuePercent(
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir,
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir,
		t
		);

	//���݂�SRP�v�Z
	SRP SRP_FrameNow, SRP_OldStart, SRP_StartEnd;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_OldStart.pos = OldBlendSRP.pos * (1 - Percent) + StartSRP.pos * Percent;
	SRP_OldStart.rot = OldBlendSRP.rot * (1 - Percent) + StartSRP.rot * Percent;
	SRP_StartEnd.pos = StartSRP.pos * (1 - Percent) + EndSRP.pos * Percent;
	SRP_StartEnd.rot = StartSRP.rot * (1 - Percent) + EndSRP.rot * Percent;
	SRP_FrameNow.pos = SRP_OldStart.pos * (1 - Percent) + SRP_StartEnd.pos * Percent;
	SRP_FrameNow.rot = SRP_OldStart.rot * (1 - Percent) + SRP_StartEnd.rot * Percent;

	//���ʃ}�g���N�X�v�Z
	D3DXMATRIX MtxNow, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
	MtxResult = MtxNow*MtxOffset*MtxParent;                        //�����̉�]�}�g���N�X*�����I�t�Z�b�g�}�g���N�X*�e�}�g���N�X

	//���f���ɃZ�b�g
	Node->m_MtxOld = Node->m_MtxNow;		//�Â��̃}�g���N�X��ۑ�
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

void CModelMotion::KeyFrameChange(void)
{
	int FrameTotal = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;

	if (m_FrameNow < FrameTotal) m_FrameNow++;
	else                                //�t���[�������t���[�����ɒB�����玟�̃L�[�t���[���ɍs������
	{
		int nNextKeyFrame = CalcNextKeyFrame();           //���̃L�[�t���[��
		if (nNextKeyFrame == m_KeyFrameNow) return;       //���[�v�ł͂Ȃ����[�V������������Ȍ�̃\�[�X���s���Ȃ�

		//���̃t���[���Ɉڍs������
		m_KeyFrameNow = nNextKeyFrame;                    //���̃L�[�t���[���Ɉڍs
		m_FrameNow = 0;                                   //���݂̃t���[���Ԗڂ�0�Ԗڂɂ���
		//�p�[�c��
		for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
		{
			m_Part[nPartNow].m_SRPBlendOld = m_Part[nPartNow].m_SRPNow;      //�u�����f�B���O���W�ۑ�
		}
	}
}

void CModelMotion::SetMotionNow(int MotionNumber)
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

void CModelMotion::SetDrawType(CModel::DRAW_TYPE type)
{
	for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		m_Part[nPartNow].m_Model.SetDrawType(type);
	}
}

void CModelMotion::SetMaterial(const D3DMATERIAL9& Mtrl)
{
	for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		m_Part[nPartNow].m_Model.SetMaterial(Mtrl);
	}
}

int CModelMotion::CalcNextKeyFrame(void)
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

void CModelMotion::LinkParentAndChildren(void)
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