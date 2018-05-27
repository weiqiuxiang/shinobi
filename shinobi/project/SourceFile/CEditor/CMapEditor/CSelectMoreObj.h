//マップエディターの複数オブジェクトを一括選択する機能
#ifndef CSELECT_MORE_OBJ_H_
#define CSELECT_MORE_OBJ_H_

#include "CMouseWindow.h"
#include "CModelStorage.h"

class CSelectMoreObj
{
public:
	CSelectMoreObj();
	~CSelectMoreObj();

	bool Init(void);
	void SelectObjs(void);		//複数オブジェクトを選択するモード
	void Draw(void);
	void Uninit(void);

	//ゲッター
	int GetSelectObjAmount(void) { return m_SelectObjAmount; }
	int GetSelectObjNum(int ID);
	bool GetUsingFlag(void) { return m_bMouseWindow.GetUsingFlag(); }

	void ResetSelectObjAmount(void) { m_SelectObjAmount = 0; }

private:
	CMouseWindow		m_bMouseWindow;
	int					m_SelectObjAmount;
	int					m_SelectObjNumber[MODEL_OBJ_MAX];
};

#endif