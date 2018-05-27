#include "TexManager.h"

//マクロ
#define TEXTURE_IS_nullptr (-1)

//グローバル
TEX *CTexManager::m_TexTureSlot[TEXSLOT_NUM] = {};

//すべてのテクスチャスロット初期化
HRESULT CTexManager::InitAll(void)
{
	for (int nCnt = 0; nCnt < TEXSLOT_NUM; nCnt++)
	{
		if (m_TexTureSlot[nCnt] != nullptr)
		{
			m_TexTureSlot[nCnt]->Uninit();
			delete m_TexTureSlot[nCnt];
			m_TexTureSlot[nCnt] = nullptr;
		}

		m_TexTureSlot[nCnt] = nullptr;
	}

	return S_OK;
}

//すべてのテクスチャスロット解放
void CTexManager::UninitAll(void)
{
	for (int nCnt = 0; nCnt < TEXSLOT_NUM; nCnt++)
	{
		if (m_TexTureSlot[nCnt] != nullptr)
		{
			m_TexTureSlot[nCnt]->Uninit();
			delete m_TexTureSlot[nCnt];
			m_TexTureSlot[nCnt] = nullptr;
		}
	}
}

//テクスチャをロードしIDを返す
TEX_ID CTexManager::LoadTex(const char *FilePass)
{
	if (FilePass == nullptr) {
		return TEXTURE_IS_nullptr;
	}
	for (TEX_ID IDNum = 0; IDNum < TEXSLOT_NUM; IDNum++)
	{
		if (m_TexTureSlot[IDNum] != nullptr)//スロットすでに使用される場合
		{
			const char *SlotFilePass = m_TexTureSlot[IDNum]->GetTexPass();   //ファイルパス取得
			int ValueStrcmp = strcmp(SlotFilePass, FilePass);
			if (ValueStrcmp == 0) return IDNum;
			else continue;
		}

		if (m_TexTureSlot[IDNum] == nullptr)//スロット空があったらテクスチャを入れる
		{
			CreateTexObj(IDNum, FilePass);
			return IDNum;
		}
	}

	//例外処理
	MessageBox(nullptr, "使用できるテクスチャスロットがない", "エラー", MB_OK | MB_ICONHAND);
	return TEXTURE_IS_nullptr;
}

//テクスチャ番号を渡し,テクスチャポインタを返す
LPDIRECT3DTEXTURE9 CTexManager::UseTex(TEX_ID IDNum)
{
	if (IDNum == TEXTURE_IS_nullptr) return nullptr;

	if (m_TexTureSlot[IDNum] == nullptr)
	{
		return nullptr;
	}
	return m_TexTureSlot[IDNum]->GetTexturePoint();
}

//テクスチャスロットにテクスチャを入れる
HRESULT CTexManager::CreateTexObj(TEX_ID IDNum, const char *FilePass)
{
	if (m_TexTureSlot[IDNum] != nullptr)
	{
		MessageBox(nullptr, "このテクスチャスロットもすでに使用されている", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}
	m_TexTureSlot[IDNum] = new TEX;//実体化
	m_TexTureSlot[IDNum]->SetTexPass(FilePass);//ファイルパスをセット
	m_TexTureSlot[IDNum]->CreateTexObj();//テクスチャオブジェクト生成

	return S_OK;
}

const char *CTexManager::GetTexturePass(TEX_ID IDNum)
{
	if (nullptr == m_TexTureSlot[IDNum]) return nullptr;
	return m_TexTureSlot[IDNum]->GetTexPass();
}