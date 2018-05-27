//説明 : 敵のAI動作のインタフェース
#pragma once
#ifndef CAI_ACTION_INTERFACE_H_
#define CAI_ACTION_INTERFACE_H_

//インクルードヘッダ
#include "main.h"
#include "CDebug.h"

//先方宣言
class CPlayer;
class CEnemy;

class CAIActionInterface 
{
public:
	virtual void AIModeAction(void) = 0;				//AIの振る舞い

	virtual void SetPlayer(CPlayer *pPlayer) = 0;
	virtual void SetAI(CEnemy *pPlayer) = 0;

	virtual CPlayer* GetPlayer(void) = 0;
	virtual CEnemy* GetAI(void) = 0;
};

class CNullAIAction : public CAIActionInterface
{
	virtual void AIModeAction(void) {
		CDebug::AddText("関数AIModeAction,AIのインタフェースはnullptrです\n");
	}

	virtual void SetPlayer(CPlayer *pPlayer) { pPlayer; CDebug::AddText("関数SetPlayer,AIのインタフェースはnullptrです\n"); }
	virtual void SetAI(CEnemy *pPlayer) { pPlayer; CDebug::AddText("関数SetAI,AIのインタフェースはnullptrです\n"); }

	virtual CPlayer* GetPlayer(void) { CDebug::AddText("関数GetPlayer,AIのインタフェースはnullptrです\n"); return nullptr; }
	virtual CEnemy* GetAI(void) { CDebug::AddText("関数GetAI,AIのインタフェースはnullptrです\n"); return nullptr;}
};

#endif