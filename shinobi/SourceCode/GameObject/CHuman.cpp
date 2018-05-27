#include "CHuman.h"
#include "CShowDebugFlag.h"

#define ROTATE_SPEED (D3DX_PI*0.1)

/*****************************CMoveEnvironment����Start*********************************/
void CMoveEnvironment::SetEnvironment(float fMass, float GravityAcc, float CoeffiResist)
{
	SetGravityAcc(GravityAcc);
	SetCoeffiResist(CoeffiResist);
	SetMass(fMass);
}
/*****************************CMoveEnvironment����End*********************************/

/***********************************CHuman����Start*********************************/
CHuman::CHuman()
{
	//�v���[���[�����̃p�����[�^��ݒ肷��
	m_pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//�ړ��p�����[�^��ݒ�
	m_Move.m_FaceDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_Move.m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Move.m_Speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//���p�����[�^��ݒ�
	m_Environment.m_fCoeffiResist = 0.0f;
	m_Environment.m_fGravityAcc = 0.0f;
	m_Environment.m_fMass = 0.0f;

	m_RorateSpeed = ROTATE_SPEED;
}

CHuman::~CHuman()
{
	m_ModelMotion.Uninit();
}

void CHuman::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass)
{
	//���f�������̃p�����[�^���Z�b�g
	m_ModelMotion.LoadMotionDataToInit(MotionPass);
	D3DXMatrixIdentity (&m_PoseMtx);
	D3DXMatrixRotationYawPitchRoll(&m_PoseMtx, rot.y, rot.x, rot.z);
	m_ModelMotion.SetPosition(pos);
	m_pos = pos;
	m_posOld = pos;
	m_ModelMotion.SetWorldRotMtx(m_PoseMtx);

	//�ړ��p�����[�^�̏�����
	m_Move.m_FaceDir = D3DXVECTOR3(0.0f,0.0f,1.0f);
	m_Move.m_FaceDirGoal = m_Move.m_FaceDir;
	m_Move.m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Move.m_Speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//���p�����[�^��ݒ�
	m_Environment.m_fCoeffiResist = 0.0f;
	m_Environment.m_fGravityAcc = 0.0f;
	m_Environment.m_fMass = 0.0f;

	//���蔻��~�S�����g�p
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		m_ColliCircle[i].SetUse(false);
		m_ColliCircle[i].Uninit();
	}
	m_ColliEnable.Uninit();

	SetCalcEnableCircle(D3DXVECTOR3(0.0f,0.0f,0.0f),-1,2.0f);							//���蔻��v�Z���苅���Z�b�g
	m_ColliCircleMaxNum = COLLI_CIRCLE_MAX;												//���蔻�苅MAX
}

void CHuman::Uninit(void)
{
	m_ModelMotion.Uninit();

	//���蔻��~�̉��
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		m_ColliCircle[i].SetUse(false);
		m_ColliCircle[i].Uninit();
	}
	m_ColliEnable.Uninit();
}

void CHuman::Update(void)
{
	//�Â����W�ۑ�
	m_posOld = m_pos;

	//�O��
	D3DXVECTOR3 PowerFromOut = m_Move.m_Power;

	//�d��
	float fGravity = m_Environment.m_fMass * m_Environment.m_fGravityAcc;

	//�R��
	D3DXVECTOR3 CoeffiResistVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	CoeffiResistVec.x = -m_Move.m_Speed.x * m_Environment.m_fCoeffiResist;
	CoeffiResistVec.z = -m_Move.m_Speed.z * m_Environment.m_fCoeffiResist;

	//���v�Z�o
	D3DXVECTOR3 PowerTotal;
	PowerTotal = PowerFromOut + CoeffiResistVec;
	PowerTotal.y += fGravity;

	//�����x�Z�o
	D3DXVECTOR3 Acc;
	if (m_Environment.m_fMass > 0)
	{
		Acc = PowerTotal / m_Environment.m_fMass;   //�����x�v�Z
		m_Move.m_Speed += Acc;         //���x�v�Z
	}
	else m_Move.m_Speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//�ʒu���Z�o
	m_pos += m_Move.m_Speed;

	CalcFaceDirNow();           //�������

	//�O�͂�0�ɂ���
	m_Move.m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_ModelMotion.SetPosition(m_pos);					//�ʒu
	m_ModelMotion.SetWorldRotMtx(m_PoseMtx);			//�|�[�Y(����,�X���Ȃ�)

	//�A�b�v�f�[�g
	m_ModelMotion.Update();
}

void CHuman::Draw(void)
{
	m_ModelMotion.Draw();           //�`��

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		//���蔻��~�̕`��
		for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
		{
			if (m_ColliCircle[i].GetUse() == true)
			{
				m_ColliCircle[i].Draw();
			}
		}

		if (CShowDebugFlag::GetShowDebugFlagLayer2() == true) m_ColliEnable.Draw();
	}
#endif
}

void CHuman::DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	m_ModelMotion.DrawAllOnDepthMap(LightView, LightProje);
}

void CHuman::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture) 
{
	m_ModelMotion.DrawUseDepthMap(LightView, LightProje, DepthTexture);

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		//���蔻��~�̕`��
		for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
		{
			if (m_ColliCircle[i].GetUse() == true)
			{
				m_ColliCircle[i].Draw();
			}
		}

		if (CShowDebugFlag::GetShowDebugFlagLayer2() == true) m_ColliEnable.Draw();
	}
#endif
}

void CHuman::DrawSpecular(void)
{
	m_ModelMotion.DrawSpecular();

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		//���蔻��~�̕`��
		for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
		{
			if (m_ColliCircle[i].GetUse() == true)
			{
				m_ColliCircle[i].Draw();
			}
		}

		if (CShowDebugFlag::GetShowDebugFlagLayer2() == true) m_ColliEnable.Draw();
	}
