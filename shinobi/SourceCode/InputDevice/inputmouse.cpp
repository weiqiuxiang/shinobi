#include "inputmouse.h"
#include "input.h"

//マクロ定義
#define LIMIT_COUNT_REPEAT (20)

//=============================================================================
// CInputMouseコンストラスタ
//=============================================================================
CInputMouse::CInputMouse()
{
	// 各ワークのクリア
	ZeroMemory(m_aMouseState, sizeof m_aMouseState);
	ZeroMemory(m_aMouseStateTrigger, sizeof m_aMouseStateTrigger);
	ZeroMemory(m_aMouseStateRelease, sizeof m_aMouseStateRelease);
	ZeroMemory(m_aMouseStateRepeat, sizeof m_aMouseStateRepeat);
	ZeroMemory(m_aMouseStateRepeatCnt, sizeof m_aMouseStateRepeatCnt);
}

//=============================================================================
// CInputMouseデストラスタ
//=============================================================================
CInputMouse::~CInputMouse()
{
}

//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT ret = S_FALSE;

	// 入力処理の初期化
	CInput::Init(hInst, hWnd);

	// デバイスの作成
	if (FAILED(m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDevice, nullptr)))
	{
		MessageBox(hWnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(m_pDIDevice->SetDataFormat(&c_dfDIMouse)))
	{
		MessageBox(hWnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）(調べる必要がある)
	if (FAILED(m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))))
	{
		MessageBox(hWnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

	ret = m_pDIDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	if (FAILED(ret)) {
		// デバイスの設定に失敗
		MessageBox(hWnd, "デバイスの設定失敗", "警告！", MB_ICONWARNING);
		return false;
	}

	// マウスへのアクセス権を獲得(入力制御開始)
	m_pDIDevice->Acquire();

	return S_OK;
}

//=============================================================================
// キーボード入力処理の開放
//=============================================================================
void CInputMouse::Uninit(void)
{
	// 入力処理の開放
	CInput::Uninit();
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void CInputMouse::Update(void)
{
	// 読取前の値を保持します
	DIMOUSESTATE zdiMouseState_bak;	// マウス情報(変化検知用)
	memcpy(&zdiMouseState_bak, &m_zdiMouseState, sizeof(zdiMouseState_bak));       //zdiMouseState_bakに古いデータを保存

	// デバイスからデータを取得
	if (SUCCEEDED(m_pDIDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_zdiMouseState)))
	{
		for (int nCnMouse = 0; nCnMouse < NUM_MOUSEKEY; nCnMouse++)
		{
			// キープレス情報を保存
			//キートリガーリリース情報の作成
			m_aMouseStateTrigger[nCnMouse] = (zdiMouseState_bak.rgbButtons[nCnMouse] ^ m_zdiMouseState.rgbButtons[nCnMouse]) & ~zdiMouseState_bak.rgbButtons[nCnMouse];                          //g_aMouseStateは前,aMouseStateは今
			m_aMouseStateRelease[nCnMouse] = (zdiMouseState_bak.rgbButtons[nCnMouse] ^ m_zdiMouseState.rgbButtons[nCnMouse]) & zdiMouseState_bak.rgbButtons[nCnMouse];                        //g_aMouseStateは前,aMouseStateは今
			m_aMouseState[nCnMouse] = m_zdiMouseState.rgbButtons[nCnMouse];

			//キーリビーと情報の生成
			if (m_zdiMouseState.rgbButtons[nCnMouse])
			{
				if (m_aMouseStateRepeatCnt[nCnMouse] < LIMIT_COUNT_REPEAT)
				{
					m_aMouseStateRepeatCnt[nCnMouse]++;
					if (m_aMouseStateRepeatCnt[nCnMouse] == 1 || m_aMouseStateRepeatCnt[nCnMouse] >= LIMIT_COUNT_REPEAT)
					{
						m_aMouseStateRepeat[nCnMouse] = m_zdiMouseState.rgbButtons[nCnMouse];
						m_aMouseStateRepeatCnt[nCnMouse] = 2;
					}

					else
					{
						m_aMouseStateRepeat[nCnMouse] = 0;
					}
				}
			}

			else
			{
				m_aMouseStateRepeat[nCnMouse] = 0;
				m_aMouseStateRepeatCnt[nCnMouse] = 0;
			}
		}
	}

	//失敗した場合はもう一度マウスへのアスセスを要求する
	else
	{
		// マウスへのアクセス権を取得
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// マウスのプレス状態を取得
//=============================================================================
bool CInputMouse::GetMousePress(int nMouse)
{
	return (m_aMouseState[nMouse] & 0x80) ? true : false;
}

//=============================================================================
// マウスのトリガー状態を取得
//=============================================================================
bool CInputMouse::GetMouseTrigger(int nMouse)
{
	return (m_aMouseStateTrigger[nMouse] & 0x80) ? true : false;
}

//=============================================================================
// マウスのリピート状態を取得
//=============================================================================
bool CInputMouse::GetMouseRepeat(int nMouse)
{
	return (m_aMouseStateRepeat[nMouse] & 0x80) ? true : false;
}

//=============================================================================
// マウスのリリ－ス状態を取得
//=============================================================================
bool CInputMouse::GetMouseRelease(int nMouse)
{
	return (m_aMouseStateRelease[nMouse] & 0x80) ? true : false;
}

//=============================================================================
//マウス状態取得
//=============================================================================
DIMOUSESTATE *CInputMouse::GetMouseStates(void)
{
	return &m_zdiMouseState;
}