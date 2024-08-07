#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CIdle_GoatH;
class CAttack_GoatH;
class CHit_GoatH;
class CDead_GoatH;
class CTaunt_GoatH;
class CSpawn_GoatH;
class CAttTrigger_GoatH;
class CGoatHollis final :public CEnemy
{
protected:
	explicit CGoatHollis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGoatHollis(const CGoatHollis& rhs);
	virtual ~CGoatHollis() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset() override;

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

public:
	const _float4x4				Get_BoneMatrix() const { return m_BoneMatrix; }

private:
	void		Update_LookVector(_double TimeDelta);
	_double		m_dLerpTime = 0.0;

public:
	void			LockBosses(_bool* _bArr);

private:
	CBoxCollider*				m_pHitColliderCom = nullptr;
	CSequence*					m_pRoot = nullptr;
	CAttack_GoatH*				m_pAttackGoat = nullptr;
	CIdle_GoatH*				m_pIdleGoat = nullptr;
	CHit_GoatH*					m_pHitGoat = nullptr;
	CTaunt_GoatH*				m_pTauntGoat = nullptr;
	CDead_GoatH*				m_pDeadGoat = nullptr;
	CSpawn_GoatH*				m_pSpawnGoat = nullptr;
	CAttTrigger_GoatH*			m_pAttTrigger = nullptr;
	CGameObject*				m_pParent = nullptr;
	_float4x4					m_BoneMatrix;
	_float4x4					m_PivotMatrix;
	_float4x4					m_OffsetMatrix;
	const	_float4x4*			m_pCombinedMatrixPtr = nullptr;



private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CGoatHollis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END