#endif
}

void CHuman::DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture)
{
	m_ModelMotion.DrawMotionBlur(OriginalTexture);
}

void CHuman::SetEnvironment(float fMass, float GravityAcc, float CoeffiResist)
{
	m_Environment.SetEnvironment(fMass, GravityAcc, CoeffiResist);
}

void CHuman::SetFaceDir(const D3DXVECTOR3& dir)  //�ړ������Z�b�g
{
	D3DXVECTOR3 MoveDir;
	D3DXVec3Normalize(&MoveDir, &dir);
	m_Move.m_FaceDir = MoveDir;
}

void CHuman::SetGoalDir(const D3DXVECTOR3& dir)
{
	if (dir == D3DXVECTOR3(0.0f, 0.0f, 0.0f)) return;
	D3DXVec3Normalize(&m_Move.m_FaceDirGoal, &dir);
}

void CHuman::CalcFaceDirNow(void)
{
	bool bValueChange = false;
	if (m_Move.m_FaceDirGoal != m_Move.m_FaceDir) {
		bValueChange = true;
	}

	if(bValueChange)
	{
		//��]��
		D3DXVECTOR3 RotateVec;
		D3DXVec3Cross(&RotateVec, &m_Move.m_FaceDir, &m_Move.m_FaceDirGoal);
		D3DXVec3Normalize(&RotateVec,&RotateVec);
		if (RotateVec == D3DXVECTOR3(0.0f, 0.0f, 0.0f)) RotateVec = D3DXVECTOR3(0.0f,1.0f,0.0f);

		//��]�N�H�[�^�j�I��������
		D3DXQUATERNION Q1;
		D3DXQuaternionRotationAxis(&Q1,&RotateVec, m_RorateSpeed);
		
		//��]��̃x�N�g���̌��ʂ����߂�
		D3DXMATRIX RotMtx;
		D3DXMatrixRotationQuaternion(&RotMtx, &Q1);
		D3DXVec3TransformCoord(&m_Move.m_FaceDir, &m_Move.m_FaceDir, &RotMtx);

		//�ڕW�ʒu�ɓ��B�����̂�
		D3DXVECTOR3 RotateVecAf;
		D3DXVec3Cross(&RotateVecAf, &m_Move.m_FaceDir, &m_Move.m_FaceDirGoal);
		D3DXVec3Normalize(&RotateVecAf, &RotateVecAf);
		if (RotateVecAf == D3DXVECTOR3(0.0f, 0.0f, 0.0f)) RotateVecAf = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
		
		float CheckDot = D3DXVec3Dot(&RotateVec,&RotateVecAf);		//��]�p�x
		if (CheckDot < 0) m_Move.m_FaceDir = m_Move.m_FaceDirGoal;
	}
}

void CHuman::UpdateCollisionCircle(void)
{
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		if (m_ColliCircle[i].GetUse() == false) continue;
		m_ColliCircle[i].Update();

		//���蔻��~�ʒu�̍X�V
		D3DXMATRIX MtxParent = m_ModelMotion.GetPartMtx(m_ColliCircle[i].GetParentID());				//�e�̃}�g���N�X���擾
		D3DXVECTOR3 offset = m_ColliCircle[i].GetOffset();
		D3DXMATRIX MtxOffset;
		D3DXMatrixTranslation(&MtxOffset, offset.x, offset.y, offset.z);							//�I�t�Z�b�g�}�g���N�X�����߂�

		D3DXMATRIX MtxResult = MtxOffset*MtxParent;													//���ʃ}�g���N�X�����߂�
		m_ColliCircle[i].SetMtxResult(MtxResult);													//���ʃ}�g���N�X��ݒ�
	}

	m_ColliEnable.Update();
	D3DXMATRIX MtxParent = m_ModelMotion.GetPartMtx(m_ColliEnable.GetParentID());				//�e�̃}�g���N�X���擾
	D3DXVECTOR3 offset = m_ColliEnable.GetOffset();
	D3DXMATRIX MtxOffset;
	D3DXMatrixTranslation(&MtxOffset, offset.x, offset.y, offset.z);							//�I�t�Z�b�g�}�g���N�X�����߂�

	D3DXMATRIX MtxResult = MtxOffset*MtxParent;													//���ʃ}�g���N�X�����߂�
	m_ColliEnable.SetMtxResult(MtxResult);														//���ʃ}�g���N�X��ݒ�
}

CCollisionCircle* CHuman::GetColli(int ID)
{
	if (ID < 0 || ID >= COLLI_CIRCLE_MAX) {
		MessageBox(nullptr, "�擾���悤�Ƃ��Ă��铖�蔻��~�̔ԍ��͔͈͊O", "�G���[", MB_OK | MB_ICONHAND);
		return nullptr;
	}
	return &m_ColliCircle[ID];
}

void CHuman::SetCollisionCircle(const D3DXVECTOR3& offset, CCollisionCircle::TYPE type, int ParentID, float Radius)
{
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		if (m_ColliCircle[i].GetUse() == false)
		{
			m_ColliCircle[i].Init(Radius, ParentID, type);
			m_ColliCircle[i].SetOffset(offset);
			m_ColliCircle[i].SetUse(true);
			break;
		}
	}
}

void CHuman::SetCalcEnableCircle(const D3DXVECTOR3& offset, int ParentID, float Radius)
{
	m_ColliEnable.Init(Radius, ParentID, CCollisionCircle::TYPE_HIT_ENABLE);
	m_ColliEnable.SetOffset(offset);
	m_ColliEnable.SetUse(true);
}
/***********************************CHuman����End*********************************/