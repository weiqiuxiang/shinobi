#include "CScene2DIndex.h"
#include "CRenderer.h"
#include "TexManager.h"

HRESULT CScene2DIndex::Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pTotalSize, const D3DXVECTOR2& PieceNum, const char *filePass)
{
	/********************CScene2Dの部分の初期化************************/
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	if (m_TexID == nullptr) m_TexID = new TEX_ID;   //テクスチャIDの実体化

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX2D, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
	{
		MessageBox(nullptr, "頂点バッファ確保が失敗しました", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	//テクスチャ読み込み
	*m_TexID = CTexManager::LoadTex(filePass);    //テクスチャをロードする

	m_pos = Pos;
	m_size = pTotalSize;
	m_RGBA = { 255,255,255,255 };
	m_IndexSize.m_PieceNum = PieceNum;			//ピースサイズの値を保存

	//pieceサイズ計算
	m_IndexSize.m_PieceSize.x = pTotalSize.x / m_IndexSize.m_PieceNum.x;
	m_IndexSize.m_PieceSize.y = pTotalSize.y / m_IndexSize.m_PieceNum.y;

	return S_OK;
}