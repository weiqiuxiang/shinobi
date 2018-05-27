#include "CPolygonMath.h"
#include "CRenderer.h"

D3DXVECTOR3 CPolygonMath::CalcTriNormalize(const MESH_FACE& MeshFace)
{
	D3DXVECTOR3 FaceNormal;
	D3DXVECTOR3 VecA, VecB;
	VecA = MeshFace.p0 - MeshFace.p1;
	VecB = MeshFace.p0 - MeshFace.p2;
	D3DXVec3Cross(&FaceNormal,&VecA,&VecB);       //�O�ς��v�Z
	D3DXVec3Normalize(&FaceNormal, &FaceNormal);  //�P�ʃx�N�g����
	return FaceNormal;
}

D3DXVECTOR3* CPolygonMath::CalcScreenToWorld
(
	D3DXVECTOR3* pOutWorld,
	int Sx/*�X�N���[��X���W*/,
	int Sy/*�X�N���[��Y���W*/,
	float fZ/* �ˉe��Ԃł�Z�l�i0�`1�j*/,
	int Screen_w, /*�X�N���[���̕�*/
	int Screen_h, /*�X�N���[���̍���*/
	D3DXMATRIX* View,
	D3DXMATRIX* Prj
)
{
	// �e�s��̋t�s����Z�o
	D3DXMATRIX InvView, InvPrj, VP, InvViewport;
	D3DXMatrixInverse(&InvView, nullptr, View);          //�r���[�s��̋t�s��
	D3DXMatrixInverse(&InvPrj, nullptr, Prj);            //�v���W�F�N�V�����s��̋t�s��
	D3DXMatrixIdentity(&VP);
	VP._11 = Screen_w / 2.0f; VP._22 = -Screen_h / 2.0f;
	VP._41 = Screen_w / 2.0f; VP._42 = Screen_h / 2.0f;
	D3DXMatrixInverse(&InvViewport, nullptr, &VP);       //�r���[�|�[�g�s��̋t�s��

	// �t�ϊ�
	D3DXMATRIX tmp = InvViewport * InvPrj * InvView;  //�t�s��̊|���Z
	D3DXVECTOR3 MouseSceenPosition = D3DXVECTOR3((float)Sx, (float)Sy, fZ);
	D3DXVec3TransformCoord(pOutWorld, &MouseSceenPosition, &tmp);

	return pOutWorld;
}

bool CPolygonMath::CalcLineAndTripoly_InterSecPoint(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint, const MESH_FACE& MeshFace, D3DXVECTOR3* pOut)
{
	D3DXVECTOR3 IntersectPoint;    //�����_

	//�����ƕ��ʂ̔���
	D3DXVECTOR3 Start_P0_Vec = StartPoint - MeshFace.p0;
	D3DXVECTOR3 End_P0_Vec = EndPoint - MeshFace.p0;
	float fDotStart = D3DXVec3Dot(&Start_P0_Vec,&MeshFace.Normalize);
	float fDotEnd = D3DXVec3Dot(&End_P0_Vec, &MeshFace.Normalize);
	if (fDotStart*fDotEnd > 0) return false;                 //�����ƃ|���S�������镽�ʏՓ˂��Ȃ�

	//�����ƕ��ʂ̌����_
	IntersectPoint = GetVecPlane_Intersect(StartPoint, EndPoint, MeshFace);

	//�����_���|���S���ʂɂ��邩�ǂ���
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
	float fDistance;             //�|�C���g�ƃ|���S���̋���
	D3DXVECTOR3 Vec = Point - Face.p0;             //���_�ƃ|�C���g�̃x�N�g��
	fDistance = D3DXVec3Dot(&Face.Normalize, &Vec) / D3DXVec3Length(&Face.Normalize);      //�@���ƃx�N�g���̓��� / �@���̒���
	return fabsf(fDistance);
}

