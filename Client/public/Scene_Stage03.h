#pragma once

#pragma once
#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Engine)
class CNavigetion;
class CGameObject;
class CRenderer;
END

BEGIN(Client)

class CScene_Stage03 final :public CScene
{
public:
	explicit CScene_Stage03(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Stage03() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual _int Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void* Get_SceneData();
	virtual void Setting_StageUpdate(_uint iSceneNum);
private:
	HRESULT Ready_Light_Desc();
	HRESULT Rendy_LightPoint();

	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_EnviromentTrigger(const _tchar* pEnviromentTriggerFilePath);
	HRESULT Ready_Layer_Cooking_UI();
	
	HRESULT Branch(_matrix mWorldMatrix);

	HRESULT Ready_Static_Object();
	HRESULT Ready_Animation_Object();

	HRESULT Ready_EnvironmentEffect();

	HRESULT Static_Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag);
	HRESULT Object_Change_Tag(OBJINFO ObjectInfo, const _tchar* LayerTag);
	HRESULT Ready_Cloud_Create();

	void Ready_Phase0_UI();
	void Ready_Phase1_UI();
	void Ready_Phase2_UI();

	HRESULT	SetUp_Renderer();
	void	Ready_ShaderOption();
	void	Ready_CameraOption();

	HRESULT	Create_FieldSmoke(_vector vPos);

private:
	void	FruitJumpAnimationCehck();
	void	Setting_FruitPlatingTriggers(_uint iNum);
private:
	HRESULT Fruit_ObjectIndex(_uint LayerIndex);
private:
	class CLoader*		m_pNextLoader = nullptr;
	CGameObject*		m_pLoadingUI = nullptr;
	CRenderer*			m_pRenderer = nullptr;


private:
	_float		m_fLoadTimeAcc = 0.0;
	_bool		m_isPhysReady = false;
	_bool		m_IsLoadingMonster = false;
	_bool		m_IsLoadingMonster2 = false;

	_uint		m_iPhaseNum = 0;
	_uint		m_iMoveStageNumber = 0;
	_bool		m_isCounting_TimeAttack = false;
	_double		m_lfStageTime = 300.0;
	_double		m_lfMoveStageTimeAcc = 0.0;

private:
	_bool		m_bSoundCheck = false;
public:
	static CScene_Stage03* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;

};

END