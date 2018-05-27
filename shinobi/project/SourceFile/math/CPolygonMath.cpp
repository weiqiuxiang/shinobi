#include "CPolygonMath.h"
#include "CRenderer.h"

D3DXVECTOR3 CPolygonMath::CalcTriNormalize(const MESH_FACE& MeshFace)
{
	D3DXVECTOR3 FaceNormal;
	D3DXVECTOR3 VecA, VecB;
	VecA = MeshFace.p0 - MeshFace.p1;
	VecB = MeshFace.p0 - MeshFace.p2;
	D3DXVec3Cross(&FaceNormal,&VecA,&VecB);       //外積を計算
	D3DXVec3Normalize(&FaceNormal, &FaceNormal);  //単位ベクトル化
	return FaceNormal;
}

D3DXVECTOR3* CPolygonMath::CalcScreenToWorld
(
	D3DXVECTOR3* pOutWorld,
	int Sx/*スクリーンX座標*/,
	int Sy/*スクリーンY座標*/,
	float fZ/* 射影空間でのZ値（0～1）*/,
	int Screen_w, /*スクリーンの幅*/
	int Screen_h, /*スクリーンの高さ*/
	D3DXMATRIX* View,
	D3DXMATRIX* Prj
)
{
	// 各行列の逆行列を算出
	D3DXMATRIX InvView, InvPrj, VP, InvViewport;
	D3DXMatrixInverse(&InvView, nullptr, View);          //ビュー行列の逆行列
	D3DXMatrixInverse(&InvPrj, nullptr, Prj);            //プロジェクション行列の逆行列
	D3DXMatrixIdentity(&VP);
	VP._11 = Screen_w / 2.0f; VP._22 = -Screen_h / 2.0f;
	VP._41 = Screen_w / 2.0f; VP._42 = Screen_h / 2.0f;
	D3DXMatrixInverse(&InvViewport, nullptr, &VP);       //ビューポート行列の逆行列

	// 逆変換
	D3DXMATRIX tmp = InvViewport * InvPrj * InvView;  //逆行列の掛け算
	D3DXVECTOR3 MouseSceenPosition = D3DXVECTOR3((float)Sx, (float)Sy, fZ);
	D3DXVec3TransformCoord(pOutWorld, &MouseSceenPosition, &tmp);

	return pOutWorld;
}

bool CPolygonMath::CalcLineAndTripoly_InterSecPoint(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint, const MESH_FACE& MeshFace, D3DXVECTOR3* pOut)
{
	D3DXVECTOR3 IntersectPoint;    //交差点

	//線分と平面の判定
	D3DXVECTOR3 Start_P0_Vec = StartPoint - MeshFace.p0;
	D3DXVECTOR3 End_P0_Vec = EndPoint - MeshFace.p0;
	float fDotStart = D3DXVec3Dot(&Start_P0_Vec,&MeshFace.Normalize);
	float fDotEnd = D3DXVec3Dot(&End_P0_Vec, &MeshFace.Normalize);
	if (fDotStart*fDotEnd > 0) return false;                 //線分とポリゴンがいる平面衝突しない

	//線分と平面の交差点
	IntersectPoint = GetVecPlane_Intersect(StartPoint, EndPoint, MeshFace);

	//交差点がポリゴン面にいるかどうか
	bool InFace = Rects3DPointPlane(IntersectPoint, MeshFace);
	if(InFace == true)
	{
		*pOut = IntersectPoint;
		return true;
	}
	return false;
}

float CPolygonMath::DistancePointPlane(const D3DXVECTOR3 &Point, const MESH_FACE& Face)
{
	float fDistance;             //ポイントとポリゴンの距離
	D3DXVECTOR3 Vec = Point - Face.p0;             //頂点とポイントのベクトル
	fDistance = D3DXVec3Dot(&Face.Normalize, &Vec) / D3DXVec3Length(&Face.Normalize);      //法線とベクトルの内積 / 法線の長さ
	return fabsf(fDistance);
}

D3DXVECTOR3 CPolygonMath::GetVecPlane_Intersect(const D3DXVECTOR3& StartPoint,const D3DXVECTOR3& EndPoint, const MESH_FACE& Face)
{
	//
	//宣言部分
	///////////////////////////
	float dStart, dEnd;           //ベクトルの始点、終点と板ポリゴンの距離
	float s;      //内分比
	D3DXVECTOR3 VecStart, VecEnd;           //頂点と始点、終点成すベクトル
	D3DXVECTOR3 Intersection;        //貫通点
	D3DXVECTOR3 IntersectionVec;        //貫通点ベクトル

	//
	//計算部分
	///////////////////////////
	dStart = DistancePointPlane(StartPoint, Face);
	dEnd   = DistancePointPlane(EndPoint, Face);
	VecStart = StartPoint - Face.p0;
	VecEnd = EndPoint - Face.p0;
	s = dStart / (dStart + dEnd);    //内分比計算
	D3DXVec3Lerp(&IntersectionVec, &VecStart , &VecEnd, s);         //交差点とポリゴン頂点p0のベクトル
	Intersection = IntersectionVec + Face.p0;         //交差点算出

	return Intersection;
}

