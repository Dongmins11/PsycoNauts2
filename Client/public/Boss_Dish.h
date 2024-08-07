#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)
class CBoss_Dish final :public CGameObject
{
public:
	enum BOSS_FOODTYPE {
		BOSS_FOODTYPE_DISH,
		BOSS_FOODTYPE_SOUP,
		BOSS_FOODTYPE_COOKIE,
		BOSS_FOODTYPE_END
	};
protected:
	explicit CBoss_Dish(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBoss_Dish(const CBoss_Dish& rhs);
	virtual ~CBoss_Dish() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	const _uint	Get_DishType() const { return (_uint)m_eMyType; }
	const _bool Get_CamSwitching() const { return m_CamSwitchingCheck; }
private:
	virtual void OnTriggerEnter(CCollisionObject& collision);
	virtual void OnTriggerExit(CCollisionObject& collision);
	
private:
	CRenderer*				m_pRendererCom = nullptr;

	CTransform*				m_pTransformCom = nullptr;

	CModel*					m_pModelCom = nullptr;

	CPhysXCollider*			m_pPhysXColliderCom = nullptr;\

	CPhysXCollider*			m_pColliderCom = nullptr;

private:
	_float4x4				m_ColiderMatirx = {};
	_float4x4*				m_pBoneMatrix = nullptr;

private:
	BOSS_FOODTYPE			m_eMyType = BOSS_FOODTYPE_END;
	LayerType				m_eTriggerLayerType = LayerType::LAYERTYPE_END;
private:
	_float4					m_vFristPosition = {};
	_float4					m_vEndPosition = {};
private:
	_bool					m_bLerpPositionCheck = false;
	_double					m_lfRatio = 0.0;

private:
	_double					m_bDishThrowTimeAcc = 0.0;
	_bool					m_bDishThrowCheck = false;
	_bool					m_bDishGrabCheck = false;

private:
	_double					m_lfAddTime = 0.0;
	_bool					m_CamSwitchingCheck = false;



private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	HRESULT	Colider_PosUpdate();
	HRESULT First_LerpPositionUpdate(_double TimeDelta);
	HRESULT Grab_Dish_PositionUpdate(_double TimeDelta);
	HRESULT	Dish_Throw_PositionUpdate(_double TimeDelta);

public:
	static CBoss_Dish* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END