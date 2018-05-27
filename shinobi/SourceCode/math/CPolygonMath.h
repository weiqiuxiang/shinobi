#pragma once
#ifndef _CPOLYGON_MATH_H_
#define _CPOLYGON_MATH_H_

#include "main.h"

//前方宣言
class CModel;

struct Segment {
	D3DXVECTOR2 s; // 始点
	D3DXVECTOR2 v; // 方向ベクトル（線分の長さも担うので正規化しないように！）
};

//クラス定義
class CPolygonMath
{
public:
	static D3DXVECTOR3 CalcTriNormalize(const MESH_FACE& MeshFace); //三角ポリゴンの面法線計算(汎用)
	static bool CalcLineAndTripoly_InterSecPoint(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint,const MESH_FACE& MeshFace, D3DXVECTOR3* pOut);//線分と平面の交差点を求めて交差点が平面上にいるかどうか(専用)
	static D3DXVECTOR3 GetVecPlane_Intersect(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint, const MESH_FACE& Face);  //線分と平面の交差点(汎用)
	static float DistancePointPlane(const D3DXVECTOR3 &Point, const MESH_FACE& Face);          //点と平面の距離(汎用)
	static bool Rects3DPointPlane(const D3DXVECTOR3 &Point, const MESH_FACE& Face);            //点は平面上にいるかどうか(汎用)
	static bool PointInXZMeshFace(const D3DXVECTOR3 &Point, const MESH_FACE& Face);              //点が三角ポリゴンの中にいるかどうか(Y方向無視)(汎用)
	static float PointElevationInFace(const D3DXVECTOR3 &Point, const MESH_FACE& Face);                 //点がポリゴンがいる平面の標高
	static void CalcPoseMtx(D3DXMATRIX *pOut/*出力*/,const D3DXVECTOR3 &FaceVec/*キャラ向き*/, const D3DXVECTOR3 &Up/*キャラ上方向*/); /*姿勢マトリクスを求める*/
	static D3DXVECTOR3* CalcScreenToWorld    //スクリーン座標からワールド座標に変換
	(
		D3DXVECTOR3* pOutWorld,
		int Sx/*スクリーンX座標*/, 
		int Sy/*スクリーンY座標*/, 
		float fZ/* 射影空間でのZ値（0～1）*/, 
		int Screen_w, 
		int Screen_h, 
		D3DXMATRIX* View,
		D3DXMATRIX* Prj); 
	static void CalcMtxFromSRP(D3DXMATRIX *pOutMtx,const SRP& SRP);
	static float Vec2AngleAndAngleDir(const D3DXVECTOR2& VecA, const D3DXVECTOR2& VecB);										//Vec2の角度と角度の方向を求める関数
	static D3DXVECTOR2 CalcTwoPoint_HorizonAngleAndVerticleAngle(const D3DXVECTOR3& Camera, const D3DXVECTOR3& Lookat);
	static void DirVec3QuaternionRot(D3DXVECTOR3* pOut/*方向ベクトル出力*/, const D3DXVECTOR3* pIn/*方向ベクトル入力*/, const D3DXVECTOR3& RotAixs/*回転軸*/,float angle/*回転角度*/);		//方向ベクトルの回転(クォータニオンを使用)(汎用)
	static bool HitAABB2D(const D3DXVECTOR2& APos, const D3DXVECTOR2& ASize, const D3DXVECTOR2& BPos, const D3DXVECTOR2& BSize);

	//二次元線分と線分の当り判定を求める
	static bool CPolygonMath::ColSegments(
		Segment &seg1,          // 線分1
		Segment &seg2,          // 線分2
		float* outT1 = 0,       // 線分1の内分比（出力）
		float* outT2 = 0,       // 線分2の内分比（出力
		D3DXVECTOR2* outPos = 0 // 交点（出力）
	);
};

#endif