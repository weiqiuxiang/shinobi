#pragma once
#ifndef CCOLLISION_PLAYER_AND_ENEMY
#define CCOLLISION_PLAYER_AND_ENEMY

#include "main.h"
#include "CPlayer.h"
#include "CEnemy.h"

//マクロ
#define BLOOD_POSITION_MAX (30)

//前方宣言
class CCollisionCircle;

typedef struct 
{
	D3DXVECTOR3			AttackCircleOldPos;	//攻撃側の古い座標
	D3DXVECTOR3			DamageCircleOldPos;	//ダーメジ側の古い座標
	CCollisionCircle*	AttackCircle;		//攻撃側の球
	CCollisionCircle*	DamageCircle;		//ダーメジ側の球
	int					Time;				//計算回数
	bool				IsUse;				//使用されたか
}BLOOD_POSITION;

class CColliPlayerEnemy
{
public:
	static void Init(void);
	static void UpdateBlood(void);
	static void GetPlayerEnemyDataAndColliCheck(void);				//プレーヤーや敵のデータ(ポインタ)取得と当り判定
	static void GetEnemiesDataAndColliCheck(void);					//敵と敵のデータ(ポインタ)取得と当り判定

	//セッター
	/*test*/
	static void SetPlayer(CPlayer *Player);
	static void SetEnemy(CEnemy *Enemy);
	static void SetEnemyOther(CEnemy *Enemy);
	/*test*/
private:
	//メンバ関数

	//プレーヤーと敵の当り判定関数
	static bool CanCalcPlayerEnemy(void);
	static void BodyCollisionPlayerEnemy(void);						//プレーヤーと敵の衝突判定
	static void PlayerHitEnemy(void);								//プレーヤーは敵にヒットしているがどうか
	static void EnemyHitPlayer(void);								//敵はプレーヤーにヒットしているがどうか

	//敵と敵の衝突判定
	static void BodyCollisionEnemies(void);							//敵と敵の衝突判定
	static void CreateBloodPosition(CCollisionCircle* Attack, CCollisionCircle* Damage);

	//メンバ変数
	static CPlayer* m_Player;
	static CEnemy* m_Enemy;
	static CEnemy* m_EnemyOtherEnemy;
	static float m_CoffiPushOut;									//押し出す係数
	static BLOOD_POSITION m_BloodPosition[BLOOD_POSITION_MAX];		//血しぶき位置
};

#endif