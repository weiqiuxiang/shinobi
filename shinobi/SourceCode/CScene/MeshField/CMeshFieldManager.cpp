#include "CMeshFieldManager.h"
#include "CPolygonMath.h"

CMeshField *CMeshFieldManager::m_MeshField[MESH_MAX] = { nullptr };

void CMeshFieldManager::InitAll(void)
{
	for (int i = 0; i < MESH_MAX; i++)
	{
		if (m_MeshField[i] != nullptr)
		{
			delete m_MeshField[i];
			m_MeshField[i] = nullptr;
		}
		m_MeshField[i] = nullptr;
	}
}

void CMeshFieldManager::UninitAll(void)
{
	for (int i = 0; i < MESH_MAX; i++)
	{
		if (m_MeshField[i] != nullptr)
		{
			delete m_MeshField[i];
			m_MeshField[i] = nullptr;
		}
	}
}

CMeshField* CMeshFieldManager::CreateMeshField(const D3DXVECTOR3& SclRotPos, const CINDEX_SIZE& IndexSize, const char *TexFilePass)
{
	for(int i = 0; i < MESH_MAX; i++)
	{
		if(m_MeshField[i] == nullptr)
		{
			m_MeshField[i] = new CMeshField();
			m_MeshField[i]->Init(SclRotPos, IndexSize, TexFilePass);
			return m_MeshField[i];
		}
	}

	MessageBox(nullptr,"�g�p�\�ȃ��b�V��ID�͑��݂��Ȃ�,�g�p�\��ID�̍ő吔�𑝂₵�Ă�������","�G���[���\�b�h : CreateMeshField",MB_OK | MB_ICONHAND);
	return nullptr;
}

float CMeshFieldManager::CalcPosYValueInMeshField(const D3DXVECTOR3& pos)
{
	for (int MeshNum = 0; MeshNum < MESH_MAX; MeshNum++)
	{
		CMeshField* pMesh = CMeshFieldManager::GetMesh(MeshNum);
		if (nullptr == pMesh) continue;

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
				bool InMesh = CPolygonMath::PointInXZMeshFace(pos, pFace[k]);
				if (InMesh == false) continue;

				//���ʓ�����->�v���[���[��XZ���W��n��,���ʂ�Y���W�����߂�
				float fElevation = CPolygonMath::PointElevationInFace(pos, pFace[k]);

				//���ʂ�Y���W��Ԃ�
				return fElevation;
			}
		}
	}

	return pos.y;
}

void CMeshFieldManager::UpdateAll(void)
{
	for (int i = 0; i < MESH_MAX; i++)
	{
		if (m_MeshField[i] != nullptr)
		{
			m_MeshField[i]->Update();
		}
	}
}

void CMeshFieldManager::DrawAll(void)
{
	for (int i = 0; i < MESH_MAX; i++)
	{
		if (m_MeshField[i] != nullptr)
		{
			m_MeshField[i]->Draw();
		}
	}
}

void CMeshFieldManager::DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	for (int i = 0; i < MESH_MAX; i++)
	{
		if (m_MeshField[i] != nullptr)
		{
			m_MeshField[i]->DrawOnDepthMap(LightView, LightProje);
		}
	}
}

void CMeshFieldManager::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje,LPDIRECT3DTEXTURE9 DepthTexture)
{
	for (int i = 0; i < MESH_MAX; i++)
	{
		if (m_MeshField[i] != nullptr)
		{
			m_MeshField[i]->DrawUseDepthMap(LightView, LightProje,DepthTexture);
		}
	}
}

CMeshField* CMeshFieldManager::GetMesh(int nMeshNumber)
{
	bool OverRange = (nMeshNumber < 0) && (nMeshNumber > MESH_MAX);
	if (OverRange == true) return nullptr;

	return m_MeshField[nMeshNumber];
}