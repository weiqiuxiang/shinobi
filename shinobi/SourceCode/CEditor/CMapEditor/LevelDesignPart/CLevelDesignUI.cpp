#include "CLevelDesignUI.h"
#include <imgui.h>
#include <stdio.h>

CLevelDesignUI::CLevelDesignUI()
{
	m_Mode = SET_CREATURE;
	m_PushEnemyButtonNumber = -1;
}

CLevelDesignUI::~CLevelDesignUI() 
{
	
}

//初期化処理
bool CLevelDesignUI::Init(void)
{
	m_Mode = SET_CREATURE;
	m_PushEnemyButtonNumber = -1;

	return true;
}

//更新処理
void CLevelDesignUI::Update(void)
{
	ModeSelectUI();

	switch (m_Mode) {
	case SET_CREATURE:
		EnemyTypeUI();
		break;
	case SET_ENEMY_PATH:
		break;
	case SET_NAV_MESH:
		break;
	}
}

//終了処理
void CLevelDesignUI::Uninit(void) 
{
	
}

bool CLevelDesignUI::AddPathPointUI(void)
{
	bool IsClickButton = false;

	ImGui::Begin("MainMenu");

	if (ImGui::Button("AddPoint", ImVec2(100, 20))) {
		IsClickButton = true;
	}

	ImGui::End();

	return IsClickButton;
}

bool CLevelDesignUI::DeletePathPointUI(void)
{
	bool IsClickButton = false;

	ImGui::Begin("MainMenu");

	if (ImGui::Button("DeletePoint", ImVec2(100, 20))) {
		IsClickButton = true;
	}

	ImGui::End();

	return IsClickButton;
}

bool CLevelDesignUI::InsertPathPointUI(void)
{
	bool IsClickButton = false;

	ImGui::Begin("MainMenu");

	if (ImGui::Button("InsertPoint", ImVec2(100, 20))) {
		IsClickButton = true;
	}

	ImGui::End();

	return IsClickButton;
}

int CLevelDesignUI::GetSelectEnemyType(void)
{
	int Number = m_PushEnemyButtonNumber;
	m_PushEnemyButtonNumber = -1;
	return Number;
}

bool CLevelDesignUI::SetIntputStopFrame(int* pStopFrameNow)
{
	static int S_StopFrame = 0;

	char StopFrameChar[32] = { '\0' };
	sprintf_s(StopFrameChar, "%d", S_StopFrame);

	ImGui::Begin("MainMenu");

	//現在のstopframeを表示
	ImGui::Text("StopFrame : %d", *pStopFrameNow);

	ImGui::PushItemWidth(150);
	ImGui::InputText("StopFrame", StopFrameChar, sizeof(StopFrameChar));
	ImGui::PopItemWidth();

	ImGui::End();

	S_StopFrame = atoi(StopFrameChar);

	//入力確認
	bool IsInput = false;
	ImGui::Begin("MainMenu");

	if (ImGui::Button("SetStopFrame", ImVec2(100, 40))) {
		*pStopFrameNow = S_StopFrame;
		IsInput = true;
	}

	ImGui::End();

	return IsInput;
}

void CLevelDesignUI::EnemyTypeUI(void)
{
	ImGui::Begin("MainMenu");

	if (ImGui::Button("DOG", ImVec2(100, 20))) {
		m_PushEnemyButtonNumber = 0;
	}

	ImGui::End();
}

void CLevelDesignUI::ModeSelectUI(void)
{
	int EditModeNow = (int)m_Mode;

	ImGui::Begin("MainMenu");

	ImGui::RadioButton("SET_CREATURE", &EditModeNow, (int)SET_CREATURE); ImGui::SameLine();
	ImGui::RadioButton("SET_ENEMY_PATH", &EditModeNow, (int)SET_ENEMY_PATH); ImGui::SameLine();
	ImGui::RadioButton("SET_NAV_MESH", &EditModeNow, (int)SET_NAV_MESH);

	ImGui::End();

	//モード変化があったら
	if (EditModeNow != (int)m_Mode)
	{
		m_Mode = (MODE)EditModeNow;
		return;
	}
}

bool CLevelDesignUI::SaveEnemyUI(void)
{
	bool IsPushButton = false;

	ImGui::Begin("Save");

	if (ImGui::Button("SAVE ENEMY DATA", ImVec2(130, 50))) {
		IsPushButton = true;
	}

	ImGui::End();

	return IsPushButton;
}