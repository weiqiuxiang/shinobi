#pragma once

//ファイル名 : CMeshEditor.h
//説明 : メッシュデータを書き出しと読み込み

#ifndef CMESH_DATA_IO_H_
#define CMESH_DATA_IO_H_

#include "main.h"
#include "CMeshField.h"

class CMeshDataIO
{
public:
	CMeshDataIO() : m_pMesh(nullptr){}
	~CMeshDataIO() {}

	void ExportMeshData(void);									//メッシュデータの書き出し
	CMeshField* ImportMeshData(const char* pMeshFilePass);		//メッシュデータの読み込み
	void SetMesh(CMeshField* pMesh) { m_pMesh = pMesh; }
private:
	CMeshField* m_pMesh;
};

#endif