#pragma once
#ifndef _NAME_SPACE_H_
#define _NAME_SPACE_H_

namespace MyNameSpace
{
	template <class pScene> void DeleteInitialize(pScene Initialize) {
		if (nullptr != Initialize) {
			Initialize->Uninit();
			delete Initialize;
			Initialize = nullptr;
		}
	}

	template <class pScene> void For_DeleteInitialize(int nCntMAX, pScene Header) {
		for (int i = 0; i < nCntMAX; i++) {
			if (nullptr != Header[i]) {
				Header[i]->Uninit();
				delete Header[i];
				Header[i] = nullptr;
			}
		}
	}

	template <class pScene>void For_Null(int nCntMAX, pScene Header) {
		for (int i = 0; i < nCntMAX; i++) {
			Header[i] = nullptr;
		}
	}
}

#endif