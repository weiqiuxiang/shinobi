#include "ModelObj.h"
#include "TexManager.h"
#include "CRenderer.h"
#include "CShaderManager.h"
#include "CLightManager.h"
#include "CCameraManager.h"

CModelObj::CModelObj() :
	m_pMesh(nullptr),
	m_pBuffMat(nullptr),
	m_nNumMat(0),
	m_TexID(nullptr)
{
	memset(&m_pFilepass[0], '\0', sizeof(&m_pFilepass[0]));
}

CModelObj::~CModelObj()
{
	
}

HRESULT CModelObj::Init(void)
{
	//������
	if(m_pMesh != nullptr){m_pMesh->Release(); m_pMesh = nullptr;}
	if (m_pBuffMat != nullptr){m_pMesh->Release(); m_pBuffMat = nullptr;}
	if (m_TexID != nullptr){delete[] m_TexID; m_TexID = nullptr;}

	m_pMesh = nullptr;
	m_pBuffMat = nullptr;
	m_TexID = nullptr;
	m_nNumMat = 0;
	memset(&m_pFilepass[0], '\0', sizeof(&m_pFilepass[0]));

	return S_OK;
}

void CModelObj::Uninit(void)
{
	if (m_pMesh != nullptr) { 
		m_pMesh->Release(); 
		m_pMesh = nullptr;
	}
	if (m_pBuffMat != nullptr) { 
		m_pBuffMat->Release(); 
		m_pBuffMat = nullptr; 
	}
	if (m_TexID != nullptr) { 
		delete[] m_TexID; 
		m_TexID = nullptr; 
	}
}

void CModelObj::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	D3DMATERIAL9 matDef;

	pDevice->GetMaterial(&matDef);

	//�`��
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pDevice->SetMaterial(&pMat[nCnt].MatD3D);
		pDevice->SetTexture(0, CTexManager::UseTex(m_TexID[nCnt]));
		m_pMesh->DrawSubset(nCnt);
	}

	pDevice->SetMaterial(&matDef);   //�}�e���A����߂�
}

void CModelObj::DrawMtrl(const D3DMATERIAL9& Mtrl)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);

	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pDevice->SetMaterial(&Mtrl);
		pDevice->SetTexture(0, CTexManager::UseTex(m_TexID[nCnt]));
		m_pMesh->DrawSubset(nCnt);
	}

	pDevice->SetMaterial(&matDef);   //�}�e���A����߂�
}

