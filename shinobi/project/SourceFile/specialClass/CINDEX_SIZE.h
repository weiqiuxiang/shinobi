#pragma once
#ifndef CINDEX_SIZE_H_
#define CINDEX_SIZE_H_

#include "main.h"

class CINDEX_SIZE
{
public:
	CINDEX_SIZE();
	CINDEX_SIZE(const D3DXVECTOR2& PieceNum, const D3DXVECTOR2& PieceSize);
	~CINDEX_SIZE();
	D3DXVECTOR2 m_PieceNum;			//���}�X���Əc�}�X��
	D3DXVECTOR2 m_PieceSize;		//���̊Ԋu�Əc�̊Ԋu
};

#endif