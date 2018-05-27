#pragma once
#ifndef MOUSEPOSITION_H_
#define MOUSEPOSITION_H_

#include "main.h"

class CMousePosition
{
public:
	static void GetMousePosFromWindow(HWND hwnd);
	static D3DXVECTOR3 GetMousePos(void) { return m_MousePos; }
private:
	static D3DXVECTOR3 m_MousePos;
};

#endif