#include "..\Public\Data_Manager.h"

IMPLEMENT_SINGLETON(CData_Manager)

CData_Manager::CData_Manager()
{

}


const _tchar* CData_Manager::Add_PrototypeData(const TCHAR * pPrototypeTag)
{
	auto	iter = find_if(m_MeshFullName.begin(), m_MeshFullName.end(), CWCharTagListFinder(pPrototypeTag));

	if (iter == m_MeshFullName.end())
		m_MeshFullName.emplace_back(pPrototypeTag);
	else
		return *iter;

	return nullptr;
}

const _tchar* CData_Manager::Add_LayerTagData(const TCHAR * pLayerTag)
{
	auto	iter = find_if(m_LayerTagList.begin(), m_LayerTagList.end(), CWCharTagListFinder(pLayerTag));

	if (iter == m_LayerTagList.end())
		m_LayerTagList.emplace_back(pLayerTag);
	else
		return *iter;

	return nullptr;
}

HRESULT CData_Manager::Add_PathData(const char * pPath)
{
	auto	iter = find_if(m_MeshFullNamePath.begin(), m_MeshFullNamePath.end(), CCharTagListFinder(pPath));

	if (iter == m_MeshFullNamePath.end())
		m_MeshFullNamePath.emplace_back(pPath);
	else
		return E_FAIL;

	return S_OK;
}


void CData_Manager::Free()
{
	for (auto& list : m_MeshFullName)
		Safe_Delete(list);
	m_MeshFullName.clear();

	for (auto& listpath : m_MeshFullNamePath)
		Safe_Delete(listpath);
	m_MeshFullNamePath.clear();

	for (auto& list : m_LayerTagList)
		Safe_Delete(list);
	m_LayerTagList.clear();
}
