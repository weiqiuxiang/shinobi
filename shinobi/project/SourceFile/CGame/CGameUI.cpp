#include "CGameUI.h"
#include "CFontUIScaleFactory.h"
#include "CScene2DStorage.h"
#include "CLinkPlayerToUI.h"
#include "CPlayer.h"
#include "SceneTransition.h"
#include "CTitle.h"
#include "CResult.h"

//マクロ
#define GAUGE_UI_WIDTH (438.0f * 0.7f)
#define GAUGE_UI_HEIGHT (111.0f * 0.7f)

#define HP_GAUGE_POSX (180.0f)
#define HP_GAUGE_POSY (27.0f)
#define HP_GAUGE_WIDTH (325.0f * 0.7f)
#define HP_GAUGE_HEIGHT (33.0f * 0.7f)

#define SP_GAUGE_POSX (180.0f)
#define SP_GAUGE_POSY (51.5f)
#define SP_GAUGE_WIDTH (325.0f * 0.7f)
#define SP_GAUGE_HEIGHT (28.0f * 0.7f)

#define MAGIC_GAUGE_SIZE (19.0f)
#define MAGIC_GAUGE_POSX (35.5f)
#define MAGIC_GAUGE_POSY (39.0f)

#define MAGIC_GAUGE_ALPHA_MIN (0)
#define MAGIC_GAUGE_ALPHA_MAX (160)

#define ALPHA_SPEED (2)				//アルファの変更速度

void CGameUI::InitUIObj(void)
{
	MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)UI_MAX, m_FontUIScale);				//全て解放
	MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGauge);	//全て解放
	MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGaugeEF);

	MyNameSpace::For_Null<CFontUIScale**>((int)UI_MAX, m_FontUIScale);							//nullptrを入れる
	MyNameSpace::For_Null<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGauge);				//nullptrを入れる
	MyNameSpace::For_Null<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGaugeEF);

	//描画フラグをtrueにする
	for (int nCnt = 0; nCnt < MAGIC_GAUGE_OBJ_MAX; nCnt++) m_bDrawFlag[nCnt] = true;

	CreateUIGauge();
	CreateMagicGauge();
}

