#include "CPlayerStunPattern.h"
#include "CPlayer.h"
#include "CPlayerPatternManager.h"
#include "CPlayerControl.h"
#include "common.h"

#define STUN_COFFI (0.1f)	//スタン時の抵抗係数

CPlayerStunPattern::CPlayerStunPattern(CPlayer* pPlayer, CPlayerPatternManager* pPlayerManager)
{
	m_PlayerPatternManager = pPlayerManager;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//スタンパラメータ設定
	StunSetting(pPlayer);

	//モーション設定
	if (pPlayer != nullptr) {
		pPlayer->DisableWeaponHitCircle(true,true);
		pPlayer->SetCoffi(STUN_COFFI);

		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_WSWORD_STUN);
			break;

		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_CLAYMORE_STUN);
			break;
		}

		pPlayer->SetStateNow(PLAYER_STUN_PATTERN);
	}
}

CPlayerStunPattern::~CPlayerStunPattern()
{
	
}

bool CPlayerStunPattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pPlayerManager)
{
	m_PlayerPatternManager = pPlayerManager;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//スタンパラメータ設定
	StunSetting(pPlayer);
	
	//モーション設定
	if (pPlayer != nullptr) {
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetCoffi(STUN_COFFI);

		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_WSWORD_STUN);
			break;

		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_CLAYMORE_STUN);
			break;
		}

		pPlayer->SetStateNow(PLAYER_STUN_PATTERN);
	}

	return true;
}

void CPlayerStunPattern::Update(CPlayer* pPlayer)
{
	//ヌルチェック
	if(pPlayer == nullptr){
		return;
	}
	if (m_PlayerPatternManager == nullptr) {
		return;
	}
	
	//例外処理
	if (pPlayer->GetDamageType() == ENEMY_ATTACK_TYPE_NONE) {
		CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPattern != nullptr) {
			pPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPattern);
		return;
	}

	if (pPlayer->GetPlayerLife() <= 0) {
		pPlayer->SetIsBeAttackedFlag(false);
		CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_DEAD_PATTERN);
		if (pPattern != nullptr) {
			pPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPattern);
		return;
	}

	//モーション時間減少
	if (m_StunMotionTime >= 0) {
		m_StunMotionTime--;
	}
	else {
		m_DecoveryTime--;
	}

	//状態遷移,lifeは0だったら死亡状態に遷移、lifeは0以上だったら攻撃状態に遷移
	if (m_DecoveryTime < 0) {
		pPlayer->SetCoffi(0.2f);
		pPlayer->SetIsBeAttackedFlag(false);
		pPlayer->SetState(m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN));
		return;
	}

	if (pPlayer->GetIsBeAttackedFlag() == true) {
		if (pPlayer->GetNoDamageTime() <= 0) {
			pPlayer->SetIsBeAttackedFlag(false);
			pPlayer->SetNoDamageTime(3);
			CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
			if (pPattern != nullptr) {
				pPattern->InitParameter(pPlayer,m_PlayerPatternManager);
			}
			pPlayer->SetState(pPattern);
			return;
		}

		if (pPlayer->GetPlayerLife() <= 0) {
			pPlayer->SetIsBeAttackedFlag(false);
			CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_DEAD_PATTERN);
			if (pPattern != nullptr) {
				pPattern->InitParameter(pPlayer, m_PlayerPatternManager);
			}
			pPlayer->SetState(pPattern);
			return;
		}
	}
}

void CPlayerStunPattern::Draw(void)
{
	
}

void CPlayerStunPattern::StunSetting(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//スタン方向取得
	D3DXVECTOR3 StunDir = pPlayer->GetStunDir();

	//プレーヤーの攻撃タイプに応じてダメージを受け、一定距離に飛ばされる
	switch (pPlayer->GetDamageType())
	{
	case PLAYER_ATTACK_TYPE_NONE:
		break;
	case DOG_BITE_ATTACK:
		pPlayer->AddPower(0.4f * StunDir);		//スタン移動パワー設定
		pPlayer->SetNoDamageTime(60);			//無敵時間設定
		pPlayer->AddLife(-200);					//HP減少
		m_DecoveryTime = 10;					//スタン時間設定
		break;
	case DOG_JUMP_ATTACK:
		pPlayer->AddPower(0.6f * StunDir);		//スタン移動パワー設定
		pPlayer->SetNoDamageTime(120);			//無敵時間設定
		pPlayer->AddLife(-300);					//HP減少
		m_DecoveryTime = 20;					//スタン時間設定
		break;
	default:
		break;
	}
}