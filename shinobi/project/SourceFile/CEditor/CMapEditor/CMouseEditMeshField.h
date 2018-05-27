//�t�@�C���� : CMouseInMeshField
//���� : ��ʏ�̃}�E�X���W��3D���b�V����̃}�E�X���W�ɕϊ�
#ifndef CMOUSE_IN_MESHFIELD_H_
#define CMOUSE_IN_MESHFIELD_H_

#include "main.h"
#include "CScene3D.h"
#include "CMeshField.h"

class CMouseEditMeshField
{
public:
	CMouseEditMeshField();
	~CMouseEditMeshField() {}

	HRESULT Init(float fRadius,RGBA rgba);
	void Uninit(void);
	void Draw(void);

	//�Q�b�^�[
	D3DXVECTOR3 GetMousePosInMesh(void) { return m_Cscene3D.GetPosition(); }           //�}�E�X�����b�V���ł̍��W��Ԃ�
	D3DXVECTOR3 GetMouseRayStartPoint(void) { return m_StartPoint; }
	D3DXVECTOR3 GetMouseRayEndPoint(void) { return m_EndPoint; }

	//���̊֐�
	void CalcMousePositionInMesh(void);
	void EditHitFloor(void);

private:
	//�����o�֐�
	void CalcLine(D3DXVECTOR3* pStartPoint, D3DXVECTOR3* pEndPoint);						//���C���v�Z
	D3DXVECTOR3 HitAllMesh(const D3DXVECTOR3& StartPoint,const D3DXVECTOR3& pEndPoint);		//�}�E�X�ƃ��b�V���̂��ׂĂ̓��蔻��
	D3DXVECTOR3 HitOneMesh(CMeshField *pMesh,const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint);
	void ChangeRange(void);               //���b�V���ɑ΂���e�����a�̕ύX

	//�����o�ϐ�
	CScene3D		m_Cscene3D;
	float			m_fRadius;
	int				m_nHitMeshID;
	D3DXVECTOR3		m_StartPoint;
	D3DXVECTOR3		m_EndPoint;
};

#endif