bool CPolygonMath::Rects3DPointPlane(const D3DXVECTOR3 &Point, const MESH_FACE& Face)
{
	D3DXVECTOR3 Vec01, Vec12, Vec20;
	D3DXVECTOR3 Vec0P, Vec1P, Vec2P;
	D3DXVECTOR3 n[3], Normarlize;

	//
	//計算部分
	////////////////////////
	//板ポリゴンの右手回りベクトル
	Vec01 = Face.p1 - Face.p0;
	Vec12 = Face.p2 - Face.p1;
	Vec20 = Face.p0 - Face.p2;

	//頂点とポイントのベクトル
	Vec0P = Point - Face.p0;
	Vec1P = Point - Face.p1;
	Vec2P = Point - Face.p2;

	//外積を求め
	D3DXVec3Cross(&n[0], &Vec01, &Vec1P);
	D3DXVec3Cross(&n[1], &Vec12, &Vec2P);
	D3DXVec3Cross(&n[2], &Vec20, &Vec0P);

	//精度調整
	for (int i = 0; i < 3; i++)
	{
		//単位ベクトル化
		D3DXVec3Normalize(&n[i], &n[i]);
	}

	float Dot[3];
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		Dot[nCnt] = D3DXVec3Dot(&n[nCnt], &Face.Normalize);             //結果法線とポリゴンの法線の内積を求め
	}

	//判定(4つ結果法線ベクトル、各とポリゴン法線の内積の結果、つまりcos()の値は無限に1に近いなら,判定成立)
	if (
		(Dot[0] > 0.99) &&
		(Dot[1] > 0.99) &&
		(Dot[2] > 0.99) 
		)
	{
		return true;
	}

	return false;
}

bool CPolygonMath::PointInXZMeshFace(const D3DXVECTOR3 &Point, const MESH_FACE& Face)
{
	D3DXVECTOR3 Vec01, Vec12, Vec20;
	D3DXVECTOR3 Vec0P, Vec1P, Vec2P;
	D3DXVECTOR3 n[3], Normarlize;

	//
	//計算部分
	////////////////////////
	//板ポリゴンの右手回りベクトル
	Vec01 = Face.p1 - Face.p0;
	Vec12 = Face.p2 - Face.p1;
	Vec20 = Face.p0 - Face.p2;

	//頂点とポイントのベクトル
	Vec0P = Point - Face.p0;
	Vec1P = Point - Face.p1;
	Vec2P = Point - Face.p2;

	//外積を求め
	D3DXVec3Cross(&n[0], &Vec01, &Vec1P);
	D3DXVec3Cross(&n[1], &Vec12, &Vec2P);
	D3DXVec3Cross(&n[2], &Vec20, &Vec0P);

	if (
		(n[0].y >= 0.0f) &&
		(n[1].y >= 0.0f) &&
		(n[2].y >= 0.0f)
		)
	{
		return true;
	}

	return false;
}

float CPolygonMath::PointElevationInFace(const D3DXVECTOR3 &Point, const MESH_FACE& Face)
{
	//数式 : (P - P0)・Normal = 0;
	float fElevation = 0;    //標高
	float DotP0FaceNormal = D3DXVec3Dot(&Face.p0,&Face.Normalize);     //面の頂点と面の法線の外積
	fElevation = (DotP0FaceNormal - Point.x*Face.Normalize.x - Point.z*Face.Normalize.z) / Face.Normalize.y;   //標高計算
	return fElevation;
}

void CPolygonMath::CalcPoseMtx(D3DXMATRIX *pOut, const D3DXVECTOR3 &FaceVec, const D3DXVECTOR3 &Up)
{
	D3DXVECTOR3 X, Y, Z;
	D3DXVec3Normalize(&Y,&Up);           //上方向を求める
	D3DXVec3Cross(&X,&Y,&FaceVec);       //X方向を求める
	D3DXVec3Normalize(&X, &X);
	D3DXVec3Cross(&Z, &X, &Y);           //プレーヤーの向きを求める
	D3DXVec3Normalize(&Z, &Z);

	//値を姿勢マトリクスに代入
	pOut->_11 = X.x; pOut->_12 = X.y; pOut->_13 = X.z; pOut->_14 = 0.0f;
	pOut->_21 = Y.x; pOut->_22 = Y.y; pOut->_23 = Y.z; pOut->_24 = 0.0f;
	pOut->_31 = Z.x; pOut->_32 = Z.y; pOut->_33 = Z.z; pOut->_34 = 0.0f;
	pOut->_41 = 0.0f; pOut->_42 = 0.0f; pOut->_43 = 0.0f; pOut->_44 = 1.0f;
}

