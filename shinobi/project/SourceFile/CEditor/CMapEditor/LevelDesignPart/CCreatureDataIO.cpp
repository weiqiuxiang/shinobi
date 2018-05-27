#include "CEnemyStorage.h"
#include "SceneTransition.h"
#include "CPolygonMath.h"
#include <stdio.h>
#include "CCreatureDataIO.h"
#include "CTextLoad.h"

#define TREE_WIDTH (24.0f)
#define TREE_HEIGHT (24.0f)

#define DEFAULT_ENEMY_COFFI (0.2f)

void CCreatureDataIO::ExportEnemyObjData(void)
{
	//ファイルを開く
	FILE *fp;
	fp = fopen("data/CREATURE_DATA/EnemyDataOutput.txt","w");

	//必要なデータ変数
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;
	CEnemy::ENEMY_TYPE type;

	//頭コメント
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "#****************敵のデータ***************\n");
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "\n");

	//ビルボードの書き出し
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for(; it != itEnd; it++)
	{
		//ポインタ取得
		if (*it == nullptr) {
			continue;
		}

		//データ取得
		POS = (*it) -> GetPos();
		ROT = (*it) ->GetRot();
		type = (*it) ->GetEnemyType();

		//データを書き出し
		switch (type) {
		case CEnemy::TYPE_DOG:
			fprintf(fp, "#-----------------------------------------\n");
			fprintf(fp, "#犬\n");
			fprintf(fp, "#-----------------------------------------\n");
			break;
		}
		fprintf(fp, "ENEMY_BEGIN\n");

		fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);	//位置書き出す
		fprintf(fp, "\tROT= %.2f %.2f %.2f\n", ROT.x, ROT.y, ROT.z);	//位置
		fprintf(fp, "\tTYPE= %d\n\n", (int)type);						//敵のタイプ
		
		//巡回ウェイポイント
		auto itPoint = (*it)->GetPathPointListBegin();
		auto itPointEnd = (*it)->GetPathPointListEnd();

		fprintf(fp, "\tPATHPOINT_LIST_BEGIN\n\n");

		for (; itPoint != itPointEnd; itPoint++) {
			if (*itPoint == nullptr) {
				continue;
			}

			D3DXVECTOR3 PointPos = (*itPoint) ->GetPos();
			int StopFrame = (*itPoint) ->GetStopFrame();

			fprintf(fp, "\t\tPOINT_BEGIN\n");
			fprintf(fp, "\t\t\tPOS= %.2f %.2f %.2f\n", PointPos.x, PointPos.y, PointPos.z);	//位置書き出す
			fprintf(fp, "\t\t\tWAITTIME= %d\n", StopFrame);									//待ち時間を描き出す
			fprintf(fp, "\t\tPOINT_END\n\n");
		}

		fprintf(fp, "\tPATHPOINT_LIST_END\n");
		fprintf(fp, "ENEMY_END\n\n");
	}

	//ファイルを閉じる
	fclose(fp);
}

void CCreatureDataIO::ImportEnemyObjData(const char* pObjFilePass)
{
	//ファイルを開く
	FILE *fp;
	fp = fopen(pObjFilePass, "r");

	//nullptrチェック
	if (nullptr == fp)           //nullptrチェック
	{
		fclose(fp);
		MessageBox(nullptr, "メッシュデータのテキストが存在しません", "エラー", MB_OK | MB_ICONHAND);
		return;
	}

	//データの読み込み
	char buf[1024];
	while(CTextLoad::GetStrToken(fp,TOKEN,buf) != -1)
	{
		if (strcmp("ENEMY_BEGIN", buf) == 0)
		{
			ReadEnemyObjs(fp);
		}
	}

	//ファイルを閉じる
	fclose(fp);
}

void CCreatureDataIO::ExportPlayerObjData(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) {
		_RPT0(0,"[CCreatureDataIO.cpp][ExportPlayerObjData]引数pPlayerはヌル\n");
		return;
	}

	//ファイルを開く
	FILE *fp;
	fp = fopen("data/CREATURE_DATA/PlayerDataOutput.txt", "w");

	//必要なデータ変数
	D3DXVECTOR3 POS = pPlayer->GetPos();
	D3DXVECTOR3 ROT = pPlayer->GetRot();

	//頭コメント
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "#************プレーヤーのデータ***********\n");
	fprintf(fp, "#-----------------------------------------\n");

	//位置と回転情報を描き出す
	fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);	//位置書き出す
	fprintf(fp, "\tROT= %.2f %.2f %.2f\n", ROT.x, ROT.y, ROT.z);	//回転情報を描き出す

	//ファイルを閉じる
	fclose(fp);
}

CPlayer* CCreatureDataIO::ImportPlayerObjData(const char* pObjFilePass)
{
	CPlayer* pPlayer = nullptr;

	//ファイルを開く
	FILE *fp;
	fp = fopen(pObjFilePass, "r");

	//nullptrチェック
	if (nullptr == fp)           //nullptrチェック
	{
		fclose(fp);
		MessageBox(nullptr, "メッシュデータのテキストが存在しません", "エラー", MB_OK | MB_ICONHAND);
		return nullptr;
	}

	char buf[1024];
	while (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1)
	{
		pPlayer = ReadPlayerObjs(fp);
	}

	//ファイルを閉じる
	fclose(fp);

	return pPlayer;
}

CPlayer* CCreatureDataIO::ReadPlayerObjs(FILE *fp)
{
	//必要なデータ変数
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;

	char buf[1024];

	//位置の読み込み
	CTextLoad::FindFont(fp, TOKEN, "POS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.z = (float)atof(buf);
	//サイズの読み込み
	CTextLoad::FindFont(fp, TOKEN, "ROT=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.z = (float)atof(buf);

	CPlayer* pPlayer = new CPlayer();
	if (pPlayer == nullptr) {
		return nullptr;
	}

	pPlayer->Init(POS, ROT, MOTION_DATA_PLAYER);

	return pPlayer;
}

void CCreatureDataIO::ReadEnemyObjs(FILE *fp)
{
	//必要なデータ変数
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;
	CEnemy::ENEMY_TYPE type;
	
	char buf[1024];

	//位置の読み込み
	CTextLoad::FindFont(fp, TOKEN, "POS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	POS.z = (float)atof(buf);

	CTextLoad::FindFont(fp, TOKEN, "ROT=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ROT.z = (float)atof(buf);

	CTextLoad::FindFont(fp, TOKEN, "TYPE=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	type = (CEnemy::ENEMY_TYPE)atoi(buf);

	CEnemy *pEnemy = CEnemyStorage::Create(type, POS, ROT, MOTION_DATA_ENEMYDOG, 1.0f, -0.1f, DEFAULT_ENEMY_COFFI);
	if (pEnemy == nullptr) {
		return;
	}

	//巡回パスを読み込み
	while (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1)
	{
		//リスエンドまで読み込み
		if (strcmp("PATHPOINT_LIST_END", buf) == 0) {
			break;
		}
		if (strcmp("POINT_BEGIN", buf) != 0) {
			continue;
		}

		D3DXVECTOR3 POINTPOS;
		int WaitTime;

		CTextLoad::FindFont(fp, TOKEN, "POS=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		POINTPOS.x = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		POINTPOS.y = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		POINTPOS.z = (float)atof(buf);

		CTextLoad::FindFont(fp, TOKEN, "WAITTIME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		WaitTime = atoi(buf);

		pEnemy->AddPathPoint(POINTPOS, WaitTime);
	}
}