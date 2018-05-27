#ifndef CBLOOD_STORAGE_H_
#define CBLOOD_STORAGE_H_

#include "main.h"
#include "CBlood.h"

//É}ÉOÉç
#define BLOOD_AMOUNT_MAX (1024)

class CBloodStorage
{
public:
	static void InitAll(void);
	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);

	static CBlood* CreateObject(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &pSize, const D3DXVECTOR3& power , int Time,float fCoffi, const char* Texfilepass);

private:
	static CBlood m_Obj[BLOOD_AMOUNT_MAX];
};

#endif