#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CRenderer;
class CTransform;
class CBoxCollider;
END

BEGIN(Client)

class CBouncer final : public CGameObject
{
public:
	enum BOUNCER { BOUNCER_IDLE, BOUNCER_BOUNCE, BOUNCER_END };

private:
	explicit CBouncer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBouncer(const CBouncer& rhs);
	virtual ~CBouncer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void OnTriggerEnter(CCollisionObject& collision)override;

private:
	void	UpdateAnimState(_double TimeDelta);
	void	SetUp_AnimValue(_uint iAnimIndex, _double dAnimSpeed);
	void	SetUp_AnimFinish(BOUNCER eNextAnim, _uint iNextAnimIndex, _double dNextAnimSpeed);

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CBoxCollider*		m_pColliderCom = nullptr;

private:
	_int				m_iNavigationIndex = 0;		// ?
	_uint				m_iPX_TYPE = 0;				// LayerType_BOUNCER
	_bool				m_IsCollision = false;
	_uint				m_iLayerIndex = 0;

private:
	BOUNCER				m_eCurAnimState = BOUNCER_END;
	_uint				m_iCurAnimation = 0;
	_double				m_dAnimSpeed = 1.0;


private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

public:
	static CBouncer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END