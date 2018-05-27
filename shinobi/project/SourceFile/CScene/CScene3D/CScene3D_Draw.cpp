#include "CScene3D_Draw.h"
#include "CCameraManager.h"
#include "CRenderer.h"

void C3DPolygon::MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxRot/*回転行列*/, mtxMove/*移動行列*/, mtxResult/*結果行列*/, WorldMatrix;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, SclPosRot.rot.y/*y軸*/, SclPosRot.rot.x/*x軸*/, SclPosRot.rot.z/*z軸*/); //回転行列を設定
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;                                           //結果の変換行列の計算
	D3DXMatrixIdentity(&WorldMatrix);                              //ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);    //回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //ワールド変換行列
	*pOutWorldMtx = WorldMatrix;                                   //保存
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

	//ポリゴン描画する前にテクスチャをセット
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtxBuffer->Unlock();
}

void CBillboard::MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxMove/*移動行列*/, mtxResult/*結果行列*/, WorldMatrix;
	D3DXMATRIX mtxView, mtxViewInverse;

	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);   //移動行列設定

	CAMERA_PARA PosLookatUp = CCameraManager::GetCameraPara(CCameraManager::GetActiveCameraName());                       //カメラステータス取得

	D3DXMatrixLookAtLH(&mtxView, &PosLookatUp.pos, &PosLookatUp.lookat, &PosLookatUp.up); //ビュー行列生成
	D3DXMatrixTranspose(&mtxViewInverse, &mtxView);                                     //転置行列生成

	mtxViewInverse._14 = 0.0f;                                                    //平行移動成分をカット
	mtxViewInverse._24 = 0.0f;													  //平行移動成分をカット
	mtxViewInverse._34 = 0.0f;													  //平行移動成分をカット

	mtxViewInverse._41 = 0.0f;                                                    //平行移動成分をカット
	mtxViewInverse._42 = 0.0f;													  //平行移動成分をカット
	mtxViewInverse._43 = 0.0f;													  //平行移動成分をカット

	mtxResult = mtxScale * mtxViewInverse * mtxMove;                                         //結果の変換行列の計算
	D3DXMatrixIdentity(&WorldMatrix);                              //ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);    //回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //ワールド変換行列
	*pOutWorldMtx = WorldMatrix;                                   //保存
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

	//ポリゴン描画する前にテクスチャをセット
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtxBuffer->Unlock();
}

void CBillboardBottom::MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxMove/*移動行列*/, mtxResult/*結果行列*/, WorldMatrix;
	D3DXMATRIX mtxView, mtxViewInverse;

	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);   //移動行列設定

	CAMERA_PARA PosLookatUp = CCameraManager::GetCameraPara(CCameraManager::GetActiveCameraName());                       //カメラステータス取得

	D3DXMatrixLookAtLH(&mtxView, &PosLookatUp.pos, &PosLookatUp.lookat, &PosLookatUp.up); //ビュー行列生成
	D3DXMatrixTranspose(&mtxViewInverse, &mtxView);                                     //転置行列生成

	mtxViewInverse._14 = 0.0f;                                                    //平行移動成分をカット
	mtxViewInverse._24 = 0.0f;													  //平行移動成分をカット
	mtxViewInverse._34 = 0.0f;													  //平行移動成分をカット

	mtxViewInverse._41 = 0.0f;                                                    //平行移動成分をカット
	mtxViewInverse._42 = 0.0f;													  //平行移動成分をカット
	mtxViewInverse._43 = 0.0f;													  //平行移動成分をカット

	mtxResult = mtxScale * mtxViewInverse * mtxMove;                                         //結果の変換行列の計算
	D3DXMatrixIdentity(&WorldMatrix);                              //ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);    //回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //ワールド変換行列
	*pOutWorldMtx = WorldMatrix;                                   //保存
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

	//ポリゴン描画する前にテクスチャをセット
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtxBuffer->Unlock();
}