#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CIdle_GoatO;
class CAttack_GoatO;
class CHit_GoatO;
class CDead_GoatO;
class CTaunt_GoatO;
class CSpawn_GoatO;
class CAttTrigger_GoatO;
class CGoatOtto final :public CEnemy
{
protected:
	explicit CGoatOtto(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGoatOtto(const CGoatOtto& rhs);
	virtual ~CGoatOtto() = default;

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

private:
	void		Update_LookVector(_double TimeDelta);
	void		Update_BoneMatrix();
	_double		m_dLerpTime = 0.0;

public:
	void			LockBosses(_bool* _bArr);
	
public:
	const _float4x4				Get_BoneMatrix() const { return m_BoneMatrix; }

private:
	CBoxCollider*				m_pHitColliderCom = nullptr;
	CSequence*					m_pRoot = nullptr;
	CAttack_GoatO*				m_pAttackGoat = nullptr;
	CIdle_GoatO*				m_pIdleGoat = nullptr;
	CHit_GoatO*					m_pHitGoat = nullptr;
	CTaunt_GoatO*				m_pTauntGoat = nullptr;
	CDead_GoatO*				m_pDeadGoat = nullptr;
	CSpawn_GoatO*				m_pSpawnGoat = nullptr;
	CAttTrigger_GoatO*			m_pAttTrigger = nullptr;
	CGameObject*				m_pParent = nullptr;
	_float4x4					m_BoneMatrix;
	_float4x4					m_PivotMatrix;
	_float4x4					m_OffsetMatrix;
	const _float4x4*			m_pCombinedMatrixPtr = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CGoatOtto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END