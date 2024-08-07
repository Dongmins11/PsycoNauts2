#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBossWall final : public CGameObject
{
private:
	explicit CBossWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBossWall(const CBossWall& rhs);
	virtual ~CBossWall() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void OnTriggerEnter(CCollisionObject& collision)override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	_int				m_iNavigationIndex = 0;		// ?
	_uint				m_iPX_TYPE = 0;				// LayerType_BOUNCER
	_bool				m_IsCollision = false;
	_uint				m_iLayerIndex = 0;
	_bool				m_IsRaise = false;
	_double				m_AccTime = 0.0;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

public:
	static CBossWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END
