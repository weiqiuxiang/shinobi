#pragma once
#ifndef CPART_H_
#define CPART_H_

#include "main.h"
#include "CModel.h"
#include <list>
#include <string.h>

#define PART_NAME_FONT_MAX_NUM (128)		//�p�[�c�̃t�H���g�̍ő吔

class CPart
{
public:
	CPart();
	~CPart();

	//Init,Uninit,Update,Draw�֐�
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(void);
	void DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture);

	//�Z�b�^�[
	void SetNowMtx(D3DXMATRIX& resultMtx) { m_MtxNow = resultMtx; }
	inline void SetParent(CPart *pParent) {
		if (this != pParent) {
			m_pParent = pParent;
		}
	}
	void SetName(const char* Name) { strcpy_s(m_Name, PART_NAME_FONT_MAX_NUM, Name); }
	void AddChildren(CPart *pChild) {m_pChildrenList.push_back(pChild); }

	//�Q�b�^�[
	CPart* GetParent(void) const { return m_pParent; }
	auto GetChildrenListBegin(void) const { return m_pChildrenList.begin(); }
	auto GetChildrenListEnd(void) const { return m_pChildrenList.end(); }
	LPD3DXMESH GetMesh(void) const { return m_Model.GetMesh(); }
	D3DXMATRIX GetNowMtx(void) const { return m_MtxNow; }
	SRP GetSRPNow(void) const { return m_SRPNow; }
	const char* GetName(void) const { return m_Name; }

	//���̊֐�
	bool IsParent(CPart*);					//�e���ǂ���
	bool IsChild(CPart*);					//�q�����ǂ���
	bool IsLowerHierarchy(CPart*);			//������艺�̊K�w���ǂ���

	//�����o�ϐ�
	CModel				m_Model;			//�`�惆�j�b�g
	SRP					m_SRPOffset;		//�I�t�Z�b�g
	D3DXMATRIX			m_MtxNow;			//���̃}�g���N�X
	D3DXMATRIX			m_MtxOld;			//�ȑO�̃}�g���N�X
	SRP					m_SRPNow;			//���̃t���[����SRP
	SRP					m_SRPBlendOld;		//�t���[���̎��s�󋵂�ۑ�,�ʂ̃��[�V���������s�����,blending����
	int					m_PartID;			//����B��ID
	int					m_ParentID;			//�e��ID
	char				m_Name[PART_NAME_FONT_MAX_NUM];	//���O
private:
	CPart*				m_pParent;			//�e�|�C���^
	std::list<CPart*>	m_pChildrenList;	//�q���|�C���^
};

#endif