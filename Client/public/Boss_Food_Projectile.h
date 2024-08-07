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
class CBoss_Food_Projectile final :public CGameObject
{
public:
	enum FOOD_TYPE
	{
		FOOD_TYPE_BROCOL,
		FOOD_TYPE_CARROT,
		FOOD_TYPE_POTATO,
		FOOD_TYPE_SOUP,
		FOOD_TYPE_COOKIE_ONE,
		FOOD_TYPE_COOKIE_TWO,
		FOOD_TYPE_COOKIE_THREE,
		FOOD_TYPE_COOKIE_FOR,
		FOOD_TYPE_END
	};
private:
	enum FOOD_PROJECTILE_SHOOT {
		FOOD_PROJECTILE_SHOOT_FIRST,
		FOOD_PROJECTILE_SHOOT_SECOND,
		FOOD_PROJECTILE_SHOOT_THIRD,
		FOOD_PROJECTILE_SHOOT_FORTH,
		FOOD_PROJECTILE_SHOOT_END
	};

protected:
	explicit CBoss_Food_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBoss_Food_Projectile(const CBoss_Food_Projectile& rhs);
	virtual ~CBoss_Food_Projectile() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	virtual void OnTriggerEnter(CCollisionObject& collision);

private:
	CRenderer*				m_pRendererCom = nullptr;

	CTransform*				m_pTransformCom = nullptr;

	CModel*					m_pModelCom = nullptr;

	CPhysXCollider*			m_pColliderCom = nullptr;

private:
	_bool					m_bFirstLerpCheck = false;
	_bool					m_bCatMulShootCheck = false;

private:
	_double					m_lfLerpRatio = 0.0;
	_double					m_lfCatMulShootRatio = 0.0;

private:
	_float4					m_vStartPosition = {};
	_float4					m_vEndPosition = {};
	_float4					m_vCatMulShootPos[FOOD_PROJECTILE_SHOOT_END];

private:
	FOOD_TYPE				m_eFoodType = FOOD_TYPE_END;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	HRESULT	Object_PosUpdate(_double TimeDelta);
	HRESULT	Colider_PosUpdate();



public:
	static CBoss_Food_Projectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END