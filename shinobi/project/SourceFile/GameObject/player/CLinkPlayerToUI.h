#pragma once
//ファイル名 : CLinkPlayerToUI.h
//説明 : 仲介役,PlayerのデータとUIのやり取りのに使います

#ifndef _CLINK_PLAYER_TO_UI_H_
#define _CLINK_PLAYER_TO_UI_H_

class CLinkPlayerToUI
{
public:
	//セッター
	static void SetPlayerLifeNow(int PlayerLife) { m_PlayerLifeNow = PlayerLife; }
	static void SetPlayerStaminaNow(int PlayerStamina) { m_PlayerStaminaNow = PlayerStamina; }

	//ゲッター
	static int GetPlayerLifeNow(void) { return m_PlayerLifeNow; }
	static int GetPlayerStaminaNow(void) { return m_PlayerStaminaNow; }

private:
	static int m_PlayerLifeNow;
	static int m_PlayerStaminaNow;
};

#endif