#include "..\public\Scene_Manager.h"
#include "Scene.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CScene_Manager)

CScene_Manager::CScene_Manager()
{

}

HRESULT CScene_Manager::Open_Scene(_uint iCurrentSceneIndex, CScene * pNewScene)
{
	if (nullptr == pNewScene)
		return E_FAIL;	
	
	m_pBackUpCurrentScene = pNewScene;

	m_iBackUpCurrentSceneIndex = iCurrentSceneIndex;

	m_bOpenScene = true;
	return S_OK;
}

_int CScene_Manager::Tick(_double TimeDelta)
{
	if (true == m_bOpenScene)
	{
		if (FAILED(Go_NextScene()))
			return -1;
	}
	if (0 > m_pCurrentScene->Tick(TimeDelta))
		return -1;

	

	return _int();
}

HRESULT CScene_Manager::Render()
{
	if (nullptr == m_pCurrentScene)
		return E_FAIL;

	return m_pCurrentScene->Render();
}

HRESULT CScene_Manager::Go_NextScene()
{

	if (nullptr != m_pCurrentScene)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Clear_SceneResource(m_iCurrentSceneIndex);
		RELEASE_INSTANCE(CGameInstance);
	}

	Safe_Release(m_pCurrentScene);

	m_pCurrentScene = m_pBackUpCurrentScene;

	m_iCurrentSceneIndex = m_iBackUpCurrentSceneIndex;

	m_bOpenScene = false;

	return S_OK;
}

void CScene_Manager::Free()
{
	Safe_Release(m_pCurrentScene);
}
