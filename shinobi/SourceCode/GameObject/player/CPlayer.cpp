#include "CPlayer.h"
#include "CPlayerPattern.h"
#include "CPlayerNeutralPattern.h"

#define LIFE_MAX (1000)
#define STAMINA_MAX (1000)
#define DEAD_TIME (180)

CPlayer::CPlayer()
{
	m_ColliCircleMaxNum = COLLI_CIRCLE_MAX;
	m_nLife = PLAYER_LIFE_MAX;
	m_nStamina = PLAYER_STAMINA_MAX;
	m_DeadFlag = false;
	m_DeadTime = DEAD_TIME;
	m_WeaponLeft = nullptr;
	m_WeaponRight = nullptr;
	m_AttackPower = 0;
	m_WeaponMode = WEAPON_WSWORD;
	m_LockOnFlag = false;
	m_LockOnPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_State = nullptr;

	m_NoDamageTime = 0;
	m_StunDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_DamageType = ENEMY_ATTACK_TYPE_NONE;
	m_AttackType = PLAYER_ATTACK_TYPE_NONE;
	m_IsBeAttacked = false;
}

CPlayer::~CPlayer()
{
	
}

void CPlayer::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass)
{
	CHuman::Init(pos, rot, MotionPass);

	//当り判定円のセット
	CHuman::SetCalcEnableCircle(D3DXVECTOR3(0.0f, 0.0f, 0.0f),0,10.0f);

	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.6f, 0.0f), CCollisionCircle::TYPE_BODY, 0, 0.7f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.0f, 0.0f), CCollisionCircle::TYPE_BODY, 0, 0.7f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, -0.6f, 0.0f), CCollisionCircle::TYPE_BODY, 0, 0.7f);

	//プレーヤー自身のメンバ変数
	m_nLife = PLAYER_LIFE_MAX;
	m_nStamina = PLAYER_STAMINA_MAX;
	m_DeadFlag = false;
	m_DeadTime = DEAD_TIME;
	
	m_NoDamageTime = 0;
	m_StunDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_DamageType = ENEMY_ATTACK_TYPE_NONE;
	m_AttackType = PLAYER_ATTACK_TYPE_NONE;
	m_IsBeAttacked = false;

	//影
	SRP testSRP;
	testSRP.pos = pos;
	testSRP.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	testSRP.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_Shadow.Init(testSRP, "data/MODEL/CircleShadow.x");

	m_StateManager.InitAllInstance(this);
	m_State = new CPlayerNeutralPattern(this, &m_StateManager);
}

void CPlayer::Uninit(void)
{
	CHuman::Uninit();

	if (nullptr != m_WeaponLeft) {
		m_WeaponLeft->Uninit();
		delete m_WeaponLeft;
		m_WeaponLeft = nullptr;
	}
	if (nullptr != m_WeaponRight) {
		m_WeaponRight->Uninit();
		delete m_WeaponRight;
		m_WeaponRight = nullptr;
	}

	m_Shadow.Uninit();
	m_StateManager.UninitAllInstance();
}

void CPlayer::Update(void)
{
	//ステートの更新
	if(m_State != nullptr){
		m_State->Update(this);
	}
	CHuman::Update();								//モデルデータの更新
	CHuman::UpdateCollisionCircle();				//当り判定円の更新
	UpdateWeaponPosition();							//武器位置更新

	m_NoDamageTime--;								//無敵時間減少
	m_NoDamageTime = max(-1, m_NoDamageTime);		//-1以上維持

	m_Shadow.SetModelPos(m_pos);			//影位置調整
	m_Shadow.Update();

	if(m_nLife <= 0){
		m_DeadTime--;
	}

	if (m_DeadTime <= 0) {
		m_DeadFlag = true;
	}
}

void CPlayer::Draw(void)
{
	if (nullptr == m_WeaponLeft) {
		return;
	}
	if (nullptr == m_WeaponRight) {
		return;
	}

	m_Shadow.Draw();		//影描画

	//描画
	CHuman::Draw();
	m_WeaponLeft->Draw();
	m_WeaponRight->Draw();

	if (m_State != nullptr) {
		m_State->Draw();
	}
}

void CPlayer::DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje) 
{
	CHuman::DrawOnDepthMap(LightView, LightProje);
	m_WeaponLeft->DrawOnDepthMap(LightView, LightProje);
	m_WeaponRight->DrawOnDepthMap(LightView, LightProje);
}

void CPlayer::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	CHuman::DrawUseDepthMap(LightView, LightProje, DepthTexture);
	m_WeaponLeft->DrawUseDepthMap(LightView, LightProje, DepthTexture);
	m_WeaponRight->DrawUseDepthMap(LightView, LightProje, DepthTexture);

	if (m_State != nullptr) {
		m_State->Draw();
	}
}

void CPlayer::DrawSpecular(void)
{
	CHuman::DrawSpecular();

	m_WeaponLeft->DrawSpecular();
	m_WeaponRight->DrawSpecular();

	if (m_State != nullptr) {
		m_State->Draw();
	}
}

void CPlayer::DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture)
{
	CHuman::DrawMotionBlur(OriginalTexture);

	m_WeaponLeft->DrawMotionBlur(OriginalTexture);
	m_WeaponRight->DrawMotionBlur(OriginalTexture);
}

