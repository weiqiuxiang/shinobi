/*==============================================================================
	ファイル説明:
		マップエディターのUI
===============================================================================*/
#ifndef OBJECT_AND_MESH_EDITOR_UI_H_
#define OBJECT_AND_MESH_EDITOR_UI_H_

/*==============================================================================
	インクルードヘッダ
===============================================================================*/
#include "main.h"
#include "CModel.h"
#include "CRenderTarget2D.h"

/*==============================================================================
	マクロ
===============================================================================*/
#define NO_PUSH_ANY_IMAGE_BUTTON (-1)
#define MODEL_MENU_OBJ_MAX (100)

/*==============================================================================
	クラス定義
===============================================================================*/
class CObjectAndMeshEditorUI
{
public:
	CObjectAndMeshEditorUI();
	~CObjectAndMeshEditorUI() {}
	typedef enum
	{
		EDIT_MESH = 0,		//メッシュの起伏を編集するモード
		SET_OBJ,			//オブジェクトをセット
		MOVE_OBJ			//モードのテスト
		//TODO::敵の配置と敵の巡回ポイントの配置
		//TODO::当り判定の配置
	}MODE;

	bool Init(void);		//初期化処理
	void Update(void);		//更新処理
	void Uninit(void);		//終了処理

	//ゲッター
	int GetPushButtonNumber(void);				//押したボタン番号を取得し、番号をリセット
	MODE GetMode(void) { return m_Mode; }
	bool GetMouseUsing(void);
	bool GetSaveMeshFlag(void) { return m_IsSaveMeshData; }
	bool GetSaveModelFlag(void) { return m_IsSaveModelData; }
	bool GetCreateMeshData(D3DXVECTOR2*, D3DXVECTOR2*);
	int GetLockDistanceX(void) { return m_LockDistanceX; }
	int GetLockDistanceY(void) { return m_LockDistanceX; }
	bool GetLockFlagX(void) { return m_LockFlagXAxis; }
	bool GetLockFlagY(void) { return m_LockFlagYAxis; }
	bool GetResetAllModel(void);
	bool GetMoveSelectObjsFlag(void) { return m_IsMoveSelectObjs; }

	//他の関数
	void DrawOnTexture(CModel* pModel);			//テクスチャに描画する

private:
	void ChangeModeUI(void);					//モードの変更UI
	void ModelImageButtonUI(void);				//モデルリストのイメージUI
	bool ExitButtonUI(void);
	bool SaveMeshUI(void);
	bool SaveModelUI(void);
	void CreateMeshUI(void);
	void LockDistanceUI(void);
	void ResetAllObjUI(void);

	//MOVE_OBJ部分のUI
	void MoveObjsCheckBoxUI(void);
private:
	RENDER_TARGET_POINTER	m_RenderTargetPointerArray[MODEL_MENU_OBJ_MAX];
	int				m_PushButtonNumber;			//押されたボタンの番号
	MODE			m_Mode;						//実行中のモード
	bool			m_IsSaveMeshData;			//メッシュデータセーブフラグ
	bool			m_IsSaveModelData;			//モデルデータセーブフラグ
	bool			m_IsCreateMesh;				//メッシュデータ取得
	D3DXVECTOR2		m_MeshMasuSize;				//メッシュマスサイズ
	D3DXVECTOR2		m_MeshFieldSize;			//メッシュフィールドサイズ
	int				m_LockDistanceX;			//モデル配置時の整数値移動距離
	int				m_LockDistanceY;			//モデル配置時の整数値移動距離
	bool			m_LockFlagXAxis;			//
	bool			m_LockFlagYAxis;			//
	bool			m_ResetAllObj;				//全てのモデル削除

	//MOVE_OBJ部分の変数
	bool			m_IsMoveSelectObjs;			//オブジェクト移動フラグ
};

#endif