#pragma once

//�t�@�C���� : CMeshEditor.h
//���� : ���b�V���f�[�^�������o���Ɠǂݍ���

#ifndef CMESH_DATA_IO_H_
#define CMESH_DATA_IO_H_

#include "main.h"
#include "CMeshField.h"

class CMeshDataIO
{
public:
	CMeshDataIO() : m_pMesh(nullptr){}
	~CMeshDataIO() {}

	void ExportMeshData(void);									//���b�V���f�[�^�̏����o��
	CMeshField* ImportMeshData(const char* pMeshFilePass);		//���b�V���f�[�^�̓ǂݍ���
	void SetMesh(CMeshField* pMesh) { m_pMesh = pMesh; }
private:
	CMeshField* m_pMesh;
};

#endif