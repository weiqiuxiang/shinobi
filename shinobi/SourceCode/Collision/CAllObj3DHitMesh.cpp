#include "CAllObj3DHitMesh.h"
#include "CScene3DStorage.h"
#include "CModelStorage.h"
#include "CMeshField.h"
#include "CMeshFieldManager.h"
#include "CPolygonMath.h"

void CAllObj3DHitMesh::CScene3DHitAllMesh(CScene3D* pCScene3D)
{
	if (nullptr == pCScene3D) return;

	D3DXVECTOR3 CScene3DPos = pCScene3D->GetPosition();     //�v���[���[�̈ʒu�擾
	CMeshField *pMesh = nullptr;

	for (int MeshNum = 0; MeshNum < MESH_MAX; MeshNum++)
	{
		pMesh = CMeshFieldManager::GetMesh(MeshNum);
		if (nullptr == pMesh) {
			continue;
		}

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
				bool InMesh = CPolygonMath::PointInXZMeshFace(CScene3DPos, pFace[k]);
				if (InMesh == false) continue;

				//���ʓ�����->�v���[���[��XZ���W��n��,���ʂ�Y���W�����߂�
				float fElevation = CPolygonMath::PointElevationInFace(CScene3DPos, pFace[k]);

				//���ʂ�Y���W���v���[���[��Y���W�ɑ��
				pCScene3D->SetPosition(D3DXVECTOR3(CScene3DPos.x, fElevation, CScene3DPos.z));
			}
		}
	}
}

void CAllObj3DHitMesh::ModelHitAllMesh(CModel* pModel)
{
	if (nullptr == pModel) {
		return;
	}
	if (pModel->GetHitMeshFlag() == false) {
		return;
	}

	D3DXVECTOR3 ModelPos = pModel->GetPosition();     //�v���[���[�̈ʒu�擾
	CMeshField *pMesh = nullptr;

	for (int MeshNum = 0; MeshNum < MESH_MAX; MeshNum++)
	{
		pMesh = CMeshFieldManager::GetMesh(MeshNum);
		if (nullptr == pMesh) {
			continue;
		}

		const MESH_FACE *pFace = pMesh->GetMeshFace();			//�ʃf�[�^�擾
		D3DXVECTOR2 PieceNum = pMesh->GetPieceNum();			//�}�X���擾

		//�ʂƃv���[���[�̓��蔻��
		for (int i = 0; i < PieceNum.y; i++)
		{
			for (int j = 0; j < PieceNum.x * 2; j++)
			{
				int k = i * ((int)PieceNum.x * 2) + j;
				if (nullptr == &pFace[k]) continue;

				//�v���[���[�͕��ʂ͈͓̔��ɂ��邩�ǂ���
				bool InMesh = CPolygonMath::PointInXZMeshFace(ModelPos, pFace[k]);
				if (InMesh == false) continue;

				//���ʓ�����->�v���[���[��XZ���W��n��,���ʂ�Y���W�����߂�
				float fElevation = CPolygonMath::PointElevationInFace(ModelPos, pFace[k]);

				//���ʂ�Y���W���v���[���[��Y���W�ɑ��
				pModel->SetPosition(D3DXVECTOR3(ModelPos.x, fElevation, ModelPos.z));

				//�p����������
				D3DXMATRIX PoseMtx;
				D3DXVECTOR3 ModelDirXZ = pModel->GetDirVecXZ();							//�����擾
				if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == ModelDirXZ) continue;
				CPolygonMath::CalcPoseMtx(&PoseMtx, ModelDirXZ, pFace[k].Normalize);
				pModel->SetPoseMtx(PoseMtx);
			}
		}
	}
}

void CAllObj3DHitMesh::ColliAllObj3DAndAllMesh(void)
{
	//CScene3D�I�u�W�F�N�g
	for (int i = 0; i < CSCENE3D_OBJ_MAX; i++)
	{
		//�|�C���^�擾
		CScene3D *pScene3D = CScene3DStorage::GetObj(i);
		if (nullptr == pScene3D) continue;
		CScene3DHitAllMesh(pScene3D);
	}

	//Model�I�u�W�F�N�g
	for (int i = 0; i < MODEL_OBJ_MAX; i++)
	{
		//�|�C���^�擾
		CModel *pModel = CModelStorage::GetObj(i);
		if (nullptr == pModel) continue;
		ModelHitAllMesh(pModel);
	}
}