void CModelObj::DrawOnDepthMap(const D3DXMATRIX& mtxWorld,const D3DXMATRIX& mtxLightView, const D3DXMATRIX& mtxLightProje)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//�V�F�[�_�[�}�l�[�W���[�̃|�C���^���擾���A�k���`�F�b�N
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//���@�[�e�b�N�X�V�F�[�_�[�ƃs�N�Z���V�F�[�_�[���擾
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(DEPTH_TEXTURE_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(DEPTH_TEXTURE_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(DEPTH_TEXTURE_PS);

	//�V�F�[�_�[���Z�b�g
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//�V�F�[�_�[�Ƀf�[�^�𑗂�
	D3DXMATRIX mtxLightWVP = mtxWorld * mtxLightView * mtxLightProje;
	HRESULT hr = E_FAIL;
	hr= pVSCT->SetMatrix(pDevice, "mtxLightWVP", &mtxLightWVP);

	//�`��
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		m_pMesh->DrawSubset(nCnt);
	}

	//�V�F�[�_�[���k���ɖ߂�
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CModelObj::DrawUseDepthMap(const D3DXMATRIX& worldMtx, const D3DXMATRIX& LightViewMtx, const D3DXMATRIX& LightProjeMtx, LPDIRECT3DTEXTURE9 DepthTexture)
{
	//�k���`�F�b�N
	if (DepthTexture == nullptr) {
		_RPT0(0, "[ModelObj.cpp][DrawUseDepthMap]����DepthTexture�̓k��");
		return;
	}

	//�f�o�C�X�擾
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//�V�F�[�_�[�}�l�[�W���[�̃|�C���^���擾���A�k���`�F�b�N
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//���@�[�e�b�N�X�V�F�[�_�[�ƃs�N�Z���V�F�[�_�[���擾
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(SPECULAR_DEPTH_SHADOW_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(SPECULAR_DEPTH_SHADOW_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(SPECULAR_DEPTH_SHADOW_PS);
	LPD3DXCONSTANTTABLE pPSCT = pShaderManager->GetPixelShaderCTPointer(SPECULAR_DEPTH_SHADOW_PS);

	//�V�F�[�_�[���Z�b�g
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//���@�[�e�b�N�X�V�F�[�_�[�̒萔�e�[�u���Ƀf�[�^�𑗂�
	D3DXMATRIX mtxWVP = worldMtx * CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);
	D3DXMATRIX mtxLightWVP = worldMtx * LightViewMtx * LightProjeMtx;
	hr = pVSCT->SetMatrix(pDevice, "mtxLightWVP", &mtxLightWVP);

	//�s�N�Z���V�F�[�_�[�̒萔�e�[�u���Ƀf�[�^�𑗂�
	D3DXMATRIX worldInverse;
	D3DXMatrixInverse(&worldInverse,nullptr,&worldMtx);
	D3DXVECTOR3 EyeL = CCameraManager::GetActiveCameraPos();
	D3DXVec3TransformCoord(&EyeL, &EyeL, &worldInverse);
	hr = pPSCT->SetFloatArray(pDevice, "posEyeL", EyeL, 3);

	D3DXVECTOR3 lightDirW = CLightManager::GetLightDirection(0);
	D3DXVECTOR3 lightDirL;
	D3DXVec3TransformNormal(&lightDirL, &lightDirW, &worldInverse);
	D3DXVec3Normalize(&lightDirL, &lightDirL);
	hr = pPSCT->SetFloatArray(pDevice, "lightDirL", lightDirL, 3);
	hr = pPSCT->SetFloat(pDevice, "specularPower", 10);

	//�T���v���[�𑗂�
	UINT DepthMapSamplerIndex = pPSCT->GetSamplerIndex("DepthMapSampler");
	pDevice->SetSamplerState(DepthMapSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDevice->SetSamplerState(DepthMapSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	hr = pDevice->SetTexture(DepthMapSamplerIndex, DepthTexture);

	//�`��
	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pVSCT->SetFloatArray(pDevice, "DiffuseColor", (FLOAT*)&pMat[nCnt].MatD3D.Diffuse, 4);
		pVSCT->SetFloatArray(pDevice, "AmbientColor", (FLOAT*)&pMat[nCnt].MatD3D.Ambient, 4);
		pVSCT->SetFloatArray(pDevice, "EmissiveColor", (FLOAT*)&pMat[nCnt].MatD3D.Emissive, 4);
		UINT SamplerIndex0 = pPSCT->GetSamplerIndex("Sampler0");
		pDevice->SetTexture(SamplerIndex0, CTexManager::UseTex(m_TexID[nCnt]) );
		m_pMesh->DrawSubset(nCnt);
	}

	//�V�F�[�_�[���k���ɖ߂�
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CModelObj::DrawSpecular(const D3DXMATRIX& worldMtx)
{
	//�f�o�C�X�擾
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//�V�F�[�_�[�}�l�[�W���[�̃|�C���^���擾���A�k���`�F�b�N
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//���@�[�e�b�N�X�V�F�[�_�[�ƃs�N�Z���V�F�[�_�[���擾
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(SPECULAR_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(SPECULAR_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(SPECULAR_PS);
	LPD3DXCONSTANTTABLE pPSCT = pShaderManager->GetPixelShaderCTPointer(SPECULAR_PS);

	//�V�F�[�_�[���Z�b�g
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//���@�[�e�b�N�X�V�F�[�_�[�̒萔�e�[�u���Ƀf�[�^�𑗂�
	D3DXMATRIX mtxWVP = worldMtx * CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);

	//�s�N�Z���V�F�[�_�[�̒萔�e�[�u���Ƀf�[�^�𑗂�
	D3DXMATRIX worldInverse;
	D3DXMatrixInverse(&worldInverse, nullptr, &worldMtx);
	D3DXVECTOR3 EyeL = CCameraManager::GetActiveCameraPos();
	D3DXVec3TransformCoord(&EyeL, &EyeL, &worldInverse);
	hr = pPSCT->SetFloatArray(pDevice, "posEyeL", EyeL, 3);

	D3DXVECTOR3 lightDirW = CLightManager::GetLightDirection(0);
	D3DXVECTOR3 lightDirL;
	D3DXVec3TransformNormal(&lightDirL, &lightDirW, &worldInverse);
	D3DXVec3Normalize(&lightDirL, &lightDirL);
	hr = pPSCT->SetFloatArray(pDevice, "lightDirL", lightDirL, 3);
	hr = pPSCT->SetFloat(pDevice, "specularPower", 10);

	//�`��
	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pVSCT->SetFloatArray(pDevice, "DiffuseColor", (FLOAT*)&pMat[nCnt].MatD3D.Diffuse, 4);
		pVSCT->SetFloatArray(pDevice, "AmbientColor", (FLOAT*)&pMat[nCnt].MatD3D.Ambient, 4);
		pVSCT->SetFloatArray(pDevice, "EmissiveColor", (FLOAT*)&pMat[nCnt].MatD3D.Emissive, 4);
		UINT SamplerIndex0 = pPSCT->GetSamplerIndex("Sampler0");
		pDevice->SetTexture(SamplerIndex0, CTexManager::UseTex(m_TexID[nCnt]));
		m_pMesh->DrawSubset(nCnt);
	}

	//�V�F�[�_�[���k���ɖ߂�
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CModelObj::DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture)
{
	//�f�o�C�X�擾
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//�V�F�[�_�[�}�l�[�W���[�̃|�C���^���擾���A�k���`�F�b�N
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//���@�[�e�b�N�X�V�F�[�_�[�ƃs�N�Z���V�F�[�_�[���擾
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(MOTION_BLUR_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(MOTION_BLUR_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(MOTION_BLUR_PS);
	LPD3DXCONSTANTTABLE pPSCT = pShaderManager->GetPixelShaderCTPointer(MOTION_BLUR_PS);

	//�V�F�[�_�[���Z�b�g
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//�o�[�e�b�N�X�V�F�[�_�[�̒萔�e�[�u���Ƀf�[�^�𑗂�
	D3DXMATRIX mtxVP = CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	D3DXMATRIX mtxWVP = worldMtx * mtxVP;
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);
	D3DXMATRIX mtxWVPOld = worldMtxOld * mtxVP;
	hr = pVSCT->SetMatrix(pDevice, "mtxWVPOld", &mtxWVPOld);

	D3DXMATRIX mtxWITVP;
	D3DXMatrixInverse(&mtxWITVP, nullptr, &worldMtx);
	D3DXMatrixTranspose(&mtxWITVP, &mtxWITVP);
	mtxWITVP = mtxWITVP * mtxVP;
	hr = pVSCT->SetMatrix(pDevice, "mtxWITVP", &mtxWITVP);

	//�s�N�Z���V�F�[�_�[�̒萔�e�[�u���Ƀf�[�^�𑗂�
	UINT OriginalSamplerIndex = pPSCT->GetSamplerIndex("OriginalSampler");
	pDevice->SetSamplerState(OriginalSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDevice->SetSamplerState(OriginalSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	hr = pDevice->SetTexture(OriginalSamplerIndex, OriginalTexture);

	//���f���`��
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		m_pMesh->DrawSubset(nCnt);
	}

	//�V�F�[�_�[���k���ɖ߂�
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

//-----------------------
//�t�@���N�V�����֐�
//-----------------------
//���f���̃t�@�C���p�X��ݒ�
void CModelObj::SetModelPass(const char* filepass)
{
	strcpy(&m_pFilepass[0], filepass);     //�t�@�C���p�X�L�^
}

//���ݒ�
void CModelObj::SetAlpha(float Alpha)
{
	Alpha = max(Alpha,0.0f);
	Alpha = min(1.0f, Alpha);

	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++) {
		pMat[nCnt].MatD3D.Diffuse.a = Alpha;
		pMat[nCnt].MatD3D.Ambient.a = Alpha;
		pMat[nCnt].MatD3D.Emissive.a = Alpha;
	}
}

//���f���p�X�̎擾
const char *CModelObj::GetModelPass(void)
{
	return m_pFilepass;
}

//���f���I�u�W�F�̐���
void CModelObj::CreateModelObj(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	m_pMesh = nullptr;
	m_pBuffMat = nullptr;
	
	//���f���f�[�^�̓ǂݍ���
	if (FAILED(D3DXLoadMeshFromX(m_pFilepass, D3DXMESH_MANAGED/*�w��������*/, pDevice/*�f�o�C�X*/, nullptr/*�אڃo�b�t�@*/, &m_pBuffMat/*�}�e���A�������i�[����*/, nullptr, &m_nNumMat/*�}�e���A����*/, &m_pMesh/*���b�V��*/)))
	{
		MessageBox(nullptr, "���f��MODEL_NAME�̓ǂݍ��݂����s����", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}

	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();   //�}�e���A�����̃|�C���^�Ƃ��ăo�b�t�@�̃A�h���X���擾

	//�e�N�X�`���ǂݍ���
	if (m_TexID != nullptr)
	{
		delete[] m_TexID;
		m_TexID = nullptr;
	}
	m_TexID = new TEX_ID[(TEX_ID)m_nNumMat];
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		m_TexID[nCnt] = CTexManager::LoadTex(pMat[nCnt].pTextureFilename);
	}
}