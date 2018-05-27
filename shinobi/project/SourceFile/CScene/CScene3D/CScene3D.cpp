#include "CScene3D.h"
#include "TexManager.h"
#include "CRenderer.h"

CScene3D::CScene3D() :
	m_size(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_pVtxBuffer(nullptr),
	m_DrawType(nullptr),
	m_TexID(nullptr)
{
	m_RGBA = { 255,255,255,255 };
	m_DrawType = new C3DPolygon();
}

CScene3D::CScene3D(CScene3D_Draw *pDrawType) :
	m_size(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_pVtxBuffer(nullptr),
	m_DrawType(nullptr),
	m_TexID(nullptr)
{
	m_RGBA = { 255,255,255,255 };
	m_DrawType = pDrawType;
}

CScene3D::CScene3D(const SRP &worldMtx, const D3DXVECTOR3 &pSize) :
	m_size(pSize),
	m_pVtxBuffer(nullptr),
	m_DrawType(nullptr),
	m_TexID(nullptr)
{
	m_pos = worldMtx.pos;
	m_rot = worldMtx.rot;
	m_scl = worldMtx.scl;
	m_DrawType = new C3DPolygon();
}

CScene3D::~CScene3D()
{
	if (m_TexID != nullptr)
	{
		delete m_TexID;
		m_TexID = nullptr;
	}

	if (m_DrawType != nullptr)
	{
		delete m_DrawType;
		m_DrawType = nullptr;
	}

	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}


HRESULT CScene3D::Init(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const char *filePass)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//テクスチャIDの実体化
	if (m_TexID == nullptr) {
		m_TexID = new TEX_ID;
	}

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX3D, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
	{
		MessageBox(nullptr, "頂点バッファ確保が失敗しました", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	//テクスチャ読み込み
	*m_TexID = CTexManager::LoadTex(filePass);    //テクスチャをロードする

	m_pos = worldMtx.pos;
	m_rot = worldMtx.rot;
	m_scl = worldMtx.scl;
	m_size = pSize;
	m_RGBA = { 255,255,255,255 };

	//レンダーステートに関する諸設定
	SetRenderStates();

	//頂点ロック
	if (m_DrawType != nullptr) {
		m_DrawType->VertexBufferLock(m_size, m_RGBA, m_pVtxBuffer);
	}

	return S_OK;
}

void CScene3D::Update(void)
{
	SetRenderStates();
	if (m_DrawType != nullptr) {
		m_DrawType->VertexBufferLock(m_size, m_RGBA, m_pVtxBuffer);
	}
}

void CScene3D::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	SRP SclRotPos;
	SclRotPos.pos = m_pos;
	SclRotPos.rot = m_rot;
	SclRotPos.scl = m_scl;
	m_DrawType->MtxSetting(SclRotPos, &m_WorldMtx);

	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));
	pDevice->SetFVF(FVF_VERTEX3D);
	pDevice->SetTexture(0, CTexManager::UseTex(*m_TexID));
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void CScene3D::Uninit(void)
{
	if (m_TexID != nullptr)
	{
		delete m_TexID;
		m_TexID = nullptr;
	}

	if (m_DrawType != nullptr)
	{
		delete m_DrawType;
		m_DrawType = nullptr;
	}

	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}

void CScene3D::SetRenderStates(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//レンダステート設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //引数1:設定名カリング 引数2:設定値(D3DCULL_CCW:逆時計回りを裏面をする)
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//セッター
void CScene3D::SetDrawType(CSCENE3D_TYPE drawType)
{
	if (drawType == TYPE_NORMAL)
	{
		if (m_DrawType != nullptr)
		{
			delete m_DrawType;
			m_DrawType = nullptr;
		}
		m_DrawType = new C3DPolygon();
		m_Type = TYPE_NORMAL;
		return;
	}

	if (drawType == TYPE_BILLBOARD)
	{
		if (m_DrawType != nullptr)
		{
			delete m_DrawType;
			m_DrawType = nullptr;
		}
		m_DrawType = new CBillboard();
		m_Type = TYPE_BILLBOARD;
		return;
	}

	if (drawType == TYPE_BILLBOARD_BOTTOM)
	{
		if (m_DrawType != nullptr)
		{
			delete m_DrawType;
			m_DrawType = nullptr;
		}
		m_DrawType = new CBillboardBottom();
		m_Type = TYPE_BILLBOARD_BOTTOM;
		return;
	}
}

void CScene3D::SetLayerR(int Value)
{
	if (Value < 0) { m_RGBA.r = 0; return; }
	if (Value > 255) { m_RGBA.r = 255; return; }
	m_RGBA.r = Value;
}
void CScene3D::SetLayerG(int Value)
{
	if (Value < 0) { m_RGBA.g = 0; return; }
	if (Value > 255) { m_RGBA.g = 255; return; }
	m_RGBA.g = Value;
}
void CScene3D::SetLayerB(int Value)
{
	if (Value < 0) { m_RGBA.b = 0; return; }
	if (Value > 255) { m_RGBA.b = 255; return; }
	m_RGBA.b = Value;
}
void CScene3D::SetLayerA(int Value)
{
	if (Value < 0) { m_RGBA.a = 0; return; }
	if (Value > 255) { m_RGBA.a = 255; return; }
	m_RGBA.a = Value;
}

void CScene3D::SetRGBA(const RGBA& rgba)
{
	SetLayerR(rgba.r);
	SetLayerG(rgba.g);
	SetLayerB(rgba.b);
	SetLayerA(rgba.a);
}
void CScene3D::ChangeAlpha(int ChangeValue)
{
	m_RGBA.a += ChangeValue;
	if (m_RGBA.a < 0) { m_RGBA.a = 0; return; }
	if (m_RGBA.a > 255) { m_RGBA.a = 255; return; }
}