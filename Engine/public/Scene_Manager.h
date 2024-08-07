#pragma once

/* 현재 할당된 레벨의 주소를가지고 있는다. */
/* 레벨 교체에 필요한 작업을 수행한다. */
#include "Base.h"

BEGIN(Engine)

class CScene_Manager final : public CBase
{
	DECLARE_SINGLETON(CScene_Manager)
public:
	CScene_Manager();
	virtual ~CScene_Manager() = default;
public:
	_uint	Get_CurrentSceneIndex() { return m_iCurrentSceneIndex; }
	HRESULT Open_Scene(_uint iCurrentSceneIndex, class CScene* pNewScene);

	_int Tick(_double TimeDelta);
	HRESULT Render();
	const CScene* Get_CurrentScene() const { return m_pCurrentScene; }
private:
	HRESULT Go_NextScene();
private:
	class CScene*			m_pCurrentScene = nullptr;
	_uint					m_iCurrentSceneIndex = 0;
	_bool					m_bOpenScene = false;

	class CScene*			m_pBackUpCurrentScene = nullptr;
	_uint					m_iBackUpCurrentSceneIndex = 0;
public:
	virtual void Free() override;

};

END