/*==============================================================================
武器01のパーツのワールドマトリクス取得
引数説明:
	pOut	:	マトリクス格納用
	PartID	:	パーツID
===============================================================================*/
void CPlayer::GetWeaponLeftPartMtx(D3DXMATRIX *pOut,int PartID)
{
	if (pOut == nullptr) 
	{
		//TODO::エラーメッセージ
		return;
	}

	if (m_WeaponLeft == nullptr) 
	{
		return;
	}

	*pOut = m_WeaponLeft->GetPartMtx(PartID);
}

void CPlayer::GetWeaponRightPartMtx(D3DXMATRIX *pOut,int PartID)
{
	if (pOut == nullptr)
	{
		//TODO::エラーメッセージ
		return;
	}

	if (m_WeaponRight == nullptr)
	{
		return;
	}

	*pOut = m_WeaponRight->GetPartMtx(PartID);
}

void CPlayer::SetWeaponLeft(CWeapon *pWeapon,const D3DXVECTOR3& offset, int ParentID)
{
	m_WeaponLeft = pWeapon;
	if (nullptr == m_WeaponLeft) {
		MessageBox(nullptr, "プレーヤーに与える武器のポインタはnullptr", "エラー", MB_ICONHAND);
		return;
	}
	m_WeaponLeft->SetOffset(offset);
	m_WeaponLeft->SetParentID(ParentID);
}

void CPlayer::SetWeaponRight(CWeapon *pWeapon, const D3DXVECTOR3& offset, int ParentID)
{
	m_WeaponRight = pWeapon;
	if (nullptr == m_WeaponRight) {
		MessageBox(nullptr, "プレーヤーに与える武器のポインタはnullptr", "エラー", MB_ICONHAND);
		return;
	}
	m_WeaponRight->SetOffset(offset);
	m_WeaponRight->SetParentID(ParentID);
}

void CPlayer::UpdateWeaponPosition(void)
{
	//武器01の位置算出
	if (m_WeaponLeft != nullptr) {
		D3DXMATRIX MtxParent = m_ModelMotion.GetPartMtx(m_WeaponLeft->GetOtherParentID());			//親のマトリクスを取得
		D3DXVECTOR3 offset = m_WeaponLeft->GetOffset();
		D3DXMATRIX MtxOffset;
		D3DXMatrixTranslation(&MtxOffset, offset.x, offset.y, offset.z);							//オフセットマトリクスを求める

		D3DXMATRIX MtxResult = MtxOffset*MtxParent;													//結果マトリクスを求める
		m_WeaponLeft->SetOtherParentMtx(MtxResult);
		m_WeaponLeft->Update();
	}

	//武器02の位置算出
	if (m_WeaponRight != nullptr) {
		D3DXMATRIX MtxParent = m_ModelMotion.GetPartMtx(m_WeaponRight->GetOtherParentID());			//親のマトリクスを取得
		D3DXVECTOR3 offset = m_WeaponRight->GetOffset();
		D3DXMATRIX MtxOffset;
		D3DXMatrixTranslation(&MtxOffset, offset.x, offset.y, offset.z);							//オフセットマトリクスを求める

		D3DXMATRIX MtxResult = MtxOffset*MtxParent;													//結果マトリクスを求める
		m_WeaponRight->SetOtherParentMtx(MtxResult);
		m_WeaponRight->Update();
	}
}

void CPlayer::SetWeaponLeftPara(CWeapon::MOTION_MODE mode, const D3DXVECTOR3 &offset, int ParentID)
{
	if (nullptr == m_WeaponLeft) return;
	m_WeaponLeft->SetMotionMode(mode);
	m_WeaponLeft->SetOffset(offset);
	m_WeaponLeft->SetParentID(ParentID);
}

void CPlayer::SetWeaponRightPara(CWeapon::MOTION_MODE mode, const D3DXVECTOR3 &offset, int ParentID)
{
	if (nullptr == m_WeaponRight) return;
	m_WeaponRight->SetMotionMode(mode);
	m_WeaponRight->SetOffset(offset);
	m_WeaponRight->SetParentID(ParentID);
}

void CPlayer::EnableWeaponHitCircle(bool bWeaponLeft, bool bWeaponRight)
{
	if (bWeaponLeft) {
		if (nullptr != m_WeaponLeft) m_WeaponLeft->EnableHitCircle();
	}

	if (bWeaponRight) {
		if (nullptr != m_WeaponRight) m_WeaponRight->EnableHitCircle();
	}
}

void CPlayer::DisableWeaponHitCircle(bool bWeaponLeft, bool bWeaponRight)
{
	if (bWeaponLeft) {
		if (nullptr != m_WeaponLeft) m_WeaponLeft->DisableHitCircle();
	}

	if (bWeaponRight) {
		if (nullptr != m_WeaponRight) m_WeaponRight->DisableHitCircle();
	}
}

CCollisionCircle* CPlayer::GetWeaponLeftColli(int Num)
{
	if (nullptr == m_WeaponLeft) return nullptr;
	return m_WeaponLeft->GetColli(Num);
}

CCollisionCircle* CPlayer::GetWeaponRightColli(int Num)
{
	if (nullptr == m_WeaponRight) return nullptr;
	return m_WeaponRight->GetColli(Num);
}

void CPlayer::SetState(CPlayerPattern* pPattern)
{
	//ヌルチェック
	if (pPattern == nullptr) {
		_RPT0(0,"[CPlayer.cpp][SetState.h]引数pPatternはヌル\n");
	}

	m_State = pPattern;
}