//�}�b�v�G�f�B�^�[�̕����I�u�W�F�N�g���ꊇ�I������@�\
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
	void SelectObjs(void);		//�����I�u�W�F�N�g��I�����郂�[�h
	void Draw(void);
	void Uninit(void);

	//�Q�b�^�[
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