#pragma once
#ifndef _CMESH_FILED_H_
#define _CMESH_FILED_H_

#include "main.h"
#include "CScene.h"
#include "CINDEX_SIZE.h"

typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex;
}VERTEX_MESHFIELD;

class CMeshField : public CScene
{
public:
	CMeshField();
	~CMeshField();

	//Init,Uninit,Update,Draw�֐�
	HRESULT Init(const D3DXVECTOR3& SclRotPos,const CINDEX_SIZE& IndexSize,const char *TexFilePass);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);

	//�Q�b�^�[
	D3DXVECTOR2 GetPieceNum(void) { return m_IndexSize.m_PieceNum; }		//���b�V���̕������擾
	D3DXVECTOR2 GetPieceSize(void) { return m_IndexSize.m_PieceSize; }
	TEX_ID GetTexID(void) { return m_TexID; }
	int GetVertexNum(void) { return m_IndexData.nVertexNum; }				//���_���擾
	const MESH_FACE* GetMeshFace(void) { return m_MeshFace; }
	const MESH_VERTEX* GetMeshVertex(void) { return m_MeshVertex; }
	const double* GetHighRelative(void) { return m_VertexHighRelative; }

	//���̊֐�
	void LoadRelativeHigh(const double *MeshHigh);

private:
	//�����o�֐�
	void LockVertexBuf(void);				//���_�o�b�t�@���b�N
	void LockIndexBuf(void);				//�C���f�b�N�X�o�b�t�@���b�N
	HRESULT CreateDeclaration(void);			//Declaration�����

	//�����o�ϐ�
	CINDEX_SIZE		m_IndexSize;				//���b�V���t�B�[���h�T�C�Y�p�����[�^
	MESH_VERTEX*	m_MeshVertexLock;			//���b�V���̒��_���b�N���
	MESH_VERTEX*	m_MeshVertex;				//���b�V���̒��_���
	MESH_FACE*		m_MeshFace;					//���b�V���t�F�[�X(�ʏ��)
	double*			m_VertexHighRelative;		//���_�̑��΍���

protected:
	//�����o�֐�
	HRESULT InitLockVertexBufAndIndexBuf(void);	//�e�N�X�`���o�b�t�@�ƃC���f�b�N�X�o�b�t�@�̏�����
	void InitMeshVertexAndFace(void);			//���_�ƃ|���S���ʂ̏�����
	void InitVertexAndFacePara(void);			//���_�ƃ|���S���ʂ̃f�[�^�̏�����
	void MtxSetting(const SRP& SclPosRot);		//���[���h�}�g���N�X�Z�b�g
	D3DXMATRIX CalcWorldMtx(void);				//���[���h�}�g���N�X�̌v�Z
	D3DXMATRIX CalcWorldInverseMtx(void);		//���[���h�}�g���N�X�̋t�s��̌v�Z

	//�����o�ϐ�
	D3DXVECTOR3 m_rot;							//�}�g���N�X�����p��]�f�[�^
	D3DXVECTOR3 m_scl;							//�g��k��
	INDEX_DATA m_IndexData;						//�C���f�b�N�X���b�V���̃f�[�^(���_��,�C���f�b�N�X��,�|���S����)
	TEX_ID m_TexID;								//�g�p�e�N�X�`���ԍ�
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;		//���_�o�b�t�@
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;		//�C���f�b�N�X�o�b�t�@
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;	//declaration

	VERTEX_MESHFIELD	*m_pVertexBuffer = nullptr;
	WORD				*m_pIndexBuffer = nullptr;
};

#endif