#include "CPlayerChangeWeaponShapePattern.h"
#include "CPlayer.h"

CPlayerChangeWeaponShapePattern::CPlayerChangeWeaponShapePattern(CPlayer* pPlayer, CPlayerPatternManager*pManager)
{
	if (pPlayer != nullptr) {
		//武器によるニュートラルモーション設定
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_CHANGE_WEAPON_SHAPE_PATTERN);
	}

	m_KeyFrameBefore = 0;

	m_PlayerPatternManager = pManager;
}

bool CPlayerChangeWeaponShapePattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	if (pPlayer != nullptr) {
		//武器によるニュートラルモーション設定
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_CHANGE_WEAPON_SHAPE_PATTERN);
	}

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerChangeWeaponShapePattern::Update(CPlayer* pPlayer) 
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	//パターン変更
	if (ChangePattern(pPlayer) == true) {
		return;
	}
}

bool CPlayerChangeWeaponShapePattern::ChangePattern(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return false;
	}

	if(m_PlayerPatternManager == nullptr){
		return false;
	}

	//攻撃受けったらスタンパターンに入る
	if (pPlayer->GetIsBeAttackedFlag() == true && pPlayer->GetNoDamageTime() <= 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
	}

	m_nMotionTime--;
	int MotionNow = pPlayer->GetWeaponLeftMotionNow();			//現在実行中のモーション取得

	//ニュートラルパターンに戻る
	if (m_nMotionTime <= 0)
	{
		if (MotionNow == (int)CWeapon::MOTION_DEFAULT) {
			pPlayer->SetWeaponMode(CPlayer::WEAPON_WSWORD);
		}
		else {
			pPlayer->SetWeaponMode(CPlayer::WEAPON_CLAYMORE);
		}

		//ニュートラルパターンに戻る
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	int KeyFrameNow = pPlayer->GetKeyFrameNow();			//現在実行中のキーフレーム取得
	bool bDiffKeyFrame = m_KeyFrameBefore != KeyFrameNow;	//前のキーフレームと異なるか

	if (bDiffKeyFrame) {
		m_KeyFrameBefore = KeyFrameNow;					//進行中のキーフレーム登録
		if (KeyFrameNow == 1) {							//キーフレームが1の時武器変形行う
			if (MotionNow == (int)CWeapon::MOTION_DEFAULT) {
				pPlayer->SetWeaponLeftPara(CWeapon::MOTION_CHANGE, D3DXVECTOR3(0.3f, 0.0f, 0.1f), 8);
				pPlayer->SetWeaponRightPara(CWeapon::MOTION_CHANGE, D3DXVECTOR3(0.4f, 0.0f, 0.1f), 8);
			}
			else {
				pPlayer->SetWeaponLeftPara(CWeapon::MOTION_DEFAULT, D3DXVECTOR3(-0.25f, 0.0f, 0.1f), 5);
				pPlayer->SetWeaponRightPara(CWeapon::MOTION_DEFAULT, D3DXVECTOR3(0.25f, 0.0f, 0.1f), 8);
			}
		}
	}

	return false;
}