#pragma once
#ifndef _CPOLYGON_MATH_H_
#define _CPOLYGON_MATH_H_

#include "main.h"

//�O���錾
class CModel;

struct Segment {
	D3DXVECTOR2 s; // �n�_
	D3DXVECTOR2 v; // �����x�N�g���i�����̒������S���̂Ő��K�����Ȃ��悤�ɁI�j
};

//�N���X��`
class CPolygonMath
{
public:
	static D3DXVECTOR3 CalcTriNormalize(const MESH_FACE& MeshFace); //�O�p�|���S���̖ʖ@���v�Z(�ėp)
	static bool CalcLineAndTripoly_InterSecPoint(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint,const MESH_FACE& MeshFace, D3DXVECTOR3* pOut);//�����ƕ��ʂ̌����_�����߂Č����_�����ʏ�ɂ��邩�ǂ���(��p)
	static D3DXVECTOR3 GetVecPlane_Intersect(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint, const MESH_FACE& Face);  //�����ƕ��ʂ̌����_(�ėp)
	static float DistancePointPlane(const D3DXVECTOR3 &Point, const MESH_FACE& Face);          //�_�ƕ��ʂ̋���(�ėp)
	static bool Rects3DPointPlane(const D3DXVECTOR3 &Point, const MESH_FACE& Face);            //�_�͕��ʏ�ɂ��邩�ǂ���(�ėp)
	static bool PointInXZMeshFace(const D3DXVECTOR3 &Point, const MESH_FACE& Face);              //�_���O�p�|���S���̒��ɂ��邩�ǂ���(Y��������)(�ėp)
	static float PointElevationInFace(const D3DXVECTOR3 &Point, const MESH_FACE& Face);                 //�_���|���S�������镽�ʂ̕W��
	static void CalcPoseMtx(D3DXMATRIX *pOut/*�o��*/,const D3DXVECTOR3 &FaceVec/*�L��������*/, const D3DXVECTOR3 &Up/*�L���������*/); /*�p���}�g���N�X�����߂�*/
	static D3DXVECTOR3* CalcScreenToWorld    //�X�N���[�����W���烏�[���h���W�ɕϊ�
	(
		D3DXVECTOR3* pOutWorld,
		int Sx/*�X�N���[��X���W*/, 
		int Sy/*�X�N���[��Y���W*/, 
		float fZ/* �ˉe��Ԃł�Z�l�i0�`1�j*/, 
		int Screen_w, 
		int Screen_h, 
		D3DXMATRIX* View,
		D3DXMATRIX* Prj); 
	static void CalcMtxFromSRP(D3DXMATRIX *pOutMtx,const SRP& SRP);
	static float Vec2AngleAndAngleDir(const D3DXVECTOR2& VecA, const D3DXVECTOR2& VecB);										//Vec2�̊p�x�Ɗp�x�̕��������߂�֐�
	static D3DXVECTOR2 CalcTwoPoint_HorizonAngleAndVerticleAngle(const D3DXVECTOR3& Camera, const D3DXVECTOR3& Lookat);
	static void DirVec3QuaternionRot(D3DXVECTOR3* pOut/*�����x�N�g���o��*/, const D3DXVECTOR3* pIn/*�����x�N�g������*/, const D3DXVECTOR3& RotAixs/*��]��*/,float angle/*��]�p�x*/);		//�����x�N�g���̉�](�N�H�[�^�j�I�����g�p)(�ėp)
	static bool HitAABB2D(const D3DXVECTOR2& APos, const D3DXVECTOR2& ASize, const D3DXVECTOR2& BPos, const D3DXVECTOR2& BSize);

	//�񎟌������Ɛ����̓��蔻������߂�
	static bool CPolygonMath::ColSegments(
		Segment &seg1,          // ����1
		Segment &seg2,          // ����2
		float* outT1 = 0,       // ����1�̓�����i�o�́j
		float* outT2 = 0,       // ����2�̓�����i�o��
		D3DXVECTOR2* outPos = 0 // ��_�i�o�́j
	);
};

#endif