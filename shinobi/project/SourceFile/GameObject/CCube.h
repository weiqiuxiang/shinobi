//�w�b�_���� : AI�̏���|�C���g�̕`�惆�j�b�g
#ifndef CCUBE_H_
#define CCUBE_H_

#include "main.h"

class CCube
{
public:
	CCube();
	~CCube();

	HRESULT Init(float X,float Y,float Z);
	void Update(void);
	void Draw(void);					//�f�o�b�N���[�h�̂ݕ`�悷��
	void DrawSelectedByEditor(void);	//�G�f�B�^�[�p�`��
	void Uninit(void);

	//�Q�b�^�[
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	LPD3DXMESH GetMesh(void) { return m_pMesh; }
	D3DXMATRIX GetWorldMtx(void) { return m_WorldMtx; }

	//�Z�b�^�[
	void SetPos(const D3DXVECTOR3& pos) { m_pos = pos; }

private:
	//�����o�֐�
	void MtxWorldSetting(void);

private:
	//�����o�ϐ�
	D3DXVECTOR3 m_pos;						//�ʒu
	D3DXVECTOR3 m_scl;						//�X�P�[��
	D3DXMATRIX	m_WorldMtx;					//���[���h�}�g���N�X
	float		m_X, m_Y, m_Z;				//�L���[�u�̃T�C�Y
	LPD3DXMESH	m_pMesh;						//���b�V���|�C���^
	D3DMATERIAL9 m_Mtrl;					//�}�e���A��
	D3DMATERIAL9 m_MtrlSelectedByEditor;	//�G�f�B�^�[���A�I�����ꂽ��Ԃ̃}�e���A��
};

#endif