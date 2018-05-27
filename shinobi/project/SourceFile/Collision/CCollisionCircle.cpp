#include "CCollisionCircle.h"
#include "CCollisionMath.h"
#include "CRenderer.h"

CCollisionCircle::CCollisionCircle()
{
	m_pMesh = nullptr;
	m_type = TYPE_NONE;
	m_bUse = false;
}

CCollisionCircle::~CCollisionCircle()
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}

HRESULT CCollisionCircle::Init(float Radius)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	if(nullptr !=m_pMesh)               //nullptr�`�F�b�N
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	if (FAILED(D3DXCreateSphere(
		pDevice,        // �`���̃f�o�C�X
		1.0f,           // ���a�B0.0f�ł�����OK�B�}�C�i�X��NG
		16,             // �X���C�X�̐��B���₷�Ƃ��ꂢ�ȋ��ɂȂ�͂�
		8,              // �X�^�b�N���B���₷�Ƃ��ꂢ�ȋ��ɂȂ�͂�
		&m_pMesh,         // �o�͂��郁�b�V��
		nullptr            // ���܂�g��Ȃ��炵�����nullptr�ɂ��Ƃ���
	)))
	{
		MessageBox(nullptr, "Sphere��Create�����s", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	D3DXMatrixIdentity(&m_MtxResult);
	m_MtxResultOld = m_MtxResult;
	m_Radius = max(0.01f,Radius);
	m_scl = D3DXVECTOR3(m_Radius, m_Radius, m_Radius);
	m_type = TYPE_NONE;

	InitMaterialBaseOnType();                          //�}�e���A���̏�����

	return S_OK;
}

HRESULT CCollisionCircle::Init( float Radius, int ParentID, TYPE type)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	if (nullptr != m_pMesh)               //nullptr�`�F�b�N
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	if (FAILED(D3DXCreateSphere(
		pDevice,        // �`���̃f�o�C�X
		1.0f,           // ���a�B0.0f�ł�����OK�B�}�C�i�X��NG
		16,             // �X���C�X�̐��B���₷�Ƃ��ꂢ�ȋ��ɂȂ�͂�
		8,              // �X�^�b�N���B���₷�Ƃ��ꂢ�ȋ��ɂȂ�͂�
		&m_pMesh,         // �o�͂��郁�b�V��
		nullptr            // ���܂�g��Ȃ��炵�����nullptr�ɂ��Ƃ���
	)))
	{
		MessageBox(nullptr, "Sphere��Create�����s", "�G���[", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	D3DXMatrixIdentity(&m_MtxResult);
	m_MtxResultOld = m_MtxResult;
	m_Radius = max(0.01f, Radius);
	m_scl = D3DXVECTOR3(m_Radius, m_Radius, m_Radius);
	m_type = type;
	m_ParentID = ParentID;

	InitMaterialBaseOnType();                               //�}�e���A���̏�����

	return S_OK;
}

void CCollisionCircle::Update(void)
{
	m_MtxResultOld = m_MtxResult;
	m_scl = D3DXVECTOR3(m_Radius, m_Radius, m_Radius);
	SetMaterialBaseOnType();                           //�}�e���A���ύX
	m_typeOld = m_type;
	m_pos = D3DXVECTOR3(m_MtxResult._41, m_MtxResult._42, m_MtxResult._43);
}

void CCollisionCircle::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);

	MtxWorldSetting();										//���[���h�s��ݒ�
	pDevice->SetMaterial(&m_Mtrl);
	m_pMesh->DrawSubset(0);
	pDevice->SetMaterial(&matDef);							//�}�e���A����߂�
}

void CCollisionCircle::Uninit(void)
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}

void CCollisionCircle::InitMaterialBaseOnType(void)
{
	switch (m_type)
	{
	case TYPE_NONE:
		m_Mtrl.Diffuse = { 0.0f,0.5f,0.0f,0.8f };				//��
		break;
	case TYPE_BODY:
		m_Mtrl.Diffuse = { 0.941f,0.894f,0.451f,0.8f };			//���F
		break;
	case TYPE_HIT_PREVIEW:
		m_Mtrl.Diffuse = { 0.509f,0.372f,0.0f,0.8f };			//��
		break;
	case TYPE_HIT:
		m_Mtrl.Diffuse = { 0.5f,0.0f,0.0f,0.8f };				//��
		break;
	case TYPE_HIT_ENABLE:
		m_Mtrl.Diffuse = { 0.0f,0.0f,0.4f,0.2f };				//��
		break;
	default:
		m_Mtrl.Diffuse = { 0.0f,0.5f,0.0f,0.8f };				//��
		break;
	}

	m_Mtrl.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_Mtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_Mtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	m_Mtrl.Power = 6.0f;
}

