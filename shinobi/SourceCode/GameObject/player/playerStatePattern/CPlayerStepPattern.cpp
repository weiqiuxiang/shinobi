#include "CPlayerStepPattern.h"
#include "CPlayer.h"
#include "SceneTransition.h"

//マクロ
#define DEFAULT_COFFI (0.2f)					//デフォルト地面摩擦係数
#define SKY_COFFI (0.02f)					//空中摩擦係数

CPlayerStepPattern::CPlayerStepPattern(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	m_nMotionTime = 0;
	MotionSet(pPlayer);

	m_PlayerPatternManager = pManager;
}

bool CPlayerStepPattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	m_nMotionTime = 0;
	MotionSet(pPlayer);

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerStepPattern::Update(CPlayer* pPlayer)
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

void CPlayerStepPattern::MotionSet(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		return;
	}

	pPlayer->SetStateNow(PLAYER_STEP_PATTERN);

	//ステップタイプを決める
	typedef enum {
		STEP_FORWORD = 0,
		STEP_BACK,
		STEP_LEFT,
		STEP_RIGHT
	}STEP_MOTION_TYPE;
	STEP_MOTION_TYPE StepType = STEP_FORWORD;

	D3DXVECTOR2 AnalogInputValue = CSceneTransition::GetXInput()->GetLeftThumbStickValueAnalog();

	float StickValueX = fabs(AnalogInputValue.x);
	float StickValueY = fabs(AnalogInputValue.y);
	if (StickValueX > StickValueY) {
		if (AnalogInputValue.x < 0) {
			StepType = STEP_LEFT;
		}
		else {
			StepType = STEP_RIGHT;
		}
	}
	else {
		if (AnalogInputValue.y > 0) {
			StepType = STEP_FORWORD;
		}
		else {
			StepType = STEP_BACK;
		}
	}
	if (D3DXVec2Length(&AnalogInputValue) < 0.1f) {
		StepType = STEP_BACK;
	}

	//ステップモーション変更
	switch (pPlayer->GetWeaponMode())
	{
	case CPlayer::WEAPON_WSWORD:
		switch (StepType) {
		case STEP_FORWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_FORWARD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_FORWARD);
			break;
		case STEP_BACK:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_BACK);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_BACK);
			break;
		case STEP_LEFT:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_LEFT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_LEFT);
			break;
		case STEP_RIGHT:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_RIGHT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_RIGHT);
			break;
		}
		break;
	case CPlayer::WEAPON_CLAYMORE:
		switch (StepType) {
		case STEP_FORWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_FORWARD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_FORWARD);
			break;
		case STEP_BACK:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_BACK);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_BACK);
			break;
		case STEP_LEFT:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_LEFT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_LEFT);
			break;
		case STEP_RIGHT:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_RIGHT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_RIGHT);
			break;
		}
		break;
	}
}

bool CPlayerStepPattern::ChangePattern(CPlayer* pPlayer)
{
	if(pPlayer == nullptr)
	{
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	m_nMotionTime--;				//モーション時間減少

	//ニュートラルパターンに遷移
	if (m_nMotionTime < 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);

		return true;
	}

	int KeyFrameNow = pPlayer->GetKeyFrameNow();		//現在実行中のキーフレーム取得

	//攻撃受けったらスタンパターンに入る
	if (pPlayer->GetIsBeAttackedFlag() == true && KeyFrameNow >= 3 && pPlayer->GetNoDamageTime() <= 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
	}

	//摩擦係数変化
	int CoffiChangeKeyFrame = pPlayer->GetMotionTotalKeyFrame(pPlayer->GetMotionNow()) - 4;
	if (KeyFrameNow <= CoffiChangeKeyFrame) pPlayer->SetCoffi(SKY_COFFI);					//摩擦係数変更のキーフレームより小さい
	else {
		PlayerGoalDir(pPlayer);						//プレーヤーのゴール向き設定
		PlayerFaceDir(pPlayer);						//プレーヤーの向き設定
		pPlayer->SetCoffi(DEFAULT_COFFI);
	}

	return false;
}