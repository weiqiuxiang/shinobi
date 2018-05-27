#include "CMouseEditMeshField.h"
#include "CRenderer.h"
#include "MousePosition.h"
#include "CPolygonMath.h"
#include "CCameraManager.h"
#include "CMeshFieldManager.h"
#include "SceneTransition.h"

#define NOT_HIT_ANY_MESH_ID (-1)
#define ACTIVE_DISTANCE (10.0f)
#define DISTANCE_BIGGER (0.3f)

#define MOUSE_RANGE_MIN (1)
#define MOUSE_RANGE_MAX (100)
#define MOUSE_RANGE_CHANGE_FRAME (0.1f)

CMouseEditMeshField::CMouseEditMeshField()
{
	m_fRadius = 0;
	m_nHitMeshID = NOT_HIT_ANY_MESH_ID;
	m_StartPoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_EndPoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

HRESULT CMouseEditMeshField::Init( float fRadius, RGBA rgba)
{
	m_fRadius = fRadius;
	D3DXVECTOR3 RadiusVec3 = D3DXVECTOR3(fRadius, fRadius,0.0f);

	SRP NewWorld;
	NewWorld.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	NewWorld.rot = D3DXVECTOR3(D3DX_PI*0.5,0.0f,0.0f);
	NewWorld.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//�`�敔��������
	m_Cscene3D.Init(NewWorld, RadiusVec3, PARTICLE_BASE_TEX);
	m_Cscene3D.SetRGBA(rgba);
	m_Cscene3D.SetDrawType(CScene3D::TYPE_NORMAL);

	return S_OK;
}

void CMouseEditMeshField::Uninit(void)
{
	m_Cscene3D.Uninit();
}

void CMouseEditMeshField::CalcMousePositionInMesh(void)
{
	CalcLine(&m_StartPoint,&m_EndPoint);									//�X�N���[�����W�����[���h�����̋N�_�ƏI�_�ɕω�
	D3DXVECTOR3 MousePosIn3DWorld = HitAllMesh(m_StartPoint, m_EndPoint);	//�}�E�X�ƃ��b�V���̌����_������
	m_Cscene3D.SetPosition(MousePosIn3DWorld);								//�`��ʒu�ݒ�
	m_Cscene3D.Update();													//�`�敔���̍X�V
	ChangeRange();
}

void CMouseEditMeshField::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//���Z�L��
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_Cscene3D.Draw();

	//���Z����
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
}

void CMouseEditMeshField::CalcLine(D3DXVECTOR3* pStartPoint, D3DXVECTOR3* pEndPoint)
{
	D3DXVECTOR3 MousePos = CMousePosition::GetMousePos();
	D3DXMATRIX ViewMtx = CCameraManager::GetActiveCameraViewMtx();
	D3DXMATRIX ProjeMtx = CCameraManager::GetActiveCameraProjeMtx();

	CPolygonMath::CalcScreenToWorld				//�}�E�X���C�̎n�_�v�Z
	(
		pStartPoint, (int)MousePos.x, (int)MousePos.y,
		0.0f,SCREEN_WIDTH,SCREEN_HEIGHT,
		&ViewMtx, &ProjeMtx
		);

	CPolygonMath::CalcScreenToWorld				//�}�E�X���C�̏I�_�v�Z
	(
		pEndPoint, (int)MousePos.x, (int)MousePos.y,
		1.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
		&ViewMtx, &ProjeMtx
	);
}

