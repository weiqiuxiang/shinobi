#include "CHermitian.h"
#include "CFontUIScaleFactory.h"
#include "CRenderer.h"
#include "MousePosition.h"
#include "CCollisionMath.h"
#include "SceneTransition.h"

#define TOTAL_SIZEX (220.0f)
#define TOTAL_SIZEY (220.0f)

#define DEFAULT_DRAW_POSX (SCREEN_WIDTH*0.8)	//�`��̎��Ȑ��̈ʒuX(����)
#define DEFAULT_DRAW_POSY (SCREEN_HEIGHT*0.8)	//�`��̎��Ȑ��̈ʒuY(����)

#define START_POINT_POSX (10.0f)			//�N�_���WX(����)
#define START_POINT_POSY (-10.0f)			//�N�_���WY(����)

#define END_POINT_POSX (210.0f)				//�I�_���WX(����)
#define END_POINT_POSY (-210.0f)			//�I�_���WY(����)

#define GRID_INTERVAL (50.0f)

#define CURVE_POINT_NUM (40)				//�Ȑ������|�C���g���ŕ`�悷�邩(�N�_�ƏI�_������)
#define DIR_VECTOR_RATE (5)					//�n�_�I�_�����x�N�g���̊g�嗦

CHermitian::CHermitian() 
{
	m_DrawPos = D3DXVECTOR2(DEFAULT_DRAW_POSX, DEFAULT_DRAW_POSY);
	m_StartDir = D3DXVECTOR2(50.0f, -50.0f);
	m_EndDir = D3DXVECTOR2(-50.0f, 50.0f);

	m_Background = nullptr;
	m_StartDirPoint = nullptr;
	m_EndDirPoint = nullptr;
	m_MouseHoverFlags = LOCK_NONE;
	m_MousePressFlags = LOCK_NONE;

	m_StartPos = m_DrawPos + D3DXVECTOR2(START_POINT_POSX, START_POINT_POSY);
	m_EndPos = m_DrawPos + D3DXVECTOR2(END_POINT_POSX, END_POINT_POSY);

	m_bUse = false;
	m_bPreView = false;
}

CHermitian::~CHermitian() 
{
	MyNameSpace::DeleteInitialize(m_StartDirPoint);
	m_StartDirPoint = nullptr;
	MyNameSpace::DeleteInitialize(m_EndDirPoint);
	m_EndDirPoint = nullptr;
	MyNameSpace::DeleteInitialize(m_Background);
	m_Background = nullptr;
}

void CHermitian::Init(const D3DXVECTOR2& DrawPos,const D3DXVECTOR2& StartDir, const D3DXVECTOR2& EndDir)
{
	Uninit();		//�f�[�^���

	m_DrawPos = DrawPos;

	//�N�_�ƏI�_�̕���������
	m_StartDir = StartDir;
	m_EndDir = EndDir;

	m_StartPos = m_DrawPos + D3DXVECTOR2(START_POINT_POSX, START_POINT_POSY);
	m_EndPos = m_DrawPos + D3DXVECTOR2(END_POINT_POSX, END_POINT_POSY);

	//�`�敔�ϐ�������
	m_CurvePointList.clear();

	float BGX = m_DrawPos.x + TOTAL_SIZEX * 0.5f;
	float BGY = m_DrawPos.y - TOTAL_SIZEY * 0.5f;
	m_Background = new CScene2D;
	m_Background->Init(D3DXVECTOR3(BGX, BGY,0.0f),D3DXVECTOR3(TOTAL_SIZEX, TOTAL_SIZEY,0.0f), HERMITIAN_BG_TEX);

	D3DXVECTOR2 StartPos = m_StartPos + m_StartDir;
	m_StartDirPoint = new CScene2D;
	m_StartDirPoint->Init(D3DXVECTOR3(StartPos.x, StartPos.y, 0.0f),D3DXVECTOR3(10.0f,10.0f,0.0f), HERMITIAN_DRAG_POINT_TEX);

	D3DXVECTOR2 EndPos = m_EndPos + m_EndDir;
	m_EndDirPoint = new CScene2D;
	m_EndDirPoint->Init(D3DXVECTOR3(EndPos.x, EndPos.y, 0.0f), D3DXVECTOR3(10.0f, 10.0f, 0.0f), HERMITIAN_DRAG_POINT_TEX);

	m_MouseUsing = false;
	m_MouseHoverFlags = LOCK_NONE;
	m_MousePressFlags = LOCK_NONE;
	m_MousePressFlagsOld = m_MousePressFlags;
	m_bUse = false;
}

