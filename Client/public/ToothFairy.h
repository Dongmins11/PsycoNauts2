#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)

class CToothFairy final :public CEnemy
{
protected:
	explicit CToothFairy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToothFairy(const CToothFairy& rhs);
	virtual ~CToothFairy() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;


private:
	void		Update_LookVector(_double TimeDelta);
	void		Update_HolderPointMatrix();

private:
	_float4x4					m_HolderPointMatrix;
	const _float4x4*			m_pCombinedMatrix = nullptr;
	_bool						m_IsActionDrop = false;
	_double						m_dLerpTime = 0.0;
	_double						m_dAccTime = 0.0;
	_float3						m_vAimLook;
	_bool						m_IsTurn = false;
	CGameObject*				m_pTooth = nullptr;

	CBoxCollider*				m_pHitColliderCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CToothFairy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END