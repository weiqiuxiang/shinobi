#pragma once
#ifndef _CHUMAN_H_
#define _CHUMAN_H_

#include "main.h"
#include "CModel.h"
#include "CModelMotion.h"
#include "CCollisionCircle.h"

#define COLLI_CIRCLE_MAX (20)

class CMove
{
public:
	//�����o�ϐ�
	D3DXVECTOR3 m_Speed;              //���x�x�N�g��(�����܂�)
	D3DXVECTOR3 m_Power;              //�v���[���[�󂯂����O��
	D3DXVECTOR3 m_FaceDir;            //�v���[���[�̈ړ�����
	D3DXVECTOR3 m_FaceDirGoal;
};

class CMoveEnvironment
{
public:
	//�����o�֐�
	//�Z�b�^�[
	void SetEnvironment( float fMass, float GravityAcc, float CoeffiResist);   //���p�����[�^�ꊇ�ݒ�
	void SetGravityAcc(float GravityAcc) { m_fGravityAcc = GravityAcc; }
	void SetCoeffiResist(float fCoeffiResist) { m_fCoeffiResist = max(0.0f, fCoeffiResist); }
	void SetMass(float fMass) { m_fMass = max(0, fMass); }

	//�Q�b�^�[
	float GetGravityAcc(void) { return m_fGravityAcc; }
	float GetCoeffiResist(void) { return m_fCoeffiResist; }
	float GetMass(void) { return m_fMass; }

	//�����o�ϐ�
	float    m_fGravityAcc;          //�d�͉����x
	float    m_fCoeffiResist;        //��R�W��
	float    m_fMass;                //����
};

class CHuman            //�ꎞ�I��static��ݒ肷��
{
public:
	CHuman();
	~CHuman();

	//Init,Uninit,Update,Draw�֐�
	virtual void Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	virtual void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	virtual void DrawSpecular(void);
	virtual void DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture);

	//�Z�b�^�[
	void SetEnvironment(float fMass, float GravityAcc, float CoeffiResist); //���p�����[�^�ꊇ�ݒ�
	void SetPos(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }
	void SetSpeed(const D3DXVECTOR3& Speed) { m_Move.m_Speed = Speed; }
	void SetSpeedY(const float Speed) { m_Move.m_Speed.y = Speed; }
	void SetPosX(float PosX) { m_pos.x = PosX; }
	void SetPosY(float PosY) { m_pos.y = PosY; }
	void SetPosZ(float PosZ) { m_pos.z = PosZ; }
	void SetRotX(float RotX) { m_rot.x = RotX; }
	void SetRotY(float RotY) { m_rot.y = RotY; }
	void SetRotZ(float RotZ) { m_rot.z = RotZ; }
	void SetFaceDir(const D3DXVECTOR3& dir);
	void SetPoseMtx(const D3DXMATRIX& PoseMtx) { m_PoseMtx = PoseMtx; }
	void SetGoalDir(const D3DXVECTOR3& Dir);
	void SetRorateSpeed(float speed) { m_RorateSpeed = (float)max(0.01, speed); }
	void SetCoffi(float Coffi) { m_Environment.m_fCoeffiResist = Coffi; }
	void SetOtherParentMtx(const D3DXMATRIX& worldMtx) { m_ModelMotion.SetOtherParentMtx(worldMtx); }
	void SetMaterial(const D3DMATERIAL9& Mtrl) { m_ModelMotion.SetMaterial(Mtrl); }

	void AddPos(const D3DXVECTOR3& pos) { m_pos += pos; }
	void AddPower(const D3DXVECTOR3& Power) { m_Move.m_Power += Power; }
	void AddSpeed(const D3DXVECTOR3& Speed) { m_Move.m_Speed += Speed; }
	void SetMotionMode(int MotionNum) { m_ModelMotion.SetMotionNow(MotionNum); }
	void SetDrawType(CModel::DRAW_TYPE type) { m_ModelMotion.SetDrawType(type); }

	//�Q�b�^�[
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetBodyPos(void) { return m_ModelMotion.GetPartPosInWorld(0); }		//�e�\���ň�ԏ�w�̃p�[�c�̈ʒu�擾
	D3DXVECTOR3 GetRot(void) { return m_rot; }
	D3DXVECTOR3 GetFaceDir(void) { return m_Move.m_FaceDir; }
	D3DXVECTOR3 GetSpeed(void) { return m_Move.m_Speed; }
	D3DXMATRIX GetBodyResultMtx(void) { return m_ModelMotion.GetPartMtx(0); }
	LPD3DXMESH GetBodyMesh(void) { return m_ModelMotion.GetMesh(0); }
	int GetMotionNow(void) { return m_ModelMotion.GetMotionNow(); }

	//���̊֐�
	int GetMotionTotalFrame(int MotionID) { return m_ModelMotion.GetTotalFrame(MotionID); }
	int GetMotionTotalKeyFrame(int MotionID) { return m_ModelMotion.GetTotalKeyFrame(MotionID); }

	int GetFrameOfOneKeyFrame(int MotionID, int KeyFrameID) { return m_ModelMotion.GetFrameOfKeyFrame(MotionID, KeyFrameID); }
	int GetKeyFrameNow(void) { return m_ModelMotion.GetKeyFrameNow(); }					//���ݐi�s���̃L�[�t���[��
	int GetMaxColliCircleNum(void) { return m_ColliCircleMaxNum; }						//���蔻��~�̍ő吔
	D3DXVECTOR3 GetVecOldPosToNowPos(void) { return m_pos - m_posOld; }

	//���̊֐�
	void ReturnOldPos(void) { m_pos = m_posOld; }										//�O�̍��W�ɖ߂�
	void ReturnOldPosXZ(void) { m_pos.x = m_posOld.x; m_pos.z = m_posOld.z;}			//XZ���̕��������O�̍��W�ɖ߂�
	inline bool TheSamePosition(void){													//�O�Ɠ������W���ǂ���(������������,true��Ԃ�)
		return (m_pos == m_posOld) ? true : false;
	}
	CCollisionCircle* GetColli(int ID);
	CCollisionCircle* GetColliEnable(void) { return &m_ColliEnable; }
protected:
	//�����o�֐�
	void CalcFaceDirNow(void);						//���̌����v�Z
	void UpdateCollisionCircle(void);
	void SetCollisionCircle(const D3DXVECTOR3& offset, CCollisionCircle::TYPE type, int ParentID, float Radius);				//���蔻��~�Z�b�g
	void SetCalcEnableCircle(const D3DXVECTOR3& offset, int ParentID, float Radius);

	//�����o�ϐ�
	CModelMotion		m_ModelMotion;			//�v���[���[�̃��[�V�������j�b�g
	D3DXVECTOR3			m_pos;					//�v���[���[�̍��W
	D3DXVECTOR3			m_posOld;				//�v���[���[�̌Â����W
	D3DXVECTOR3			m_rot;					//�v���[���[�L�����̉�]
	D3DXMATRIX			m_PoseMtx;				//Pose�}�g���N�X
	CMove				m_Move;					//�ړ��p�����[�^
	CMoveEnvironment	m_Environment;			//���p�����[�^
	float				m_RorateSpeed;			//(�����ύX��)��]���x

	//���蔻��~�����o�ϐ�
	CCollisionCircle	m_ColliCircle[COLLI_CIRCLE_MAX];		//���蔻�苅
	CCollisionCircle	m_ColliEnable;							//���蔻��v�Z��L���ɂ��鋅
	int					m_ColliCircleMaxNum;					//���蔻��~�ő吔(�O���p)
};

#endif