void CCollisionCircle::SetMaterialBaseOnType(void)
{
	if (m_typeOld == m_type) return;							//�^�C�v��������������ύX����K�v���Ȃ�

	switch (m_type)
	{
	case TYPE_NONE:
		m_Mtrl.Diffuse = { 0.0f,0.5f,0.0f,0.8f };				//��
		break;
	case TYPE_BODY:
		m_Mtrl.Diffuse = { 0.941f,0.894f,0.451f,0.8f };			//���F
		break;
	case TYPE_HIT_PREVIEW:
		m_Mtrl.Diffuse = { 0.509f,0.372f,0.0f,0.8f };			//��
		break;
	case TYPE_HIT:
		m_Mtrl.Diffuse = { 0.5f,0.0f,0.0f,0.8f };				//��
		break;
	case TYPE_HIT_ENABLE:
		m_Mtrl.Diffuse = { 0.0f,0.0f,0.4f,0.2f };				//��
		break;
	default:
		m_Mtrl.Diffuse = { 0.0f,0.5f,0.0f,0.8f };				//��
		break;
	}

	m_Mtrl.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_Mtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_Mtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	m_Mtrl.Power = 6.0f;
}

void CCollisionCircle::MtxWorldSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //�f�o�C�X�擾
	D3DXMATRIX mtxScale/*�g��s��*/, mtxResult/*���ʍs��*/, WorldMatrix;

	//���[���h�s��̐ݒ�
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	mtxResult = mtxScale*m_MtxResult;                     //���ʂ̕ϊ��s��̌v�Z
	D3DXMatrixIdentity(&WorldMatrix);                    //���[���h�s���P�ʍs��ŏ�����
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);  //��]�s��ƃ��[���h�s��̊|���Z
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //���[���h�ϊ��s��
}

bool CCollisionCircle::HitCheckUseNormal(CCollisionCircle* pA, CCollisionCircle* pB)
{
	if (nullptr == pA || nullptr == pB) return false;

	//�f�[�^�擾
	D3DXMATRIX ResultMtxA = pA->GetMtxResult();
	D3DXMATRIX ResultMtxB = pB->GetMtxResult();
	float fRadiusA = pA->GetRadius();
	float fRadiusB = pB->GetRadius();
	D3DXVECTOR3 PosA, PosB;

	D3DXVec3TransformCoord(&PosA,&D3DXVECTOR3(0.0f,0.0f,0.0f), &ResultMtxA);
	D3DXVec3TransformCoord(&PosB, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxB);

	bool bHit = CCollisionMath::CirclesCollision(PosA, fRadiusA, PosB, fRadiusB);
	return bHit;
}

bool CCollisionCircle::HitCheckUseSSV(CCollisionCircle* pA, CCollisionCircle* pB)
{
	if (nullptr == pA || nullptr == pB) return false;

	//�f�[�^�擾
	D3DXMATRIX ResultMtxA = pA->GetMtxResult();
	D3DXMATRIX ResultMtxAOld = pA->GetMtxResultOld();
	D3DXMATRIX ResultMtxB = pB->GetMtxResult();
	D3DXMATRIX ResultMtxBOld = pB->GetMtxResultOld();
	float fRadiusA = pA->GetRadius();
	float fRadiusB = pB->GetRadius();
	D3DXVECTOR3 PosA, PosB,PosAOld,PosBOld;

	D3DXVec3TransformCoord(&PosA, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxA);
	D3DXVec3TransformCoord(&PosB, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxB);
	D3DXVec3TransformCoord(&PosAOld, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxAOld);
	D3DXVec3TransformCoord(&PosBOld, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxBOld);

	float t;				//�_�~�[
	bool bHit = CCollisionMath::CircleAndCircleSSV(PosAOld,PosA, fRadiusA, PosBOld, PosB, fRadiusB, &t);
	return bHit;
}

bool HitCheckUseSSV(CCollisionCircle* pA, CCollisionCircle* pB, float *t)
{
	if (nullptr == pA || nullptr == pB) return false;

	//�f�[�^�擾
	D3DXMATRIX ResultMtxA = pA->GetMtxResult();
	D3DXMATRIX ResultMtxAOld = pA->GetMtxResultOld();
	D3DXMATRIX ResultMtxB = pB->GetMtxResult();
	D3DXMATRIX ResultMtxBOld = pB->GetMtxResultOld();
	float fRadiusA = pA->GetRadius();
	float fRadiusB = pB->GetRadius();
	D3DXVECTOR3 PosA, PosB, PosAOld, PosBOld;

	D3DXVec3TransformCoord(&PosA, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxA);
	D3DXVec3TransformCoord(&PosB, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxB);
	D3DXVec3TransformCoord(&PosAOld, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxAOld);
	D3DXVec3TransformCoord(&PosBOld, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ResultMtxBOld);

	bool bHit = CCollisionMath::CircleAndCircleSSV(PosAOld, PosA, fRadiusA, PosBOld, PosB, fRadiusB, t);
	return bHit;
}