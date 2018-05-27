#include "CShaderManager.h"
#include "CRenderer.h"
#include <algorithm>

CShaderManager* CShaderManager::m_pShaderManager = nullptr;

CShaderManager::CShaderManager()
{
	for (int i = 0; i < VS_MAX;i++) {
		m_pVertexShader[i] = nullptr;
	}
	for (int i = 0; i < VS_MAX; i++) {
		m_pVertexShaderConstantTable[i] = nullptr;
	}
	for (int i = 0; i < PS_MAX; i++) {
		m_pPixelShader[i] = nullptr;
	}
	for (int i = 0; i < PS_MAX; i++) {
		m_pPixelShaderConstantTable[i] = nullptr;
	}
}

bool CShaderManager::CreateAllShader(HWND hWnd)
{
	//ヴァーテックスシェーダー生成
	if (E_FAIL == CreateVertexShader(hWnd, "ShaderCode/DeffuseDepthShadowVS .hlsl", &m_pVertexShader[DIFFUSE_DEPTH_SHADOW_VS], &m_pVertexShaderConstantTable[DIFFUSE_DEPTH_SHADOW_VS])) {
		return false;
	}
	if (E_FAIL == CreateVertexShader(hWnd, "ShaderCode/SpecularVS.hlsl", &m_pVertexShader[SPECULAR_VS], &m_pVertexShaderConstantTable[SPECULAR_VS])) {
		return false;
	}
	if (E_FAIL == CreateVertexShader(hWnd, "ShaderCode/DepthTextureVS.hlsl", &m_pVertexShader[DEPTH_TEXTURE_VS], &m_pVertexShaderConstantTable[DEPTH_TEXTURE_VS])) {
		return false;
	}
	if (E_FAIL == CreateVertexShader(hWnd, "ShaderCode/SpecularDepthShadowVS.hlsl", &m_pVertexShader[SPECULAR_DEPTH_SHADOW_VS], &m_pVertexShaderConstantTable[SPECULAR_DEPTH_SHADOW_VS])) {
		return false;
	}
	if (E_FAIL == CreateVertexShader(hWnd, "ShaderCode/MotionBlurVS.hlsl", &m_pVertexShader[MOTION_BLUR_VS], &m_pVertexShaderConstantTable[MOTION_BLUR_VS])) {
		return false;
	}

	//ピクセルシェーダーを生成
	if (E_FAIL == CreatePixelShader(hWnd, "ShaderCode/DeffuseDepthShadowPS.hlsl", &m_pPixelShader[DIFFUSE_DEPTH_SHADOW_PS], &m_pPixelShaderConstantTable[DIFFUSE_DEPTH_SHADOW_PS])) {
		return false;
	}
	if (E_FAIL == CreatePixelShader(hWnd, "ShaderCode/SpecularPS.hlsl", &m_pPixelShader[SPECULAR_PS], &m_pPixelShaderConstantTable[SPECULAR_PS])) {
		return false;
	}
	if (E_FAIL == CreatePixelShader(hWnd, "ShaderCode/DepthTexturePS.hlsl", &m_pPixelShader[DEPTH_TEXTURE_PS], &m_pPixelShaderConstantTable[DEPTH_TEXTURE_PS])) {
		return false;
	}
	if (E_FAIL == CreatePixelShader(hWnd, "ShaderCode/SpecularDepthShadowPS.hlsl", &m_pPixelShader[SPECULAR_DEPTH_SHADOW_PS], &m_pPixelShaderConstantTable[SPECULAR_DEPTH_SHADOW_PS])) {
		return false;
	}
	if (E_FAIL == CreatePixelShader(hWnd, "ShaderCode/BasicPS.hlsl", &m_pPixelShader[BASIC_PS], &m_pPixelShaderConstantTable[BASIC_PS])) {
		return false;
	}
	if (E_FAIL == CreatePixelShader(hWnd, "ShaderCode/MotionBlurPS.hlsl", &m_pPixelShader[MOTION_BLUR_PS], &m_pPixelShaderConstantTable[MOTION_BLUR_PS])) {
		return false;
	}

	return true;
}

void CShaderManager::DeleteAllShader(void)
{
	for (int i = 0; i < VS_MAX; i++) {
		if (m_pVertexShader[i] != nullptr) {
			m_pVertexShader[i]->Release();
			m_pVertexShader[i] = nullptr;
		}
		if (m_pVertexShaderConstantTable[i] != nullptr) {
			m_pVertexShaderConstantTable[i]->Release();
			m_pVertexShaderConstantTable[i] = nullptr;
		}
	}
	for (int i = 0; i < PS_MAX; i++) {
		if (m_pPixelShader[i] != nullptr) {
			m_pPixelShader[i]->Release();
			m_pPixelShader[i] = nullptr;
		}
		if (m_pPixelShaderConstantTable[i] != nullptr) {
			m_pPixelShaderConstantTable[i]->Release();
			m_pPixelShaderConstantTable[i] = nullptr;
		}
	}
}

LPDIRECT3DVERTEXSHADER9 CShaderManager::GetVertexShaderPointer(int Number)
{
	if (Number < 0 && Number >= VS_MAX) {
		_RPT0(0,"[CShaderManager.cpp][GetVertexShaderPointer]引数Numberの値は範囲外");
		return nullptr;
	}

	return m_pVertexShader[Number];
}

