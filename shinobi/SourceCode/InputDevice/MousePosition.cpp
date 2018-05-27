#include "MousePosition.h"
#define WINDOW_FRAME_WIDTH_SIZE (7.0f)
#define WINDOW_FRAME_HEIGHT_SIZE (30.0f)

D3DXVECTOR3 CMousePosition::m_MousePos = D3DXVECTOR3(0.0f,0.0f,0.0f);

void CMousePosition::GetMousePosFromWindow(HWND hwnd)
{
	WINDOWINFO windowInfo;
	windowInfo.cbSize = sizeof(WINDOWINFO);

	GetWindowInfo(hwnd, &windowInfo);

	int wx = windowInfo.rcWindow.left;												//Window����x���W
	int wy = windowInfo.rcWindow.top;												//Window����y���W
	//int ww = windowInfo.rcWindow.right - windowInfo.rcWindow.left;					//Window��
	//int wh = windowInfo.rcWindow.bottom - windowInfo.rcWindow.top;					//Window����

	POINT lpPoint;
	GetCursorPos(&lpPoint);																	//�}�E�X���W�擾
	m_MousePos.x = lpPoint.x - wx - WINDOW_FRAME_WIDTH_SIZE;								//�}�E�X�̃E�B���h�Ex���W�v�Z
	m_MousePos.y = lpPoint.y - wy - WINDOW_FRAME_HEIGHT_SIZE;								//�}�E�X�̃E�B���h�Ey���W�v
}