void CHermitian::Update(void)
{
	if (false == m_bUse) {
		return;
	}

	MouseControl();				//�}�E�X�Ń|�C���g�̍��W��������
	LimitedPosition();			//�ʒu����
	UpdateCoordinate();			//���W�n�X�V
	UpdateCurve();				//�Ȑ��X�V

	if (nullptr != m_Background) {
		m_Background->Update();
	}
	if (nullptr != m_StartDirPoint) {
		m_StartDirPoint->Update();
	}
	if (nullptr != m_EndDirPoint){
		m_EndDirPoint->Update();
	}
}

/*==============================================================================
	�Ȑ�����`��Ԃɖ߂�
===============================================================================*/
void CHermitian::CurveReset(void)
{
	m_StartDir = D3DXVECTOR2(50.0f, -50.0f);
	m_EndDir = D3DXVECTOR2(-50.0f, 50.0f);

	SetStartDir(m_StartDir);
	SetEndDir(m_EndDir);
}

void CHermitian::UpdateCoordinate(void)
{
	if (nullptr == m_Background) return;
	D3DXVECTOR3 Pos = m_Background->GetPosition();
	m_DrawPos.x = Pos.x - TOTAL_SIZEX * 0.5f;
	m_DrawPos.y = Pos.y + TOTAL_SIZEY * 0.5f;

	if (nullptr == m_StartDirPoint) {
		return;
	}
	if (nullptr == m_EndDirPoint) {
		return;
	}

	m_StartPos = m_DrawPos + D3DXVECTOR2(START_POINT_POSX, START_POINT_POSY);
	m_EndPos = m_DrawPos + D3DXVECTOR2(END_POINT_POSX, END_POINT_POSY);

	//�n�_�ƏI�_�̐ڐ��x�N�g�������߂�
	m_StartDir.x = m_StartDirPoint->GetPosition().x - m_StartPos.x;
	m_StartDir.y = m_StartDirPoint->GetPosition().y - m_StartPos.y;
	m_EndDir.x = m_EndDirPoint->GetPosition().x - m_EndPos.x;
	m_EndDir.y = m_EndDirPoint->GetPosition().y - m_EndPos.y;
}

void CHermitian::UpdateCurve(void)
{
	m_CurvePointList.clear();			//���X�g�N���A

	m_StartPos = m_DrawPos + D3DXVECTOR2(START_POINT_POSX, START_POINT_POSY);
	m_CurvePointList.push_back(m_StartPos);	//�n�_
	for (int PointNum = 0; PointNum < CURVE_POINT_NUM; PointNum++) {
		//�G���~�[�g�Ȑ��̎l�̗v�f������
		float t = (float) (PointNum + 1) / (CURVE_POINT_NUM + 1);
		float h00 = ( 2*t*t*t ) - ( 3*t*t ) + 1;
		float h01 = (-2*t*t*t) + (3*t*t);
		float h10 = (t*t*t) - (2*t*t) + t;
		float h11 = (t*t*t) - (t*t);

		//���_�̍��W������
		D3DXVECTOR2 VertexPos;
		VertexPos.x = h00 * START_POINT_POSX + h01 * END_POINT_POSX + h10 * m_StartDir.x * DIR_VECTOR_RATE + h11 * -m_EndDir.x * DIR_VECTOR_RATE;
		VertexPos.y = h00 * START_POINT_POSY + h01 * END_POINT_POSY + h10 * m_StartDir.y * DIR_VECTOR_RATE + h11 * -m_EndDir.y * DIR_VECTOR_RATE;
		VertexPos += m_DrawPos;

		//���_�ۑ�
		m_CurvePointList.push_back(VertexPos);
	}
	m_EndPos = m_DrawPos + D3DXVECTOR2(END_POINT_POSX, END_POINT_POSY);
	m_CurvePointList.push_back(m_EndPos);	//�I�_
}

void CHermitian::Draw(void)
{
	if (false == m_bUse) return;
	if (nullptr == m_Background) return;
	m_Background->Draw();	//���w�i�`��
	DrawCoordinate();		//���W�n�`��
	DrawHermitianCurve();	//�G���~�[�g�Ȑ��`��
	DrawVectorPoint();		//�n�_�ƏI�_�̓�����x�N�g���y�уh���b�O�|�C���g�`��
}

