#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCharacterController;
END

BEGIN(Client)
class CEnemy abstract : public CGameObject
{
public:
	enum ATTACKED { FW, BK, RT, LT, TO, PB, B,TYPE_END}; //순서대로 포워드 방향으로부터 피격, 백 방향, 오른쪽 방향, 왼쪽 방향, 던진 물체, 전기 충격,직선빔
protected:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene)override;
	virtual _int    Tick(_double TimeDelta)override;
	virtual _int    LateTick(_double TimeDelta)override;
public:
	virtual void	Reset() {};

public:
	void Attacked(CEnemy::ATTACKED eAttackType);
	void Damaged(_int iDamage);

private:
	HRESULT SetUp_TargetPosPtr();

private:
	void Update_TargetData();

public:
	void Set_AttackLoop(_bool _bAttackLoop);
	void Reset_Hp();
	void Reset_HitState();
	void Lock(_bool _bLock);
	void Taunt(_bool _bTaunt);
	void HitPhase(_bool _bHitPhase);
	void PatternLock(_bool _bPatternLock);
	void Spawned(_bool _bSpawned);
	void Set_Ready(_bool _bReady);

public:
	const _bool IsGround() const;
	const _bool	IsSlided() const;
	const _int Get_Hp() const {
		return m_iHp;
	}
	const _bool IsHit() const {
		return m_IsHit;
	}
	const _bool	IsAttack() const {
		return m_IsAttackLoop;
	}
	const _bool IsLock() const {
		return m_IsLock;
	}
	const _bool IsPatternLock() const {
		return m_IsPatternLock;
	}
	const _bool IsTaunt() const {
		return m_IsTaunt;
	}
	const _bool IsHitPhase() const {
		return m_IsHitPhase;
	}
	const _bool IsSpawned() const {
		return m_IsSpawned;
	}
	const _bool IsReady() const {
		return m_IsReady;
	}
	const ATTACKED Get_Attacked_Type() const {
		return m_AttackedType;
	}

public:
	CModel*		Get_Model() {
		if (nullptr == m_pModelCom)
			return nullptr;
		return m_pModelCom;
	}
	CTransform*		Get_Transform() {
		if (nullptr == m_pTransformCom)
			return nullptr;
		return m_pTransformCom; }

public:
	const _float		Get_Distance() const {
		return m_fTargetDistance;
	}
	const _float		Get_Degree() const {
		return m_fDegree;
	}
	const _float4		Get_TargetDir() const {
		return m_vTargetDir;
	}

private:
	_float4*				m_pTargetPos = nullptr;
	_float4					m_vTargetDir = { 0.f,0.f,0.f,0.f };
	_float					m_fTargetDistance;
	_float					m_fDegree = 0.f;

protected:
	_int			m_iHp = 100;
	_bool			m_IsHit = false;
	_bool			m_IsAttackLoop = false;
	_bool			m_IsLock = false;
	_bool			m_IsPatternLock = false;
	_bool			m_IsTaunt = false;
	_bool			m_IsHitPhase = false;
	_bool			m_IsSpawned = false;
	_bool			m_IsReady = false;
	ATTACKED		m_AttackedType = TYPE_END;

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCharacterController*	m_pCharacterController = nullptr;

public:
	virtual void Free();

};

END