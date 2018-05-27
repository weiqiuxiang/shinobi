#include "CCollisionJudge.h"
#include "CCollisionPlayerAndEnemy.h"
#include "CAllObj3DHitMesh.h"
#include "CEnemyStorage.h"

CPlayer *CCollisionJudge::m_Player = nullptr;

void CCollisionJudge::Update(void)
{
	//ƒvƒŒ[ƒ„[‚Æ“G‚Ì“–‚è”»’è
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for (; it != itEnd; ++it) {
		CColliPlayerEnemy::SetPlayer(m_Player);
		CColliPlayerEnemy::SetEnemy(*it);
		CColliPlayerEnemy::GetPlayerEnemyDataAndColliCheck();			//“G‚ÆƒvƒŒ[ƒ„[‚Æ‚Ì“–‚è”»’è
	}

	//“G‚Æ“G‚Ì‰“Ë”»’è
	it = CEnemyStorage::GetEnemiesListBegin();
	for (int i = 0; it != itEnd; ++it,++i) {
		CColliPlayerEnemy::SetEnemy(*it);
		auto itOther = it;
		for (++itOther; itOther != itEnd; ++itOther) {
			CColliPlayerEnemy::SetEnemyOther(*itOther);
			CColliPlayerEnemy::GetEnemiesDataAndColliCheck();			//“G‚Æ“G‚Ì“–‚è”»’è
		}
	}
}