void CHermitian::Uninit(void)
{
	MyNameSpace::DeleteInitialize(m_StartDirPoint);
	m_StartDirPoint = nullptr;
	MyNameSpace::DeleteInitialize(m_EndDirPoint);
	m_EndDirPoint = nullptr;
	MyNameSpace::DeleteInitialize(m_Background);
	m_Background = nullptr;
}

/*==============================================================================
	�h���b�N�|�C���g�𑀍삵����
		�߂�l���� : 
		true : ���삵��
		false : ���삵�Ȃ�����
===============================================================================*/
bool CHermitian::IsDragedDragPoint(void)
{
	bool IsDragedDragPointBefore = (m_MousePressFlagsOld == LOCK_START || m_MousePressFlagsOld == LOCK_END);
	bool IsReleaseMouseInThisFrame = (m_MousePressFlags == LOCK_NONE);

	bool IsDragedDragPointFlag = false;
	if (IsDragedDragPointBefore && IsReleaseMouseInThisFrame) {
		IsDragedDragPointFlag = true;
	}
	return IsDragedDragPointFlag;
}

/*==============================================================================
	�ω��ʂ̃p�[�Z���e�[�W�擾
		��������:
		StartDir: �n�_�̕����x�N�g��
		EndDir: �I�_�̕����x�N�g��
		t: ���Ԃ̃p�[�Z���e�[�W(�l�͈�0~1)

		�߂�l:
		0: t��0�ȉ��̎�
		1: t��1�ȏ�̎�
		0~1: t��0~1�̊Ԃɂ��鎞
===============================================================================*/
float CHermitian::GetChangeValuePercent(const D3DXVECTOR2& StartDir, const D3DXVECTOR2& EndDir,float t)
{
	if (t <= 0) 
	{
		return 0;
	}
	if (t >= 1) 
	{
		return 1;
	}

	//�p�����[�^������
	float h00 = (2 * t*t*t) - (3 * t*t) + 1;
	float h01 = (-2 * t*t*t) + (3 * t*t);
	float h10 = (t*t*t) - (2 * t*t) + t;
	float h11 = (t*t*t) - (t*t);

	//�Ȑ���Y�l�����߂�
	float CurveYPosition = h00 * START_POINT_POSY + h01 * END_POINT_POSY + h10 * StartDir.y * DIR_VECTOR_RATE + h11 * -EndDir.y * DIR_VECTOR_RATE;
	float Percent = CurveYPosition / (END_POINT_POSY - START_POINT_POSY);		//�ω��ʂ̃p�[�Z���e�[�W�v�Z

	return Percent;
}

void CHermitian::DrawCoordinate(void)
{
	LPD3DXLINE pLine;
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXCreateLine(pDevice, &pLine);
	D3DCOLOR col = D3DCOLOR_RGBA(200, 200, 200, 200);

	//�`��
	pDevice->SetFVF(FVF_VERTEX2D);
	pDevice->SetTexture(0, nullptr);
	pLine->SetAntialias(TRUE);
	pLine->SetWidth(1.0f);
	for (int i = 0; i < 3; i++) {
		D3DXVECTOR2 VertexesHori[] = {
			D3DXVECTOR2(m_StartPos.x,m_StartPos.y - GRID_INTERVAL*(i + 1)),
			D3DXVECTOR2(m_EndPos.x,m_StartPos.y - GRID_INTERVAL*(i + 1))
		};
		pLine->Begin();
		pLine->Draw(VertexesHori, 2, col);
		pLine->End();

		D3DXVECTOR2 VertexesVerti[] = {
			D3DXVECTOR2(m_StartPos.x + GRID_INTERVAL*(i + 1),m_StartPos.y),
			D3DXVECTOR2(m_StartPos.x + GRID_INTERVAL*(i + 1),m_EndPos.y)
		};
		pLine->Begin();
		pLine->Draw(VertexesVerti, 2, col);
		pLine->End();
	}

	//���
	pLine->Release();
}


/*==============================================================================
	�J�n�|�C���g�̕����x�N�g���Z�o
===============================================================================*/
void CHermitian::SetStartDir(const D3DXVECTOR2& StartDir)
{
	m_StartDir = StartDir;
	m_StartPos = m_DrawPos + D3DXVECTOR2(START_POINT_POSX, START_POINT_POSY);
	
	//�X�^�[�g�|�C���g�ʒu�ݒ�
	if (m_StartDirPoint != nullptr) 
	{
		D3DXVECTOR2 ResultPos = m_StartDir + m_StartPos;
		m_StartDirPoint->SetPosition(D3DXVECTOR3(ResultPos.x, ResultPos.y,0.0f));
	}
}

