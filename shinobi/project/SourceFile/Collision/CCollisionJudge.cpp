#include "CCollisionJudge.h"
#include "CCollisionPlayerAndEnemy.h"
#include "CAllObj3DHitMesh.h"
#include "CEnemyStorage.h"

CPlayer *CCollisionJudge::m_Player = nullptr;

void CCollisionJudge::Update(void)
{
	//�v���[���[�ƓG�̓��蔻��
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for (; it != itEnd; ++it) {
		CColliPlayerEnemy::SetPlayer(m_Player);
		CColliPlayerEnemy::SetEnemy(*it);
		CColliPlayerEnemy::GetPlayerEnemyDataAndColliCheck();			//�G�ƃv���[���[�Ƃ̓��蔻��
	}

	//�G�ƓG�̏��˔���
	it = CEnemyStorage::GetEnemiesListBegin();
	for (int i = 0; it != itEnd; ++it,++i) {
		CColliPlayerEnemy::SetEnemy(*it);
		auto itOther = it;
		for (++itOther; itOther != itEnd; ++itOther) {
			CColliPlayerEnemy::SetEnemyOther(*itOther);
			CColliPlayerEnemy::GetEnemiesDataAndColliCheck();			//�G�ƓG�̓��蔻��
		}
	}
}