void CGameUI::CreateUIGauge(void)
{
	//UIを作る
	CScene2D* pScene2D_HP = nullptr;
	pScene2D_HP = CScene2DStorage::CreateObject(
		D3DXVECTOR3(HP_GAUGE_POSX, HP_GAUGE_POSY, 0.0f),
		D3DXVECTOR3(HP_GAUGE_WIDTH, HP_GAUGE_HEIGHT, 0.0f),
		0, nullptr);
	if (pScene2D_HP != nullptr) {
		pScene2D_HP->SetRGBA({ 100,100,100,255 });
		pScene2D_HP->Update();
	}

	CScene2D* pScene2D_SP = nullptr;
	pScene2D_SP = CScene2DStorage::CreateObject(
		D3DXVECTOR3(SP_GAUGE_POSX, SP_GAUGE_POSY, 0.0f),
		D3DXVECTOR3(SP_GAUGE_WIDTH, SP_GAUGE_HEIGHT, 0.0f),
		0, nullptr);
	if (pScene2D_SP != nullptr) {
		pScene2D_SP->SetRGBA({ 100,100,100,255 });
		pScene2D_SP->Update();
	}

	CScene2D* pScene_Magic = nullptr;
	pScene_Magic = CScene2DStorage::CreateObject(
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE * 2, MAGIC_GAUGE_SIZE * 2, 0.0f),
		0, nullptr);
	if (pScene_Magic != nullptr) {
		pScene_Magic->SetRGBA({ 100,100,100,255 });
		pScene_Magic->Update();
	}

	m_FontUIScale[UI_GAUGE] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(GAUGE_UI_WIDTH*0.5, GAUGE_UI_HEIGHT*0.5, 0.0f),
		D3DXVECTOR3(GAUGE_UI_WIDTH, GAUGE_UI_HEIGHT, 0.0f),
		D3DXVECTOR3(GAUGE_UI_WIDTH*0.5, GAUGE_UI_HEIGHT*0.5, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 255,255,255,255 },
		false,
		GAME_GAUGE_UI_TEX
	);

	m_FontUIScale[UI_HP] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(HP_GAUGE_POSX, HP_GAUGE_POSY, 0.0f),
		D3DXVECTOR3(HP_GAUGE_WIDTH, HP_GAUGE_HEIGHT, 0.0f),
		D3DXVECTOR3(HP_GAUGE_POSX - HP_GAUGE_WIDTH*0.5, HP_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 0,70,0,255 },
		false,
		nullptr
	);

	m_FontUIScale[UI_HPEF] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(HP_GAUGE_POSX, HP_GAUGE_POSY, 0.0f),
		D3DXVECTOR3(HP_GAUGE_WIDTH, HP_GAUGE_HEIGHT, 0.0f),
		D3DXVECTOR3(HP_GAUGE_POSX - HP_GAUGE_WIDTH*0.5, HP_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 255,255,255,220 },
		true,
		GAUGE_EF_TEX
	);

	m_FontUIScale[UI_SP] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(SP_GAUGE_POSX, SP_GAUGE_POSY, 0.0f),
		D3DXVECTOR3(SP_GAUGE_WIDTH, SP_GAUGE_HEIGHT, 0.0f),
		D3DXVECTOR3(SP_GAUGE_POSX - SP_GAUGE_WIDTH*0.5, SP_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 70,70,0,255 },
		false,
		nullptr
	);

	m_FontUIScale[UI_SPEF] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(SP_GAUGE_POSX, SP_GAUGE_POSY, 0.0f),
		D3DXVECTOR3(SP_GAUGE_WIDTH, SP_GAUGE_HEIGHT, 0.0f),
		D3DXVECTOR3(SP_GAUGE_POSX - SP_GAUGE_WIDTH*0.5, SP_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 255,255,255,220 },
		true,
		GAUGE_EF_TEX
	);
}

void CGameUI::CreateMagicGauge(void)
{
	//左上
	m_MagicGauge[0] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX - MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY - MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(0.5f, 1.0f),
		{ 255,255,255,255 },
		false,
		MAGIC_GAUGE_TEX
	);

	//左上
	m_MagicGaugeEF[0] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX - MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY - MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.5f, 0.0f),
		D3DXVECTOR2(0.5f, 1.0f),
		{ 255,255,255,255 },
		true,
		MAGIC_GAUGE_TEX
	);

	//左下
	m_MagicGauge[1] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX - MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY + MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.0f, 1.0f),
		D3DXVECTOR2(0.5f, -1.0f),
		{ 255,255,255,255 },
		false,
		MAGIC_GAUGE_TEX
	);

	//左下
	m_MagicGaugeEF[1] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX - MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY + MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.5f, 1.0f),
		D3DXVECTOR2(0.5f, -1.0f),
		{ 255,255,255,255 },
		true,
		MAGIC_GAUGE_TEX
	);

	//右下
	m_MagicGauge[2] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX + MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY + MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.5f, 1.0f),
		D3DXVECTOR2(-0.5f, -1.0f),
		{ 255,255,255,255 },
		false,
		MAGIC_GAUGE_TEX
	);

	//右下
	m_MagicGaugeEF[2] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX + MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY + MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		D3DXVECTOR2(-0.5f, -1.0f),
		{ 255,255,255,255 },
		true,
		MAGIC_GAUGE_TEX
	);

	//右上
	m_MagicGauge[3] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX + MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY - MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(0.5f, 0.0f),
		D3DXVECTOR2(-0.5f, 1.0f),
		{ 255,255,255,255 },
		false,
		MAGIC_GAUGE_TEX
	);

	//右上
	m_MagicGaugeEF[3] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(MAGIC_GAUGE_POSX + MAGIC_GAUGE_SIZE*0.5, MAGIC_GAUGE_POSY - MAGIC_GAUGE_SIZE*0.5, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_SIZE, MAGIC_GAUGE_SIZE, 0.0f),
		D3DXVECTOR3(MAGIC_GAUGE_POSX, MAGIC_GAUGE_POSY, 0.0f),
		D3DXVECTOR2(1.0f, 0.0f),
		D3DXVECTOR2(-0.5f, 1.0f),
		{ 255,255,255,255 },
		true,
		MAGIC_GAUGE_TEX
	);
}

