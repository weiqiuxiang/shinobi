#include "C_Obj3D_DataIO.h"
#include "CScene3DStorage.h"
#include "CModelStorage.h"
#include "SceneTransition.h"
#include "MousePosition.h"
#include "TexManager.h"
#include "ModelManager.h"
#include "CTextLoad.h"
#include "CMeshFieldManager.h"
#include "CPolygonMath.h"
#include <stdio.h>

#define TREE_WIDTH (24.0f)
#define TREE_HEIGHT (24.0f)

#define MODEL_ROTATION_SPEED (D3DX_PI * 0.05)

void C_Obj3D_DataIO::ExportObj3DData(void)
{
	//ファイルを開く
	FILE *fp;
	fp = fopen("data/3D_OBJ_DATA/3D_OBJ_SAVE_DATA.txt","w");

	//必要なデータ変数
	D3DXVECTOR3 POS;
	D3DXVECTOR3 SIZE;
	CScene3D::CSCENE3D_TYPE type;
	TEX_ID TexID;

	//頭コメント
	fprintf(fp, "#-----------------------------------------\n");
	fprintf(fp, "#3Dオブジェクトデータ\n");
	fprintf(fp, "#-----------------------------------------\n");

	//ビルボードの書き出し
	for(int i = 0; i < CSCENE3D_OBJ_MAX; i++)
	{
		//ポインタ取得
		CScene3D *pScene3D = CScene3DStorage::GetObj(i);
		if (nullptr == pScene3D) continue;

		//データ取得
		POS = pScene3D->GetPosition();
		SIZE = pScene3D->GetSize();
		type = pScene3D->GetType();
		TexID = pScene3D->GetTexID();
		const char* TEXTURE_PASS = CTexManager::GetTexturePass(TexID);

		//データを書き出し
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "#ビルボードオブジェの名前\n");//名前を書き出す
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "\tDATATYPE CSCENE3D\n");                               //オブジェクトのタイプはCScene3D
		fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);      //位置書き出す
		fprintf(fp, "\tSIZE= %.2f %.2f %.2f\n", SIZE.x, SIZE.y, SIZE.z);  //位置
		//タイプナンバー決定
		int TypeNumber = 0;
		if (type == CScene3D::TYPE_NORMAL) TypeNumber = 0;
		if (type == CScene3D::TYPE_BILLBOARD) TypeNumber = 1;
		if (type == CScene3D::TYPE_BILLBOARD_BOTTOM) TypeNumber = 2;
		fprintf(fp, "\tTYPE= %d\n", TypeNumber);                          //タイプ書き出す
		fprintf(fp, "\tTEXTURE_PASS= %s\n\n", TEXTURE_PASS);              //テクスチャパス書き出す
	}

	D3DXVECTOR3 ROT;                          //モデルの回転
	MODEL_ID MODEL_ID;                        //モデルID

	//3Dモデルの書き出す
	for (int i = 0; i < MODEL_OBJ_MAX; i++)
	{
		//ポインタ取得
		CModel *pModel = CModelStorage::GetObj(i);
		if (nullptr == pModel) continue;

		//データ取得
		POS = pModel->GetPosition();
		ROT = pModel->GetRot();
		MODEL_ID = pModel->GetModelID();
		const char* MODEL_PASS = CModelManager::GetModelPass(MODEL_ID);

		//データ書き出す
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "#モデルオブジェの名前\n");//名前を書き出す
		fprintf(fp, "#-----------------------------------------\n");
		fprintf(fp, "\tDATATYPE MODEL\n");                               //オブジェクトのタイプはCScene3D
		fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);      //位置書き出す
		fprintf(fp, "\tROT= %.2f %.2f %.2f\n", ROT.x, ROT.y, ROT.z);      //回転情報書き出す
		fprintf(fp, "\tMODEL_PASS= %s\n\n", MODEL_PASS);              //テクスチャパス書き出す
	}

	//ファイルを閉じる
	fclose(fp);
}

void C_Obj3D_DataIO::ImportObj3DData(const char* pMeshFilePass)
{
	//ファイルを開く
	FILE *fp;
	fp = fopen(pMeshFilePass, "r");

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
		if (strcmp("DATATYPE", buf) == 0)
		{
			CTextLoad::GetStrToken(fp, TOKEN, buf);      //タイプの読み込み

			if(strcmp("CSCENE3D", buf) == 0)             //タイプはCSCENE3Dの場合
			{
				//読み込み部分
				ReadBillBoard(fp);
			}

			if(strcmp("MODEL", buf) == 0)
			{
				//読み込み部分
				ReadModel(fp);
			}
		}
	}

	//ファイルを閉じる
	fclose(fp);
}

void C_Obj3D_DataIO::ReadBillBoard(FILE *fp)
{
	//必要なデータ変数
	D3DXVECTOR3 POS;
	D3DXVECTOR3 SIZE;
	int ObjType;
	char TexPass[1024];

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
	CTextLoad::FindFont(fp, TOKEN, "SIZE=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	SIZE.x = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	SIZE.y = (float)atof(buf);
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	SIZE.z = (float)atof(buf);
	//タイプの読み込み
	CTextLoad::FindFont(fp, TOKEN, "TYPE=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	ObjType = atoi(buf);
	//テクスチャパスの読み込み
	CTextLoad::FindFont(fp, TOKEN, "TEXTURE_PASS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	strcpy(TexPass, buf);

	//クリエイトする
	SRP PosRotScl;
	PosRotScl.pos = POS;
	PosRotScl.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	PosRotScl.scl = D3DXVECTOR3(1.0f,1.0f,1.0f);
	CScene3DStorage::CreateObject(PosRotScl, SIZE, (CScene3D::CSCENE3D_TYPE)ObjType, TexPass);
}

void C_Obj3D_DataIO::ReadModel(FILE *fp)
{
	//必要なデータ変数
	D3DXVECTOR3 POS;
	D3DXVECTOR3 ROT;
	char ModelPass[1024];

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

	CTextLoad::FindFont(fp, TOKEN, "MODEL_PASS=");
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	strcpy(ModelPass, buf);

	//クリエイトする
	SRP PosRotScl;
	PosRotScl.pos = POS;
	PosRotScl.rot = ROT;
	PosRotScl.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	CModelStorage::CreateObject(PosRotScl, ModelPass);
}