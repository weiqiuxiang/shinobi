#ifndef CNAV_MESH
#define CNAV_MESH

#include "main.h"
#include "CScene3D.h"
#include <list>

typedef enum 
{
	NAVMESH_NO_CHECK= 0,
	NAVMESH_OPEN,
	NAVMESH_CLOSE
}NAVMESH_STATE_ENUM;

class CNavMesh 
{
public:
	CNavMesh();
	~CNavMesh();
	
	bool Init(const D3DXVECTOR3& pos,const D3DXVECTOR3& size,bool CanWalkFlag,bool IsUse);
	void Update(void);
	void DrawInEditor(void);						//�G�f�B�^�[���̕`��
	void Uninit(void);

	//�Q�b�^�[
	D3DXVECTOR3 GetCenterPosition(void) { return m_CenterPos; }							//���b�V���̒��S���W�擾
	D3DXVECTOR3 GetLeftTopVertexPosition(void) { return m_VertexPosition[0]; }			//����̒��_���W�擾
	D3DXVECTOR3 GetRightBottomVertexPosition(void) { return m_VertexPosition[3]; }		//�E���̒��_���W�擾
	NAVMESH_STATE_ENUM GetState(void) { return m_State; }
	bool GetIsUse(void) { return m_IsUse; }

	//�Z�b�^�[
	void SetCommingMesh(CNavMesh* pNavMesh) { m_CommingMesh = pNavMesh; }
	void SetState(NAVMESH_STATE_ENUM state) { m_State = state; }
	void SetIsCanWalk(bool flag) { m_IsCanWalk = flag; }
	void SetIsUse(bool flag) { m_IsUse = flag; }

	//�|�C���^���Z�b�g
	void ResetCommingMesh(void) { m_CommingMesh = nullptr; }
	void ResetConnectMesh(void) { m_NavMeshPointerList.clear(); }

	//���̃��b�V���ƌq���鎞�g�p
	void AddConnectDoubleDir(CNavMesh*);			//�o�����q����
	void AddConnectSingleDir(CNavMesh*);			//�P�����q����

private:
	CScene3D				m_Polygon3D;			//�`�惆�j�b�g
	D3DXVECTOR3				m_CenterPos;			//���b�V�����S���W
	D3DXVECTOR3				m_VertexPosition[4];	//���_���W
	CNavMesh*				m_CommingMesh;			//�ǂ����痈�����L�^���邽�߂̃|�C���^(�o�H�T����,�g�p)
	std::list<CNavMesh*>	m_NavMeshPointerList;	//�q����p�̃|�C���^���X�g
	NAVMESH_STATE_ENUM		m_State;				//���b�V���̏��(���`�F�b�N�Aopen�Aclose��3�̏��)
	bool					m_IsCanWalk;			//�����郁�b�V���t���O
	bool					m_IsUse;				//�g�p�t���O
	RGBA					m_RGBA;					//�F
};

#endif