void CGameUI::MagicGaugeEffect(void)
{
	for (int i = 0; i < MAGIC_GAUGE_OBJ_MAX; i++)
	{
		if (nullptr == m_MagicGaugeEF[i]) continue;

		if (false == m_bMagicFade) {
			m_MagicGaugeEF[i]->AddLayerA(ALPHA_SPEED);
			int Alpha = m_MagicGaugeEF[i]->GetLayerA();
			if (Alpha >= MAGIC_GAUGE_ALPHA_MAX) {
				m_MagicGaugeEF[i]->SetLayerA(MAGIC_GAUGE_ALPHA_MAX);
				m_bMagicFade = true;
			}
		}

		else {
			m_MagicGaugeEF[i]->AddLayerA(-ALPHA_SPEED);
			int Alpha = m_MagicGaugeEF[i]->GetLayerA();
			if (Alpha <= MAGIC_GAUGE_ALPHA_MIN) {
				m_MagicGaugeEF[i]->SetLayerA(MAGIC_GAUGE_ALPHA_MIN);
				m_bMagicFade = false;
			}
		}
	}
}

void CGameUI::UninitUIObj(void)
{
	//全て解放
	MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)UI_MAX, m_FontUIScale);
	MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGauge);
	MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGaugeEF);
}

void CGameUI::UpdateUIObj(void)
{
	//プレーヤーのデータをUIに反映
	int PlayerLifeNow = CLinkPlayerToUI::GetPlayerLifeNow();
	int PlayerStaminaNow = CLinkPlayerToUI::GetPlayerStaminaNow();
	float PercentLife = (float)PlayerLifeNow / PLAYER_LIFE_MAX;
	float PercentStamina = (float)PlayerStaminaNow / PLAYER_STAMINA_MAX;

	//UI変化
	m_FontUIScale[UI_HP]->SetScaleX(PercentLife);
	m_FontUIScale[UI_HPEF]->SetScaleX(PercentLife);
	m_FontUIScale[UI_SP]->SetScaleX(PercentStamina);
	m_FontUIScale[UI_SPEF]->SetScaleX(PercentStamina);

	for (int i = 0; i < (int)UI_MAX; i++) {
		if (nullptr == m_FontUIScale[i])  continue;
		m_FontUIScale[i]->Update();
	}

	//魔法ゲージ更新
	for (int i = 0; i < (int)MAGIC_GAUGE_OBJ_MAX; i++) {
		if (nullptr == m_MagicGauge[i]) continue;
		if (nullptr == m_MagicGaugeEF[i]) continue;
		if (false == m_bDrawFlag) continue;
		m_MagicGauge[i]->Update();
		m_MagicGaugeEF[i]->Update();
	}

	MagicGaugeEffect();					//魔力ゲージエフェクト
}

void CGameUI::DrawUIObj(void)
{
	//魔法ゲージ描画
	for (int i = 0; i < (int)MAGIC_GAUGE_OBJ_MAX; i++) {
		if (nullptr == m_MagicGauge[i]) continue;
		if (nullptr == m_MagicGaugeEF[i]) continue;
		if (false == m_bDrawFlag) continue;
		m_MagicGauge[i]->Draw();
		m_MagicGaugeEF[i]->Draw();
	}

	//ゲージ欄描画
	for (int i = 0; i < (int)UI_MAX; i++) {
		if (nullptr == m_FontUIScale[i]) continue;
		m_FontUIScale[i]->Draw();
	}
}