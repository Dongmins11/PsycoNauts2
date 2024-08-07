#pragma once
#include "Enemy.h"

BEGIN(Client)
class CSequence;
class CSelector;
class CSpawn_BadIdea;
class CIdle_BadIdea;
class CDead_BadIdea;
class CRun_BadIdea;
class CWalk_BadIdea;
class CChase_BadIdea;
class CHit_BadIdea;
class CJump_BadIdea;
class CAttack_BadIdea;
class CBadIdea final :public CEnemy
{
protected:
	explicit CBadIdea(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBadIdea(const CBadIdea& rhs);
	virtual ~CBadIdea() = default;

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

public:
	void		Bite(_bool _IsBite);

private:
	CSequence*					m_pRoot = nullptr;
	CSelector*					m_pSelector = nullptr;
	CSpawn_BadIdea*				m_pSpawnBadIdea = nullptr;
	CIdle_BadIdea*				m_pIdleBadIdea = nullptr;
	CDead_BadIdea*				m_pDeadBadIdea = nullptr;
	CRun_BadIdea*				m_pRunBadIdea = nullptr;
	CWalk_BadIdea*				m_pWalkBadIdea = nullptr;
	CChase_BadIdea*				m_pChaseBadIdea = nullptr;
	CHit_BadIdea*				m_pHitBadIdea = nullptr;
	CJump_BadIdea*				m_pJumpBadIdea = nullptr;
	CAttack_BadIdea*			m_pAttackBadIdea = nullptr;
	CBoxCollider*				m_pHitColliderCom = nullptr;
	CBoxCollider*				m_pAttColliderCom = nullptr;
	_double					m_dLerpTime = 0.0;

	_float4x4					m_AttackTriggerMatrix;
	_bool						m_IsBite = false;
	CGameObject*			m_pHpBar = nullptr;

	_float4x4				m_HpBarMatrix;

	_bool						m_IsReadyDead = false;

private:
	CGameObject*				m_pAttTrigger = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBadIdea* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END