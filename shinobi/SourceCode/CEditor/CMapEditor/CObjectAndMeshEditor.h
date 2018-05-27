#pragma once

#ifndef OBJECT_AND_MESH_EDITOR_H_
#define OBJECT_AND_MESH_EDITOR_H_

#include "SceneManager.h"
#include "CModel.h"
#include "CScene3D.h"
#include "CFontUIScale.h"
#include "CCoordinate.h"
#include "CSkyDome.h"
#include "CMeshDataIO.h"
#include "CMouseEditMeshField.h"
#include "CObjectAndMeshEditorUI.h"
#include "C_Obj3D_DataIO.h"
#include "CModelStorage.h"
#include "CSelectMoreObj.h"
#include <list>

class CObjectAndMeshEditor
{
public:
	CObjectAndMeshEditor();
	~CObjectAndMeshEditor();

	typedef enum 
	{
		SET_OBJ_MODE_NONE = 0,
		SET_OBJECT,
		SELECT_OBJECT
	}SET_OBJ_MODE;

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	//初期化関数
	void InitAndCreateLight(void);
	bool LoadModels(const char* ModelTextPass);			//モデルのテキストからモデルを読み込んで初期化する

	//更新関数
	void UpdateEdit(void);
	void UpdateRightButtonClickTimer(void);			//右クリックのタイマー関連

	//他の関数
	void DetectModeChange(void);					//編集モード変更検出
	void ModelRotate(void);							//配置時モデルを回転できる
	void ShowCoordinateUpdate(void);				//座標軸表示を更新する
	void SaveOrLoad(void);							//セーブとロード
	void MeshCreate(void);							//新しいメッシュを作る

	//各編集モードの具体動作
	void ActionInEditMode(void);					//各編集モードの具体動作をまとめる関数
	bool SelectObject(void);						//オブジェクトを選択
	void SetObject(void);							//オブジェクトを設置
	bool SelectModelObjOnFiled(void);				//設置されたオブジェクトを選択する
	
	//座標移動カーソルの関数
	void CoordinateAxisSelect(void);				//座標移動カーソルの軸選択
	void SelectedAxisAction(void);					//選択された軸のアクション

	//MOVE_OBJモードの関数
	void DeleteObjs(void);							//複数オブジェクトの削除
	void MoveObjs(void);							//複数オブジェクトの移動

private:
	//メンバ変数
	CObjectAndMeshEditorUI::MODE	m_EditMode;		//前一個モード
	CScene3D*			m_pCScene3D = nullptr;		//エディットオブジェクト
	CModel*				m_pModel = nullptr;			//エディットオブジェクト
	D3DMATERIAL9		m_SelectObjMtrl;			//選択しているオブジェクトのマテリアル
	D3DMATERIAL9		m_MouseInAxisMtrl;			//マウスを翳している軸のマテリアル
	D3DMATERIAL9		m_SelectAxisMtrl;			//マウスでドラックしている軸のマテリアル
	CCoordinate			m_Coordinate;				//座標移動カーソルの動作
	int					m_AxisSelected;				//選択された軸番号
	bool				m_bClickFlag;				//クリックしたフラグ(別のものを選択した時衝突しないように、例えばUIをクリックしたら、3Dオブジェを選択させないように)

	CMeshDataIO				m_MeshDataIO;				//メッシュデータの読み込みと書き出し役
	C_Obj3D_DataIO			m_ModelDataIO;				//モデルデータの読み込みと書き出し役
	CMouseEditMeshField		m_MouseEditMeshField;		//メッシュ上のマウス座標
	CObjectAndMeshEditorUI	m_UI;						//マップエディターUI
	std::list<CModel*>		m_ModelList;
	SET_OBJ_MODE			m_SetObjChildModeNum;			//SET_OBJモードの子モード番号
	int						m_MouseRightButtonFrameTime;	//右ボタンを押しっぱなしの時,時刻を保存用変数
	int						m_MouseRightButtonPushTime;		//右ボタンを押しっぱなしの累積時間

	//複数オブジェクト選択関連変数
	bool				m_MoveObjsFlag;
	CSelectMoreObj		m_SelectMoreObjMode;
};

#endif