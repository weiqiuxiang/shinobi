#include "CPlayerDeadPattern.h"
#include "CPlayer.h"
#include "CPlayerPatternManager.h"

CPlayerDeadPattern::CPlayerDeadPattern(CPlayer* pPlayer, CPlayerPatternManager* pPlayerManager)
{
	m_PlayerPatternManager = pPlayerManager;

	//モーション設定
	if (pPlayer != nullptr) {
		CPlayer::WEAPON_MODE weaponMode = pPlayer->GetWeaponMode();

		switch (weaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_DEAD);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_DEAD);
			break;
		}

		pPlayer->SetStateNow(PLAYER_DEAD_PATTERN);
	}
}

CPlayerDeadPattern::~CPlayerDeadPattern()
{
	
}

bool CPlayerDeadPattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pPlayerManager)
{
	m_PlayerPatternManager = pPlayerManager;

	//モーション設定
	if (pPlayer != nullptr) {
		CPlayer::WEAPON_MODE weaponMode = pPlayer->GetWeaponMode();

		switch (weaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_DEAD);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_DEAD);
			break;
		}

		pPlayer->SetStateNow(PLAYER_DEAD_PATTERN);
	}

	return true;
}

void CPlayerDeadPattern::Update(CPlayer* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return;
	}

	return;
}

void CPlayerDeadPattern::Draw(void)
{
	
}