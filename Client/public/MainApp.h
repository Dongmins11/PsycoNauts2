#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"
#include "Base.h"


BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CMainApp final : public CBase
{
public:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT NativeConstruct();
	_int Tick(_double TimeDelta);
	HRESULT Render();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	CGameInstance*			m_pGameInstance = nullptr;
	CRenderer*				m_pRenderer = nullptr;

#ifdef _DEBUG
private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_uint					m_iNumRender = 0;
	_double					m_TimeAcc = 0.0;
	_bool					m_bRenderGUI = true;
#endif // _DEBUG

private:
	HRESULT Initialize_Engine();
	HRESULT Ready_Component_Prototype_ForStatic();
	HRESULT Ready_GameObject_Prototype_ForStatic();
	HRESULT Ready_GameObject_Layer_ForStatic();
	HRESULT Open_Scene(SCENE eScene);

	HRESULT Ready_Gara();



public:
	static CMainApp* Create();
	virtual void Free() override;
};

END