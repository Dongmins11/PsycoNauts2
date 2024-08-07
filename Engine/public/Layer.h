#pragma once

#include "Base.h"

/* 개발자가 의도한 객체 분류의 그룹. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent*	Get_Components(const _tchar* pComponentTag, _uint iIndex);
	class CComponent*	Get_Components_Effect(const _tchar* pComponentTag, _uint iIndex);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	_int Tick(_double TimeDelta);
	_int LateTick(_double TimeDelta);

public:
	_uint	Get_LayerSize() const { return (_uint)m_ObjectList.size(); }
	list<class CGameObject*> Get_ObjectList() const { return m_ObjectList; };

public:
	_bool	RectPicking(_uint iIndex, HWND hWnd);
	HRESULT Delete_Object(_uint iIndex);

private:
	HRESULT Delete_NullObjects();

private:
	list<class CGameObject*>			m_ObjectList;
	typedef list<class CGameObject*>	OBJECTLIST;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END