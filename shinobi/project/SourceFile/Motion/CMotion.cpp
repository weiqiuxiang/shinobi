#include "CMotion.h"
#include <string.h>

/**************************CKeyFrame���� START**************************/
CKeyFrame::CKeyFrame()
{
	m_HermitianStartDir = D3DXVECTOR2(50,-50);
	m_HermitianEndDir = D3DXVECTOR2(-50,50);
}

CKeyFrame::~CKeyFrame()
{
	
}
/**************************CKeyFrame���� END**************************/

/**************************CMotion���� START**************************/
CMotion::CMotion()
{
	strcpy_s(m_Name, MOTION_NAME_MAX, "NO_NAME");
}

CMotion::~CMotion()
{
	
}
/**************************CMotion���� START**************************/