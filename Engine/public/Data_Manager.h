#pragma once

#include "Base.h"

BEGIN(Engine)

class CData_Manager final : public CBase
{
	DECLARE_SINGLETON(CData_Manager)
private:
	CData_Manager();
	virtual ~CData_Manager() = default;

public:
	const _tchar* Add_PrototypeData(const TCHAR* pPrototypeTag);
	const _tchar* Add_LayerTagData(const TCHAR* pLayerTag);
	HRESULT Add_PathData(const char* pPath);
	//HRESULT Draw_Font(const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor);
	
private:
	list<const TCHAR*>		 m_MeshFullName;
	list<const TCHAR*>		 m_LayerTagList;

	list<const char*>		m_MeshFullNamePath;
	
public:
	virtual void Free() override;
};

END