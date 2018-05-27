#include "CEditorMotion.h"
#include "CTextLoad.h"
#include "CPolygonMath.h"
#include "CHermitian.h"
#include <stdio.h>

CEditorMotion::CEditorMotion()
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_WorldRotMtx);
	D3DXMatrixIdentity(&m_OtherParentMtx);
	m_MotionNow = 0;
	m_MotionOld = 0;
}

CEditorMotion::~CEditorMotion()
{

}

void CEditorMotion::UpdateOffset(void)		//オフセット表示
{
	ShowOffset();							//全パーツ今の位置を計算
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::UpdateKeyFrame(void)	//キーフレーム表示
{
	ShowKeyFrame();							//全パーツ今の位置を計算
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::UpdateKeyFramePreview(float Percent)
{
	bool OverRange = Percent < 0 || Percent> 1;	//パーセンテージは0～1の範囲外
	if (OverRange) {
		MessageBox(nullptr, "引数Percentの値は0～1の範囲外", "関数CEditorMotion::UpdateKeyFramePreviewエラー", MB_ICONHAND);
		Percent = 0;
	}

	ShowKeyFramePreview(Percent);					//全パーツ今の位置を計算
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::UpdateMotion(void)		//モーション再生
{
	KeyFrameChange();						//キーフレームのインクリメント判定
	ShowMotion();							//全パーツ今の位置を計算
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CEditorMotion::Draw(void)
{
	for(int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Draw();
	}
}

void CEditorMotion::Uninit(void)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Uninit();
	}
}

void CEditorMotion::LoadMotionDataToInit(const char* MotionPass)
{
	FILE *fp;
	fp = fopen(MotionPass,"r");

	if(nullptr == fp)
	{
		MessageBox(nullptr,"モーションデータのテキストが存在しない","エラー",MB_OK | MB_ICONHAND);
		fclose(fp);
		return;
	}

	//モデル数とモーション数読み込み
	char buf[1024];				//文字バッファ
	CTextLoad::FindFont(fp,TOKEN, "NUM_MODEL=");            //モデルフォントを探す
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	
	m_PartNum = atoi(buf);		//パーツ数取得

	LoadPartData(fp);						//パーツのデータ読み込む
	LoadMotionData(fp);						//モーションのデータ読み込む

	fclose(fp);

	LinkParentAndChildren();				//親と子供の関係を結びつき

	//初期化
	m_MotionNow = 0;
	m_MotionOld = 0;
	m_KeyFrameNow = 0;
	m_FrameNow = 0;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_WorldRotMtx);

	//初期モーションロード
	for (int i = 0;i < m_Motion[0].m_KeyFrame[0].m_TotalFrame;i++)
	{
		KeyFrameChange();
		ShowOffset();						//モデルのオフセットセット
	}
}

/*==============================================================================
	現在のモーションにキーフレームを増やす
===============================================================================*/
void CEditorMotion::AddKeyFrame(void)
{
	//現在モーションのキーフレーム総数
	int TotalKeyFrame = m_Motion[m_MotionNow].m_TotalKeyFrame;

	//使用可能なキーフレームチェック
	if (TotalKeyFrame >= MAX_PART) 
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddKeyFrame]使用可能なキーフレームはない", "WARNING", MB_ICONERROR);
		return;
	}

	//新しいキーフレームを作る
	for (int PartNum = 0; PartNum < m_PartNum; PartNum++)
	{
		//拡大縮小、回転、移動初期化
		SRP *pSRP = &m_Motion[m_MotionNow].m_KeyFrame[TotalKeyFrame].m_SRPData[PartNum];
		pSRP->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		//曲線データ初期化
		m_Motion[m_MotionNow].m_KeyFrame[TotalKeyFrame].m_HermitianStartDir = D3DXVECTOR2(50, -50);
		m_Motion[m_MotionNow].m_KeyFrame[TotalKeyFrame].m_HermitianEndDir = D3DXVECTOR2(-50, 50);
	}

	//キーフレーム数1つ増やす
	m_Motion[m_MotionNow].m_TotalKeyFrame++;
}

