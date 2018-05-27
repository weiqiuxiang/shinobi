#pragma once
#ifndef _CHUMAN_H_
#define _CHUMAN_H_

#include "main.h"
#include "CModel.h"
#include "CModelMotion.h"
#include "CCollisionCircle.h"

#define COLLI_CIRCLE_MAX (20)

class CMove
{
public:
	//メンバ変数
	D3DXVECTOR3 m_Speed;              //速度ベクトル(方向含む)
	D3DXVECTOR3 m_Power;              //プレーヤー受けった外力
	D3DXVECTOR3 m_FaceDir;            //プレーヤーの移動方向
	D3DXVECTOR3 m_FaceDirGoal;
};

class CMoveEnvironment
{
public:
	//メンバ関数
	//セッター
	void SetEnvironment( float fMass, float GravityAcc, float CoeffiResist);   //環境パラメータ一括設定
	void SetGravityAcc(float GravityAcc) { m_fGravityAcc = GravityAcc; }
	void SetCoeffiResist(float fCoeffiResist) { m_fCoeffiResist = max(0.0f, fCoeffiResist); }
	void SetMass(float fMass) { m_fMass = max(0, fMass); }

	//ゲッター
	float GetGravityAcc(void) { return m_fGravityAcc; }
	float GetCoeffiResist(void) { return m_fCoeffiResist; }
	float GetMass(void) { return m_fMass; }

	//メンバ変数
	float    m_fGravityAcc;          //重力加速度
	float    m_fCoeffiResist;        //抵抗係数
	float    m_fMass;                //質量
};

class CHuman            //一時的にstaticを設定する
{
public:
	CHuman();
	~CHuman();

	//Init,Uninit,Update,Draw関数
	virtual void Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	virtual void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	virtual void DrawSpecular(void);
	virtual void DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture);

	//セッター
	void SetEnvironment(float fMass, float GravityAcc, float CoeffiResist); //環境パラメータ一括設定
	void SetPos(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }
	void SetSpeed(const D3DXVECTOR3& Speed) { m_Move.m_Speed = Speed; }
	void SetSpeedY(const float Speed) { m_Move.m_Speed.y = Speed; }
	void SetPosX(float PosX) { m_pos.x = PosX; }
	void SetPosY(float PosY) { m_pos.y = PosY; }
	void SetPosZ(float PosZ) { m_pos.z = PosZ; }
	void SetRotX(float RotX) { m_rot.x = RotX; }
	void SetRotY(float RotY) { m_rot.y = RotY; }
	void SetRotZ(float RotZ) { m_rot.z = RotZ; }
	void SetFaceDir(const D3DXVECTOR3& dir);
	void SetPoseMtx(const D3DXMATRIX& PoseMtx) { m_PoseMtx = PoseMtx; }
	void SetGoalDir(const D3DXVECTOR3& Dir);
	void SetRorateSpeed(float speed) { m_RorateSpeed = (float)max(0.01, speed); }
	void SetCoffi(float Coffi) { m_Environment.m_fCoeffiResist = Coffi; }
	void SetOtherParentMtx(const D3DXMATRIX& worldMtx) { m_ModelMotion.SetOtherParentMtx(worldMtx); }
	void SetMaterial(const D3DMATERIAL9& Mtrl) { m_ModelMotion.SetMaterial(Mtrl); }

	void AddPos(const D3DXVECTOR3& pos) { m_pos += pos; }
	void AddPower(const D3DXVECTOR3& Power) { m_Move.m_Power += Power; }
	void AddSpeed(const D3DXVECTOR3& Speed) { m_Move.m_Speed += Speed; }
	void SetMotionMode(int MotionNum) { m_ModelMotion.SetMotionNow(MotionNum); }
	void SetDrawType(CModel::DRAW_TYPE type) { m_ModelMotion.SetDrawType(type); }

	//ゲッター
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetBodyPos(void) { return m_ModelMotion.GetPartPosInWorld(0); }		//親構造で一番上層のパーツの位置取得
	D3DXVECTOR3 GetRot(void) { return m_rot; }
	D3DXVECTOR3 GetFaceDir(void) { return m_Move.m_FaceDir; }
	D3DXVECTOR3 GetSpeed(void) { return m_Move.m_Speed; }
	D3DXMATRIX GetBodyResultMtx(void) { return m_ModelMotion.GetPartMtx(0); }
	LPD3DXMESH GetBodyMesh(void) { return m_ModelMotion.GetMesh(0); }
	int GetMotionNow(void) { return m_ModelMotion.GetMotionNow(); }

	//他の関数
	int GetMotionTotalFrame(int MotionID) { return m_ModelMotion.GetTotalFrame(MotionID); }
	int GetMotionTotalKeyFrame(int MotionID) { return m_ModelMotion.GetTotalKeyFrame(MotionID); }

	int GetFrameOfOneKeyFrame(int MotionID, int KeyFrameID) { return m_ModelMotion.GetFrameOfKeyFrame(MotionID, KeyFrameID); }
	int GetKeyFrameNow(void) { return m_ModelMotion.GetKeyFrameNow(); }					//現在進行中のキーフレーム
	int GetMaxColliCircleNum(void) { return m_ColliCircleMaxNum; }						//当り判定円の最大数
	D3DXVECTOR3 GetVecOldPosToNowPos(void) { return m_pos - m_posOld; }

	//他の関数
	void ReturnOldPos(void) { m_pos = m_posOld; }										//前の座標に戻る
	void ReturnOldPosXZ(void) { m_pos.x = m_posOld.x; m_pos.z = m_posOld.z;}			//XZ軸の部分だけ前の座標に戻る
	inline bool TheSamePosition(void){													//前と同じ座標がどうか(同じだったら,trueを返す)
		return (m_pos == m_posOld) ? true : false;
	}
	CCollisionCircle* GetColli(int ID);
	CCollisionCircle* GetColliEnable(void) { return &m_ColliEnable; }
protected:
	//メンバ関数
	void CalcFaceDirNow(void);						//今の向き計算
	void UpdateCollisionCircle(void);
	void SetCollisionCircle(const D3DXVECTOR3& offset, CCollisionCircle::TYPE type, int ParentID, float Radius);				//当り判定円セット
	void SetCalcEnableCircle(const D3DXVECTOR3& offset, int ParentID, float Radius);

	//メンバ変数
	CModelMotion		m_ModelMotion;			//プレーヤーのモーションユニット
	D3DXVECTOR3			m_pos;					//プレーヤーの座標
	D3DXVECTOR3			m_posOld;				//プレーヤーの古い座標
	D3DXVECTOR3			m_rot;					//プレーヤーキャラの回転
	D3DXMATRIX			m_PoseMtx;				//Poseマトリクス
	CMove				m_Move;					//移動パラメータ
	CMoveEnvironment	m_Environment;			//環境パラメータ
	float				m_RorateSpeed;			//(向き変更の)回転速度

	//当り判定円メンバ変数
	CCollisionCircle	m_ColliCircle[COLLI_CIRCLE_MAX];		//当り判定球
	CCollisionCircle	m_ColliEnable;							//当り判定計算を有効にする球
	int					m_ColliCircleMaxNum;					//当り判定円最大数(外部用)
};

#endif