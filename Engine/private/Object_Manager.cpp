#include "..\public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "EffectData.h"
#include "Effect_Manager.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Components(_uint iSceneIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	CLayer*		pLayer = Find_Layer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Components(pComponentTag, iIndex);	
}

CComponent * CObject_Manager::Get_Components_Effect(_uint iSceneIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	CLayer*		pLayer = Find_Layer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Components_Effect(pComponentTag, iIndex);
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumScenes)
{
	if (nullptr != m_pPrototypes ||
		nullptr != m_pLayers)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumScenes];
	m_pLayers = new LAYERS[iNumScenes];
	m_pEffectData = new EFFECTDATAS[iNumScenes];

	m_iNumScenes = iNumScenes;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (iSceneIndex >= m_iNumScenes ||
		nullptr == m_pPrototypes ||
		nullptr == pPrototype || 
		nullptr != Find_Prototype(iSceneIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iSceneIndex].insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

HRESULT CObject_Manager::Add_ObjectToLayer(_uint iPrototypeSceneIndex, const _tchar * pPrototypeTag, 
	_uint iSceneIndex, const _tchar * pLayerTag, void * pArg)
{
	if (iPrototypeSceneIndex >= m_iNumScenes ||
		iSceneIndex >= m_iNumScenes ||
		nullptr == m_pPrototypes ||
		nullptr == m_pLayers)
		return E_FAIL;

	CGameObject*	pPrototype = Find_Prototype(iPrototypeSceneIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone(pArg,iSceneIndex);
	
	pGameObject->Set_CurrentScene(iSceneIndex);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*		pLayer = Find_Layer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		
		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iSceneIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else
		pLayer->Add_GameObject(pGameObject);	
	
	return S_OK;
}

list<class CGameObject*> CObject_Manager::Get_ObjectLayer(_uint iSceneIndex, const _tchar * pLayerTag)
{
	list<CGameObject*> temp;
	if (iSceneIndex >= m_iNumScenes ||
		nullptr == m_pPrototypes ||
		nullptr == m_pLayers)
		return temp;

	CLayer*		pLayer = Find_Layer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
		return temp;

	return pLayer->Get_ObjectList();
}

_bool CObject_Manager::Is_Prototype_Existed(_uint iSceneIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == Find_Prototype(iSceneIndex, pPrototypeTag))
		return false;
	else
		return true;
}

HRESULT CObject_Manager::Add_ObjectToLayer_GetImmediately(_uint iPrototypeSceneIndex, const _tchar * pPrototypeTag, _uint iSceneIndex, const _tchar * pLayerTag, CGameObject ** ppOut, void * pArg)
{
	if (m_iNumScenes <= iPrototypeSceneIndex || m_iNumScenes <= iSceneIndex)
		return E_FAIL;

	CGameObject* pGameObject = Find_Prototype(iPrototypeSceneIndex, pPrototypeTag);
	if (nullptr == pGameObject)
		return E_FAIL;
	CGameObject* pGameObject_Clone = pGameObject->Clone(pArg, iSceneIndex);

	if (nullptr == pGameObject_Clone)
		return E_FAIL;
	pGameObject_Clone->Set_CurrentScene(iSceneIndex);

	CLayer* pLayer = Find_Layer(iSceneIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject_Clone);
		m_pLayers[iSceneIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else
	{
		pLayer->Add_GameObject(pGameObject_Clone);
	}
	*ppOut = pGameObject_Clone;

	return S_OK;
}

_int CObject_Manager::Tick(_double TimeDelta)
{
	_int		iProgressID = 0;

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				iProgressID = Pair.second->Tick(TimeDelta);
				if (0 > iProgressID)
					return iProgressID;
			}
		}		
	}

	return iProgressID;
}

_int CObject_Manager::LateTick(_double TimeDelta)
{
	_int		iProgressID = 0;

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				iProgressID = Pair.second->LateTick(TimeDelta);
				if (0 > iProgressID)
					return iProgressID;
			}
		}		
	}

	return iProgressID;
}

HRESULT CObject_Manager::Clear_Object(_uint iSceneIndex)
{
	if (iSceneIndex > m_iNumScenes)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iSceneIndex])
		Safe_Release(Pair.second);

	m_pLayers[iSceneIndex].clear();

	for (auto& Pair : m_pPrototypes[iSceneIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iSceneIndex].clear();

	for (auto& Pair : m_pEffectData[iSceneIndex])
	{
		Safe_Release(Pair.second);
	}

	m_pEffectData[iSceneIndex].clear();


	return S_OK;
}

