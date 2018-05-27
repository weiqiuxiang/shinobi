#include "CScene3D_Draw.h"
#include "CCameraManager.h"
#include "CRenderer.h"

void C3DPolygon::MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale/*�g��s��*/, mtxRot/*��]�s��*/, mtxMove/*�ړ��s��*/, mtxResult/*���ʍs��*/, WorldMatrix;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, SclPosRot.rot.y/*y��*/, SclPosRot.rot.x/*x��*/, SclPosRot.rot.z/*z��*/); //��]�s���ݒ�
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;                                           //���ʂ̕ϊ��s��̌v�Z
	D3DXMatrixIdentity(&WorldMatrix);                              //���[���h�s���P�ʍs��ŏ�����
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);    //��]�s��ƃ��[���h�s��̊|���Z
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //���[���h�ϊ��s��
	*pOutWorldMtx = WorldMatrix;                                   //�ۑ�
}

void C3DPolygon::VertexBufferLock(const D3DXVECTOR3 &size, const RGBA& rgba, LPDIRECT3DVERTEXBUFFER9 pVtxBuffer)
{
	VERTEX_3D *pVtx = nullptr;

	D3DXVECTOR3 HalfSize = size * 0.5f;
	
	pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-HalfSize.x, HalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(HalfSize.x, HalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-HalfSize.x, -HalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(HalfSize.x, -HalfSize.y, 0.0f);

	pVtx[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVtx[0].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[1].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[2].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[3].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtxBuffer->Unlock();
}

void CBillboard::MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale/*�g��s��*/, mtxMove/*�ړ��s��*/, mtxResult/*���ʍs��*/, WorldMatrix;
	D3DXMATRIX mtxView, mtxViewInverse;

	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);   //�ړ��s��ݒ�

	CAMERA_PARA PosLookatUp = CCameraManager::GetCameraPara(CCameraManager::GetActiveCameraName());                       //�J�����X�e�[�^�X�擾

	D3DXMatrixLookAtLH(&mtxView, &PosLookatUp.pos, &PosLookatUp.lookat, &PosLookatUp.up); //�r���[�s�񐶐�
	D3DXMatrixTranspose(&mtxViewInverse, &mtxView);                                     //�]�u�s�񐶐�

	mtxViewInverse._14 = 0.0f;                                                    //���s�ړ��������J�b�g
	mtxViewInverse._24 = 0.0f;													  //���s�ړ��������J�b�g
	mtxViewInverse._34 = 0.0f;													  //���s�ړ��������J�b�g

	mtxViewInverse._41 = 0.0f;                                                    //���s�ړ��������J�b�g
	mtxViewInverse._42 = 0.0f;													  //���s�ړ��������J�b�g
	mtxViewInverse._43 = 0.0f;													  //���s�ړ��������J�b�g

	mtxResult = mtxScale * mtxViewInverse * mtxMove;                                         //���ʂ̕ϊ��s��̌v�Z
	D3DXMatrixIdentity(&WorldMatrix);                              //���[���h�s���P�ʍs��ŏ�����
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);    //��]�s��ƃ��[���h�s��̊|���Z
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //���[���h�ϊ��s��
	*pOutWorldMtx = WorldMatrix;                                   //�ۑ�
}

void CBillboard::VertexBufferLock(const D3DXVECTOR3 &size, const RGBA& rgba, LPDIRECT3DVERTEXBUFFER9 pVtxBuffer)
{
	VERTEX_3D *pVtx = nullptr;

	D3DXVECTOR3 HalfSize = size*0.5;

	pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-HalfSize.x, HalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(HalfSize.x, HalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-HalfSize.x, -HalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(HalfSize.x, -HalfSize.y, 0.0f);

	pVtx[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVtx[0].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[1].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[2].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[3].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtxBuffer->Unlock();
}

void CBillboardBottom::MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale/*�g��s��*/, mtxMove/*�ړ��s��*/, mtxResult/*���ʍs��*/, WorldMatrix;
	D3DXMATRIX mtxView, mtxViewInverse;

	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);   //�ړ��s��ݒ�

	CAMERA_PARA PosLookatUp = CCameraManager::GetCameraPara(CCameraManager::GetActiveCameraName());                       //�J�����X�e�[�^�X�擾

	D3DXMatrixLookAtLH(&mtxView, &PosLookatUp.pos, &PosLookatUp.lookat, &PosLookatUp.up); //�r���[�s�񐶐�
	D3DXMatrixTranspose(&mtxViewInverse, &mtxView);                                     //�]�u�s�񐶐�

	mtxViewInverse._14 = 0.0f;                                                    //���s�ړ��������J�b�g
	mtxViewInverse._24 = 0.0f;													  //���s�ړ��������J�b�g
	mtxViewInverse._34 = 0.0f;													  //���s�ړ��������J�b�g

	mtxViewInverse._41 = 0.0f;                                                    //���s�ړ��������J�b�g
	mtxViewInverse._42 = 0.0f;													  //���s�ړ��������J�b�g
	mtxViewInverse._43 = 0.0f;													  //���s�ړ��������J�b�g

	mtxResult = mtxScale * mtxViewInverse * mtxMove;                                         //���ʂ̕ϊ��s��̌v�Z
	D3DXMatrixIdentity(&WorldMatrix);                              //���[���h�s���P�ʍs��ŏ�����
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);    //��]�s��ƃ��[���h�s��̊|���Z
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //���[���h�ϊ��s��
	*pOutWorldMtx = WorldMatrix;                                   //�ۑ�
}

void CBillboardBottom::VertexBufferLock(const D3DXVECTOR3 &size, const RGBA& rgba, LPDIRECT3DVERTEXBUFFER9 pVtxBuffer)
{
	VERTEX_3D *pVtx = nullptr;

	float HalfSizeX = size.x * 0.5f;

	pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-HalfSizeX, size.y*0.92f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(HalfSizeX, size.y*0.92f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-HalfSizeX, -size.y*0.08f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(HalfSizeX, -size.y*0.08f, 0.0f);

	pVtx[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVtx[0].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[1].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[2].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);
	pVtx[3].color = D3DCOLOR_RGBA(rgba.r, rgba.g, rgba.b, rgba.a);

	//�|���S���`�悷��O�Ƀe�N�X�`�����Z�b�g
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtxBuffer->Unlock();
}