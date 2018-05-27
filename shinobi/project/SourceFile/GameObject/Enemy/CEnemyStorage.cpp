#include "CEnemyStorage.h"
#include <iostream>
#include <iterator>
#include "CCollisionEnemyAndModel.h"
#include "CMeshFieldManager.h"

D3DXVECTOR3 CEnemyStorage::m_EnemyBodyPosOld[COLLISION_ENMEY_MODEL_MAX];
std::list<CEnemy*> CEnemyStorage::m_Enemies;
CPlayer *CEnemyStorage::m_Player = nullptr;

void CEnemyStorage::InitAll(void) 
{
	m_Player = nullptr;

	//全要素削除
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end();++it) {
		if ((*it) != nullptr) {
			(*it)->Uninit();
			delete (*it);
			(*it) = nullptr;
		}
	}
	m_Enemies.clear();			//リストクリア
}

void CEnemyStorage::UninitAll(void) 
{
	//全要素削除
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			(*it)->Uninit();
			delete (*it);
			(*it) = nullptr;
		}
	}
}

void CEnemyStorage::UpdateAll(void) 
{
	//リスト全更新
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			if (nullptr == (*it)->GetPlayer()) (*it)->SetPlayer(m_Player);		//プレーヤーポインタがヌルだったら再取得
			(*it)->Update();
		}
	}
}

void CEnemyStorage::UpdateEditorAll(void)
{
	//リスト全更新
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			(*it)->UpdateEditor();
		}
	}
}

void CEnemyStorage::UpdateAllEnemyPathPosition(void)
{
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			//パスリストのbeginとendを取得
			auto itPath = (*it)->GetPathPointListBegin();
			auto itPathEnd = (*it)->GetPathPointListEnd();

			//全てのパスポイントの位置を更新
			for (; itPath != itPathEnd; ++itPath) {
				if (*itPath != nullptr) {
					D3DXVECTOR3 PathPosition = (*itPath)->GetPos();
					float posYInMesh = CMeshFieldManager::CalcPosYValueInMeshField(PathPosition);
					PathPosition.y = posYInMesh + 2.0f;
					(*itPath)->SetPos(PathPosition);
				}
			}
		}
	}
}

void CEnemyStorage::UpdateAllCollisionToModel(void)
{
	//リスト全更新
	int Count = 0;
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			CCollisionEnemyAndModel::Collision(*it,&m_EnemyBodyPosOld[Count]);
		}
		Count++;
	}
}

void CEnemyStorage::DrawAll(void) 
{
	//リスト全描画
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			(*it)->Draw();
		}
	}
}

void CEnemyStorage::DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	//リスト全描画
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			(*it)->DrawOnDepthMap(LightView, LightProje);
		}
	}
}

void CEnemyStorage::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	//リスト全描画
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			(*it)->DrawUseDepthMap(LightView, LightProje, DepthTexture);
		}
	}
}

void CEnemyStorage::DrawSpecular(void)
{
	//リスト全描画
	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (; it != m_Enemies.end(); ++it) {
		if ((*it) != nullptr) {
			(*it)->DrawSpecular();
		}
	}
}

CEnemy* CEnemyStorage::Create(CEnemy::ENEMY_TYPE type,const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass, float fMass, float GravityAcc, float CoeffiResist)
{
	CEnemy* pEnemy;
	pEnemy = new CEnemy();
	pEnemy->Init(type,pos, rot, MotionPass);
	pEnemy->SetEnvironment(fMass, GravityAcc, CoeffiResist);
	pEnemy->SetPlayer(m_Player);
	pEnemy->SetUseID(m_Enemies.size());
	m_Enemies.push_back(pEnemy);
	return pEnemy;
}

bool CEnemyStorage::EnemyAllDead(void)
{
	bool AllDead = false;
	int nCnt;
	int nNumDeadEnemy = 0;

	std::list<CEnemy*>::iterator it = m_Enemies.begin();
	for (nCnt = 0; it != m_Enemies.end(); ++it,++nCnt) {
		if ((*it) != nullptr) {
			if ((*it)->GetDeadFlag()) nNumDeadEnemy++;
		}
	}

	if (nCnt > 0 && nCnt == nNumDeadEnemy) {
		AllDead = true;
	}
	return AllDead;
}

std::list<CEnemy*>::iterator CEnemyStorage::GetEnemiesListNumber(int Number) {
	//早期リターン
	if (Number < 0) { MessageBox(nullptr, "この要素番目は存在しない,イテレーターのEndを返す", "関数CEnemyStorage::GetEnemiesListNumber", MB_ICONHAND); return m_Enemies.end(); }

	auto it = m_Enemies.begin();
	for (int nCnt = 0; it != m_Enemies.end(); ++it, ++nCnt) {
		if (nCnt == Number) return it;
	}
	MessageBox(nullptr, "この要素番目は存在しない,イテレーターのEndを返す", "関数CEnemyStorage::GetEnemiesListNumber", MB_ICONHAND);
	return m_Enemies.end();
}

void CEnemyStorage::Delete(int ID)
{
	if (ID < 0) {
		_RPT0(0,"[CEnemyStorage.cpp][Delete]引数IDは範囲外です");
	}

	auto it = m_Enemies.begin();
	for (int nCnt = 0; it != m_Enemies.end(); ++it, ++nCnt) {
		if (nCnt == ID) {
			if (*it != nullptr) {
				(*it)->Uninit();
				delete (*it);
				(*it) = nullptr;

				m_Enemies.erase(it);
				break;
			}
		}
	}
}