#include "..\public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{

}

CComponent * CLayer::Get_Components(const _tchar * pComponentTag, _uint iIndex)
{
	auto		iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_Components(pComponentTag);	
}

CComponent * CLayer::Get_Components_Effect(const _tchar * pComponentTag, _uint iIndex)
{
	auto		iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	if (nullptr == (*iter))
		return nullptr;

	return (*iter)->Get_Components(pComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return S_OK;
}

_int CLayer::Tick(_double TimeDelta)
{
	_int	iProgressID = 0;

	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->Get_IsDestroy())
			{
				Safe_Release(pGameObject);

			}
			else
			{
				iProgressID = pGameObject->Tick(TimeDelta);

				if (0 > iProgressID)
					return iProgressID;
			}
		}
	}

	Delete_NullObjects();

	return iProgressID;
}

_int CLayer::LateTick(_double TimeDelta)
{
	_int	iProgressID = 0;

	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			if (pGameObject->Get_IsDestroy())
				Safe_Release(pGameObject);
			else
			{
				iProgressID = pGameObject->LateTick(TimeDelta);

				if (0 > iProgressID)
					return iProgressID;
			}
		}
	}
	return iProgressID;
}

_bool CLayer::RectPicking(_uint iIndex, HWND hWnd)
{
	if (iIndex > m_ObjectList.size())
		return false;

	auto iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->RectPicking(hWnd);
}

HRESULT CLayer::Delete_Object(_uint iIndex)
{
	auto        iter = m_ObjectList.begin();
	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_ObjectList.end())
		return E_FAIL;

	Safe_Release(*iter);
	m_ObjectList.erase(iter);

	return S_OK;
}

HRESULT CLayer::Delete_NullObjects()
{
	auto        iter = m_ObjectList.begin();

	for (; iter != m_ObjectList.end();)
	{
		if (*iter == nullptr)
		{
			iter = m_ObjectList.erase(iter);
		}
		else
			++iter;
	}

	return S_OK;
}


CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();
	
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
		Safe_Release(pGameObject);

	m_ObjectList.clear();

}
