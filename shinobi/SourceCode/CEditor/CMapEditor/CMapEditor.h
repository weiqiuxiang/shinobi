#pragma once
#ifndef _CMAP_EDITOR_H_
#define _CMAP_EDITOR_H_

#include "SceneManager.h"
#include "CObjectAndMeshEditor.h"
#include "CLevelDesignEditor.h"

class CMapEditor : public CSceneManager
{
public:
	CMapEditor();
	~CMapEditor();

	typedef enum 
	{
		OBJECT_AND_MESH_EDITOR = 0,
		LEVEL_EDITOR
	}MODE;

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	void EditCameraControl(void);					//エディットモードのカメラコントロール

	//UI部分
	bool ExitButtonUI(void);
	void ModeSelectUI(void);
	void ShowAABBUI(void);
	
private:
	MODE		m_Mode;
	bool		m_IsExit;
	bool		m_IsShowAABB;

	CSkyDome		m_SkyDoom;
	CSkyDome		m_SkyDoom2;

	//エディター変数
	CObjectAndMeshEditor	m_ObjAndMeshEditor;
	CLevelDesignEditor		m_LevelDesignEditor;
};


#endif