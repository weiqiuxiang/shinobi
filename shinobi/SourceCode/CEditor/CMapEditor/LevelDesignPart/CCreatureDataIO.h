//敵の位置と巡回ポイントを入出力クラス
#ifndef CCRETURE_DATA_IO_H_
#define CCRETURE_DATA_IO_H_

#include "main.h"
#include <stdio.h>

//先方宣言
class CPlayer;

//クラス定義
class CCreatureDataIO
{
public:
	void ExportEnemyObjData(void);								//敵データの書き出し
	void ImportEnemyObjData(const char* pObjFilePass);			//敵データの読み込み

	void ExportPlayerObjData(CPlayer*);							//プレーヤーデータの書き出し
	CPlayer* ImportPlayerObjData(const char* pObjFilePass);		//プレーヤーデータの読み込み

private:
	//読み込み関数
	CPlayer* ReadPlayerObjs(FILE *fp);
	void ReadEnemyObjs(FILE *fp);
};

#endif