/*==============================================================================
	�I�_�|�C���g�̕����x�N�g���Z�o
===============================================================================*/
void CHermitian::SetEndDir(const D3DXVECTOR2& EndDir)
{
	m_EndDir = EndDir;
	m_EndPos = m_DrawPos + D3DXVECTOR2(END_POINT_POSX, END_POINT_POSY);

	//�X�^�[�g�|�C���g�ʒu�ݒ�
	if (m_EndDirPoint != nullptr)
	{
		D3DXVECTOR2 ResultPos = m_EndDir + m_EndPos;
		m_EndDirPoint->SetPosition(D3DXVECTOR3(ResultPos.x, ResultPos.y, 0.0f));
	}
}

void CHermitian::DrawHermitianCurve(void)
{
	//�f�[�^�擾
	LPD3DXLINE pLine;
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXCreateLine(pDevice, &pLine);
	D3DXVECTOR2 *pVertex = new D3DXVECTOR2[m_CurvePointList.size()];
	for (unsigned i = 0; i < m_CurvePointList.size(); i++) {
		pVertex[i] = m_CurvePointList[i];
	}
	D3DCOLOR col = D3DCOLOR_RGBA(0,0,0,255);

	//�`��
	pDevice->SetFVF(FVF_VERTEX2D);
	pDevice->SetTexture(0, nullptr);
	pLine->SetAntialias(TRUE);
	pLine->SetWidth(1.0f);
	pLine->Begin();
	pLine->Draw(pVertex, CURVE_POINT_NUM+2, col);
	pLine->End();

	//�`��I������
	pLine->Release();
	delete[] pVertex;
}

void CHermitian::DrawVectorPoint(void)
{
	if (nullptr == m_StartDirPoint) return;
	if (nullptr == m_EndDirPoint) return;

	LPD3DXLINE pStartLine, pEndLine;
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXCreateLine(pDevice, &pStartLine);
	D3DXCreateLine(pDevice, &pEndLine);
	
	D3DXVECTOR2 StartVertex[] = {
		m_DrawPos + D3DXVECTOR2(START_POINT_POSX, START_POINT_POSY),
		m_StartDirPoint->GetPosition()
	};
	D3DXVECTOR2 EndVertex[] = {
		m_DrawPos + D3DXVECTOR2(END_POINT_POSX, END_POINT_POSY),
		m_EndDirPoint->GetPosition()
	};
	D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 255, 255);

	//�`��
	pDevice->SetFVF(FVF_VERTEX2D);
	pDevice->SetTexture(0, nullptr);

	pStartLine->SetAntialias(TRUE);
	pStartLine->SetWidth(1.0f);
	pStartLine->Begin();
	pStartLine->Draw(StartVertex, 2, col);
	pStartLine->End();

	pEndLine->SetAntialias(TRUE);
	pEndLine->SetWidth(1.0f);
	pEndLine->Begin();
	pEndLine->Draw(EndVertex, 2, col);
	pEndLine->End();

	m_StartDirPoint->Draw();
	m_EndDirPoint->Draw();

	//���
	pStartLine->Release();
	pEndLine->Release();
}

void CHermitian::MouseControl(void)
{
	D3DXVECTOR3 MousePos = CMousePosition::GetMousePos();	//�}�E�X���W�擾
	MouseHover(MousePos, &m_MouseHoverFlags);				//�}�E�X���I�u�W�F�Əd�Ȃ��Ă��邩

	//�O��̃t���O��ۑ�
	m_MousePressFlagsOld = m_MousePressFlags;

	if (m_MouseHoverFlags == LOCK_NONE && m_MousePressFlags == LOCK_NONE) {
		m_MouseUsing = false;
		return;
	}

	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT)) {
		if (LOCK_NONE == m_MousePressFlags) {
			m_MousePressFlags = m_MouseHoverFlags;
			m_MousePressPos = D3DXVECTOR2(MousePos.x, MousePos.y);
		}
		else {
			MousePress(MousePos, m_MousePressFlags);
			m_MousePressPos = D3DXVECTOR2(MousePos.x, MousePos.y);
		}
	}
	else {
		m_MousePressFlags = LOCK_NONE;
	}

	m_MouseUsing = true;
}

