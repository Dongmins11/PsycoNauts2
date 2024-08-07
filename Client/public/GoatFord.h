#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CIdle_GoatF;
class CAttack_GoatF;
class CHit_GoatF;
class CDead_GoatF;
class CTaunt_GoatF;
class CSpawn_GoatF;
class CAttTrigger_GoatF;
class CGoatFord final :public CEnemy
{
protected:
	explicit CGoatFord(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGoatFord(const CGoatFord& rhs);
	virtual ~CGoatFord() = default;

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
	virtual void Reset() override;

public:
	const _float4x4				Get_BoneMatrix() const { return m_BoneMatrix; }

private:
	void		Update_LookVector(_double TimeDelta);
	_double		m_dLerpTime = 0.0;

public:
	void			LockBosses(_bool* _bArr);
	void			Set_LinkSkill(_bool _bLinkSkill);

public:
	_bool			IsLinkSkill();

private:
	CBoxCollider*				m_pHitColliderCom = nullptr;
	CSequence*					m_pRoot = nullptr;
	CAttack_GoatF*				m_pAttackGoat = nullptr;
	CIdle_GoatF*				m_pIdleGoat = nullptr;
	CHit_GoatF*					m_pHitGoat = nullptr;
	CTaunt_GoatF*				m_pTauntGoat = nullptr;
	CDead_GoatF*				m_pDeadGoat = nullptr;
	CSpawn_GoatF*				m_pSpawnGoat = nullptr;
	CAttTrigger_GoatF*			m_pAttTrigger = nullptr;
	CGameObject*				m_pParent = nullptr;
	_bool						m_IsLinkSkill = false;
	_float4x4					m_BoneMatrix;
	_float4x4					m_PivotMatrix;
	_float4x4					m_OffsetMatrix;
	const _float4x4*			m_pCombinedMatrixPtr = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CGoatFord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END