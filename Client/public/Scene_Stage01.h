#pragma once

#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Engine)
class CNavigation;
class CGameObject;
class CRenderer;
END

BEGIN(Client)

class CScene_Stage01 final : public CScene
{
public:
	explicit CScene_Stage01(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Stage01() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual _int Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Light_Desc();
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_EnviromentTrigger(const _tchar* pEnviromentTriggerFilePath);
	HRESULT Branch(_matrix mWorldMatrix);

	HRESULT Ready_Static_Object();
	HRESULT Ready_Animation_Object();
	HRESULT Rendy_LightPoint();

	HRESULT Ready_EnvironmentEffect();
	HRESULT Ready_QuestNpc();

	HRESULT Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag);
	HRESULT Object_Change_Tag(OBJINFO ObjectInfo,const _tchar* LayerTag);
	HRESULT	SetUp_Renderer();
	void	Ready_ShaderOption();
	void	Ready_CameraOption();

private:
	class CLoader*		m_pNextLoader = nullptr;
	CGameObject*	m_pLoadingUI = nullptr;
	CRenderer*			m_pRenderer = nullptr;

private:
	_float		m_fLoadTimeAcc = 0.0;
	_bool		m_isPhysReady = false;
public:
	static CScene_Stage01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END