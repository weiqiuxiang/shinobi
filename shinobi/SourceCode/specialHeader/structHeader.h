#pragma once
#ifndef _STRUCT_HEADER01_H_
#define _STRUCT_HEADER01_H_

#include <d3dx9.h>

//-----------------------------------------------------------------------------------------
//構造体宣言
//-----------------------------------------------------------------------------------------
//頂点構造体(↑のファーマットの型に合わせること)
typedef struct
{
	D3DXVECTOR3 pos;                         //
	float rhw;								 //座標変換済み頂点フラグ(必ず1.0f)
	D3DCOLOR color;							 //頂点カラー
	D3DXVECTOR2 tex;                         //テクスチャ座標(UV or ST)
}VERTEX_2D;

typedef struct
{
	D3DXVECTOR3 pos;						//
	D3DXVECTOR3 normal;						//ベクトルの方向
	D3DCOLOR	color;						//
	D3DXVECTOR2 tex;						//テクスチャ座標(UV or ST)
}VERTEX_3D;

//マトリクス配列
typedef struct
{
	D3DXVECTOR3 pos;   //モデル移動(座標)
	D3DXVECTOR3 rot;   //モデル回転
	D3DXVECTOR3 scl;   //モデル拡大率
}SRP;

//カメラのステータス
typedef struct
{
	D3DXVECTOR3 pos;      //モデル移動(座標)
	D3DXVECTOR3 lookat;   //モデル回転
	D3DXVECTOR3 up;       //モデル拡大率
}CAMERA_PARA;

typedef struct
{
	int r;
	int g;
	int b;
	int a;
}RGBA;

typedef struct
{
	int nVertexNum;     //頂点数
	int nIndexNum;      //インデックス数
	int nPolygonNum;    //ポリゴン数
}INDEX_DATA;

typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 Normalize;
}MESH_VERTEX;

typedef struct
{
	D3DXVECTOR3 p0, p1, p2;
	D3DXVECTOR3 Normalize;
}MESH_FACE;

typedef struct
{
	float x, y, z;
}CUSTOMVERTEX;

typedef struct {
	LPDIRECT3DTEXTURE9 TexturePointer;		//テクスチャポインタ
	LPDIRECT3DSURFACE9 SurfacePointer;		//サーフェースポインタ
	LPDIRECT3DSURFACE9 DepthBufferPointer;	//深度バッファポインタ
	bool bUse;
}RENDER_TARGET_POINTER;

#endif