D3DXVECTOR3 CPolygonMath::GetVecPlane_Intersect(const D3DXVECTOR3& StartPoint,const D3DXVECTOR3& EndPoint, const MESH_FACE& Face)
{
	//
	//�錾����
	///////////////////////////
	float dStart, dEnd;           //�x�N�g���̎n�_�A�I�_�Ɣ|���S���̋���
	float s;      //������
	D3DXVECTOR3 VecStart, VecEnd;           //���_�Ǝn�_�A�I�_�����x�N�g��
	D3DXVECTOR3 Intersection;        //�ђʓ_
	D3DXVECTOR3 IntersectionVec;        //�ђʓ_�x�N�g��

	//
	//�v�Z����
	///////////////////////////
	dStart = DistancePointPlane(StartPoint, Face);
	dEnd   = DistancePointPlane(EndPoint, Face);
	VecStart = StartPoint - Face.p0;
	VecEnd = EndPoint - Face.p0;
	s = dStart / (dStart + dEnd);    //������v�Z
	D3DXVec3Lerp(&IntersectionVec, &VecStart , &VecEnd, s);         //�����_�ƃ|���S�����_p0�̃x�N�g��
	Intersection = IntersectionVec + Face.p0;         //�����_�Z�o

	return Intersection;
}

bool CPolygonMath::Rects3DPointPlane(const D3DXVECTOR3 &Point, const MESH_FACE& Face)
{
	D3DXVECTOR3 Vec01, Vec12, Vec20;
	D3DXVECTOR3 Vec0P, Vec1P, Vec2P;
	D3DXVECTOR3 n[3], Normarlize;

	//
	//�v�Z����
	////////////////////////
	//�|���S���̉E����x�N�g��
	Vec01 = Face.p1 - Face.p0;
	Vec12 = Face.p2 - Face.p1;
	Vec20 = Face.p0 - Face.p2;

	//���_�ƃ|�C���g�̃x�N�g��
	Vec0P = Point - Face.p0;
	Vec1P = Point - Face.p1;
	Vec2P = Point - Face.p2;

	//�O�ς�����
	D3DXVec3Cross(&n[0], &Vec01, &Vec1P);
	D3DXVec3Cross(&n[1], &Vec12, &Vec2P);
	D3DXVec3Cross(&n[2], &Vec20, &Vec0P);

	//���x����
	for (int i = 0; i < 3; i++)
	{
		//�P�ʃx�N�g����
		D3DXVec3Normalize(&n[i], &n[i]);
	}

	float Dot[3];
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		Dot[nCnt] = D3DXVec3Dot(&n[nCnt], &Face.Normalize);             //���ʖ@���ƃ|���S���̖@���̓��ς�����
	}

	//����(4���ʖ@���x�N�g���A�e�ƃ|���S���@���̓��ς̌��ʁA�܂�cos()�̒l�͖�����1�ɋ߂��Ȃ�,���萬��)
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
	//�v�Z����
	////////////////////////
	//�|���S���̉E����x�N�g��
	Vec01 = Face.p1 - Face.p0;
	Vec12 = Face.p2 - Face.p1;
	Vec20 = Face.p0 - Face.p2;

	//���_�ƃ|�C���g�̃x�N�g��
	Vec0P = Point - Face.p0;
	Vec1P = Point - Face.p1;
	Vec2P = Point - Face.p2;

	//�O�ς�����
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
	//���� : (P - P0)�ENormal = 0;
	float fElevation = 0;    //�W��
	float DotP0FaceNormal = D3DXVec3Dot(&Face.p0,&Face.Normalize);     //�ʂ̒��_�Ɩʂ̖@���̊O��
	fElevation = (DotP0FaceNormal - Point.x*Face.Normalize.x - Point.z*Face.Normalize.z) / Face.Normalize.y;   //�W���v�Z
	return fElevation;
}

