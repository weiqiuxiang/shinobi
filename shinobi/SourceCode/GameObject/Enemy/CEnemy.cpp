#include "CEnemy.h"
#include "CPlayer.h"
#include "CEnemyControl.h"
#include "CShowDebugFlag.h"
#include "CRenderer.h"
#include "CCameraManager.h"
#include "CEnemyDogTopState.h"
#include "CEnemyDogStateManager.h"

#define DOG_LIFE (300)

CEnemy::CEnemy()
{
	m_EnemyType = TYPE_DOG;
	m_Controller = new CEnemyControl();
	m_Controller->SetEnemy(this);
	m_InitPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PathNumberNow = nullptr;
	m_IsPathPointIsAscendingOrder = true;

	m_NoDamageTime = 0;
	m_StunDir = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_DamageType = PLAYER_ATTACK_TYPE_NONE;
	m_AttackType = ENEMY_ATTACK_TYPE_NONE;
	m_IsBeAttacked = false;

	m_pEnemyState = nullptr;
	m_pEnemyStateManager = nullptr;
	m_UseID = -1;
}

CEnemy::~CEnemy()
{
	if (nullptr != m_Controller) {
		delete m_Controller;
		m_Controller = nullptr;
	}
}

void CEnemy::Init(ENEMY_TYPE type,const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass)
{
	CHuman::Init(pos, rot, MotionPass);	//�X�[�p�[�N���X�����̐ݒ�
	m_EnemyType = type;					//�G�^�C�v�ݒ�
	EnemyTypeToSetColliCircle();		//���蔻��~�̃Z�b�g

	//���g�����o�[�ϐ�������
	m_Controller->Init();
	m_Controller->SetEnemy(this);

	m_InitPos = pos;
	m_IsPathPointIsAscendingOrder = true;
	
	m_NoDamageTime = 0;
	m_StunDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_DamageType = PLAYER_ATTACK_TYPE_NONE;
	m_AttackType = ENEMY_ATTACK_TYPE_NONE;
	m_IsBeAttacked = false;
	m_UseID = -1;

	switch (type) {
	case TYPE_DOG:
		m_pEnemyStateManager = new CEnemyDogStateManager();
		if (m_pEnemyStateManager != nullptr) {
			m_pEnemyStateManager->InitAllInstance(this);
			m_pEnemyState = m_pEnemyStateManager->GetState(ENEMY_DOG_TOP_STATE);
		}
		break;
	}

	InitPathPoint();						//����|�C���g�̏�����
}

void CEnemy::Uninit(void)
{
	CHuman::Uninit();

	if (nullptr != m_Controller) {
		delete m_Controller;
		m_Controller = nullptr;
	}

	m_pEnemyState = nullptr;

	if (m_pEnemyStateManager != nullptr) {
		m_pEnemyStateManager->UninitAllInstance();
		delete m_pEnemyStateManager;
		m_pEnemyStateManager = nullptr;
	}

	UninitPathPoint();
}

void CEnemy::Update(void)
{
	m_Controller->EnemyControl();			//����

	if (m_pEnemyState != nullptr) {
		m_pEnemyState->Update(this);
	}

	CHuman::Update();
	CHuman::UpdateCollisionCircle();
	UpdatePathPoint();
	if (m_nLife <= 0) {
		m_DeadFlag = true;
	}

	//���G���Ԃ̌���
	m_NoDamageTime--;
	m_NoDamageTime = max(-1, m_NoDamageTime);
}
void CEnemy::UpdateEditor(void)
{
	m_Controller->EnemyControl();

	m_ModelMotion.SetPosition(m_pos);
	m_ModelMotion.SetWorldRotMtx(m_PoseMtx);
	m_ModelMotion.Update();

	//�ŏ��̏���|�C���g�̈ʒu��G�̈ʒu�ɐݒ�
	auto it = m_PathPoints.begin();
	if (*it != nullptr) {
		(*it)->SetPos(m_pos);
	}

	UpdatePathPoint();
}

void CEnemy::Draw(void)
{
	CHuman::Draw();

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlagLayer2()) {
		DrawPathPoint();
		DrawLineBetweenPathPoint();
	}
