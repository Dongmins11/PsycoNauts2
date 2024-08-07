#pragma once

#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Engine)
class CNavigation;
class CGameObject;
END

BEGIN(Client)

class CScene_Stage02 final : public CScene
{
public:
	explicit CScene_Stage02(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Stage02() = default;
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
	HRESULT Ready_Shader_Value();

	HRESULT Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag);
	HRESULT Object_Change_Tag(OBJINFO ObjectInfo,const _tchar* LayerTag);

	HRESULT Ready_EnvironmentEffect();

private:
	class CLoader*		m_pNextLoader = nullptr;
	CGameObject*	m_pLoadingUI = nullptr;


private:
	_float		m_fLoadTimeAcc = 0.0;
	_bool		m_isPhysReady = false;
public:
	static CScene_Stage02* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END