/*==============================================================================
	指定するモーションにキーフレームを増やす
===============================================================================*/
void CEditorMotion::AddKeyFrame(int MotionNum)
{
	//範囲外チェック
	if (MotionNum < 0 || MotionNum >= m_TotalMotion)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddKeyFrame]引数MotionNumは範囲外", "WARNING", MB_ICONERROR);
		return;
	}

	//現在モーションのキーフレーム総数
	int TotalKeyFrame = m_Motion[MotionNum].m_TotalKeyFrame;

	//使用可能なキーフレームチェック
	if (TotalKeyFrame >= MAX_PART)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddKeyFrame]使用可能なキーフレームはない", "WARNING", MB_ICONERROR);
		return;
	}

	//新しいキーフレームを作る
	for (int PartNum = 0; PartNum < m_PartNum; PartNum++)
	{
		//拡大縮小、回転、移動初期化
		SRP *pSRP = &m_Motion[MotionNum].m_KeyFrame[TotalKeyFrame].m_SRPData[PartNum];
		pSRP->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		//曲線データ初期化
		m_Motion[MotionNum].m_KeyFrame[TotalKeyFrame].m_HermitianStartDir = D3DXVECTOR2(50, -50);
		m_Motion[MotionNum].m_KeyFrame[TotalKeyFrame].m_HermitianEndDir = D3DXVECTOR2(-50, 50);
	}

	//キーフレーム数1つ増やす
	m_Motion[MotionNum].m_TotalKeyFrame++;
}

/*==============================================================================
	キーフレームの挿入
===============================================================================*/
void CEditorMotion::InsertKeyFrame(void)
{
	//現在モーションのキーフレーム総数
	int TotalKeyFrame = m_Motion[m_MotionNow].m_TotalKeyFrame;

	//使用可能なキーフレームチェック
	if (TotalKeyFrame >= MAX_PART)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:InsertKeyFrame]使用可能なキーフレームはない", "WARNING", MB_ICONERROR);
		return;
	}

	//キーフレームのデータを後ろにずれる
	for (int KeyframeNum = TotalKeyFrame - 1; KeyframeNum >= m_KeyFrameNow; KeyframeNum--) {
		m_Motion[m_MotionNow].m_KeyFrame[KeyframeNum + 1] = m_Motion[m_MotionNow].m_KeyFrame[KeyframeNum];
	}

	//今のキーフレームのデータを初期化
	for (int PartNum = 0; PartNum < m_PartNum; PartNum++)
	{
		//拡大縮小、回転、移動初期化
		SRP *pSRP = &m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[PartNum];
		pSRP->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pSRP->scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		//曲線データ初期化
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir = D3DXVECTOR2(50, -50);
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir = D3DXVECTOR2(-50, 50);
	}

	//キーフレーム数1つ増やす
	m_Motion[m_MotionNow].m_TotalKeyFrame++;
}

/*==============================================================================
	モーションを増やす(キーフレームのデフォルトは2つ)
===============================================================================*/
void CEditorMotion::AddMotion(void)
{
	if (m_TotalMotion >= EDITOR_MOTION_MAX)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:AddMotion]使用可能なモーションはない", "WARNING", MB_ICONERROR);
		return;
	}

	m_Motion[m_TotalMotion].m_bLoop = false;
	m_Motion[m_TotalMotion].m_TotalKeyFrame = 0;

	//モーション数を増やす
	m_TotalMotion++;

	//キーフレーム2つを作る
	AddKeyFrame(m_TotalMotion - 1);
	AddKeyFrame(m_TotalMotion - 1);
}

/*==============================================================================
	モーションの挿入
===============================================================================*/
void CEditorMotion::InsertMotion(void)
{
	if (m_TotalMotion >= EDITOR_MOTION_MAX)
	{
		MessageBox(nullptr, "[CEditorMotion.cpp][Function:InsertMotion]使用可能なモーションはない", "WARNING", MB_ICONERROR);
		return;
	}

	//現在モーション番号とその後ろの要素を後ろに一要素分ずれる
	for (int MotionNum = m_TotalMotion - 1; MotionNum >= m_MotionNow; MotionNum--)
	{
		m_Motion[MotionNum + 1] = m_Motion[MotionNum];
	}

	strcpy_s(m_Motion[m_MotionNow].m_Name, "NewMotion");
	m_Motion[m_MotionNow].m_bLoop = false;
	m_Motion[m_MotionNow].m_TotalKeyFrame = 0;

	//モーション数を増やす
	m_TotalMotion++;

	//キーフレーム2つを作る
	AddKeyFrame(m_MotionNow);
	AddKeyFrame(m_MotionNow);
}