void CHermitian::LimitedPosition(void)
{
	if (nullptr == m_StartDirPoint) return;
	D3DXVECTOR3 pos = m_StartDirPoint->GetPosition();
	if (pos.x < m_StartPos.x) m_StartDirPoint->SetPositionX(m_StartPos.x);
	if (pos.x > m_EndPos.x) m_StartDirPoint->SetPositionX(m_EndPos.x);
	if (pos.y < m_EndPos.y) m_StartDirPoint->SetPositionY(m_EndPos.y);
	if (pos.y > m_StartPos.y) m_StartDirPoint->SetPositionY(m_StartPos.y);

	if (nullptr == m_EndDirPoint) return;
	pos = m_EndDirPoint->GetPosition();
	if (pos.x < m_StartPos.x) m_EndDirPoint->SetPositionX(m_StartPos.x);
	if (pos.x > m_EndPos.x) m_EndDirPoint->SetPositionX(m_EndPos.x);
	if (pos.y < m_EndPos.y) m_EndDirPoint->SetPositionY(m_EndPos.y);
	if (pos.y > m_StartPos.y) m_EndDirPoint->SetPositionY(m_StartPos.y);
}

void CHermitian::MouseHover(const D3DXVECTOR3& MousePos, MOUSE_IN_OBJ *pOut)
{
	bool bHover = false;

	//�}�E�X���I�u�W�F�N�g���Ȃ��Ă��邩
	if (m_Background != nullptr) {
		D3DXVECTOR2 Pos = D3DXVECTOR2(m_Background->GetPosition().x, m_Background->GetPosition().y);
		D3DXVECTOR2 Size = D3DXVECTOR2(m_Background->GetSize().x, m_Background->GetSize().y);
		if (CCollisionMath::PointInRect2DCenter(D3DXVECTOR2(MousePos.x, MousePos.y), Pos, Size)) {
			*pOut = LOCK_WINDOW;
			bHover = true;
		}
	}

	if (m_StartDirPoint != nullptr) {
		D3DXVECTOR2 Pos = D3DXVECTOR2(m_StartDirPoint->GetPosition().x, m_StartDirPoint->GetPosition().y);
		D3DXVECTOR2 Size = D3DXVECTOR2(m_StartDirPoint->GetSize().x + 30.0f, m_StartDirPoint->GetSize().y + 30.0f);
		if (CCollisionMath::PointInRect2DCenter(D3DXVECTOR2(MousePos.x, MousePos.y), Pos, Size)) {
			*pOut = LOCK_START;
			bHover = true;
		}
	}

	if (m_EndDirPoint != nullptr) {
		D3DXVECTOR2 Pos = D3DXVECTOR2(m_EndDirPoint->GetPosition().x, m_EndDirPoint->GetPosition().y);
		D3DXVECTOR2 Size = D3DXVECTOR2(m_EndDirPoint->GetSize().x + 30.0f, m_EndDirPoint->GetSize().y + 30.0f);
		if (CCollisionMath::PointInRect2DCenter(D3DXVECTOR2(MousePos.x, MousePos.y), Pos, Size)) {
			*pOut = LOCK_END;
			bHover = true;
		}
	}

	if (!bHover) *pOut = LOCK_NONE;
}

void CHermitian::MousePress(const D3DXVECTOR3& MousePos, MOUSE_IN_OBJ PressFlags)
{
	if (m_bPreView) return;
	D3DXVECTOR2 MoveVec = D3DXVECTOR2(MousePos.x, MousePos.y) - m_MousePressPos;
	switch (PressFlags) {
	case LOCK_WINDOW:
		m_Background->AddPosition(D3DXVECTOR3(MoveVec.x, MoveVec.y,0.0f));
		m_StartDirPoint->AddPosition(D3DXVECTOR3(MoveVec.x, MoveVec.y, 0.0f));
		m_EndDirPoint->AddPosition(D3DXVECTOR3(MoveVec.x, MoveVec.y, 0.0f));
		break;
	case LOCK_START:
		m_StartDirPoint->AddPosition(D3DXVECTOR3(MoveVec.x, MoveVec.y, 0.0f));
		break;
	case LOCK_END:
		m_EndDirPoint->AddPosition(D3DXVECTOR3(MoveVec.x, MoveVec.y, 0.0f));
		break;

	}
}