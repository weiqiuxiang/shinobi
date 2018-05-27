#ifndef XINPUT_H_
#define XINPUT_H_

#include "main.h"

#define STICK_VALUE_MAX (32767)

class XInputControl
{
public:
	void Init(bool UseDeadZone);
	HRESULT Update(int PadNumber = 0);
	void UpdateState(void);
	void RenderInput(void);

	//ゲッター
	D3DXVECTOR2 GetLeftThumbStickValue(void) { return D3DXVECTOR2(m_state.Gamepad.sThumbLX, m_state.Gamepad.sThumbLY); }
	D3DXVECTOR2 GetRightThumbStickValue(void) { return D3DXVECTOR2(m_state.Gamepad.sThumbRX, m_state.Gamepad.sThumbRY); }
	D3DXVECTOR2 GetLeftThumbStickValueAnalog(void) {	//値範囲-1.0f~1.0f
		float X = (float)m_state.Gamepad.sThumbLX / STICK_VALUE_MAX;
		float Y = (float)m_state.Gamepad.sThumbLY / STICK_VALUE_MAX;
		X = max(-1.0f, X); X = min(1.0f, X);
		Y = max(-1.0f, Y); Y = min(1.0f, Y);
		return D3DXVECTOR2(X, Y);
	}
	D3DXVECTOR2 GetRightThumbStickValueAnalog(void) {	//値範囲-1.0f~1.0f
		float X = (float)m_state.Gamepad.sThumbRX / STICK_VALUE_MAX;
		float Y = (float)m_state.Gamepad.sThumbRY / STICK_VALUE_MAX;
		X = max(-1.0f, X); X = min(1.0f, X);
		Y = max(-1.0f, Y); Y = min(1.0f, Y);
		return D3DXVECTOR2(X, Y);
	}
	int GetLeftTriggerValue(void) { return m_state.Gamepad.bLeftTrigger; }
	int GetRightTriggerValue(void) { return m_state.Gamepad.bRightTrigger; }

	bool GetHoldButton(WORD Key) {
		return (m_wButtonPress & Key) ? true : false;
	}
	bool GetTriggerButton(WORD Key) {
		return (m_wButtonTrigger & Key) ? true : false;
	}
	bool GetReleaseButton(WORD Key) {
		return (m_wButtonRelease & Key) ? true : false;
	}
	//bool GetRepeatButton(int Key);
private:
	XINPUT_STATE m_state;
	WORD m_wButtonPress;
	WORD m_wButtonTrigger;
	WORD m_wButtonRelease;
	bool m_bConnected;
	bool m_bDeadZoneOn;			//デットゾーンオン/オフ
};

#endif