/*==============================================================================
	今のモーションを削除し、配列の後ろの部分を詰める
===============================================================================*/
void CEditorMotion::DeleteMotionNow(void)
{
	//モーション数を1以上保持
	if (m_TotalMotion <= 1)
	{
		return;
	}

	//現在モーション番号を基準に、後ろの配列を前に詰める
	for(int MotionNum = m_MotionNow; MotionNum < m_TotalMotion - 1; MotionNum++)
	{
		m_Motion[MotionNum] = m_Motion[MotionNum + 1];
	}

	//今のモーション番号を一個前ずらす
	if (m_MotionNow == (m_TotalMotion - 1))
	{
		m_MotionNow--;
	}

	//モーション総数減少
	m_TotalMotion--;
}

/*==============================================================================
	今のモーションの今のキーフレームを削除し、配列の後ろの部分を詰める
===============================================================================*/
void CEditorMotion::DeleteKeyFrameNow(void)
{
	//現在モーションのキーフレーム総数
	int TotalKeyFrame = m_Motion[m_MotionNow].m_TotalKeyFrame;

	//キーフレーム数を1以上保持
	if (TotalKeyFrame <= 1) 
	{
		return;
	}

	//現在キーフレーム番号を基準に、後ろの配列を前に詰める
	for (int KeyFrameNum = m_KeyFrameNow; KeyFrameNum < TotalKeyFrame - 1; KeyFrameNum++)
	{
		m_Motion[m_MotionNow].m_KeyFrame[KeyFrameNum] = 
			m_Motion[m_MotionNow].m_KeyFrame[KeyFrameNum + 1];
	}

	//今のキーフレーム番号を一個前ずらす
	if (m_KeyFrameNow == (m_Motion[m_MotionNow].m_TotalKeyFrame - 1) )
	{
		m_KeyFrameNow--;
	}

	//キーフレーム総数減少
	m_Motion[m_MotionNow].m_TotalKeyFrame--;
}

