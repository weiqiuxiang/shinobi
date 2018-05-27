#include "CPlayerRollingPattern.h"
#include "CPlayer.h"

//マクロ
#define DEFAULT_COFFI (0.2f)					//デフォルト地面摩擦係数
#define SKY_COFFI (0.02f)					//空中摩擦係数

CPlayerRollingPattern::CPlayerRollingPattern(CPlayer* pPlayer, CPlayerPatternManager*pManager)
{
	//武器によるモーション初期化
	if (pPlayer != nullptr) {
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ROLLING);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ROLLING);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ROLLING_PATTERN);
	}

	m_PlayerPatternManager = pManager;
}

bool CPlayerRollingPattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	//武器によるモーション初期化
	if (pPlayer != nullptr) {
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ROLLING);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ROLLING);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ROLLING_PATTERN);
	}

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerRollingPattern::Update(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	if (ChangePattern(pPlayer) == true) {
		return;
	}
}

bool CPlayerRollingPattern::ChangePattern(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	m_nMotionTime--;

	//ニュートラルパターンに遷移
	if (m_nMotionTime < 0){
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);

		return true;
	}

	int KeyFrameNow = pPlayer->GetKeyFrameNow();			//現在実行中のキーフレーム取得
	bool bDiffKeyFrame = m_KeyFrameBefore != KeyFrameNow;	//前のキーフレームと異なるか

	//攻撃受けったらスタンパターンに入る
	if (pPlayer->GetIsBeAttackedFlag() == true && KeyFrameNow >= 6 && pPlayer->GetNoDamageTime() <= 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
	}

	//異なるなら
	if (bDiffKeyFrame) {
		m_KeyFrameBefore = KeyFrameNow;					//進行中のキーフレーム登録
		if (KeyFrameNow == 4) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
		else if (KeyFrameNow == 5) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
		else if (KeyFrameNow == 6) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
		else if (KeyFrameNow == 7) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
	}

	//摩擦係数変化
	if (KeyFrameNow < 4) {
		pPlayer->SetCoffi(SKY_COFFI);
	}
	else {
		pPlayer->SetCoffi(DEFAULT_COFFI);
	}

	return false;
}