void CPolygonMath::CalcMtxFromSRP(D3DXMATRIX *pOutMtx, const SRP& SRP)
{
	D3DXMATRIX mtxScale/*拡大行列*/, mtxRot/*回転行列*/, mtxMove/*移動行列*/;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, SRP.scl.x, SRP.scl.y, SRP.scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, SRP.rot.y/*y軸*/, SRP.rot.x/*x軸*/, SRP.rot.z/*z軸*/); //回転行列を設定
	D3DXMatrixTranslation(&mtxMove, SRP.pos.x, SRP.pos.y, SRP.pos.z);
	*pOutMtx = mtxScale*mtxRot*mtxMove;                     //結果の変換行列の計算
}

float CPolygonMath::Vec2AngleAndAngleDir(const D3DXVECTOR2& VecA, const D3DXVECTOR2& VecB)
{
	float Cross = VecA.x*VecB.y - VecA.y*VecB.x;	//両ベクトルの外積のスカラー値を求める
	float Dot = VecA.x * VecB.x + VecA.y * VecB.y;	//両ベクトルの内積のスカラー値を求める
	float DirAngle = atan2f(Cross, Dot);
	return DirAngle;
}

D3DXVECTOR2 CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(const D3DXVECTOR3& Camera, const D3DXVECTOR3& Lookat)
{
	D3DXVECTOR2 HoriVertiAngle;
	D3DXVECTOR3 CameraLookatVec = Camera - Lookat;
	float fDistance = D3DXVec3Length( &CameraLookatVec);

	float asinVertical = (Camera.y - Lookat.y) / fDistance;
	float fAngleVertical = asinf(asinVertical);
	float acosHorizon = (Camera.x - Lookat.x) / (fDistance * cosf(fAngleVertical));
	float asinHorizon = (Camera.z - Lookat.z) / (fDistance * cosf(fAngleVertical));
	float fAngleHorizon = atan2f(asinHorizon, acosHorizon);

	HoriVertiAngle = D3DXVECTOR2(fAngleHorizon, fAngleVertical);

	return HoriVertiAngle;
}

//引数説明 : 
//方向ベクトル出力
//方向ベクトル入力
//回転軸(単位か必要)
//回転角度
void CPolygonMath::DirVec3QuaternionRot(D3DXVECTOR3* pOut, const D3DXVECTOR3* pIn, const D3DXVECTOR3& RotAixs, float angle)
{
	//回転クォータニオンを求め
	D3DXQUATERNION Q1;
	D3DXQuaternionRotationAxis(&Q1, &RotAixs, angle);

	//回転後のベクトルの結果を求める
	D3DXMATRIX RotMtx;
	D3DXMatrixRotationQuaternion(&RotMtx, &Q1);
	D3DXVec3TransformCoord(pOut, pIn, &RotMtx);		//頂点変換
	D3DXVec3Normalize(pOut, pOut);					//単位化
}

//2D当り判定
bool CPolygonMath::HitAABB2D(const D3DXVECTOR2& APos, const D3DXVECTOR2& ASize, const D3DXVECTOR2& BPos, const D3DXVECTOR2& BSize)
{
	float ALeft = APos.x - ASize.x * 0.5f;
	float ARight = APos.x + ASize.x * 0.5f;
	float ATop = APos.y - ASize.y * 0.5f;
	float ABottom = APos.y + ASize.y * 0.5f;

	float BLeft = BPos.x - BSize.x * 0.5f;
	float BRight = BPos.x + BSize.x * 0.5f;
	float BTop = BPos.y - BSize.y * 0.5f;
	float BBottom = BPos.y + BSize.y * 0.5f;

	bool HitX = (ALeft < BRight) && (ARight > BLeft);
	bool HitY = (ATop < BBottom) && (ABottom > BTop);

	if (HitX && HitY) {
		return true;
	}
	return false;
}

bool CPolygonMath::ColSegments(
	Segment &seg1,			// 線分1
	Segment &seg2,			// 線分2
	float* outT1,			// 線分1の内分比（出力）
	float* outT2,			// 線分2の内分比（出力
	D3DXVECTOR2* outPos		// 交点（出力）
) {

	D3DXVECTOR2 v = seg2.s - seg1.s;
	float Crs_v1_v2 = D3DXVec2CCW(&seg1.v, &seg2.v);
	if (Crs_v1_v2 == 0.0f) {
		// 平行状態
		return false;
	}

	float Crs_v_v1 = D3DXVec2CCW(&v, &seg1.v);
	float Crs_v_v2 = D3DXVec2CCW(&v, &seg2.v);

	float t1 = Crs_v_v2 / Crs_v1_v2;
	float t2 = Crs_v_v1 / Crs_v1_v2;

	if (outT1)
		*outT1 = Crs_v_v2 / Crs_v1_v2;
	if (outT2)
		*outT2 = Crs_v_v1 / Crs_v1_v2;

	const float eps = 0.00001f;
	if (t1 + eps < 0 || t1 - eps > 1 || t2 + eps < 0 || t2 - eps > 1) {
		// 交差していない
		return false;
	}

	if (outPos)
		*outPos = seg1.s + seg1.v * t1;

	return true;
}