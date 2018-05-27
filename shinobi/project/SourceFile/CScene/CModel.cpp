#include "CModel.h"
#include "ModelManager.h"
#include "TexManager.h"
#include "CRenderer.h"

const D3DVERTEXELEMENT9 g_ConstElements[] = {
	{ 0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0 },
	D3DDECL_END()
};

CModel::CModel()
{
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	D3DXMatrixIdentity(&m_PoseMtx);				//�p���}�g���N�X������
	D3DXMatrixIdentity(&m_Result);				//���ʃ}�g���N�X������
	m_ModelID = -1;
	m_DrawType = DRAW_USE_MTRL_DEFALUT;			//�`������w��
	m_IsHitMesh = false;
	m_IsAlwaysCalcAABB = false;
	m_AABBMinXMaxX = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinYMaxY = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinZMaxZ = D3DXVECTOR2(0.0f, 0.0f);
	m_UseID = -1;
}

CModel::~CModel()
{
	
}

HRESULT CModel::Init(const SRP &PosRotScl, const char* filePass)
{
	m_ModelID = CModelManager::LoadModel(filePass); //���f���f�[�^�̓ǂݍ���
	m_UseID = -1;

	//�f�[�^������
	m_pos = PosRotScl.pos;
	m_rot = PosRotScl.rot;
	m_scl = PosRotScl.scl;

	D3DXMatrixIdentity(&m_PoseMtx);

	m_DirVecXZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_IsHitMesh = false;

	//AABB�����̏�����
	m_IsAlwaysCalcAABB = false;
	m_AABBMinXMaxX = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinYMaxY = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinZMaxZ = D3DXVECTOR2(0.0f, 0.0f);
	CalcAABB();			//AABB�̌v�Z���s��

	float X = fabsf(m_AABBMinXMaxX.x - m_AABBMinXMaxX.y);
	float Y = fabsf(m_AABBMinYMaxY.x - m_AABBMinYMaxY.y);
	float Z = fabsf(m_AABBMinZMaxZ.x - m_AABBMinZMaxZ.y);
	m_Cube.Init(X,Y,Z);

	return S_OK;
}

void CModel::Update(void)
{
	//��]�p�x��XZ���ʂ̕����x�N�g�����v�Z����
	D3DXVECTOR3 DirXZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 RotY = D3DXVECTOR3(0.0f, m_rot.y,0.0f);
	D3DXMATRIX RotMtx;
	D3DXMatrixRotationYawPitchRoll(&RotMtx, RotY.y, RotY.x, RotY.z);
	D3DXVec3TransformCoord(&m_DirVecXZ,&DirXZ,&RotMtx);

	if (m_IsAlwaysCalcAABB) {
		//CalcAABB();
	}

	m_Cube.SetPos(m_pos);
}

void CModel::Draw(void)
{
	if (m_DrawType == DRAW_USE_MTRL_DEFALUT) {
		DrawDefault();
	}
	else if (m_DrawType == DRAW_USE_MTRL_CUSTOMIZE) {
		DrawMtrl();
	}
}

void CModel::DrawAABBTest(void)
{
	m_Cube.Draw();
}

void CModel::DrawOnDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	CModelManager::DrawModelOnDepthMap(m_ModelID, world, LightView, LightProje);
}

void CModel::DrawUseDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	CModelManager::DrawUseDepthMap(m_ModelID, world, LightView, LightProje, DepthTexture);
}

void CModel::DrawSpecular(const D3DXMATRIX& world)
{
	CModelManager::DrawSpecular(m_ModelID, world);
}

void CModel::DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture)
{
	CModelManager::DrawMotionBlur(m_ModelID, worldMtx, worldMtxOld, OriginalTexture);
}

void CModel::DrawDefault(void)
{
	CModelManager::DrawModel(m_ModelID);				//���f���`��
}

void CModel::DrawMtrl(void)
{
	CModelManager::DrawModelUseMtrl(m_ModelID, m_Mtrl);	//���f���`��
}

//�}�e���A�������g�p���ĕ`�悷��
void CModel::DrawUseMtrl(const D3DMATERIAL9& Mtrl)
{
	CModelManager::DrawModelUseMtrl(m_ModelID, Mtrl);	//���f���`��
}

void CModel::Uninit(void)
{

}

D3DXMATRIX CModel::GetResultMtx(void) const
{
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;

	return mtxResult;
}

D3DXMATRIX CModel::GetWorldPoseMtx(void) const
{
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*m_PoseMtx*mtxMove;

	return mtxResult;
}

