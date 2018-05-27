//=============================================================================
//
// 入力処理 [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	COUNT_WAIT_REPEAT	(20)	// リピート待ち時間

//*****************************************************************************
// 静的変数
//*****************************************************************************
LPDIRECTINPUT8	CInput::m_pDInput = nullptr;	// DirectInputオブジェクト

//=============================================================================
// CInputコンストラスタ
//=============================================================================
CInput::CInput()
{
	m_pDIDevice = nullptr;
}

//=============================================================================
// CInputデストラスタ
//=============================================================================
CInput::~CInput()
{
}

//=============================================================================
// 入力処理の初期化処理
//=============================================================================
HRESULT CInput::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_FALSE;
	hWnd;			//ダミー参照

	// DirectInputオブジェクトの作成
	if(!m_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, nullptr);
	}

	return hr;
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
#if 0	// ---> 純粋仮想関数化
HRESULT CInput::Update(void)
{
	return S_OK;
}
#endif

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void CInput::Uninit(void)
{
	if(m_pDIDevice)
	{// デバイスオブジェクトの開放
		m_pDIDevice->Unacquire();

		m_pDIDevice->Release();
		m_pDIDevice = nullptr;
	}

	if(m_pDInput)
	{// DirectInputオブジェクトの開放
		m_pDInput->Release();
		m_pDInput = nullptr;
	}
}

//=============================================================================
// CInputKeyboardコンストラスタ
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	// 各ワークのクリア
	ZeroMemory(m_aKeyState, sizeof m_aKeyState);
	ZeroMemory(m_aKeyStateTrigger, sizeof m_aKeyStateTrigger);
	ZeroMemory(m_aKeyStateRelease, sizeof m_aKeyStateRelease);
	ZeroMemory(m_aKeyStateRepeat, sizeof m_aKeyStateRepeat);
	ZeroMemory(m_aKeyStateRepeatCnt, sizeof m_aKeyStateRepeatCnt);
}

//=============================================================================
// CInputKeyboardデストラスタ
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{
}

//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// 入力処理の初期化
	CInput::Init(hInst, hWnd);

	// デバイスオブジェクトを作成
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, nullptr);
	if(FAILED(hr) || m_pDIDevice == nullptr)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// m_pDIDeviceアクセス権を獲得(入力制御開始)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// キーボード入力処理の開放
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// 入力処理の開放
	CInput::Uninit();
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void CInputKeyboard::Update(void)
{
	HRESULT hr;
	BYTE aKeyState[NUM_KEY_MAX];

	if(!m_pDIDevice)
	{
		return;
	}

	// デバイスからデータを取得
	hr = m_pDIDevice->GetDeviceState(sizeof(aKeyState), aKeyState);
	if(SUCCEEDED(hr))
	{
		for(int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & ~aKeyState[nCntKey];
			m_aKeyStateRepeat[nCntKey] = m_aKeyStateTrigger[nCntKey];

			if(aKeyState[nCntKey])
			{
				m_aKeyStateRepeatCnt[nCntKey]++;
				if(m_aKeyStateRepeatCnt[nCntKey] >= COUNT_WAIT_REPEAT)
				{
					m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
				}
			}
			else
			{
				m_aKeyStateRepeatCnt[nCntKey] = 0;
				m_aKeyStateRepeat[nCntKey] = 0;
			}

			m_aKeyState[nCntKey] = aKeyState[nCntKey];
		}
	}
	else
	{
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// キーボードデータ取得(プレス)
//=============================================================================
bool CInputKeyboard::GetKeyPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true: false;
}

//=============================================================================
// キーボードデータ取得(トリガー)
//=============================================================================
bool CInputKeyboard::GetKeyTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードデータ取得(リリース)
//=============================================================================
bool CInputKeyboard::GetKeyRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードデータ取得(リピート)
//=============================================================================
bool CInputKeyboard::GetKeyRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードデータ削除(トリガー)
//=============================================================================
void CInputKeyboard::FlushKeyTrigger(int nKey)
{
	m_aKeyStateTrigger[nKey] = 0;
}

