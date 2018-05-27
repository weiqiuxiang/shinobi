#pragma once
#ifndef SHOW_DEBUG_FLAG_H_
#define SHOW_DEBUG_FLAG_H_

#include "main.h"

class CShowDebugFlag
{
public:
	//ゲッター
	static bool GetShowDebugFlag(void) { return m_bShowDebugFlag; }
	static bool GetShowDebugFlagLayer2(void) { return m_bShowDebugFlagLayer2; }

	//セッター
	static void SetDebugFlag(bool Flag) { m_bShowDebugFlag = Flag; }
	static void SetDebugFlagLayer2(bool Flag) { m_bShowDebugFlagLayer2 = Flag; }
private:
	static bool m_bShowDebugFlag;
	static bool m_bShowDebugFlagLayer2;
};

#endif