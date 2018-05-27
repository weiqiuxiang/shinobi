#include "CModelMotion.h"
#include "CTextLoad.h"
#include "CPolygonMath.h"
#include <stdio.h>
#include "CHermitian.h"

CModelMotion::CModelMotion()
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_WorldRotMtx);
	D3DXMatrixIdentity(&m_OtherParentMtx);
	m_MotionNow = 0;
	m_MotionOld = 0;
}

CModelMotion::~CModelMotion()
{
	for (int i = 0; i < m_PartNum; i++) {
		m_Part[i].Uninit();
	}
}

void CModelMotion::Update(void)
{
	KeyFrameChange();
	CalcWorldMtx();
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Update();
	}
}

void CModelMotion::Draw(void)
{
	for(int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].Draw();
	}
}

void CModelMotion::DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawOnDepthMap(LightView, LightProje);
	}
}

void CModelMotion::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawUseDepthMap(LightView, LightProje, DepthTexture);
	}
}

void CModelMotion::DrawSpecular(void)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawSpecular();
	}
}

void CModelMotion::DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture)
{
	for (int i = 0; i < m_PartNum; i++)
	{
		m_Part[i].DrawMotionBlur(OriginalTexture);
	}
}

void CModelMotion::Uninit(void)
{
	for (int i = 0; i < m_PartNum; i++) {
		m_Part[i].Uninit();
	}
}

void CModelMotion::LoadMotionDataToInit(const char* MotionPass)
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
	char buf[1024];            //文字バッファ
	CTextLoad::FindFont(fp,TOKEN, "NUM_MODEL=");            //モデルフォントを探す
	CTextLoad::GetStrToken(fp, TOKEN, buf);
	//パーツ数取得
	m_PartNum = atoi(buf);
	if (m_PartNum > MAX_PART) {
		MessageBox(nullptr,"パーツ数が上限に達した","[CModelMotion.cpp][LoadMotionDataToInit]",MB_OK);
	}

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
		CalcWorldMtx();						//モデルのワールドマトリクス計算
	}
}

void CModelMotion::LoadPartData(FILE *fp)
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
		m_Part[i].m_Model.Init(SRP, buf);
	}

	//パーツのオフセットとID番号を読み込む
	for (int i = 0; i < m_PartNum; i++)
	{
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

void CModelMotion::LoadMotionData(FILE *fp)
{
	if (nullptr == fp) return;

	m_TotalMotion = 0;             //総モーション数を0にする
	char buf[1024];            //文字バッファ

	//モーション情報の読み込み
	while ( (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1) && (m_TotalMotion < MOTION_MAX) )
	{
		//ループフラグの読み込み
		if (strcmp("LOOP=", buf) != 0) continue;      //文字LOOP=を見つかるまでループ
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
}

void CModelMotion::CalcPartWorldMtx(void)
{
	int NextKeyFrame = CalcNextKeyFrame();       //次のキーフレームを求める

	//必要な変数
	D3DXMATRIX MtxOffset, MtxParent,MtxResult,MtxWorld;
	SRP SRP_WorldMtx,SRP_FrameNow,SRP_OldStart,SRP_StartEnd;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//世界のSRPマトリクス計算
	SRP_WorldMtx.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_WorldMtx.pos = m_pos;

	//ワールドマトリクスの作成
	D3DXMATRIX PosMtx, SclMtx;
	D3DXMatrixScaling(&SclMtx, SRP_WorldMtx.scl.x, SRP_WorldMtx.scl.y, SRP_WorldMtx.scl.z);
	D3DXMatrixTranslation(&PosMtx, SRP_WorldMtx.pos.x, SRP_WorldMtx.pos.y, SRP_WorldMtx.pos.z);
	MtxWorld = SclMtx*m_WorldRotMtx*PosMtx;								//
	m_WorldMtx = MtxWorld;												//ワールドマトリクスの保存

	//パーツごとのマトリクスの計算
	for(int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		//親のSRP(scl,rot,pos)取得
		int ParentID = m_Part[nPartNow].m_ParentID;          //親のID
		if (-1 == ParentID) //親の親は世界の場合
		{
			MtxParent = MtxWorld * m_OtherParentMtx;		//m_OtherParentMtxをセットしている場合だけ,m_OtherParentMtxが反映される
		}
		else
		{
			MtxParent = m_Part[ParentID].m_MtxNow;
		}

		CPolygonMath::CalcMtxFromSRP(&MtxOffset, m_Part[nPartNow].m_SRPOffset);      //オフセットマトリクス計算

		SRP StartSRP = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_SRPData[nPartNow];  //今のキーフレームのSRPを求め
		SRP EndSRP = m_Motion[m_MotionNow].m_KeyFrame[NextKeyFrame].m_SRPData[nPartNow];     //次のキーフレームのSRP求める
		SRP OldBlendSRP = m_Part[nPartNow].m_SRPBlendOld;                            //ブレンディングSRPを求める
		float t = (float)m_FrameNow / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;      //今のフレーム目は総フレームの何％

		//現在のSRP計算
		SRP_OldStart.pos = OldBlendSRP.pos * (1 - t) + StartSRP.pos * t;
		SRP_OldStart.rot = OldBlendSRP.rot * (1 - t) + StartSRP.rot * t;
		SRP_StartEnd.pos = StartSRP.pos * (1 - t) + EndSRP.pos * t;
		SRP_StartEnd.rot = StartSRP.rot * (1 - t) + EndSRP.rot * t;
		SRP_FrameNow.pos = SRP_OldStart.pos * (1 - t) + SRP_StartEnd.pos * t;
		SRP_FrameNow.rot = SRP_OldStart.rot * (1 - t) + SRP_StartEnd.rot * t;

		D3DXMATRIX MtxNow;
		CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
		MtxResult = MtxNow*MtxOffset*MtxParent;                        //自分の回転マトリクス*自分オフセットマトリクス*親マトリクス

		//モデルにセット
		m_Part[nPartNow].m_MtxOld = m_Part[nPartNow].m_MtxNow;		//古いのマトリクスを保存
		m_Part[nPartNow].SetNowMtx(MtxResult);						//マトリクス反映
		m_Part[nPartNow].m_SRPNow = SRP_FrameNow;					//SRPNow記録
	}
}

void CModelMotion::CalcWorldMtx(void)
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
		MessageBox(nullptr, "一番上の親はない！", "関数CModelMotion::CalcWorldMtxエラー発生", MB_ICONHAND);
		return;
	}

	CalcPartWorldMtxRecursive(pTopNode);			//再帰的なモーション計算
}

