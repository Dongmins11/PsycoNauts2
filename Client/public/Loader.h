#pragma once

#include "Client_Defines.h"
#include "Base.h"
BEGIN(Engine)
class CNavigation;
END
/* 필요한 자원을 로드하기위한 클래스다. */
BEGIN(Client)

class CLoader final : public CBase
{
public:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLoader() = default;
public:
	SCENE Get_SceneID() {
		return m_eSceneID;
	}

	CRITICAL_SECTION* Get_CriticalSection() {
		return &m_CriticalSection;
	}

	_bool Get_Finished() const {
		return m_isFinished;
	}

	void Set_ThreadLoadingFinished(_bool isFinish) {
		m_isThreadLoadingComplete = isFinish;
	}

	void Set_NextThreadingSceneID(SCENE eScene)
	{
		m_eThreadingNextID = eScene;
	}

	
public:
	HRESULT NativeConstruct(SCENE eSceneID);
public:
	HRESULT Loading_ForLogoScene();
	HRESULT Loading_ForDemoScene();
	// 이빨맵
	HRESULT Loading_ForStage01();
	HRESULT Loading_Stage01_Npc();
	// 쥐맵
	HRESULT Loading_ForStage02();
	// 중보 + 요리대회맵
	HRESULT Loading_ForStage03();
	HRESULT Loading_Stage03_StaticObjects();
	HRESULT Loading_Stage03UI();
	// 요리대회 중간맵
	HRESULT Loading_ForStage04();

	// 요리대회 보스맵
	HRESULT Loading_ForStage05();

	HRESULT Loading_DuringPlaying();
	HRESULT Draw_LoadingText();

	HRESULT Loading_TutorialScripts();
	// 지울거임
	HRESULT Loading_Ending();
private:
	HRESULT SetUp_Component();

private:
	HRESULT Ready_Model_Data(const _tchar* FinderFilePath, _float3 pivotScale = {0.01f,0.01f,0.01f});
	HRESULT Loading_NaviData(const _tchar * pNaviDataFilePath);
	HRESULT Loading_Monster();
	HRESULT Loading_PlayerTrigger();
private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_CriticalSection;
	ID3D11Device*		m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	SCENE				m_eSceneID = SCENE_END;
	SCENE				m_eThreadingNextID = SCENE_END;

	_tchar				m_szLoading[MAX_PATH] = TEXT("");
	_bool				m_isFinished = false;

	_bool				m_isThreadLoadingComplete = false;
	_double				m_lfLoadingTimeAcc = 0.0;
	_double				m_lfLoadingTimeMax = 2.0;
	//class CNavigation*	m_pNavigation = nullptr;

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SCENE eSceneID);
	virtual void Free() override;
};

END