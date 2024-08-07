#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CSphereCollider;
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CSelector;
class CSpawn_CensorB;
class CIdle_CensorB;
class CDead_CensorB;
class CHit_CensorB;
class CWalk_CensorB;
class CRun_CensorB;
class CJump_CensorB;
class CAttack_CensorB;
class CChase_CensorB;
class CCensor_Burly final :public CEnemy
{
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

public:
	enum HANDTYPE {HAND_R, HAND_L, HAND_END};

protected:
	explicit CCensor_Burly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCensor_Burly(const CCensor_Burly& rhs);
	virtual ~CCensor_Burly() = default;

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
	void SetOnTrail_LeftHand(_bool IsOnOff);
	void SetOnTrail_RightHand(_bool IsOnOff);

private:
	void Update_AttMatrix();

public:
	void		Update_LookVector(_double TimeDelta);

public:
	void		Attack(HANDTYPE eHandType, _bool _IsAttack);

private:
	CSequence*					m_pRoot = nullptr;
	CSelector*					m_pSelector = nullptr;
	CSpawn_CensorB*			m_pSpawn_Censor = nullptr;
	CIdle_CensorB*			m_pIdle_Censor = nullptr;
	CDead_CensorB*			m_pDead_Censor = nullptr;
	CRun_CensorB*			m_pRun_Censor = nullptr;
	CWalk_CensorB*			m_pWalk_Censor = nullptr;
	CHit_CensorB*			m_pHit_Censor = nullptr;
	CAttack_CensorB*		m_pAttack_Censor = nullptr;
	CJump_CensorB*			m_pJump_Censor = nullptr;
	CChase_CensorB*			m_pChase_Censor = nullptr;
	_bool					m_IsGravity = false;
	_double					m_dLerpTime = 0.0;
	_bool					m_IsRightHandTrail = false;
	_bool					m_IsLeftHandTrail = false;
	_bool					m_IsReadyDead = false;
	CBoxCollider*			m_pHitColliderCom = nullptr;

	_float4x4				m_PivotMatrix;
	const _float4x4*		m_pCombinedMatrixPtr[HAND_END] = { nullptr, nullptr };
	_float4x4				m_OffSetMatrix[HAND_END];
	_float4x4				m_AttMatrix[HAND_END];


	CGameObject*			m_pAttTriggerL = nullptr;
	CGameObject*			m_pAttTriggerR = nullptr;
	CGameObject*			m_pHpBar = nullptr;

	CGameObject*			m_pLeftHandTrail = nullptr;
	CGameObject*			m_pRightHandTrail = nullptr;

	_float4x4				m_HpBarMatrix;

	_bool					m_IsAttack[HAND_END] = { false, false };

private:
	JUMPINFO				m_tJumpInfo;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CCensor_Burly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END