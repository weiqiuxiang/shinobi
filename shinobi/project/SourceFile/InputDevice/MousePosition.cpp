#include "MousePosition.h"
#define WINDOW_FRAME_WIDTH_SIZE (7.0f)
#define WINDOW_FRAME_HEIGHT_SIZE (30.0f)

D3DXVECTOR3 CMousePosition::m_MousePos = D3DXVECTOR3(0.0f,0.0f,0.0f);

void CMousePosition::GetMousePosFromWindow(HWND hwnd)
{
	WINDOWINFO windowInfo;
	windowInfo.cbSize = sizeof(WINDOWINFO);

	GetWindowInfo(hwnd, &windowInfo);

	int wx = windowInfo.rcWindow.left;												//Window左上x座標
	int wy = windowInfo.rcWindow.top;												//Window左上y座標
	//int ww = windowInfo.rcWindow.right - windowInfo.rcWindow.left;					//Window幅
	//int wh = windowInfo.rcWindow.bottom - windowInfo.rcWindow.top;					//Window高さ

	POINT lpPoint;
	GetCursorPos(&lpPoint);																	//マウス座標取得
	m_MousePos.x = lpPoint.x - wx - WINDOW_FRAME_WIDTH_SIZE;								//マウスのウィンドウx座標計算
	m_MousePos.y = lpPoint.y - wy - WINDOW_FRAME_HEIGHT_SIZE;								//マウスのウィンドウy座標計
}