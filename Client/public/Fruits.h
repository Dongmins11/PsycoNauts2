#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
END

BEGIN(Client)
class CFruits final :public CGameObject
{
public:
	enum FRUITTYPE {TYPE_WATERMELON, TYPE_BEEHIVE, TYPE_BREAD, TYPE_EGG, TYPE_ONION, TYPE_PIG, TYPE_STRAWBERRY, TYPE_END};
public:
	typedef struct tagFruitsInfo
	{
		_float3		vPos;
		FRUITTYPE	eFruitsType;
	}FRUITSINFO;

private:
	CFruits(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFruits(const CFruits& rhs);
	virtual ~CFruits() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	virtual void Setting_Throwing(_fvector vDir)override;

public:
	FRUITTYPE				Get_Type();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;

private:
	FRUITTYPE				m_eType = TYPE_END;
	FRUITSINFO				m_tFruitsInfo;
	CGameObject*			m_pBlob = nullptr;
	_float4x4				m_WorldMatrix;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CFruits* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END