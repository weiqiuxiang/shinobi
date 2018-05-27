#pragma once
#ifndef CSCENE2D_INDEX_H_
#define CSCENE2D_INDEX_H_

#include "main.h"
#include "CScene2D.h"
#include "Tex.h"
#include "CINDEX_SIZE.h"

class CScene2DIndex : public CScene2D
{
public:
	virtual HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pTotalSize,const D3DXVECTOR2& PieceNum, const char *filePass);
	virtual void Update(void) override;
	virtual void Draw(void) override;
	virtual void Uninit(void) override;
private:
	void IndexVertexLock(void);

	CINDEX_SIZE m_IndexSize;				//インデックスサイズ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;	//インデックスバッファ
	INDEX_DATA m_IndexData;					//頂点数、インデックス数、面数
};

#endif