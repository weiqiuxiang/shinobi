#include "CHuman.h"
#include "CShowDebugFlag.h"

#define ROTATE_SPEED (D3DX_PI*0.1)

/*****************************CMoveEnvironment部分Start*********************************/
void CMoveEnvironment::SetEnvironment(float fMass, float GravityAcc, float CoeffiResist)
{
	SetGravityAcc(GravityAcc);
	SetCoeffiResist(CoeffiResist);
	SetMass(fMass);
}
/*****************************CMoveEnvironment部分End*********************************/

/***********************************CHuman部分Start*********************************/
CHuman::CHuman()
{
	//プレーヤー自分のパラメータを設定する
	m_pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//移動パラメータを設定
	m_Move.m_FaceDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_Move.m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Move.m_Speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//環境パラメータを設定
	m_Environment.m_fCoeffiResist = 0.0f;
	m_Environment.m_fGravityAcc = 0.0f;
	m_Environment.m_fMass = 0.0f;

	m_RorateSpeed = ROTATE_SPEED;
}

CHuman::~CHuman()
{
	m_ModelMotion.Uninit();
}

void CHuman::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass)
{
	//モデル部分のパラメータをセット
	m_ModelMotion.LoadMotionDataToInit(MotionPass);
	D3DXMatrixIdentity (&m_PoseMtx);
	D3DXMatrixRotationYawPitchRoll(&m_PoseMtx, rot.y, rot.x, rot.z);
	m_ModelMotion.SetPosition(pos);
	m_pos = pos;
	m_posOld = pos;
	m_ModelMotion.SetWorldRotMtx(m_PoseMtx);

	//移動パラメータの初期化
	m_Move.m_FaceDir = D3DXVECTOR3(0.0f,0.0f,1.0f);
	m_Move.m_FaceDirGoal = m_Move.m_FaceDir;
	m_Move.m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Move.m_Speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//環境パラメータを設定
	m_Environment.m_fCoeffiResist = 0.0f;
	m_Environment.m_fGravityAcc = 0.0f;
	m_Environment.m_fMass = 0.0f;

	//当り判定円全部未使用
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		m_ColliCircle[i].SetUse(false);
		m_ColliCircle[i].Uninit();
	}
	m_ColliEnable.Uninit();

	SetCalcEnableCircle(D3DXVECTOR3(0.0f,0.0f,0.0f),-1,2.0f);							//当り判定計算判定球をセット
	m_ColliCircleMaxNum = COLLI_CIRCLE_MAX;												//当り判定球MAX
}

void CHuman::Uninit(void)
{
	m_ModelMotion.Uninit();

	//当り判定円の解放
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		m_ColliCircle[i].SetUse(false);
		m_ColliCircle[i].Uninit();
	}
	m_ColliEnable.Uninit();
}

void CHuman::Update(void)
{
	//古い座標保存
	m_posOld = m_pos;

	//外力
	D3DXVECTOR3 PowerFromOut = m_Move.m_Power;

	//重力
	float fGravity = m_Environment.m_fMass * m_Environment.m_fGravityAcc;

	//抗力
	D3DXVECTOR3 CoeffiResistVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	CoeffiResistVec.x = -m_Move.m_Speed.x * m_Environment.m_fCoeffiResist;
	CoeffiResistVec.z = -m_Move.m_Speed.z * m_Environment.m_fCoeffiResist;

	//合計算出
	D3DXVECTOR3 PowerTotal;
	PowerTotal = PowerFromOut + CoeffiResistVec;
	PowerTotal.y += fGravity;

	//加速度算出
	D3DXVECTOR3 Acc;
	if (m_Environment.m_fMass > 0)
	{
		Acc = PowerTotal / m_Environment.m_fMass;   //加速度計算
		m_Move.m_Speed += Acc;         //速度計算
	}
	else m_Move.m_Speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//位置を算出
	m_pos += m_Move.m_Speed;

	CalcFaceDirNow();           //向き補間

	//外力を0にする
	m_Move.m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_ModelMotion.SetPosition(m_pos);					//位置
	m_ModelMotion.SetWorldRotMtx(m_PoseMtx);			//ポーズ(向き,傾きなど)

	//アップデート
	m_ModelMotion.Update();
}

void CHuman::Draw(void)
{
	m_ModelMotion.Draw();           //描画

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		//当り判定円の描画
		for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
		{
			if (m_ColliCircle[i].GetUse() == true)
			{
				m_ColliCircle[i].Draw();
			}
		}

		if (CShowDebugFlag::GetShowDebugFlagLayer2() == true) m_ColliEnable.Draw();
	}
#endif
}

void CHuman::DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	m_ModelMotion.DrawAllOnDepthMap(LightView, LightProje);
}

void CHuman::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture) 
{
	m_ModelMotion.DrawUseDepthMap(LightView, LightProje, DepthTexture);

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		//当り判定円の描画
		for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
		{
			if (m_ColliCircle[i].GetUse() == true)
			{
				m_ColliCircle[i].Draw();
			}
		}

		if (CShowDebugFlag::GetShowDebugFlagLayer2() == true) m_ColliEnable.Draw();
	}
#endif
}

void CHuman::DrawSpecular(void)
{
	m_ModelMotion.DrawSpecular();

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		//当り判定円の描画
		for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
		{
			if (m_ColliCircle[i].GetUse() == true)
			{
				m_ColliCircle[i].Draw();
			}
		}

		if (CShowDebugFlag::GetShowDebugFlagLayer2() == true) m_ColliEnable.Draw();
	}
