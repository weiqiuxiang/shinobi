#include "XInputControl.h"
#include "CDebug.h"
#include <string.h>
#include <stdio.h>

//マクロ
#define INPUT_DEADZONE  ( 0.60f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

void XInputControl::Init(bool UseDeadZone)
{
	m_bDeadZoneOn = UseDeadZone;

	m_wButtonPress = 0;
	m_wButtonTrigger = 0;
	m_wButtonRelease = 0;
}

HRESULT XInputControl::Update(int PadNumber)
{
	DWORD dwResult;

	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState(PadNumber, &m_state);

	if (dwResult == ERROR_SUCCESS) 
		m_bConnected = true;
	else
		m_bConnected = false;

	UpdateState();

	return S_OK;
}

void XInputControl::UpdateState(void)
{
	if (m_bConnected) {
		//スティックの入力
		if (m_bDeadZoneOn)
		{
			// Zero value if thumbsticks are within the dead zone 
			if ((m_state.Gamepad.sThumbLX < INPUT_DEADZONE &&
				m_state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
				(m_state.Gamepad.sThumbLY < INPUT_DEADZONE &&
					m_state.Gamepad.sThumbLY > -INPUT_DEADZONE))
			{
				m_state.Gamepad.sThumbLX = 0;
				m_state.Gamepad.sThumbLY = 0;
			}

			if ((m_state.Gamepad.sThumbRX < INPUT_DEADZONE &&
				m_state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
				(m_state.Gamepad.sThumbRY < INPUT_DEADZONE &&
					m_state.Gamepad.sThumbRY > -INPUT_DEADZONE))
			{
				m_state.Gamepad.sThumbRX = 0;
				m_state.Gamepad.sThumbRY = 0;
			}
		}

		WORD wButtons = m_state.Gamepad.wButtons;
		m_wButtonTrigger = (m_wButtonPress ^ wButtons) & wButtons;
		m_wButtonRelease = (m_wButtonPress ^ wButtons) & ~wButtons;
		m_wButtonPress = wButtons;
	}
}

void XInputControl::RenderInput(void)
{
	char buf[1024] = {};
	sprintf(buf,
		"ButtonsPress: %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		(m_wButtonPress & XINPUT_GAMEPAD_DPAD_UP) ? "DPAD_UP " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_DPAD_DOWN) ? "DPAD_DOWN " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_DPAD_LEFT) ? "DPAD_LEFT " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_DPAD_RIGHT) ? "DPAD_RIGHT " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_START) ? "START " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_BACK) ? "BACK " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_LEFT_THUMB) ? "LEFT_THUMB " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_RIGHT_THUMB) ? "RIGHT_THUMB " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_LEFT_SHOULDER) ? "LEFT_SHOULDER " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? "RIGHT_SHOULDER " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_A) ? "A " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_B) ? "B " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_X) ? "X " : "",
		(m_wButtonPress & XINPUT_GAMEPAD_Y) ? "Y " : ""
		);
	CDebug::AddText(buf);

	sprintf(buf,
		"ButtonTrigger: %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		(m_wButtonTrigger & XINPUT_GAMEPAD_DPAD_UP) ? "DPAD_UP " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_DPAD_DOWN) ? "DPAD_DOWN " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_DPAD_LEFT) ? "DPAD_LEFT " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_DPAD_RIGHT) ? "DPAD_RIGHT " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_START) ? "START " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_BACK) ? "BACK " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_LEFT_THUMB) ? "LEFT_THUMB " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_RIGHT_THUMB) ? "RIGHT_THUMB " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_LEFT_SHOULDER) ? "LEFT_SHOULDER " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? "RIGHT_SHOULDER " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_A) ? "A " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_B) ? "B " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_X) ? "X " : "",
		(m_wButtonTrigger & XINPUT_GAMEPAD_Y) ? "Y " : ""
	);
	CDebug::AddText(buf);

	sprintf(buf,
		"ButtonRelease: %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		(m_wButtonRelease & XINPUT_GAMEPAD_DPAD_UP) ? "DPAD_UP " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_DPAD_DOWN) ? "DPAD_DOWN " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_DPAD_LEFT) ? "DPAD_LEFT " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_DPAD_RIGHT) ? "DPAD_RIGHT " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_START) ? "START " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_BACK) ? "BACK " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_LEFT_THUMB) ? "LEFT_THUMB " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_RIGHT_THUMB) ? "RIGHT_THUMB " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_LEFT_SHOULDER) ? "LEFT_SHOULDER " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? "RIGHT_SHOULDER " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_A) ? "A " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_B) ? "B " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_X) ? "X " : "",
		(m_wButtonRelease & XINPUT_GAMEPAD_Y) ? "Y " : ""
	);
	CDebug::AddText(buf);

	sprintf(buf,
		"LeftStick: %.2f / %.2f\n",
		GetLeftThumbStickValueAnalog().x,
		GetLeftThumbStickValueAnalog().y
		);
	CDebug::AddText(buf);

	sprintf(buf,
		"RightStick: %.2f / %.2f\n",
		GetRightThumbStickValueAnalog().x,
		GetRightThumbStickValueAnalog().y
	);
	CDebug::AddText(buf);

	sprintf(buf,
		"TriggerValue: %d / %d\n",
		GetLeftTriggerValue(),
		GetRightTriggerValue()
	);
	CDebug::AddText(buf);
}