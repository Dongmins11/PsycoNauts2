#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CSelector;
class CSpawn_Censor;
class CIdle_Censor;
class CDead_Censor;
class CHit_Censor;
class CWalk_Censor;
class CRun_Censor;
class CJump_Censor;
class CAttack_Censor;
class CChase_Censor;
class CCensor final :public CEnemy
{
public:
	typedef struct tagCensorInfo
	{
		_float4x4 WorldMatrix;
		_uint     iMeshType;
	}CENSORINFO;
public:
	typedef struct tagJumpInfo
	{
		_float		fJumpPower;
		_float		fJumpValue;
		_double		lfJumpTime;
		_bool		IsJump;
		_bool		IsFallCheck;
		_bool		IsLandCheck;
	}JUMPINFO;
protected:
	explicit CCensor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCensor(const CCensor& rhs);
	virtual ~CCensor() = default;

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
	void Set_Gravity(_bool _IsGravity);

public:
	void		Update_LookVector(_double TimeDelta);
	void		Swing(_bool	_IsSwing);

private:
	void Update_WeaponMatrix();

private:
	CSequence*				m_pRoot = nullptr;
	CSelector*				m_pSelector = nullptr;
	CSpawn_Censor*			m_pSpawn_Censor = nullptr;
	CIdle_Censor*			m_pIdle_Censor = nullptr;
	CDead_Censor*			m_pDead_Censor = nullptr;
	CRun_Censor*			m_pRun_Censor = nullptr;
	CWalk_Censor*			m_pWalk_Censor = nullptr;
	CHit_Censor*			m_pHit_Censor = nullptr;
	CAttack_Censor*			m_pAttack_Censor = nullptr;
	CJump_Censor*			m_pJump_Censor = nullptr;
	CChase_Censor*			m_pChase_Censor = nullptr;

	CBoxCollider*			m_pHitColliderCom = nullptr;

	_double					m_dLerpTime = 0.0;
	_bool					m_IsGravity = false;
	_bool					m_IsSwing = false;
	_bool					m_IsReadyDead = false;

	_float4x4				m_WeaponMatrix;
	_float4x4				m_WeaponOffsetMatrix;
	_float4x4				m_PivotMatrix;
	_float4x4				m_HpBarMatrix;
	const	_float4x4*		m_pCombinedMatrixptr = nullptr;
	_uint					m_iMeshType = 0;
	_double					m_dAccTime = 0.0;
	_double					m_dSpeed = 1.0;

	CGameObject*			m_pAttTrigger = nullptr;
	CGameObject*			m_pHpBar = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CCensor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END