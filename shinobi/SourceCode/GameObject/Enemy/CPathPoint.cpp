#include "CPathPoint.h"

//ƒ}ƒNƒ
#define PATH_BALL_RADIUS (1.0f)

CPathPoint::CPathPoint() {
	m_CPathBall = nullptr;
	m_DrawType = DRAW_USE_MTRL_DEFALUT;
}

CPathPoint::~CPathPoint() {
	if (nullptr != m_CPathBall) {
		m_CPathBall->Uninit();
		delete m_CPathBall;
		m_CPathBall = nullptr;
	}
}

void CPathPoint::Init(const D3DXVECTOR3& pos,int StopFrame) {
	if (nullptr != m_CPathBall) {
		m_CPathBall->Uninit();
		delete m_CPathBall;
		m_CPathBall = nullptr;
	}
	m_CPathBall = new CPathBall();
	m_CPathBall->Init(PATH_BALL_RADIUS);
	m_CPathBall->SetPos(pos);

	m_pos = pos;
	m_StopFrame = StopFrame;
}

void CPathPoint::Update(void) {
	m_CPathBall->SetPos(m_pos);
	m_CPathBall->Update();
}

void CPathPoint::Draw(void) {
	if (m_CPathBall == nullptr) {
		return;
	}

	if (m_DrawType == DRAW_USE_MTRL_DEFALUT) {
		m_CPathBall->Draw();
	}
	else if (m_DrawType == DRAW_USE_MTRL_SELECTED) {
		m_CPathBall->DrawSelectedByEditor();
	}
}

void CPathPoint::Uninit(void) {
	if (nullptr != m_CPathBall) {
		m_CPathBall->Uninit();
		delete m_CPathBall;
		m_CPathBall = nullptr;
	}
}