_bool CObject_Manager::RectPicking(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex, HWND hWnd)
{
	CLayer* pLayer = Find_Layer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
		return false;

	return pLayer->RectPicking(iIndex,hWnd);
}

_uint CObject_Manager::Get_LayerSize(_uint iSceneIndex, const _tchar * pLayerTag)
{
	CLayer*		pLayer = Find_Layer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
		return 0;

	return pLayer->Get_LayerSize();
}

const _tchar* CObject_Manager::Get_LayerName(_uint iSceneIndex, _uint idx) 
{
	const _tchar*	pLayerName = Find_LayerName(iSceneIndex, idx);
	if (nullptr == pLayerName)
		return nullptr;
	
	return pLayerName;
}

HRESULT CObject_Manager::Delete_GameObject(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex)
{
	CLayer*		pLayer = Find_Layer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
		return E_FAIL;

	pLayer->Delete_Object(iIndex);
	return S_OK;
}

HRESULT CObject_Manager::Create_EffectRectData(_uint iPrototypeEffectSceneIndex, const _tchar * pEffectTag, EFFECTRECTINFO* pRectInfo)
{
	CEffectData*	pEffectData = Find_EffectData(iPrototypeEffectSceneIndex, pEffectTag);
	if (nullptr == pEffectData)
	{
		pEffectData = CEffectData::Create();

		pEffectData->Add_RectInfo(pRectInfo);

		m_pEffectData[iPrototypeEffectSceneIndex].insert(EFFECTDATAS::value_type(pEffectTag, pEffectData));
	}
	else
	{
		pEffectData->Add_RectInfo(pRectInfo);
	}

	return S_OK;
}

HRESULT CObject_Manager::Create_EffectMeshData(_uint iPrototypeEffectSceneIndex, const _tchar * pEffectTag, EFFECTMESHINFO * pMeshInfo)
{
	CEffectData*	pEffectData = Find_EffectData(iPrototypeEffectSceneIndex, pEffectTag);
	if (nullptr == pEffectData)
	{
		pEffectData = CEffectData::Create();

		pEffectData->Add_MeshInfo(pMeshInfo);

		m_pEffectData[iPrototypeEffectSceneIndex].insert(EFFECTDATAS::value_type(pEffectTag, pEffectData));
	}
	else
	{
		pEffectData->Add_MeshInfo(pMeshInfo);
	}

	return S_OK;
}

HRESULT CObject_Manager::Create_EffectParticleData(_uint iPrototypeEffectSceneIndex, const _tchar * pEffectTag, EFFECTPARTICLEINFO * pParticleInfo)
{
	CEffectData*	pEffectData = Find_EffectData(iPrototypeEffectSceneIndex, pEffectTag);
	if (nullptr == pEffectData)
	{
		pEffectData = CEffectData::Create();

		pEffectData->Add_ParticleInfo(pParticleInfo);

		m_pEffectData[iPrototypeEffectSceneIndex].insert(EFFECTDATAS::value_type(pEffectTag, pEffectData));
	}
	else
	{
		pEffectData->Add_ParticleInfo(pParticleInfo);
	}

	return S_OK;
}

list<EFFECTRECTINFO*> CObject_Manager::Get_EffectRectInfoList(_uint iSceneIndex, const _tchar * pEffectTag)
{
	list<EFFECTRECTINFO*> temp;
	if (iSceneIndex >= m_iNumScenes ||
		nullptr == m_pPrototypes ||
		nullptr == m_pLayers)
		return temp;

	CEffectData*	pEffectData = Find_EffectData(iSceneIndex, pEffectTag);
	if (nullptr == pEffectData)
		return temp;

	return pEffectData->Get_RectInfoList();
}

list<EFFECTMESHINFO*> CObject_Manager::Get_EffectMeshInfoList(_uint iSceneIndex, const _tchar * pEffectTag)
{
	list<EFFECTMESHINFO*> temp;
	if (iSceneIndex >= m_iNumScenes ||
		nullptr == m_pPrototypes ||
		nullptr == m_pLayers)
		return temp;

	CEffectData*	pEffectData = Find_EffectData(iSceneIndex, pEffectTag);
	if (nullptr == pEffectData)
		return temp;

	return pEffectData->Get_MeshInfoList();
}

list<EFFECTPARTICLEINFO*> CObject_Manager::Get_EffectParticleInfoList(_uint iSceneIndex, const _tchar * pEffectTag)
{
	list<EFFECTPARTICLEINFO*> temp;
	if (iSceneIndex >= m_iNumScenes ||
		nullptr == m_pPrototypes ||
		nullptr == m_pLayers)
		return temp;

	CEffectData*	pEffectData = Find_EffectData(iSceneIndex, pEffectTag);
	if (nullptr == pEffectData)
		return temp;

	return pEffectData->Get_ParticleInfoList();
}

