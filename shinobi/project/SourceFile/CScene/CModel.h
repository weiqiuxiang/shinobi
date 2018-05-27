#pragma once
#ifndef _CMODEL_H_
#define _CMODEL_H_

#include "main.h"
#include "CScene.h"
#include "CCube.h"

class CModel : public CScene
{
public:
	typedef enum {
		DRAW_USE_MTRL_DEFALUT = 0,			//�ǂݍ��݂̎����f�����g�̃}�e���A���ŕ`��
		DRAW_USE_MTRL_CUSTOMIZE,			//�ݒ肵���̃J�X�^�}�C�Y�̃}�e���A���ŕ`��
		DRAW_USE_MTRL_MAX
	}DRAW_TYPE;

	//�����o�֐�
	CModel();
	~CModel();

	HRESULT Init(const SRP &PosRotScl,const char* filePass);
	void Update(void);
	void Draw(void);
	void DrawAABBTest(void);
	void DrawDefault(void);
	void DrawMtrl(void);
	void DrawUseMtrl(const D3DMATERIAL9& Mtrl);
	void DrawOnDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(const D3DXMATRIX& world);
	void DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture);
	void Uninit(void);

	//�Q�b�^�[
	D3DXMATRIX GetPoseMtx(void) const { return m_PoseMtx; }
	D3DXMATRIX GetResultMtx(void) const;
	D3DXMATRIX GetWorldPoseMtx(void) const;
	D3DXVECTOR3 GetRot(void) const { return m_rot; }
	MODEL_ID GetModelID(void) const { return m_ModelID; }
	SRP GetWorldSRP(void) const {
		SRP SclRotPos = { m_pos,m_rot,m_scl };
		return SclRotPos; }
	D3DXVECTOR3 GetDirVecXZ(void) const { return m_DirVecXZ; }
	LPD3DXMESH GetMesh(void) const;
	bool GetHitMeshFlag(void) const { return m_IsHitMesh; }
	int GetUseID(void) { return m_UseID; }

	//�Z�b�^�[
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }
	void SetPoseMtx(const D3DXMATRIX& PoseMtx) { m_PoseMtx = PoseMtx; }
	void SetMtxSetting(void);
	void SetMtxSettingPose(void);
	void SetMaterial(const D3DMATERIAL9& Mtrl) { m_Mtrl = Mtrl; }		//�}�e���A���w��`��̎��̃}�e���A���̐ݒ�
	void SetDrawType(DRAW_TYPE);
	void SetScl(const D3DXVECTOR3& scl) { m_scl = scl; }
	void SetHitMeshFlag(bool flag) { m_IsHitMesh = flag; }
	void SetIsAlwaysCalcAABBFlag(bool flag) { m_IsAlwaysCalcAABB = flag; }
	void SetAlpha(float Alpha);
	void SetUseID(int ID) { m_UseID = ID; }

	//���̊֐�
	void AddRotateY(float AddAngle) { m_rot.y += AddAngle; }
	void AddPosX(float AddValue) { m_pos.x += AddValue; }
	void AddPosZ(float AddValue) { m_pos.z += AddValue; }

	void CalcAABB(void);			//AABB���v�Z����
	void GetAABBValue(D3DXVECTOR2 *OutMinXMaxX, D3DXVECTOR2 *OutMinYMaxY, D3DXVECTOR2 *OutMinZMaxZ);

private:
	//�����o�ϐ�
	D3DXVECTOR3		m_rot;
	D3DXVECTOR3		m_scl;
	MODEL_ID		m_ModelID;
	D3DXVECTOR3		m_DirVecXZ;				//XZ�����̌����x�N�g��
	D3DXMATRIX		m_PoseMtx;				//�p���}�g���N�X(��]�s��ł���,�g��k���ƈړ��͓����Ă��Ȃ�)
	D3DXMATRIX		m_Result;				//���U���g
	D3DMATERIAL9	m_Mtrl;					//�}�e���A��
	DRAW_TYPE		m_DrawType;				//�`��̎d��
	bool			m_IsHitMesh;			//true�������烁�b�V���Ƃ̓��蔻�肠��
	int				m_UseID;				//�ėpID

	//AABB�ϐ�
	bool			m_IsAlwaysCalcAABB;		//AABB�̌v�Z�����ɍs���t���O
	D3DXVECTOR2		m_AABBMinXMaxX;			//AABB��X�����̍ŏ��l�ƍő�l(m_MinXMaxX.x�͍ŏ��l)
	D3DXVECTOR2		m_AABBMinYMaxY;			//AABB��Y�����̍ŏ��l�ƍő�l(m_MinYMaxY.x�͍ŏ��l)
	D3DXVECTOR2		m_AABBMinZMaxZ;			//AABB��Z�����̍ŏ��l�ƍő�l(m_MinZMaxZ.x�͍ŏ��l)

	//AABB�̕`��p�L���[�u
	CCube			m_Cube;
};

#endif