void CPolygonMath::CalcPoseMtx(D3DXMATRIX *pOut, const D3DXVECTOR3 &FaceVec, const D3DXVECTOR3 &Up)
{
	D3DXVECTOR3 X, Y, Z;
	D3DXVec3Normalize(&Y,&Up);           //����������߂�
	D3DXVec3Cross(&X,&Y,&FaceVec);       //X���������߂�
	D3DXVec3Normalize(&X, &X);
	D3DXVec3Cross(&Z, &X, &Y);           //�v���[���[�̌��������߂�
	D3DXVec3Normalize(&Z, &Z);

	//�l���p���}�g���N�X�ɑ��
	pOut->_11 = X.x; pOut->_12 = X.y; pOut->_13 = X.z; pOut->_14 = 0.0f;
	pOut->_21 = Y.x; pOut->_22 = Y.y; pOut->_23 = Y.z; pOut->_24 = 0.0f;
	pOut->_31 = Z.x; pOut->_32 = Z.y; pOut->_33 = Z.z; pOut->_34 = 0.0f;
	pOut->_41 = 0.0f; pOut->_42 = 0.0f; pOut->_43 = 0.0f; pOut->_44 = 1.0f;
}

void CPolygonMath::CalcMtxFromSRP(D3DXMATRIX *pOutMtx, const SRP& SRP)
{
	D3DXMATRIX mtxScale/*�g��s��*/, mtxRot/*��]�s��*/, mtxMove/*�ړ��s��*/;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, SRP.scl.x, SRP.scl.y, SRP.scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, SRP.rot.y/*y��*/, SRP.rot.x/*x��*/, SRP.rot.z/*z��*/); //��]�s���ݒ�
	D3DXMatrixTranslation(&mtxMove, SRP.pos.x, SRP.pos.y, SRP.pos.z);
	*pOutMtx = mtxScale*mtxRot*mtxMove;                     //���ʂ̕ϊ��s��̌v�Z
}

float CPolygonMath::Vec2AngleAndAngleDir(const D3DXVECTOR2& VecA, const D3DXVECTOR2& VecB)
{
	float Cross = VecA.x*VecB.y - VecA.y*VecB.x;	//���x�N�g���̊O�ς̃X�J���[�l�����߂�
	float Dot = VecA.x * VecB.x + VecA.y * VecB.y;	//���x�N�g���̓��ς̃X�J���[�l�����߂�
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

//�������� : 
//�����x�N�g���o��
//�����x�N�g������
//��]��(�P�ʂ��K�v)
//��]�p�x
void CPolygonMath::DirVec3QuaternionRot(D3DXVECTOR3* pOut, const D3DXVECTOR3* pIn, const D3DXVECTOR3& RotAixs, float angle)
{
	//��]�N�H�[�^�j�I��������
	D3DXQUATERNION Q1;
	D3DXQuaternionRotationAxis(&Q1, &RotAixs, angle);

	//��]��̃x�N�g���̌��ʂ����߂�
	D3DXMATRIX RotMtx;
	D3DXMatrixRotationQuaternion(&RotMtx, &Q1);
	D3DXVec3TransformCoord(pOut, pIn, &RotMtx);		//���_�ϊ�
	D3DXVec3Normalize(pOut, pOut);					//�P�ʉ�
}

//2D���蔻��
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
	Segment &seg1,			// ����1
	Segment &seg2,			// ����2
	float* outT1,			// ����1�̓�����i�o�́j
	float* outT2,			// ����2�̓�����i�o��
	D3DXVECTOR2* outPos		// ��_�i�o�́j
) {

	D3DXVECTOR2 v = seg2.s - seg1.s;
	float Crs_v1_v2 = D3DXVec2CCW(&seg1.v, &seg2.v);
	if (Crs_v1_v2 == 0.0f) {
		// ���s���
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
		// �������Ă��Ȃ�
		return false;
	}

	if (outPos)
		*outPos = seg1.s + seg1.v * t1;

	return true;
}