#endif

	//�X�e�[�g�`��
	if (m_pEnemyState != nullptr) {
		m_pEnemyState->Draw();
	}
}

void CEnemy::DrawPath(void)
{
	DrawPathPoint();
	DrawLineBetweenPathPoint();
}

void CEnemy::DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	CHuman::DrawOnDepthMap(LightView, LightProje);
}

void CEnemy::InitPathPoint(void)
{
	//���X�g�N���A
	for(auto it = m_PathPoints.begin(); it != m_PathPoints.end();++it){
		(*it)->Uninit();
		delete (*it);
	}
	m_PathPoints.clear();

	//����|�C���g���Ȃ��Ƃ�����݂���,����͎����̏����ʒu
	CPathPoint* pPathPoint = new CPathPoint();
	pPathPoint->Init(m_pos, 0);
	m_PathPoints.push_back(pPathPoint);

	//��ڂ̏���|�C���g�̃|�C���^�����|�C���g�ɐݒ�
	m_PathNumberNow = pPathPoint;
}

void CEnemy::UpdatePathPoint(void)
{
	for (auto it = m_PathPoints.begin(); it != m_PathPoints.end(); ++it) {
		(*it)->Update();
	}
}

void CEnemy::DrawPathPoint(void)
{
	for (auto it = m_PathPoints.begin(); it != m_PathPoints.end(); ++it) {
		(*it)->Draw();
	}
}

void CEnemy::UninitPathPoint(void)
{
	//���X�g�N���A
	for (auto it = m_PathPoints.begin(); it != m_PathPoints.end(); ++it) {
		(*it)->Uninit();
		delete (*it);
	}
	m_PathPoints.clear();
}

void CEnemy::DrawLineBetweenPathPoint(void)
{
	
	//����|�C���g��0�̏ꍇ�`�悵�Ȃ�
	int PointNum = m_PathPoints.size();
	if (PointNum == 0) {
		return;
	}
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	
	//�e�X�g
	D3DXVECTOR3 *vertexList = new D3DXVECTOR3[PointNum + 1];
	vertexList[0] = m_pos;

	int nCnt = 1;
	for (auto it = m_PathPoints.begin(); it != m_PathPoints.end(); ++it, nCnt++) {
		D3DXVECTOR3 PointPos = (*it)->GetPos();
		vertexList[nCnt] = PointPos;
	}

	D3DXMATRIX WorldMatrix;
	D3DXMatrixIdentity(&WorldMatrix);							//���[���h�s���P�ʍs��ŏ�����
	D3DXMATRIX resultMtx = WorldMatrix*CCameraManager::GetActiveCameraViewMtx()*CCameraManager::GetActiveCameraProjeMtx();
	LPD3DXLINE _line;
	D3DXCreateLine(pDevice, &_line);
	_line->SetWidth(10.0f);
	_line->Begin();
	_line->DrawTransform(vertexList, PointNum+1, &resultMtx, D3DCOLOR_RGBA(255,255,0,100));
	_line->End();

	//�N���A
	delete[] vertexList;
	vertexList = nullptr;
	_line->Release();
}

void CEnemy::SetPlayer(CPlayer* pPlayer)
{
	m_Controller->SetPlayer(pPlayer);
}

CPlayer* CEnemy::GetPlayer(void)
{
	return m_Controller->GetPlayer();
}

void CEnemy::AddPathPoint(const D3DXVECTOR3& pos, int StopFrame)
{
	CPathPoint* pPathPoint = new CPathPoint();
	pPathPoint->Init(pos, StopFrame);
	m_PathPoints.push_back(pPathPoint);
}

void CEnemy::DeletePathPointUsePointer(CPathPoint* pPath)
{
	auto FindIt = std::find(m_PathPoints.begin(), m_PathPoints.end(), pPath);
	if (FindIt != m_PathPoints.end()) {
		(*FindIt)->Uninit();
		delete (*FindIt);
		(*FindIt) = nullptr;
		m_PathPoints.erase(FindIt);
	}
}