void CModelMotion::CalcPartWorldMtxRecursive(CPart* Node)
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
	SRP OldBlendSRP = m_Part[nPartID].m_SRPBlendOld;										//ブレンディングSRPを求める
	float t = (float)m_FrameNow / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;		//今のフレーム目は総フレームの何％

	//エルミート曲線を使って、変化量のパーセンテージを求め
	float Percent = CHermitian::GetChangeValuePercent(
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir,
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir,
		t
		);

	//現在のSRP計算
	SRP SRP_FrameNow, SRP_OldStart, SRP_StartEnd;
	SRP_FrameNow.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	SRP_OldStart.pos = OldBlendSRP.pos * (1 - Percent) + StartSRP.pos * Percent;
	SRP_OldStart.rot = OldBlendSRP.rot * (1 - Percent) + StartSRP.rot * Percent;
	SRP_StartEnd.pos = StartSRP.pos * (1 - Percent) + EndSRP.pos * Percent;
	SRP_StartEnd.rot = StartSRP.rot * (1 - Percent) + EndSRP.rot * Percent;
	SRP_FrameNow.pos = SRP_OldStart.pos * (1 - Percent) + SRP_StartEnd.pos * Percent;
	SRP_FrameNow.rot = SRP_OldStart.rot * (1 - Percent) + SRP_StartEnd.rot * Percent;

	//結果マトリクス計算
	D3DXMATRIX MtxNow, MtxResult;
	CPolygonMath::CalcMtxFromSRP(&MtxNow, SRP_FrameNow);
	MtxResult = MtxNow*MtxOffset*MtxParent;                        //自分の回転マトリクス*自分オフセットマトリクス*親マトリクス

	//モデルにセット
	Node->m_MtxOld = Node->m_MtxNow;		//古いのマトリクスを保存
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

void CModelMotion::KeyFrameChange(void)
{
	int FrameTotal = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame;

	if (m_FrameNow < FrameTotal) m_FrameNow++;
	else                                //フレームが総フレーム数に達したら次のキーフレームに行く判定
	{
		int nNextKeyFrame = CalcNextKeyFrame();           //次のキーフレーム
		if (nNextKeyFrame == m_KeyFrameNow) return;       //ループではないモーションだったら以後のソース実行しない

		//次のフレームに移行時処理
		m_KeyFrameNow = nNextKeyFrame;                    //次のキーフレームに移行
		m_FrameNow = 0;                                   //現在のフレーム番目を0番目にする
		//パーツの
		for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
		{
			m_Part[nPartNow].m_SRPBlendOld = m_Part[nPartNow].m_SRPNow;      //ブレンディング座標保存
		}
	}
}

void CModelMotion::SetMotionNow(int MotionNumber)
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

void CModelMotion::SetDrawType(CModel::DRAW_TYPE type)
{
	for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		m_Part[nPartNow].m_Model.SetDrawType(type);
	}
}

void CModelMotion::SetMaterial(const D3DMATERIAL9& Mtrl)
{
	for (int nPartNow = 0; nPartNow < m_PartNum; nPartNow++)
	{
		m_Part[nPartNow].m_Model.SetMaterial(Mtrl);
	}
}

int CModelMotion::CalcNextKeyFrame(void)
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

void CModelMotion::LinkParentAndChildren(void)
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