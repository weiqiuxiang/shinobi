#include <algorithm>
#include "CPlayerPatternManager.h"
#include "CPlayerPattern.h"
#include "CPlayerNeutralPattern.h"
#include "CPlayerAttack01Pattern.h"
#include "CPlayerAttack02Pattern.h"
#include "CPlayerAttack03Pattern.h"
#include "CPlayerAttack04Pattern.h"
#include "CPlayerRollingPattern.h"
#include "CPlayerStepPattern.h"
#include "CPlayerChangeWeaponShapePattern.h"
#include "CPlayerStunPattern.h"
#include "CPlayerDeadPattern.h"

CPlayerPatternManager::CPlayerPatternManager() 
{
	std::for_each(m_PatternArray.begin(), m_PatternArray.end(),
		[](CPlayerPattern* pPlayerPattern) {pPlayerPattern = nullptr; });
}

CPlayerPatternManager::~CPlayerPatternManager() 
{
	
}

//���ׂẴp�^�[���C���X�^���X��
bool CPlayerPatternManager::InitAllInstance(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}

	//�z��̃N���A
	int ArrayNum = m_PatternArray.size();
	if(ArrayNum != 0){
		std::for_each(m_PatternArray.begin(),m_PatternArray.end(),
			[](CPlayerPattern* pPlayerPattern) {
			if (pPlayerPattern != nullptr) {
				delete pPlayerPattern;
				pPlayerPattern = nullptr;
			}
		});
	}

	CPlayerPattern* pPlayerPattern = nullptr;

	//�j���[�g�����p�^�[��
	pPlayerPattern = new CPlayerNeutralPattern(pPlayer,this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_NEUTRAL_PATTERN] = pPlayerPattern;

	//attack01�p�^�[��
	pPlayerPattern = new CPlayerAttack01Pattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_ATTACK01_PATTERN] = pPlayerPattern;

	//attack02�p�^�[��
	pPlayerPattern = new CPlayerAttack02Pattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_ATTACK02_PATTERN] = pPlayerPattern;

	//attack03�p�^�[��
	pPlayerPattern = new CPlayerAttack03Pattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_ATTACK03_PATTERN] = pPlayerPattern;

	//attack04�p�^�[��
	pPlayerPattern = new CPlayerAttack04Pattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_ATTACK04_PATTERN] = pPlayerPattern;

	//���[�����O�p�^�[��
	pPlayerPattern = new CPlayerRollingPattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_ROLLING_PATTERN] = pPlayerPattern;

	//�X�e�b�v�p�^�[��
	pPlayerPattern = new CPlayerStepPattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_STEP_PATTERN] = pPlayerPattern;

	//����ό`�p�^�[��
	pPlayerPattern = new CPlayerChangeWeaponShapePattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_CHANGE_WEAPON_SHAPE_PATTERN] = pPlayerPattern;
	
	//�X�^���p�^�[��
	pPlayerPattern = new CPlayerStunPattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_STUN_PATTERN] = pPlayerPattern;

	//���S�p�^�[��
	pPlayerPattern = new CPlayerDeadPattern(pPlayer, this);
	if (pPlayerPattern == nullptr) {
		return false;
	}
	m_PatternArray[PLAYER_DEAD_PATTERN] = pPlayerPattern;

	return true;
}

CPlayerPattern* CPlayerPatternManager::GetPattern(int PatternNum) 
{
	if(PatternNum < 0 || PatternNum >= PLAYER_PATTERN_MAX){
		_RPT0(0,"[CPlayerPatternManager.cpp][GetPattern]����PatternNum�̒l�͔͈͊O\n");
		return nullptr;
	}

	return m_PatternArray[PatternNum];
}

void CPlayerPatternManager::UninitAllInstance(void)
{
	int ArrayNum = m_PatternArray.size();
	if (ArrayNum != 0) {
		std::for_each(m_PatternArray.begin(), m_PatternArray.end(),
			[](CPlayerPattern* pPlayerPattern) {
			if (pPlayerPattern != nullptr) {
				delete pPlayerPattern;
				pPlayerPattern = nullptr;
			}
		});
	}
}