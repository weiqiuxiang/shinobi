#pragma once
#ifndef _MODELOBJ_H_
#define _MODELOBJ_H_

#include "main.h"

class CModelObj
{
public:
	CModelObj();
	~CModelObj();

	//Init,Uninit,Draw�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Draw(void);
	void DrawMtrl(const D3DMATERIAL9& Mtrl);
	void DrawOnDepthMap(const D3DXMATRIX& mtxWorld, const D3DXMATRIX& mtxLightView, const D3DXMATRIX& mtxLightProje);
	void DrawUseDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(const D3DXMATRIX& world);
	void DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture);
	
	//�Z�b�^�[
	void SetModelPass(const char* filepass);		//���f���̃t�@�C���p�X��ݒ�
	void SetAlpha(float Alpha);

	//�Q�b�^�[
	const char *GetModelPass(void);					//���f���p�X�̎擾
	LPD3DXMESH GetMesh(void) { return m_pMesh; }

	//���̊֐�
	void CreateModelObj(void);						//���f���I�u�W�F�̐���

private:
	LPD3DXMESH m_pMesh;								//���b�V���C���^�[�t�F�[�X�|�C���^
	LPD3DXBUFFER m_pBuffMat;						//�}�e���A�����o�b�t�@(D3DX�ėp�o�b�t�@�C���^�[�t�F�[�X)
	DWORD m_nNumMat;								//�}�e���A���̐�
	TEX_ID *m_TexID;								//�e�N�X�`���ԍ�
	char m_pFilepass[256];							//���f���t�@�C���p�X
};

#endif