#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CSelector;
class CSpawn_Judge;
class CChase_Judge;
class CStolen_Judge;
class CIdle_Judge;
class CIdle_Judge_UA;
class CHit_Judge;
class CHit_Judge_UA;
class CDead_Judge;
class CDead_Judge_UA;
class CWalk_Judge;
class CWalk_Judge_UA;
class CRun_Judge;
class CRun_Judge_UA;
class CJump_Judge;
class CJump_Judge_UA;
class CAttack_Judge;
class CAttack_Judge_UA;
class CJudge final :public CEnemy
{
public:
	enum TYPE { TYPE_DEFAULT , TYPE_CHEF, TYPE_PUKECHEF, TYPE_END};

private:
	enum BONE { BONE_PROPHAND, BONE_WEAPONHEAD, BONE_ROOT, BONE_END };

public:
	enum ARMEDTYPE { ARMED, UNARMED, ARMEDTYPE_END };

public:
	typedef struct tagJudgeInfo
	{
		TYPE eMeshType;
		_float4x4 WorldMatrix;
	}JUDGEINFO;



protected:
	explicit CJudge(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CJudge(const CJudge& rhs);
	virtual ~CJudge() = default;

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
	void		SetOn_RotateWeapon(_bool _bRotate);
	void		Set_Armed(_bool _bArmed);
	void		SetOn_Drop(_bool _bDrop);
	void		Destroy_Weapon();
	const		_bool&		IsArmed()const;
	const		_float4x4*	Get_PropHandMatrixPtr()const;
	const		_float4x4*	Get_WeaponMatrixPtr()const;

public:
	void		Attack(ARMEDTYPE eType, _bool IsAttack);
	void		Render_Trail(_bool  _IsTrail);

public:
	TYPE		Get_Type();

private:
	CSequence*				m_pRoot = nullptr;
	CSelector*				m_pSelector = nullptr;
	CSelector*				m_pArmedMove = nullptr;
	CSelector*				m_pUnArmedMove = nullptr;
	CSequence*				m_pArmedSeq = nullptr;
	CSequence*				m_pUnArmedSeq = nullptr;
	CSpawn_Judge*			m_pSpawnJudge = nullptr;
	CChase_Judge*			m_pChaseJudge = nullptr;
	CStolen_Judge*			m_pStolenJudge = nullptr;
	CIdle_Judge*			m_pIdleJudge = nullptr;
	CIdle_Judge_UA*			m_pIdleJudgeUA = nullptr;
	CHit_Judge*				m_pHitJudge = nullptr;
	CHit_Judge_UA*			m_pHitJudgeUA = nullptr;
	CDead_Judge*			m_pDeadJudge = nullptr;
	CDead_Judge_UA*			m_pDeadJudgeUA = nullptr;
	CWalk_Judge*			m_pWalkJudge = nullptr;
	CWalk_Judge_UA*			m_pWalkJudgeUA = nullptr;
	CRun_Judge*				m_pRunJudge = nullptr;
	CRun_Judge_UA*			m_pRunJudgeUA = nullptr;
	CJump_Judge*			m_pJumpJudge = nullptr;
	CJump_Judge_UA*			m_pJumpJudgeUA = nullptr;
	CAttack_Judge*			m_pAttackJudge = nullptr;
	CAttack_Judge_UA*		m_pAttackJudgeUA = nullptr;

private:
	CGameObject*				m_pWeapon = nullptr;
	CGameObject*				m_pWeaponhead = nullptr;
	CGameObject*				m_pHpBar = nullptr;
	_float4x4					m_BoneMatirx[BONE_END];
	_float4x4					m_OffSetMatrix[BONE_END];
	_float4x4					m_PivotMatrix[BONE_END];
	_float4x4					m_HpBarMatrix;
	const _float4x4*			m_pCombinedMatrix[BONE_END] = { nullptr, nullptr };
	_double						m_dLerpTime = 0.0;
	_bool						m_IsUnArmed = false;
	_bool						m_IsActionDrop = false;
	_bool						m_IsArmed = true;
	_bool						m_IsAttack[ARMEDTYPE_END] = { false, false };
	_bool						m_IsTrailRender = false;
	_bool						m_IsReadyDead = false;
	_bool						m_IsReadyDestroy = false;
	_float3						m_vPrePos;

	CBoxCollider*				m_pHitColliderCom = nullptr;
	CGameObject*				m_pAttTrigger = nullptr;
	CGameObject*				m_pWeaponAttTrigger = nullptr;

	CGameObject*				m_pTrail = nullptr;
	TYPE						m_eType = TYPE_END;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(); 
	void Update_BoneMatrix();

public:
	static CJudge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END