#endif
}

void CHuman::DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture)
{
	m_ModelMotion.DrawMotionBlur(OriginalTexture);
}

void CHuman::SetEnvironment(float fMass, float GravityAcc, float CoeffiResist)
{
	m_Environment.SetEnvironment(fMass, GravityAcc, CoeffiResist);
}

void CHuman::SetFaceDir(const D3DXVECTOR3& dir)  //移動方向セット
{
	D3DXVECTOR3 MoveDir;
	D3DXVec3Normalize(&MoveDir, &dir);
	m_Move.m_FaceDir = MoveDir;
}

void CHuman::SetGoalDir(const D3DXVECTOR3& dir)
{
	if (dir == D3DXVECTOR3(0.0f, 0.0f, 0.0f)) return;
	D3DXVec3Normalize(&m_Move.m_FaceDirGoal, &dir);
}

void CHuman::CalcFaceDirNow(void)
{
	bool bValueChange = false;
	if (m_Move.m_FaceDirGoal != m_Move.m_FaceDir) {
		bValueChange = true;
	}

	if(bValueChange)
	{
		//回転軸
		D3DXVECTOR3 RotateVec;
		D3DXVec3Cross(&RotateVec, &m_Move.m_FaceDir, &m_Move.m_FaceDirGoal);
		D3DXVec3Normalize(&RotateVec,&RotateVec);
		if (RotateVec == D3DXVECTOR3(0.0f, 0.0f, 0.0f)) RotateVec = D3DXVECTOR3(0.0f,1.0f,0.0f);

		//回転クォータニオンを求め
		D3DXQUATERNION Q1;
		D3DXQuaternionRotationAxis(&Q1,&RotateVec, m_RorateSpeed);
		
		//回転後のベクトルの結果を求める
		D3DXMATRIX RotMtx;
		D3DXMatrixRotationQuaternion(&RotMtx, &Q1);
		D3DXVec3TransformCoord(&m_Move.m_FaceDir, &m_Move.m_FaceDir, &RotMtx);

		//目標位置に到達したのか
		D3DXVECTOR3 RotateVecAf;
		D3DXVec3Cross(&RotateVecAf, &m_Move.m_FaceDir, &m_Move.m_FaceDirGoal);
		D3DXVec3Normalize(&RotateVecAf, &RotateVecAf);
		if (RotateVecAf == D3DXVECTOR3(0.0f, 0.0f, 0.0f)) RotateVecAf = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
		
		float CheckDot = D3DXVec3Dot(&RotateVec,&RotateVecAf);		//回転角度
		if (CheckDot < 0) m_Move.m_FaceDir = m_Move.m_FaceDirGoal;
	}
}

void CHuman::UpdateCollisionCircle(void)
{
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		if (m_ColliCircle[i].GetUse() == false) continue;
		m_ColliCircle[i].Update();

		//当り判定円位置の更新
		D3DXMATRIX MtxParent = m_ModelMotion.GetPartMtx(m_ColliCircle[i].GetParentID());				//親のマトリクスを取得
		D3DXVECTOR3 offset = m_ColliCircle[i].GetOffset();
		D3DXMATRIX MtxOffset;
		D3DXMatrixTranslation(&MtxOffset, offset.x, offset.y, offset.z);							//オフセットマトリクスを求める

		D3DXMATRIX MtxResult = MtxOffset*MtxParent;													//結果マトリクスを求める
		m_ColliCircle[i].SetMtxResult(MtxResult);													//結果マトリクスを設定
	}

	m_ColliEnable.Update();
	D3DXMATRIX MtxParent = m_ModelMotion.GetPartMtx(m_ColliEnable.GetParentID());				//親のマトリクスを取得
	D3DXVECTOR3 offset = m_ColliEnable.GetOffset();
	D3DXMATRIX MtxOffset;
	D3DXMatrixTranslation(&MtxOffset, offset.x, offset.y, offset.z);							//オフセットマトリクスを求める

	D3DXMATRIX MtxResult = MtxOffset*MtxParent;													//結果マトリクスを求める
	m_ColliEnable.SetMtxResult(MtxResult);														//結果マトリクスを設定
}

CCollisionCircle* CHuman::GetColli(int ID)
{
	if (ID < 0 || ID >= COLLI_CIRCLE_MAX) {
		MessageBox(nullptr, "取得しようとしている当り判定円の番号は範囲外", "エラー", MB_OK | MB_ICONHAND);
		return nullptr;
	}
	return &m_ColliCircle[ID];
}

void CHuman::SetCollisionCircle(const D3DXVECTOR3& offset, CCollisionCircle::TYPE type, int ParentID, float Radius)
{
	for (int i = 0; i < COLLI_CIRCLE_MAX; i++)
	{
		if (m_ColliCircle[i].GetUse() == false)
		{
			m_ColliCircle[i].Init(Radius, ParentID, type);
			m_ColliCircle[i].SetOffset(offset);
			m_ColliCircle[i].SetUse(true);
			break;
		}
	}
}

void CHuman::SetCalcEnableCircle(const D3DXVECTOR3& offset, int ParentID, float Radius)
{
	m_ColliEnable.Init(Radius, ParentID, CCollisionCircle::TYPE_HIT_ENABLE);
	m_ColliEnable.SetOffset(offset);
	m_ColliEnable.SetUse(true);
}
/***********************************CHuman部分End*********************************/