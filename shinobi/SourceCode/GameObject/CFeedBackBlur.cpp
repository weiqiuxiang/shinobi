#include "CFeedBackBlur.h"
#include "SceneTransition.h"
#include "CRenderer.h"
#include "CGame.h"

#define BLUR_SIZEX_START (SCREEN_WIDTH * 1.03f)
#define BLUR_SIZEY_START (SCREEN_HEIGHT * 1.03f)

#define BLUR_SIZEX_INCREMENT (1)
#define BLUR_SIZEY_INCREMENT (1)

#define BLUR_COUNT_MAX (10)			//ブラー回数

#define ALPHA_START (0.99f * 255)
#define ALPHA_DECREMENT (10)

CFeedBackBlur::CFeedBackBlur() 
{
	m_BlurCount = 0;
}

bool CFeedBackBlur::Init(void) 
{
	m_RenderTargetBlur.Init(D3DXVECTOR3(SCREEN_WIDTH*0.5,SCREEN_HEIGHT*0.5f,0.0f),D3DXVECTOR3(SCREEN_WIDTH,SCREEN_HEIGHT,0.0f));
	m_RenderTargetDrawInGame.Init(D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5f, 0.0f), D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f));

	m_RenderTargetBlur.SetSize(D3DXVECTOR3(BLUR_SIZEX_START, BLUR_SIZEY_START,0.0f));
	m_RenderTargetBlur.SetLayerA((int)ALPHA_START);

	return true;
}

void CFeedBackBlur::DrawBlurOnDrawTarget(CGame* pGame)
{
	m_RenderTargetBlur.Update();
	m_RenderTargetDrawInGame.Update();

	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	bool	IsUse;
	if(CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_BACKSPACE) )
	{
		if (m_BlurCount <= BLUR_COUNT_MAX) 
		{
			D3DXVECTOR3 Size;
			Size.x = BLUR_SIZEX_START + m_BlurCount * SCREEN_WIDTH * 0.00008f;
			Size.y = BLUR_SIZEY_START + m_BlurCount * SCREEN_HEIGHT * 0.00008f;
			Size.z = 0.0f;
			m_RenderTargetBlur.SetSize(Size);

			//カウントアップ
			m_BlurCount++;
		}
	}
	else 
	{
		m_RenderTargetBlur.SetSize(D3DXVECTOR3(BLUR_SIZEX_START, BLUR_SIZEY_START, 0.0f));
		m_RenderTargetBlur.SetLayerA((int)ALPHA_START);
		m_BlurCount = 0;
		pDevice->SetRenderTarget(0, m_RenderTargetBlur.GetSurfacePointer());
		CRenderer::DrawClear();
		pDevice->SetRenderTarget(0, CRenderer::GetBackBufferSurface());
	}

	//ブラーをゲーム描画のサーフェースに描画
	pDevice->SetRenderTarget(0, m_RenderTargetDrawInGame.GetSurfacePointer());
	CRenderer::DrawClear();
	CRenderer::DrawBegin();

	if (pGame != nullptr) {
		pGame->DrawInRenderTarget();
	}

	m_RenderTargetBlur.DrawTest();

	CRenderer::DrawEnd();

	pDevice->SetRenderTarget(0, CRenderer::GetBackBufferSurface());
}

void CFeedBackBlur::Draw(void) 
{
	//描画
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_RenderTargetDrawInGame.DrawTest();

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	

	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_BACKSPACE))
	{
		//テクスチャを入れ替え
		LPDIRECT3DTEXTURE9 pTex = m_RenderTargetBlur.GetTexturePointer();
		m_RenderTargetBlur.SetTexturePointer(m_RenderTargetDrawInGame.GetTexturePointer());
		m_RenderTargetDrawInGame.SetTexturePointer(pTex);

		//サーフェースを入れ替え
		LPDIRECT3DSURFACE9 pSurface = m_RenderTargetBlur.GetSurfacePointer();
		m_RenderTargetBlur.SetSurfacePointer(m_RenderTargetDrawInGame.GetSurfacePointer());
		m_RenderTargetDrawInGame.SetSurfacePointer(pSurface);
	}
}

void CFeedBackBlur::Uninit(void) 
{
	m_RenderTargetBlur.Uninit();
	m_RenderTargetDrawInGame.Uninit();
}