LPD3DXCONSTANTTABLE CShaderManager::GetVertexShaderCTPointer(int Number)
{
	if (Number < 0 && Number >= VS_MAX) {
		_RPT0(0, "[CShaderManager.cpp][GetVertexShaderCTPointer]引数Numberの値は範囲外");
		return nullptr;
	}

	return m_pVertexShaderConstantTable[Number];
}

LPDIRECT3DPIXELSHADER9 CShaderManager::GetPixelShaderPointer(int Number)
{
	if (Number < 0 && Number >= PS_MAX) {
		_RPT0(0, "[CShaderManager.cpp][GetPixelShaderPointer]引数Numberの値は範囲外");
		return nullptr;
	}

	return m_pPixelShader[Number];
}

LPD3DXCONSTANTTABLE CShaderManager::GetPixelShaderCTPointer(int Number)
{
	if (Number < 0 && Number >= PS_MAX) {
		_RPT0(0, "[CShaderManager.cpp][GetPixelShaderCTPointer]引数Numberの値は範囲外");
		return nullptr;
	}

	return m_pPixelShaderConstantTable[Number];
}

CShaderManager* CShaderManager::GetInstance(void)
{
	if (m_pShaderManager == nullptr) {
		_RPT0(0,"[CShaderManager.cpp][GetInstance]インスタンスはヌル、生成する");
		m_pShaderManager = new CShaderManager();
	}

	return m_pShaderManager;
}

void CShaderManager::DeleteInstance(void)
{
	if (m_pShaderManager != nullptr) {
		m_pShaderManager->DeleteAllShader();
		delete m_pShaderManager;
		m_pShaderManager = nullptr;
	}
}

HRESULT CShaderManager::CreatePixelShader(HWND hWnd, const char*Filepass, LPDIRECT3DPIXELSHADER9* PS, LPD3DXCONSTANTTABLE* Constant)
{
	HRESULT hr;

	//シェーダーファイルを読み込んでコンパイルする
	LPD3DXBUFFER code = nullptr;		//シェーダーコードをバイナリ化にしたデータの保存メモリ
	LPD3DXBUFFER err = nullptr;			//バイナリ化したシェーダーコードの文法的なエラーの保存場所
	hr = D3DXCompileShaderFromFile(		//シェーダーファイルをコンパイルする
		Filepass, NULL, NULL, "main"/*エントリー関数名(最初実行した関数)*/,
		"ps_3_0"/*シェーダーモデル*/, 0,
		&code/*値NULL:コンパイルが失敗,別の値:コンパイル後のバイナリコード*/,
		&err/*値NULL:コンパイルが成功,別の値：エラーコードのアドレス*/,
		Constant);

	if (FAILED(hr)) {
		MessageBox(hWnd, (char*)err->GetBufferPointer(), "D3DXCompileShaderFromFile(ps)", MB_ICONHAND);
		err->Release();
		return E_FAIL;
	}

	//シェーダーを作る
	hr = CRenderer::GetDevice()->CreatePixelShader(
		(LPDWORD)code->GetBufferPointer(),
		PS/*シェーダー管理してくれる人の居場所*/
	);
	//メッセージデータの解放
	if (code != nullptr) {
		code->Release();
	}
	if (err != nullptr) {
		err->Release();
	}
	if (FAILED(hr)) {
		MessageBox(hWnd, "シェーダーの生成が失敗しました", "CreatePixelShader", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShaderManager::CreateVertexShader(HWND hWnd, const char*Filepass, LPDIRECT3DVERTEXSHADER9* VS, LPD3DXCONSTANTTABLE* Constant)
{
	HRESULT hr;

	//シェーダーファイルを読み込んでコンパイルする
	LPD3DXBUFFER code = nullptr;		//シェーダーコードをバイナリ化にしたデータの保存メモリ
	LPD3DXBUFFER err = nullptr;			//バイナリ化したシェーダーコードの文法的なエラーの保存場所
	hr = D3DXCompileShaderFromFile(		//シェーダーファイルをコンパイルする
		Filepass, NULL, NULL, "main"/*エントリー関数名(最初実行した関数)*/,
		"vs_3_0"/*シェーダーモデル*/, 0,
		&code/*値NULL:コンパイルが失敗,別の値:コンパイル後のバイナリコード*/,
		&err/*値NULL:コンパイルが成功,別の値：エラーコードのアドレス*/,
		Constant);

	if (FAILED(hr)) {
		MessageBox(hWnd, (char*)err->GetBufferPointer(), "D3DXCompileShaderFromFile(ps)", MB_ICONHAND);
		err->Release();
		return E_FAIL;
	}

	//シェーダーを作る
	hr = CRenderer::GetDevice()->CreateVertexShader(
		(LPDWORD)code->GetBufferPointer(),
		VS/*シェーダー管理してくれる人の居場所*/
	);
	//メッセージデータの解放
	if (code != nullptr) {
		code->Release();
	}
	if (err != nullptr) {
		err->Release();
	}
	if (FAILED(hr)) {
		MessageBox(hWnd, "シェーダーの生成が失敗しました", "CreatePixelShader", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}