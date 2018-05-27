#include "CEnemyControl.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CMeshFieldManager.h"
#include "CPolygonMath.h"
#include "CEnemy.h"
#include "CPathPoint.h"
#include "CAIDogAction.h"

CEnemyControl::CEnemyControl() 
{
}

CEnemyControl::~CEnemyControl() 
{

}

//������
void CEnemyControl::Init(void)
{
	m_AI = nullptr;
	m_Player = nullptr;
}

//�v���[���[���R���g���[������
void CEnemyControl::EnemyControl(void)
{
	EnemyFaceDir();							//�����x�N�g���ɂ��p������
	EnemyHitAllMesh();						//���b�V���̓��蔻��
}


void CEnemyControl::SetEnemy(CEnemy *Enemy)
{
	if (nullptr == Enemy)
	{
		MessageBox(nullptr, "CEnemyControl�ɓn�����v���[���[�|�C���^��nullptr!", "�|�C���^�̒l��nullptr", MB_OK | MB_ICONHAND);
	}
	m_AI = Enemy;
}

void CEnemyControl::SetPlayer(CPlayer *Player)
{
	m_Player = Player;
}

void CEnemyControl::EnemyFaceDir(void)           //AI�̌�������|�[�Y���Z�o
{
	if (nullptr == m_AI) return;

	//�|�[�Y�����߂�
	D3DXMATRIX PoseMtx;
	D3DXVECTOR3 X, Y, Z;
	Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 EnemyDir = m_AI->GetFaceDir();             //���������擾

	//���������ɂ��|�[�Y�v�Z
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == EnemyDir) return;
	D3DXVec3Normalize(&Z, &EnemyDir);
	D3DXVec3Cross(&X, &Y, &Z);
	D3DXVec3Normalize(&X, &X);

	PoseMtx._11 = X.x; PoseMtx._12 = X.y; PoseMtx._13 = X.z; PoseMtx._14 = 0.0f;
	PoseMtx._21 = Y.x; PoseMtx._22 = Y.y; PoseMtx._23 = Y.z; PoseMtx._24 = 0.0f;
	PoseMtx._31 = Z.x; PoseMtx._32 = Z.y; PoseMtx._33 = Z.z; PoseMtx._34 = 0.0f;
	PoseMtx._41 = 0.0f; PoseMtx._42 = 0.0f; PoseMtx._43 = 0.0f; PoseMtx._44 = 1.0f;

	m_AI->SetPoseMtx(PoseMtx);
}

void CEnemyControl::EnemyHitAllMesh(void)       //���ł�
{
	if (nullptr == m_AI) {
		return;
	}

	D3DXVECTOR3 EnemyPos = m_AI->GetPos();     //�v���[���[�̈ʒu�擾
	CMeshField *pMesh = nullptr;

	for (int MeshNum = 0; MeshNum < MESH_MAX; MeshNum++)
	{
		pMesh = CMeshFieldManager::GetMesh(MeshNum);
		if (nullptr == pMesh) continue;

		const MESH_FACE *pFace = pMesh->GetMeshFace();       //�ʃf�[�^�擾
		D3DXVECTOR2 PieceNum = pMesh->GetPieceNum();         //�}�X���擾

															 //�ʂƃv���[���[�̓��蔻��
		for (int i = 0; i < PieceNum.y; i++)
		{
			for (int j = 0; j < PieceNum.x * 2; j++)
			{
				int k = i * ((int)PieceNum.x * 2) + j;
				if (nullptr == &pFace[k]) continue;

				//�v���[���[�͕��ʂ͈͓̔��ɂ��邩�ǂ���
				bool InMesh = CPolygonMath::PointInXZMeshFace(EnemyPos, pFace[k]);
				if (InMesh == false) continue;

				//���ʓ�����->�v���[���[��XZ���W��n��,���ʂ�Y���W�����߂�
				float fElevation = CPolygonMath::PointElevationInFace(EnemyPos, pFace[k]);

				//���ʂ�Y���W���v���[���[��Y���W�ɑ��
				if (m_AI->GetPos().y < fElevation) {
					m_AI->SetPosY(fElevation);
					m_AI->SetSpeedY(0);
				}
				/*
				//�p��������
				D3DXMATRIX PoseMtx;
				D3DXVECTOR3 EnemyDir = m_AI->GetEnemyMoveDir();
				if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == EnemyDir) continue;
				CPolygonMath::CalcPoseMtx(&PoseMtx, EnemyDir, pFace[k].Normalize);
				m_AI->SetPoseMtx(PoseMtx);
				*/
			}
		}
	}
}