/*==============================================================================
	�֐����� : AABB�̌v�Z������
===============================================================================*/
void CModel::CalcAABB(void)
{
	//AABB�p���b�V�������
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	LPD3DXMESH AABBMesh = nullptr;
	LPD3DXMESH pMesh = CModelManager::GetMesh(m_ModelID);
	if (pMesh != nullptr) {
		HRESULT hr = pMesh->CloneMesh(D3DXMESH_MANAGED, g_ConstElements, pDevice, &AABBMesh);
		if (FAILED(hr)) {
			MessageBox(nullptr, "���b�V���̃N���[���͎��s����", "[CModel.cpp][Init]", MB_ICONHAND);
			return;
		}
	}

	//���_�\���̂̏��擾
	const DWORD NumVertices = AABBMesh->GetNumVertices();	//���_�̐����擾
	DWORD FVF = AABBMesh->GetFVF();						//���_�t�H�[�}�b�g���擾
	const DWORD VertexSize = D3DXGetFVFVertexSize(FVF);		//���_�̃T�C�Y���擾

	//���_���擾
	BYTE* p_vertex = nullptr;
	AABBMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&p_vertex);

	//��ڂ̒��_���W
	D3DXVECTOR3 VertexPos = *(D3DXVECTOR3*)(p_vertex);

	//�f�[�^���Z�b�g
	m_AABBMinXMaxX = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinYMaxY = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinZMaxZ = D3DXVECTOR2(0.0f, 0.0f);

	//1���_�ڂ���
	if (VertexPos.x < 0) {
		m_AABBMinXMaxX.x = VertexPos.x;
	}
	else {
		m_AABBMinXMaxX.y = VertexPos.x;
	}
	if (VertexPos.y < 0) {
		m_AABBMinYMaxY.x = VertexPos.y;
	}
	else {
		m_AABBMinYMaxY.y = VertexPos.y;
	}
	if (VertexPos.z < 0) {
		m_AABBMinZMaxZ.x = VertexPos.z;
	}
	else {
		m_AABBMinZMaxZ.y = VertexPos.z;
	}

	//AABB�̌v�Z
	for (int VertexNum = 1; VertexNum < (int)NumVertices; VertexNum++) {
		//���_�ʒu�擾
		VertexPos = *(D3DXVECTOR3*)(p_vertex + VertexNum * VertexSize);

		//X����r
		if (VertexPos.x < m_AABBMinXMaxX.x) {
			m_AABBMinXMaxX.x = VertexPos.x;
		}
		if (VertexPos.x > m_AABBMinXMaxX.y) {
			m_AABBMinXMaxX.y = VertexPos.x;
		}
		//Y����r
		if (VertexPos.y < m_AABBMinYMaxY.x) {
			m_AABBMinYMaxY.x = VertexPos.y;
		}
		if (VertexPos.y > m_AABBMinYMaxY.y) {
			m_AABBMinYMaxY.y = VertexPos.y;
		}
		//Z����r
		if (VertexPos.z < m_AABBMinZMaxZ.x) {
			m_AABBMinZMaxZ.x = VertexPos.z;
		}
		if (VertexPos.z > m_AABBMinZMaxZ.y) {
			m_AABBMinZMaxZ.y = VertexPos.z;
		}
	}

	if (AABBMesh != nullptr) {
		AABBMesh->Release();
		AABBMesh = nullptr;
	}
}


/*==============================================================================
�֐����� : AABB�̌v�Z���ʂ��擾
�������� :
OutMinXMaxX : AABB��x�����̒l�AOutMinXMaxX.x�͍ŏ��l�AOutMinXMaxX.y�͍ő�l
OutMinYMaxY : AABB��y�����̒l�AOutMinYMaxY.x�͍ŏ��l�AOutMinYMaxY.y�͍ő�l
OutMinZMaxZ : AABB��z�����̒l�AOutMinZMaxZ.x�͍ŏ��l�AOutMinZMaxZ.y�͍ő�l
===============================================================================*/
void CModel::GetAABBValue(D3DXVECTOR2 *OutMinXMaxX, D3DXVECTOR2 *OutMinYMaxY, D3DXVECTOR2 *OutMinZMaxZ)
{
	*OutMinXMaxX = m_AABBMinXMaxX;
	*OutMinYMaxY = m_AABBMinYMaxY;
	*OutMinZMaxZ = m_AABBMinZMaxZ;
}

void CModel::SetMtxSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale/*�g��s��*/, mtxRot/*��]�s��*/, mtxMove/*�ړ��s��*/, mtxResult/*���ʍs��*/;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y/*y��*/, m_rot.x/*x��*/, m_rot.z/*z��*/); //��]�s���ݒ�
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;						//���ʂ̕ϊ��s��̌v�Z
	D3DXMatrixIdentity(&m_Result);								//���[���h�s���P�ʍs��ŏ�����
	D3DXMatrixMultiply(&m_Result, &m_Result, &mtxResult);		//��]�s��ƃ��[���h�s��̊|���Z
	pDevice->SetTransform(D3DTS_WORLD, &m_Result);				//���[���h�ϊ��s��
}

void CModel::SetMtxSettingPose(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale/*�g��s��*/, mtxMove/*�ړ��s��*/, mtxResult/*���ʍs��*/;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*m_PoseMtx*mtxMove;                     //���ʂ̕ϊ��s��̌v�Z
	D3DXMatrixIdentity(&m_Result);                    //���[���h�s���P�ʍs��ŏ�����
	D3DXMatrixMultiply(&m_Result, &m_Result, &mtxResult);  //��]�s��ƃ��[���h�s��̊|���Z
	pDevice->SetTransform(D3DTS_WORLD, &m_Result);              //���[���h�ϊ��s��
}

LPD3DXMESH CModel::GetMesh(void) const
{
	return CModelManager::GetMesh(m_ModelID);
}

void CModel::SetDrawType(DRAW_TYPE type)
{
	bool OveerRange = (int)type < 0 && (int)type >= DRAW_USE_MTRL_MAX;
	if (OveerRange) {
		MessageBox(nullptr,"�֐��uvoid CModel::SetDrawType(DRAW_TYPE type)�v�̈����̒l�w�肪�������Ȃ�","�G���[",MB_ICONHAND);
		return;
	}
	m_DrawType = type;
}

void CModel::SetAlpha(float Alpha)
{	
	CModelManager::SetAlpha(m_ModelID,Alpha);
}