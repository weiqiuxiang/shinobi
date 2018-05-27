#pragma once
#ifndef CENEMY_STORAGE_H_
#define CENEMY_STORAGE_H_

#include "CEnemy.h"
#include "CEnemyControl.h"
#include <list>
#include "CPlayer.h"
#include "main.h"

//マクロ
#define COLLISION_ENMEY_MODEL_MAX (200)			//モデルと当り判定できる敵の数

class CEnemyStorage
{
public:
	static void InitAll(void);
	static void UninitAll(void);
	static void UpdateAll(void);
	static void UpdateEditorAll(void);
	static void UpdateAllEnemyPathPosition(void);
	static void UpdateAllCollisionToModel(void);
	static void DrawAll(void);
	static void DrawAllOnDepthMap(const D3DXMATRIX& LightView,const D3DXMATRIX& LightProje);
	static void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	static void DrawSpecular(void);
	static CEnemy* Create(CEnemy::ENEMY_TYPE type,const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass, float fMass, float GravityAcc, float CoeffiResist);

	static void SetPlayer(CPlayer* pPlayer) { m_Player = pPlayer; }
	static void ClearPlayer(void) { m_Player = nullptr; }

	//ゲッター
	static int GetEnemyAmount(void) { return m_Enemies.size(); }		//敵の数取得

	static std::list<CEnemy*>::iterator GetEnemiesListBegin(void) { return m_Enemies.begin(); }
	static std::list<CEnemy*>::iterator GetEnemiesListEnd(void) { return m_Enemies.end(); }
	static std::list<CEnemy*>::iterator GetEnemiesListNumber(int Number);

	static bool EnemyAllDead(void);
	static void Delete(int ID);
private:
	static std::list<CEnemy*>	m_Enemies;
	static D3DXVECTOR3			m_EnemyBodyPosOld[COLLISION_ENMEY_MODEL_MAX];
	static CPlayer*				m_Player;
private:
};

#endif