#ifndef CSHADER_MANAGER_H_
#define CSHADER_MANAGER_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <array>
#include "common.h"

typedef enum 
{
	DEPTH_TEXTURE_VS = 0,
	DIFFUSE_DEPTH_SHADOW_VS,
	SPECULAR_VS,
	SPECULAR_DEPTH_SHADOW_VS,
	MOTION_BLUR_VS,
	VS_MAX
}VS_NAME;

typedef enum
{
	BASIC_PS = 0,
	DIFFUSE_DEPTH_SHADOW_PS,
	SPECULAR_PS,
	DEPTH_TEXTURE_PS,
	SPECULAR_DEPTH_SHADOW_PS,
	MOTION_BLUR_PS,
	PS_MAX
}PS_NAME;

class CShaderManager 
{
public:
	bool CreateAllShader(HWND hWnd);
	void DeleteAllShader(void);

	LPDIRECT3DVERTEXSHADER9 GetVertexShaderPointer(int Number);
	LPD3DXCONSTANTTABLE GetVertexShaderCTPointer(int Number);
	LPDIRECT3DPIXELSHADER9 GetPixelShaderPointer(int Number);
	LPD3DXCONSTANTTABLE GetPixelShaderCTPointer(int Number);

	static CShaderManager* GetInstance(void);
	static void DeleteInstance(void);
private:
	CShaderManager();
	CShaderManager(const CShaderManager & ref) { ref; }
	CShaderManager& operator=(const CShaderManager & ref) { ref; }

	HRESULT CreatePixelShader(HWND hWnd,const char*Filepass, LPDIRECT3DPIXELSHADER9* PS, LPD3DXCONSTANTTABLE* Constant);
	HRESULT CreateVertexShader(HWND hWnd,const char*Filepass, LPDIRECT3DVERTEXSHADER9* VS, LPD3DXCONSTANTTABLE* Constant);
private:
	std::array<LPDIRECT3DVERTEXSHADER9, VS_MAX>	m_pVertexShader;
	std::array<LPD3DXCONSTANTTABLE, VS_MAX>		m_pVertexShaderConstantTable;
	std::array<LPDIRECT3DPIXELSHADER9, PS_MAX>	m_pPixelShader;
	std::array<LPD3DXCONSTANTTABLE, PS_MAX>		m_pPixelShaderConstantTable;
	static CShaderManager*						m_pShaderManager;
};

#endif