HRESULT CObject_Manager::Add_ObjectToEffect(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iLayerSceneIndex, const _tchar* pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag)
{
	if (iPrototypeSceneIndex >= m_iNumScenes ||
		iLayerSceneIndex >= m_iNumScenes ||
		iEffectSceneIndex >= m_iNumScenes ||
		nullptr == m_pEffectData)
		return E_FAIL;
	
	for (auto& Rectiter : Get_EffectRectInfoList(iEffectSceneIndex, pEffectTag))
	{
		if (FAILED(Add_ObjectToLayer(iPrototypeSceneIndex, pPrototypeTag, iLayerSceneIndex, pLayerTag, Rectiter)))
			return E_FAIL;
	}

	for (auto& Meshiter : Get_EffectMeshInfoList(iEffectSceneIndex, pEffectTag))
	{
	}
	for (auto& Particleiter : Get_EffectParticleInfoList(iEffectSceneIndex, pEffectTag))
	{
		PARTICLECREATE tParticleCreate;
		tParticleCreate.wstLayerTag = pEffectTag;
		ZeroMemory(&tParticleCreate.tParticleInfo, sizeof(EFFECTPARTICLEINFO));
		memcpy(&tParticleCreate.tParticleInfo, Particleiter, sizeof(EFFECTPARTICLEINFO));
		if (FAILED(Add_ObjectToLayer(iPrototypeSceneIndex, pPrototypeTag, iLayerSceneIndex, pLayerTag, &tParticleCreate)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CObject_Manager::Add_ObjectToParticle_GetImmediately(_uint iPrototypeSceneIndex, const _tchar * pPrototypeTag, _uint iLayerSceneIndex, const _tchar * pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag, CGameObject ** ppOut)
{
	if (iPrototypeSceneIndex >= m_iNumScenes ||
		iLayerSceneIndex >= m_iNumScenes ||
		iEffectSceneIndex >= m_iNumScenes ||
		nullptr == m_pEffectData)
		return E_FAIL;

	EFFECTPARTICLEINFO* Particleiter = Get_EffectParticleInfoList(iEffectSceneIndex, pEffectTag).front();

	PARTICLECREATE tParticleCreate;
	tParticleCreate.wstLayerTag = pEffectTag;
	ZeroMemory(&tParticleCreate.tParticleInfo, sizeof(EFFECTPARTICLEINFO));
	memcpy(&tParticleCreate.tParticleInfo, Particleiter, sizeof(EFFECTPARTICLEINFO));
	if (FAILED(Add_ObjectToLayer_GetImmediately(iPrototypeSceneIndex, pPrototypeTag, iLayerSceneIndex, pLayerTag, ppOut,&tParticleCreate)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Manager::Clear_EffectData(_uint iSceneIndex)
{
	for (auto& Pair : m_pEffectData[iSceneIndex])
	{
		Safe_Release(Pair.second);

	}

	m_pEffectData[iSceneIndex].clear();

	return S_OK;
}

CGameObject * CObject_Manager::Find_Prototype(_uint iSceneIndex, const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iSceneIndex].begin(), m_pPrototypes[iSceneIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iSceneIndex, const _tchar * pLayerTag)
{
	auto	iter = find_if(m_pLayers[iSceneIndex].begin(), m_pLayers[iSceneIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iSceneIndex, _uint idx)
{
	auto	iter = m_pLayers[iSceneIndex].begin();

	for (int i = 0; i < idx; ++i)
		++iter;

	if (iter == m_pLayers[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

const _tchar* CObject_Manager::Find_LayerName(_uint iSceneIndex, _uint idx)
{
	auto	iter = m_pLayers[iSceneIndex].begin();

	for (int i = 0; i < idx; ++i)
		++iter;

	if (iter == m_pLayers[iSceneIndex].end())
		return nullptr;

	return iter->first;
}

CEffectData * CObject_Manager::Find_EffectData(_uint iSceneIndex, const _tchar * pEffectDataTag)
{
	auto	iter = find_if(m_pEffectData[iSceneIndex].begin(), m_pEffectData[iSceneIndex].end(), CTagFinder(pEffectDataTag));

	if (iter == m_pEffectData[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}
	Safe_Delete_Array(m_pPrototypes);

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pEffectData[i])
		{
			Safe_Release(Pair.second);
		}

		m_pEffectData[i].clear();
	}
	Safe_Delete_Array(m_pEffectData);

}