void CMouseEditMeshField::EditHitFloor(void)
{
	if (NOT_HIT_ANY_MESH_ID == m_nHitMeshID) return;

	D3DXVECTOR3 MousePosInMesh = m_Cscene3D.GetPosition();
	CMeshField* pMesh = CMeshFieldManager::GetMesh(m_nHitMeshID);
	
	const MESH_VERTEX *pVertexData = pMesh->GetMeshVertex();
	int VertexNum = pMesh->GetVertexNum();             //���_���擾
	const double *pVertexHeight = pMesh->GetHighRelative();     //���ΕW���m��
	double *pVertexHeightEdit = new double[VertexNum];
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_LSHIFT))
	{
		if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT))
		{
			//���_�f�[�^�擾
			for (int i = 0; i < VertexNum; i++)
			{
				pVertexHeightEdit[i] = pVertexHeight[i];
				D3DXVECTOR3 DistanceVec = MousePosInMesh - pVertexData[i].pos;
				float nDistance = D3DXVec3Length(&DistanceVec);
				if (nDistance < m_fRadius)
				{
					pVertexHeightEdit[i] += DISTANCE_BIGGER;
				}
			}
			pMesh->LoadRelativeHigh(pVertexHeightEdit);         //���b�V�����X�V
		}

		if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_RIGHT))
		{
			//���_�f�[�^�擾
			for (int i = 0; i < VertexNum; i++)
			{
				pVertexHeightEdit[i] = pVertexHeight[i];
				D3DXVECTOR3 DistanceVec = MousePosInMesh - pVertexData[i].pos;
				float nDistance = D3DXVec3Length(&DistanceVec);
				if (nDistance < ACTIVE_DISTANCE)
				{
					pVertexHeightEdit[i] -= DISTANCE_BIGGER * nDistance / ACTIVE_DISTANCE;
				}
			}
			pMesh->LoadRelativeHigh(pVertexHeightEdit);         //���b�V�����X�V
		}
	}

	m_nHitMeshID = NOT_HIT_ANY_MESH_ID;
	delete[] pVertexHeightEdit;
	pVertexHeightEdit = nullptr;
}

D3DXVECTOR3 CMouseEditMeshField::HitAllMesh(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3 &EndPoint)
{
	D3DXVECTOR3 MousePosIn3DWorld = EndPoint;
	for (int i = 0; i < MESH_MAX; i++)
	{
		CMeshField *pMesh = CMeshFieldManager::GetMesh(i);
		if (nullptr == pMesh) continue;
		D3DXVECTOR3 MousePosIn3DWorldNew = HitOneMesh(pMesh, StartPoint, EndPoint);  //���肵�Ă���|���S���̃}�E�X�ʒu
		
		//�������
		D3DXVECTOR3 nDistanceNow = MousePosIn3DWorld - StartPoint;
		D3DXVECTOR3 nDistanceNew = MousePosIn3DWorldNew - StartPoint;
		if (D3DXVec3Length(&nDistanceNow) > D3DXVec3Length(&nDistanceNew)) {
			MousePosIn3DWorld = MousePosIn3DWorldNew;     //��O�̕������
			m_nHitMeshID = i;
		}
	}
	return MousePosIn3DWorld;
}

D3DXVECTOR3 CMouseEditMeshField::HitOneMesh(CMeshField *pMesh, const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint)
{
	D3DXVECTOR3 MousePosIn3DWorld = EndPoint;
	const MESH_FACE *pFace = pMesh->GetMeshFace();
	D3DXVECTOR2 PieceNum = pMesh->GetPieceNum();
	
	for (int i = 0; i < PieceNum.y; i++)
	{
		for (int j = 0; j < PieceNum.x * 2; j++)
		{
			int k = i * ((int)PieceNum.x * 2) + j;
			if (nullptr == &pFace[k]) continue;

			//�}�E�X�����ƈ�|���S���̌����_
			D3DXVECTOR3 MousePosIn3DWorldNew = EndPoint;
			bool bHitCheck = CPolygonMath::CalcLineAndTripoly_InterSecPoint(StartPoint, EndPoint,pFace[k], &MousePosIn3DWorldNew);
			if (bHitCheck == false) continue;

			//�������
			D3DXVECTOR3 nDistanceNow = MousePosIn3DWorld - StartPoint;
			D3DXVECTOR3 nDistanceNew = MousePosIn3DWorldNew - StartPoint;
			if (D3DXVec3Length(&nDistanceNow) > D3DXVec3Length(&nDistanceNew)) {
				MousePosIn3DWorld = MousePosIn3DWorldNew;     //��O�̕������
			}
		}
	}

	return MousePosIn3DWorld;
}

void CMouseEditMeshField::ChangeRange(void)
{
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_ADD))
	{
		m_fRadius += MOUSE_RANGE_CHANGE_FRAME;
		m_fRadius = min(m_fRadius, MOUSE_RANGE_MAX);
		D3DXVECTOR3 RadiusVec3 = D3DXVECTOR3(m_fRadius, m_fRadius, 0.0f);
		m_Cscene3D.SetSize(RadiusVec3);
	}

	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_SUBTRACT))
	{
		m_fRadius -= MOUSE_RANGE_CHANGE_FRAME;
		m_fRadius = max(MOUSE_RANGE_MIN, m_fRadius);
		D3DXVECTOR3 RadiusVec3 = D3DXVECTOR3(m_fRadius, m_fRadius, 0.0f);
		m_Cscene3D.SetSize(RadiusVec3);
	}
}