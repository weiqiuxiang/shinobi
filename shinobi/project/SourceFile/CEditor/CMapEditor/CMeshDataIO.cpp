#include "CMeshDataIO.h"
#include "TexManager.h"
#include "CTextLoad.h"
#include "CMeshFieldManager.h"
#include <stdio.h>

void CMeshDataIO::ExportMeshData(void)
{
	if (m_pMesh == nullptr) return;        //メッシュデータ存在しない

	//データ取得(初期化部分)
	D3DXVECTOR3 POS = m_pMesh->GetPosition();
	D3DXVECTOR2 PIECE_SIZE = m_pMesh->GetPieceSize();
	D3DXVECTOR2 PIECE_NUM = m_pMesh->GetPieceNum();
	TEX_ID MeshTexID = m_pMesh->GetTexID();
	const char* TEXTURE_PASS = CTexManager::GetTexturePass(MeshTexID);

	//データ取得(相対標高部分)
	int VERTEX_NUM = m_pMesh->GetVertexNum();
	const double *pMeshRelativeHeight = m_pMesh->GetHighRelative();

	//データの書き出し
	FILE *fp;
	fp = fopen("data/MESH_DATA/EDIT_MESH_SAVE.txt","w");    //書き出し先指定

	//先頭コメント書き出し
	fprintf(fp, "#-------------------------------------------------\n");
	fprintf(fp, "#MESH_DATA\n");
	fprintf(fp, "#-------------------------------------------------\n");

	//初期化部分書き出し
	fprintf(fp, "MESH_INIT START\n");
	fprintf(fp, "\tPOS= %.2f %.2f %.2f\n", POS.x, POS.y, POS.z);              //位置の書き出し
	fprintf(fp, "\tPIECE_SIZE= %.2f %.2f\n", PIECE_SIZE.x, PIECE_SIZE.y);     //マスサイズの書き出し
	fprintf(fp, "\tPIECE_NUM= %.0f %.0f\n", PIECE_NUM.x, PIECE_NUM.y);        //マス数の書き出し
	fprintf(fp, "\tTEXTURE_PASS= %s\n", TEXTURE_PASS);
	fprintf(fp, "MESH_INIT END\n\n");

	//相対標高部分の書き出し
	fprintf(fp, "RELATIVE_HEIGHT START\n");
	fprintf(fp, "\tVERTEX_NUM= %d\n", VERTEX_NUM);                 //頂点数の書き出し
	for (int i = 0; i < (PIECE_NUM.y + 1); i++)
	{
		fprintf(fp, "\t");
		fprintf(fp, "ELEVATION ");
		for (int j = 0; j < (PIECE_NUM.x + 1); j++)
		{
			int k = i * ((int)PIECE_NUM.x + 1) + j;
			fprintf(fp,"%.2f ",pMeshRelativeHeight[k]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "RELATIVE_HEIGHT END\n\n");
	fclose(fp);
}


CMeshField* CMeshDataIO::ImportMeshData(const char* pMeshFilePass)
{
	m_pMesh = nullptr; //他のメッシュを干渉しないように

	//初期化部分
	D3DXVECTOR3 POS;                  //位置情報
	D3DXVECTOR2 PIECE_SIZE;           //マスサイズ情報
	D3DXVECTOR2 PIECE_NUM;            //マス数情報
	char TEXTURE_PASS[256];          //テクスチャパス情報

	//標高部分
	int VERTEX_NUM;                   //頂点情報
	double *pMeshRelativeHeight;      //標高ポインタ

	//データの読み込み開始
	FILE *fp;
	fp = fopen(pMeshFilePass,"r");    //読み込み先指定
	if(nullptr == fp)           //nullptrチェック
	{
		fclose(fp);
		MessageBox(nullptr, "メッシュデータのテキストが存在しません", "エラー", MB_OK | MB_ICONHAND);
		return nullptr;
	}

	char FontBuf[1024];     //フォントバッファを作る

	//位置を読み込む
	CTextLoad::FindFont(fp,TOKEN, "POS=");     //位置データを見つかる
	CTextLoad::GetStrToken(fp,TOKEN, FontBuf); 
	POS.x = (float)atof(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	POS.y = (float)atof(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	POS.z = (float)atof(FontBuf);

	//マスサイズを読み込む
	CTextLoad::FindFont(fp, TOKEN, "PIECE_SIZE=");     //マスサイズデータを見つかる
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_SIZE.x = (FLOAT)atof(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_SIZE.y = (FLOAT)atof(FontBuf);

	//マス数を読み込む
	CTextLoad::FindFont(fp, TOKEN, "PIECE_NUM=");
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_NUM.x = (FLOAT)atoi(FontBuf);
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	PIECE_NUM.y = (FLOAT)atoi(FontBuf);

	//テクスチャパスをコピー
	CTextLoad::FindFont(fp, TOKEN, "TEXTURE_PASS=");
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	strcpy(TEXTURE_PASS,FontBuf);

	//頂点数をコピー
	CTextLoad::FindFont(fp, TOKEN, "VERTEX_NUM=");
	CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
	VERTEX_NUM = atoi(FontBuf);

	pMeshRelativeHeight = new double[VERTEX_NUM];    //頂点分の標高データを確保する

	//標高データを読み込み
	for (int i = 0; i < (PIECE_NUM.y + 1); i++)
	{
		CTextLoad::FindFont(fp, TOKEN, "ELEVATION");
		for (int j = 0; j < (PIECE_NUM.x + 1); j++)
		{
			CTextLoad::GetStrToken(fp, TOKEN, FontBuf);
			int k = i * ((int)PIECE_NUM.x + 1) + j;
			pMeshRelativeHeight[k] = atof(FontBuf);
		}
	}

	//以上のデータでメッシュフィールドを作る
	CMeshField *pMesh = CMeshFieldManager::CreateMeshField(POS, CINDEX_SIZE(PIECE_NUM, PIECE_SIZE), TEXTURE_PASS);
	pMesh->LoadRelativeHigh(pMeshRelativeHeight);

	//いろいろ解放
	delete[] pMeshRelativeHeight;
	fclose(fp);

	return pMesh;
}