void CEnemy::InsertPathPointUsePointer(const D3DXVECTOR3& pos, int StopFrame,CPathPoint* pPath)
{
	auto FindIt = std::find(m_PathPoints.begin(), m_PathPoints.end(), pPath);
	if (FindIt != m_PathPoints.end()) {
		CPathPoint* pPathPoint = new CPathPoint();
		pPathPoint->Init(pos, StopFrame);
		m_PathPoints.insert(FindIt, pPathPoint);
	}
}

void CEnemy::DeletePathPoint(void)
{
	if (m_PathPoints.size() == 0) return;
	auto itEnd = m_PathPoints.end();
	auto itBack = std::prev(itEnd, 1);
	(*itBack)->Uninit();
	delete (*itBack);
	m_PathPoints.erase(itBack);
}

std::list<CPathPoint*>::iterator CEnemy::GetPathPointListNumber(int Number)
{
	//�������^�[��
	if (Number < 0) { MessageBox(nullptr, "���̗v�f�Ԗڂ͑��݂��Ȃ�,�C�e���[�^�[��End��Ԃ�", "�֐�CEnemyStorage::GetEnemiesListNumber", MB_ICONHAND); return m_PathPoints.end(); }

	auto it = m_PathPoints.begin();
	for (int nCnt = 0; it != m_PathPoints.end(); ++it, ++nCnt) {
		if (nCnt == Number) return it;
	}
	MessageBox(nullptr, "���̗v�f�Ԗڂ͑��݂��Ȃ�,�C�e���[�^�[��End��Ԃ�", "�֐�CEnemyStorage::GetEnemiesListNumber", MB_ICONHAND);
	return m_PathPoints.end();
}

std::list<CPathPoint*>::iterator CEnemy::GetPathPointListPrev(CPathPoint* pPoint, int Num) {
	auto Found = std::find(m_PathPoints.begin(), m_PathPoints.end(), pPoint);
	if (Found == m_PathPoints.end()) return m_PathPoints.end();
	auto prevElement = std::prev(Found, Num);
	return prevElement;
}

std::list<CPathPoint*>::iterator CEnemy::GetPathPointListNext(CPathPoint* pPoint, int Num) {
	auto Found = std::find(m_PathPoints.begin(), m_PathPoints.end(), pPoint);
	if (Found == m_PathPoints.end()) return m_PathPoints.end();
	auto nextElement = std::next(Found, Num);
	return nextElement;
}

void CEnemy::EnableHitCircle(void)
{
	for (int i = 0; i < m_ColliCircleMaxNum; i++) {
		if (m_ColliCircle[i].GetUse() == false) continue;
		if (m_ColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT_PREVIEW) continue;
		m_ColliCircle[i].SetType(CCollisionCircle::TYPE_HIT);
	}
}

void CEnemy::DisableHitCircle(void)
{
	for (int i = 0; i < m_ColliCircleMaxNum; i++) {
		if (m_ColliCircle[i].GetUse() == false) continue;
		if (m_ColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;
		m_ColliCircle[i].SetType(CCollisionCircle::TYPE_HIT_PREVIEW);
	}
}

void CEnemy::EnemyTypeToSetColliCircle(void)
{
	//���蔻�苅�̐ݒu
	switch (m_EnemyType) 
	{
	case TYPE_DOG:			//�G�͌��̏ꍇ
		CHuman::SetCalcEnableCircle(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0, 10.0f);

		CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.0f, -1.0f), CCollisionCircle::TYPE_BODY, 0, 1.0f);
		CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.0f, 0.0f), CCollisionCircle::TYPE_BODY, 0, 1.0f);
		CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.0f, 1.0f), CCollisionCircle::TYPE_BODY, 0, 1.0f);
		CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.0f, 0.0f), CCollisionCircle::TYPE_BODY, 2, 1.0f);

		CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.0f, 1.2f), CCollisionCircle::TYPE_HIT_PREVIEW, 2, 1.0f);

		m_nLife = DOG_LIFE;
		m_DeadFlag = false;
		break;
	case TYPE_VILLAGER:		//�G�͑��l�̏ꍇ
		break;
	case TYPE_BIGMAN:		//�G�͑�j�̏ꍇ
		break;
	default:
		m_EnemyType = TYPE_DOG;
		break;
	}
}