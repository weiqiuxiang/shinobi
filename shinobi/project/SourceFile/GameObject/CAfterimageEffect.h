/*==============================================================================
	�w�b�_����:
		�c���G�t�F�N�g�̃w�b�_
===============================================================================*/
#ifndef C_AFTERIMAGE_EFFECT_H_
#define C_AFTERIMAGE_EFFECT_H_

/*==============================================================================
	�C���N���[�h�w�b�_
===============================================================================*/
#include "main.h"
#include "CScene.h"
#include "CINDEX_SIZE.h"

/*==============================================================================
	�}�N����`
===============================================================================*/
#define AFTERIMAGE_EF_VERTEX_MAX (10)			//�G�t�F�N�g�̒��_�ő吔

/*==============================================================================
	�N���X��`
===============================================================================*/
class CAfterimageEffect : public CScene
{
public:
	CAfterimageEffect();
	~CAfterimageEffect();

	HRESULT Init(const char *TexFilePass);
	void Update(void);
	void Uninit(void);
	void Draw(void);
	
	void MoveEffectPosition(const D3DXVECTOR3& TopPos,const D3DXVECTOR3& BottomPos);	//�G�t�F�N�g�ʒu�̈ړ�
	bool UseEffectVertex(const D3DXVECTOR3& TopPos, const D3DXVECTOR3& BottomPos);		//�G�t�F�N�g���_�̎g�p
	void NoUseAllVertex(void);			//�S�Ă̒��_�𖢎g�p��Ԃɂ���
private:
	void LockVertexBuf(void);				//���_�o�b�t�@���b�N
	void LockIndexBuf(void);				//�C���f�b�N�X�o�b�t�@���b�N
	void MtxSetting(const SRP& SclPosRot);		//���[���h�}�g���N�X�Z�b�g
private:
	D3DXVECTOR3 m_VertexPos[AFTERIMAGE_EF_VERTEX_MAX];		//���_���W
	float		m_TextureUValue[AFTERIMAGE_EF_VERTEX_MAX];	//���_�̃e�N�X�`��U�l
	bool		m_UseVertex[AFTERIMAGE_EF_VERTEX_MAX];		//���_�̎g�p

	D3DXVECTOR3		m_rot;					//�}�g���N�X�����p��]�f�[�^
	D3DXVECTOR3		m_scl;					//�g��k��
	INDEX_DATA		m_IndexData;			//�C���f�b�N�X���b�V���̃f�[�^(���_��,�C���f�b�N�X��,�|���S����)
	TEX_ID			m_TexID;				//�g�p�e�N�X�`���ԍ�

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;			//���_�o�b�t�@
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;			//�C���f�b�N�X�o�b�t�@
	LPDIRECT3DINDEXBUFFER9 m_pIdxBufferBackFace;	//�G�t�F�N�g�̗��ʃC���f�b�N�X�o�b�t�@
};

#endif