#pragma once
//説明 : モーションエディター
#ifndef _CMOTION_EDITOR_H_
#define _CMOTION_EDITOR_H_

#include "SceneManager.h"
#include "CModel.h"
#include "CScene3D.h"
#include "CFontUIScale.h"
#include "CCoordinate.h"
#include "CPart.h"
#include "CEditorMotion.h"
#include "CMotion.h"
#include "CHermitian.h"
#include <list>
#include "CSkyDome.h"
#include "CMouseEditMeshField.h"
#include "CMeshDataIO.h"

class CMotionEditor : public CSceneManager
{
public:
	typedef enum {
		EDIT_OFFSET = 0,			//オフセット設定
		EDIT_LINK,					//他のパーツとのリング
		EDIT_MOTION					//モーション編集
	}EDIT_MODE;			//編集モード

	typedef enum {
		MOTION_EDIT_MODE_EDIT_KEYFRAME = 0,		//キーフレームの追加削除、変更,キーフレームの継続フレーム数、ループフラグ設定
		MOTION_EDIT_MODE_MOTION_PREVIEW,		//モーションのプレビュー
		MOTION_EDIT_MODE_KEYFRAME_PREVIEW,		//指定キーフレームから次のキーフレームまでのプレビュー
	}MOTION_EDIT_MODE;				//EDIT_MOTIONの子階層

	typedef struct {
		CEditorMotion		ModelMotion;			//モデルモーション
		EDIT_MODE			EditMode;				//エディットモード
		MOTION_EDIT_MODE	MotionEdit;				//モーションエディットモード
	}UNDO_REDO_STUCK;

	CMotionEditor();
	~CMotionEditor();

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	//初期化関数
	void InitAndCreateLight(void);

	//更新関数
	void UpdateEdit(void);

	//他の関数
	void HermitianEdit(void);						//曲線編集
	void ChangeEditMode(void);						//編集モード変更
	void ChangeEditModeUI(void);					//編集モード変更対応UI
	void ChangeEditModeMotion(void);				//モーション編集モード変更
	void ChangeEditModeMotionUI(void);				//モーション編集モード変更UI
	void EditChangeMotionUI(void);					//モーション変更
	void MotionListUI(void);						//モーションのリストUI
	void PartListUI(void);							//パーツのリストUI
	void PartListUIInEditKeyFrame(void);			//パーツのリストUI
	void SelectParentListUI(void);					//パーツの親選択時のパーツリスト
	void EditChangeKeyFrameUI(void);				//キーフレーム変更
	void EditLoopFlagUI(void);						//ループフラグの設定
	void EditChangeFrameUI(void);					//キーフレームのフレーム数の変更
	void CopyPastKeyFrameUI(void);					//キーフレームの複製と貼り付け
	void EditKeyFrameUI(void);						//キーフレームの編集
	void ChangeOffset(void);						//オフセット変更
	void ShowPartDetailUI(void);					//パーツの詳細表示(SRP、NAME、INDEX、PARENT)
	void EditCameraControl(void);					//エディットモードのカメラコントロール
	void ImguiForEditor(void);						//メッセージ表示
	void SelectPartObj(void);						//パーツの選択
	void OtherOptionButton(void);					//他のボタン

	//Undo、Redoに関する関数
	void DetectMouseClickInWindow(void);
	void DetectMouseRelease(void);
	void DetectDataChangeAndPushData(void);			//データの変化を検知し、データが変化がある場合データをスタックにプッシュ
	void PushData(void);							//データをスタックにプッシュ
	void UndoRedoUI(void);							//
	void Undo(void);								//元に戻す
	void Redo(void);								//やり直す
	void ShowUndoRedoStuckSize(void);				//デバックメッセージ

	//Save、Loadに関する関数
	void SaveData(void);							//データの保存
	void GetTimeNow(char*);							//文字配列に現在の時刻を詰め込む

	void ActionInEditMode(void);					//各編集モードの具体動作
	void EditLinkParent(void);						//親設定

	//他の関数
	bool ExitButtonUI(void);						//モード終了ボタンが押されたかを判定

private:
	//メンバ変数
	EDIT_MODE			m_EditMode;					//エディットモード
	MOTION_EDIT_MODE	m_MotionEdit;				//モーションエディットモード
	bool				m_bEditModeChange;			//エディットモードチェンジのフラグ
	bool				m_bMotionEditChange;		//モーション編集モードチェンジのフラグ
	bool				m_bMotionPlay;				//モーションを再生するフラグ
	bool				m_bKeyFramePreviewPlay;		//キーフレーム再生
	CPart*				m_pSelectedPart;			//選択されたパーツ
	CPart*				m_pShowParent;				//マウスを翳しているパーツ
	D3DMATERIAL9		m_SelectPartMtrl;			//選択しているパーツのマテリアル
	D3DMATERIAL9		m_ShowParentMtrl;			//選択しているパーツのマテリアル
	bool				m_bClickFlag;				//クリックしたフラグ(別のものを選択した時衝突しないように、例えばUIをクリックしたら、3Dオブジェを選択させないように)
	float				m_KeyFramePercent;			//キーフレームプレビュー時、今のキーフレームから次のキーフレームに補間するパーセンテージ
	CKeyFrame			m_CopyKeyFrame;				//コピーされたキーフレーム
	CEditorMotion		m_ModelMotion;				//モーションユニット
	CHermitian			*m_pHermitian;				//エルミート曲線
	bool				m_IsShowMotionList;			//モーションリスト表示フラグ
	bool				m_IsShowPartList;			//パーツリストの表示

	//Undo、Redo変数
	UNDO_REDO_STUCK					m_PushReadyData;	//プッシュ予備データ
	std::list<UNDO_REDO_STUCK*>		m_UndoRedoStuck;	//UndoとRedoのスタック
	UNDO_REDO_STUCK*				m_pPushPosition;	//今編集しているリストのイテレーター位置
	UNDO_REDO_STUCK					m_DefaultData;		//最初のデータ
	bool m_bMouseHoverAndClickInWindow;					//マウスがウィンドウの所にクリックしたのか

	CMouseEditMeshField m_MouseEditMesh;				//計算時マウスのレイを取得用
	CMeshDataIO			m_MeshLoader;					//メッシュデータ読み込み用
	CSkyDome		m_SkyDoom;
	CSkyDome		m_SkyDoom2;
};

#endif