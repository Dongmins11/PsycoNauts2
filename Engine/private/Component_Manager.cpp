#include "..\public\Component_Manager.h"
#include "Component.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iNumScenes)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumScenes = iNumScenes;

	m_pPrototypes = new PROTOTYPES[iNumScenes];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pPrototypes ||
		iSceneIndex >= m_iNumScenes ||
		nullptr == pPrototype || 
		nullptr != Find_Prototype(iSceneIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iSceneIndex].insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iSceneIndex, const _tchar * pPrototypeTag, void* pArg)
{
	if (nullptr == m_pPrototypes ||
		iSceneIndex >= m_iNumScenes)
		return nullptr;

	CComponent*	pPrototype = Find_Prototype(iSceneIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent*	pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CComponent_Manager::Clear_Component(_uint iSceneIndex)
{
	if (iSceneIndex > m_iNumScenes)
		return E_FAIL;

	for (auto& Pair : m_pPrototypes[iSceneIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iSceneIndex].clear();

	return S_OK;
}

CComponent * CComponent_Manager::Find_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iSceneIndex].begin(), m_pPrototypes[iSceneIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

			m_pPrototypes[i].clear();
	}
	Safe_Delete_Array(m_pPrototypes);
}
