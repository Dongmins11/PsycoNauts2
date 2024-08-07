#pragma once

#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Engine)
class CNavigation;
class CGameObject;
class CRenderer;
END

BEGIN(Client)

class CScene_Demo final : public CScene
{
public:
	explicit CScene_Demo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Demo() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual _int Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Light_Desc();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	//HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_EnviromentTrigger(const _tchar* pEnviromentTriggerFilePath);
	HRESULT Ready_Layer_DemoRect(const _tchar* pLayerTag);
	HRESULT Branch(_matrix mWorldMatrix);

	HRESULT Ready_Static_Object();
	HRESULT Rendy_Animation_Object();
	HRESULT Rendy_LightPoint();
	
	HRESULT Ready_EnvironmentEffect();
	HRESULT Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag);
	HRESULT Object_Change_Tag(OBJINFO ObjectInfo,const _tchar* LayerTag);

	HRESULT	SetUp_Renderer();
	void	Ready_ShaderOption();
	void	Ready_CameraOption();

private:
	//class CNavigation*	m_pNavigation = nullptr;
	class CLoader*		m_pNextLoader = nullptr;
	CGameObject*		m_pLoadingUI = nullptr;
	CRenderer*			m_pRenderer = nullptr;
private:
	_float		m_fLoadTimeAcc = 0.0;
	_bool		m_isPhysReady = false;
	_bool		m_isFadeIn = false;
	_bool		m_isOns = true;
	_float		m_fFadeTime = 0.f;

public:
	static CScene_Demo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END