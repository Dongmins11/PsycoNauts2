#pragma once
#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Engine)
class CNavigetion;
END

BEGIN(Client)

class CScene_Stage04 final :public CScene
{
public:
	explicit CScene_Stage04(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Stage04() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual _int Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void* Get_SceneData();

private:
	HRESULT Ready_Light_Desc();
	HRESULT Rendy_LightPoint();

	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_EnviromentTrigger(const _tchar* pEnviromentTriggerFilePath);

	HRESULT Branch(_matrix mWorldMatrix);

	HRESULT Ready_Static_Object();
	HRESULT Ready_Animation_Object();

	HRESULT Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag);
	HRESULT Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag);

	void	Ready_ShaderOption();

private:
	_float		m_fLoadTimeAcc = 0.0;
	_double		m_dPlayTime = 0.0;
	_bool		m_isPhysReady = false;
	_bool		m_IsLoadingMonster = false;
	_bool		m_IsSpawned = false;
	_uint		m_iPhaseNum = 0;
public:
	static CScene_Stage04* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;

};

END