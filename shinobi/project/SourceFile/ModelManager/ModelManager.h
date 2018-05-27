#pragma once
#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include "main.h"
#include "ModelObj.h"

#define MODELSLOT_NUM (60)

class CModelManager
{
public:
	//�ÓI�����o�֐�
	static HRESULT InitAll(void);
	static void UninitAll(void);

	static MODEL_ID LoadModel(const char *FilePass);						//���f�������[�h��ID��Ԃ�
	static void DrawModel(MODEL_ID IDNum);									//���f����`�悷��
	static void DrawModelUseMtrl(MODEL_ID IDNum, const D3DMATERIAL9& Mtrl);	//���f���Ƀ}�e���A���ݒ肵�ĕ`��
	static void DrawModelOnDepthMap(MODEL_ID IDNum,const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	static void DrawUseDepthMap(MODEL_ID IDNum, const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje,LPDIRECT3DTEXTURE9 DepthTexture);
	static void DrawSpecular(MODEL_ID IDNum, const D3DXMATRIX& world);
	static void DrawMotionBlur(MODEL_ID IDNum, const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture);
	static const char* GetModelPass (MODEL_ID IDNum);						//���f���p�X�擾
	static LPD3DXMESH GetMesh (MODEL_ID IDNum);								//���f���̃��b�V���|�C���^�擾
	static void SetAlpha(MODEL_ID IDNum, float Alpha);

private:
	//�ÓI�����o�֐�
	static HRESULT CreateModelObj(MODEL_ID IDNum, const char *FilePass);           //���f���X���b�g�Ƀ��f��������

	//�ÓI�����o�ϐ�
	static CModelObj *m_ModelSlot[MODELSLOT_NUM];    //���f���X���b�g
};

#endif