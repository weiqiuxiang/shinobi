#include "CDebug.h"
#include "CRenderer.h"

char CDebug::m_Text[1024] = {};
LPD3DXFONT CDebug::m_pFont = nullptr;			// フォントへのポインタ

void CDebug::Init(void)
{
	if (m_pFont != nullptr) {
		return;
	}
	LPDIRECT3DDEVICE9 pD3DDevice = CRenderer::GetDevice();
	D3DXCreateFont(pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &m_pFont);
}

void CDebug::ClearText(void)
{
	strcpy(m_Text,"");
}

void CDebug::AddText(char *Text)
{
	strcat(m_Text,Text);
}

void CDebug::Draw(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	if(m_pFont != nullptr) m_pFont->DrawText(nullptr, m_Text, -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 0, 255));
}

void CDebug::Uninit(void)
{
	if (m_pFont != nullptr)
	{
		m_pFont->Release();
		m_pFont = nullptr;
	}
}