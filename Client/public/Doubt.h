#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CAnvil;
class CSequence;
class CSelector;
class CSpawn_Doubt;
class CIdle_Doubt;
class CMove_Doubt;
class CChase_Doubt;
class CAttack_Doubt;
class CDead_Doubt;
class CHit_Doubt;
class CDoubt final :public CEnemy
{
protected:
	explicit CDoubt(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDoubt(const CDoubt& rhs);
	virtual ~CDoubt() = default;

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

public:
	void		Update_LookVector(_double TimeDelta);
	_bool		Is_AttackSure();

public:
	void		Set_Armed(_bool _bArmed);
	
private:
	void Update_PropHandMatrix();

private:
	CAnvil*						m_pAnvil = nullptr;
	CSequence*					m_pRoot = nullptr;
	CSpawn_Doubt*				m_pSpawnDoubt = nullptr;
	CIdle_Doubt*				m_pIdleDoubt = nullptr;
	CMove_Doubt*				m_pMoveDoubt = nullptr;
	CChase_Doubt*				m_pChaseDoubt = nullptr;
	CAttack_Doubt*				m_pAttackDoubt = nullptr;
	CDead_Doubt*				m_pDeadDoubt = nullptr;
	CHit_Doubt*					m_pHitDoubt = nullptr;
	CBoxCollider*				m_pHitColliderCom = nullptr;

private:
	_float4x4					m_PropHandMatrix;
	const _float4x4*			m_pCombinedMatrix = nullptr;
	_bool						m_IsUnArmed = false;
	_bool						m_IsActionDrop = false;
	_double						m_dLerpTime = 0.0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CDoubt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END