void CEditorMotion::LoadPartData(FILE *fp)
{
	if (nullptr == fp) return;

	char buf[1024];            //文字バッファ

	//モデル読み込み
	SRP SRP;
	SRP.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SRP.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SRP.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < m_PartNum; i++)
	{
		CTextLoad::FindFont(fp, TOKEN, "MODEL_FILENAME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_PartFilePass[i].clear();
		m_PartFilePass[i] += buf;
		m_Part[i].m_Model.Init(SRP, buf);
	}

	//パーツのオフセットとID番号を読み込む
	for (int i = 0; i < m_PartNum; i++)
	{
		//名前読み込み
		CTextLoad::FindFont(fp, TOKEN, "NAME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		strcpy_s( m_Part[i].m_Name,PART_NAME_FONT_MAX_NUM, buf);

		//自分のパーツIDを読み込み
		CTextLoad::FindFont(fp, TOKEN, "INDEX=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_PartID = atoi(buf);
		//親のパーツIDを読み込み
		CTextLoad::FindFont(fp, TOKEN, "PARENT=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_ParentID = atoi(buf);
		//位置オフセットを読み込み
		CTextLoad::FindFont(fp, TOKEN, "POS=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.pos.x = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.pos.y = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.pos.z = (float)atof(buf);
		//回転オフセットを読み込み
		CTextLoad::FindFont(fp, TOKEN, "ROT=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.rot.x = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.rot.y = (float)atof(buf);
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Part[i].m_SRPOffset.rot.z = (float)atof(buf);

		m_Part[i].m_SRPOffset.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}
}

void CEditorMotion::LoadMotionData(FILE *fp)
{
	if (nullptr == fp) return;

	m_TotalMotion = 0;             //総モーション数を0にする
	char buf[1024];            //文字バッファ

	//モーション情報の読み込み
	while ( (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1) && (m_TotalMotion < EDITOR_MOTION_MAX) )
	{
		//文字列MOTIONSETを見つかるまでループ
		if (strcmp("MOTIONSET", buf) != 0) {
			continue;	
		}

		CTextLoad::FindFont(fp, TOKEN, "NAME=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		strcpy_s(m_Motion[m_TotalMotion].m_Name, MOTION_NAME_MAX, buf);

		//ループフラグの読み込み
		CTextLoad::FindFont(fp, TOKEN, "LOOP=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		int nLoop = atoi(buf);
		if (0 == nLoop) m_Motion[m_TotalMotion].m_bLoop = false;
		else m_Motion[m_TotalMotion].m_bLoop = true;

		//キー数の読み込み
		CTextLoad::FindFont(fp, TOKEN, "NUM_KEY=");
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		m_Motion[m_TotalMotion].m_TotalKeyFrame = atoi(buf);
		m_MotionTotalFrame[m_TotalMotion] = 0;

		//キーフレームデータの読み込み
		for(int nKeyFrameNum = 0; nKeyFrameNum < m_Motion[m_TotalMotion].m_TotalKeyFrame; nKeyFrameNum++)
		{
			//キーフレームのフレーム数を読み込み
			CTextLoad::FindFont(fp, TOKEN, "FRAME=");
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_TotalFrame = atoi(buf);
			m_MotionTotalFrame[m_TotalMotion] += m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_TotalFrame;     //このモーションのフレーム総数(時間)の和

			//キーフレームのエルミート曲線データの読み込み
			CTextLoad::FindFont(fp, TOKEN, "START_DIR=");
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianStartDir.x = (FLOAT)atof(buf);
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianStartDir.y = (FLOAT)atof(buf);
			CTextLoad::FindFont(fp, TOKEN, "END_DIR=");
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianEndDir.x = (FLOAT)atof(buf);
			CTextLoad::GetStrToken(fp, TOKEN, buf);
			m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_HermitianEndDir.y = (FLOAT)atof(buf);

			//キーフレームの具体POSとROTの読み込み
			for (int k = 0; k < m_PartNum; k++)
			{
				SRP SRP;
				SRP.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				//位置データの読み込み
				CTextLoad::FindFont(fp, TOKEN, "POS=");
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.pos.x = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.pos.y = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.pos.z = (float)atof(buf);
				//回転データの読み込み
				CTextLoad::FindFont(fp, TOKEN, "ROT=");
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.rot.x = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.rot.y = (float)atof(buf);
				CTextLoad::GetStrToken(fp, TOKEN, buf);
				SRP.rot.z = (float)atof(buf);

				m_Motion[m_TotalMotion].m_KeyFrame[nKeyFrameNum].m_SRPData[k] = SRP;
			}
		}
		m_TotalMotion++;                   //モーションカウントアップ
	}

	//モーション読み取れなかったら
	if (m_TotalMotion == 0) {
		m_TotalMotion = 1;
		m_MotionNow = 0;
		AddKeyFrame();
	}
}

//オフセットの再生
void CEditorMotion::ShowOffset(void)
{
	CPart* pTopNode = nullptr;

	//Topの親を取得
	for (int i = 0; i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//親代入
		}
	}

	//一番の親はない
	if (nullptr == pTopNode)
	{
		MessageBox(nullptr, "一番上の親はない！", "関数CEditorMotion::CalcWorldMtxエラー発生", MB_ICONHAND);
		return;
	}

	CalcOffset(pTopNode);			//再帰的なモーション計算
}

//指定キーフレームの再生
void CEditorMotion::ShowKeyFrame(void)
{
	CPart* pTopNode = nullptr;

	//Topの親を取得
	for (int i = 0; i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//親代入
		}
	}

	//一番の親はない
	if (nullptr == pTopNode)
	{
		MessageBox(nullptr, "一番上の親はない！", "関数CEditorMotion::CalcWorldMtxエラー発生", MB_ICONHAND);
		return;
	}

	CalcKeyFrame(pTopNode);			//再帰的なモーション計算
}

void CEditorMotion::ShowKeyFramePreview(float Percent)
{
	CPart* pTopNode = nullptr;

	//Topの親を取得
	for (int i = 0; i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//親代入
		}
	}

	//一番の親はない
	if (nullptr == pTopNode)
	{
		MessageBox(nullptr, "一番上の親はない！", "関数CEditorMotion::CalcWorldMtxエラー発生", MB_ICONHAND);
		return;
	}

	CalcKeyFramePreview(pTopNode, Percent);			//再帰的なモーション計算
}

void CEditorMotion::ShowMotion(void)
{
	CPart* pTopNode = nullptr;

	//Topの親を取得
	for (int i = 0;i < m_PartNum; i++) {
		if (nullptr == m_Part[i].GetParent()) {
			pTopNode = &m_Part[i];			//親代入
		}
	}

	//一番の親はない
	if(nullptr == pTopNode)
	{
		MessageBox(nullptr, "一番上の親はない！", "関数CEditorMotion::CalcWorldMtxエラー発生", MB_ICONHAND);
		return;
	}

	CalcPartWorldMtxRecursive(pTopNode);			//再帰的なモーション計算
}

void CEditorMotion::CalcOffset(CPart* Node)
{
	/*************************親のマトリクス計算************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//親マトリクス
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//ワールドマトリクスの保存
		MtxParent = MtxWorld * m_OtherParentMtx;							//親マトリクスに代入
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//オフセットマトリクス
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//オフセットマトリクス計算

	//結果マトリクス計算
	D3DXMATRIX MtxResult;
	MtxResult = MtxOffset*MtxParent;

	//モデルにセット
	Node->SetNowMtx(MtxResult);						//マトリクス反映
	Node->m_SRPNow.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	Node->m_SRPNow.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	/*************************子供のマトリクス計算************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcOffset(*itChild);					//再帰
	}
}

void CEditorMotion::CalcKeyFrame(CPart* Node)
{
	/*************************親のマトリクス計算************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//親マトリクス
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//ワールドマトリクスの保存
		MtxParent = MtxWorld * m_OtherParentMtx;							//親マトリクスに代入
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//オフセットマトリクス
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//オフセットマトリクス計算

	int nPartID = Node->m_PartID;
	SRP SRPKeyFrame = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartID];	//今のキーフレームのSRPを求め
	
	//結果マトリクス計算
	D3DXMATRIX MtxKeyFrame, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxKeyFrame, SRPKeyFrame);
	MtxResult = MtxKeyFrame*MtxOffset*MtxParent;                        //自分の回転マトリクス*自分オフセットマトリクス*親マトリクス

	//モデルにセット
	Node->SetNowMtx(MtxResult);						//マトリクス反映
	Node->m_SRPNow = SRPKeyFrame;					//SRPKeyFrame記録

	/*************************子供のマトリクス計算************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcKeyFrame(*itChild);					//再帰
	}
}

void CEditorMotion::CalcKeyFramePreview(CPart* Node, float Percent)
{
	int NextKeyFrame = CalcNextKeyFrame();       //次のキーフレームを求める

	/*************************親のマトリクス計算************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//親マトリクス
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//ワールドマトリクスの保存
		MtxParent = MtxWorld * m_OtherParentMtx;							//親マトリクスに代入
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//オフセットマトリクス
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//オフセットマトリクス計算

	int nPartID = Node->m_PartID;
	SRP StartSRP = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartID];	//今のキーフレームのSRPを求め
	SRP EndSRP = m_Motion[m_MotionNow].m_KeyFrame[NextKeyFrame].m_SRPData[nPartID];		//次のキーフレームのSRP求める
	float t = Percent;			//今のフレーム目は総フレームの何％

	//現在のSRP計算
	SRP SRP_FrameNow;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_FrameNow.pos = StartSRP.pos * (1 - t) + EndSRP.pos * t;
	SRP_FrameNow.rot = StartSRP.rot * (1 - t) + EndSRP.rot * t;

	//結果マトリクス計算
	D3DXMATRIX MtxNow, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
	MtxResult = MtxNow*MtxOffset*MtxParent;                        //自分の回転マトリクス*自分オフセットマトリクス*親マトリクス

	//モデルにセット
	Node->SetNowMtx(MtxResult);						//マトリクス反映
	Node->m_SRPNow = SRP_FrameNow;					//SRPNow記録

	/*************************子供のマトリクス計算************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcKeyFramePreview(*itChild,Percent);					//再帰
	}
}

void CEditorMotion::CalcPartWorldMtxRecursive(CPart* Node)
{
	int NextKeyFrame = CalcNextKeyFrame();       //次のキーフレームを求める

	/*************************親のマトリクス計算************************/
	CPart *pParent = Node->GetParent();
	D3DXMATRIX MtxParent;				//親マトリクス
	if (nullptr == pParent) {
		D3DXMATRIX MtxWorld;
		SRP SRP_WorldMtx;
		D3DXMATRIX PosMtx, SclMtx;
		SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		SRP_WorldMtx.pos = m_pos;
		D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
		D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
		MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
		m_WorldMtx = MtxWorld;												//ワールドマトリクスの保存
		MtxParent = MtxWorld * m_OtherParentMtx;							//親マトリクスに代入
	}
	else MtxParent = pParent->m_MtxNow;

	D3DXMATRIX MtxOffset;													//オフセットマトリクス
	CPolygonMath::CalcMtxFromSRP(&MtxOffset, Node->m_SRPOffset);			//オフセットマトリクス計算

	int nPartID = Node->m_PartID;
	SRP StartSRP = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartID];	//今のキーフレームのSRPを求め
	SRP EndSRP = m_Motion[m_MotionNow].m_KeyFrame[NextKeyFrame].m_SRPData[nPartID];		//次のキーフレームのSRP求める
	float t = (float)m_FrameNow / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;		//今のフレーム目は総フレームの何％

	//エルミート曲線を使って、変化量のパーセンテージを求め
	float Percent = CHermitian::GetChangeValuePercent(
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir,
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir,
		t
		);

	//現在のSRP計算
	SRP SRP_FrameNow;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_FrameNow.pos = StartSRP.pos + (EndSRP.pos - StartSRP.pos) * Percent;
	SRP_FrameNow.rot = StartSRP.rot + (EndSRP.rot - StartSRP.rot) * Percent;

	//結果マトリクス計算
	D3DXMATRIX MtxNow, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
	MtxResult = MtxNow*MtxOffset*MtxParent;                        //自分の回転マトリクス*自分オフセットマトリクス*親マトリクス

	//モデルにセット
	Node->SetNowMtx(MtxResult);						//マトリクス反映
	Node->m_SRPNow = SRP_FrameNow;					//SRPNow記録

	/*************************子供のマトリクス計算************************/
	auto itChild = Node->GetChildrenListBegin();
	auto itChildEnd = Node->GetChildrenListEnd();
	for(; itChild != itChildEnd;++itChild)
	{
		CalcPartWorldMtxRecursive(*itChild);					//再帰
	}
}

void CEditorMotion::KeyFrameChange(void)
{
	int FrameTotal = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;

	if (m_FrameNow < FrameTotal) m_FrameNow++;
	else                                //フレームが総フレーム数に達したら次のキーフレームに行く判定
	{
		int nNextKeyFrame = CalcNextKeyFrame();				//次のキーフレーム
		if (nNextKeyFrame == m_KeyFrameNow) return;			//ループではないモーションだったら以後のソース実行しない

		//次のフレームに移行時処理
		m_KeyFrameNow = nNextKeyFrame;						//次のキーフレームに移行
		m_FrameNow = 0;										//現在のフレーム番目を0番目にする

		//パーツの
		for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
		{
			m_Part[nPartNow].m_SRPBlendOld = m_Part[nPartNow].m_SRPNow;      //ブレンディング座標保存
		}
	}
}

void CEditorMotion::SetMotionNow(int MotionNumber)
{
	bool bOverRange = (MotionNumber < 0) || (MotionNumber >= m_TotalMotion);
	if(bOverRange)
	{
		MessageBox(nullptr, "入力したモーション番号は範囲外", "エラー", MB_OK | MB_ICONHAND);
		return;
	}
	m_MotionOld = m_MotionNow;

	//移行時処理
	m_MotionNow = MotionNumber;
	m_KeyFrameNow = 0;
	m_FrameNow = 0;

	for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		m_Part[nPartNow].m_SRPBlendOld = m_Part[nPartNow].m_SRPNow;
	}
}

void CEditorMotion::SetKeyFrameNow(int KeyFrame)
{
	bool bOverRange = (KeyFrame < 0) || (KeyFrame >= m_Motion[m_MotionNow].m_TotalKeyFrame);
	if (bOverRange)
	{
		MessageBox(nullptr, "入力したキーフレーム番号は範囲外", "エラー", MB_OK | MB_ICONHAND);
		return;
	}

	//移行時処理
	m_KeyFrameNow = KeyFrame;
	m_FrameNow = 0;
}

int CEditorMotion::CalcNextKeyFrame(void)
{
	int NextKeyFrame;
	if (m_KeyFrameNow < m_Motion[m_MotionNow].m_TotalKeyFrame - 1)      //今のキーフレームはこのモーションの最後のキーフレームかどうか
	{
		NextKeyFrame = m_KeyFrameNow + 1;
	}

	else
	{
		//モーションのループ判定
		if (m_Motion[m_MotionNow].m_bLoop == false)
		{
			NextKeyFrame = m_KeyFrameNow;
		}

		else
		{
			NextKeyFrame = 0;
		}
	}
	return NextKeyFrame;
}

void CEditorMotion::LinkParentAndChildren(void)
{
	for (int i = 0; i < m_PartNum; i++) 
	{
		int ParentID = m_Part[i].m_ParentID;
		if(ParentID >= 0)m_Part[i].SetParent(&m_Part[ParentID]);		//親設定
		CPart *pParent = nullptr;
		pParent = m_Part[i].GetParent();						//親取得
		if(pParent != nullptr) pParent->AddChildren(&m_Part[i]);	//子供設定
	}
}