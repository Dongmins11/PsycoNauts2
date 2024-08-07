#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CIdle_Beater;
class CAttack_Beater;
class CIntro_Beater;
class CTaunt_Beater;
class CBeater final :public CEnemy
{
private:
	enum BONE{BONE_P1, BONE_P2, BONE_END};
protected:
	explicit CBeater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBeater(const CBeater& rhs);
	virtual ~CBeater() = default;

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
	void Update_Matrix();

private:
	void		Update_LookVector(_double TimeDelta);
	_double		m_dLerpTime = 0.0;

public:
	void			LockBosses(_bool* _bArr);
	void			Set_LinkSkill(_bool _bLinkSkill);

public:
	_bool			IsLinkSkill();

private:
	CBoxCollider*				m_pAttColliderCom = nullptr;
	CSequence*					m_pRoot = nullptr;
	CAttack_Beater*				m_pAttackBeater = nullptr;
	CIdle_Beater*				m_pIdleBeater = nullptr;
	CTaunt_Beater*				m_pTauntBeater = nullptr;
	CIntro_Beater*				m_pIntroBeater = nullptr;
	CGameObject*				m_pParent = nullptr;
	_bool						m_IsLinkSkill = false;
	_float4x4					m_BoneMatrix;
	_float4x4					m_PivotMatrix;
	_float4x4					m_OffsetMatrix[BONE_END];
	const _float4x4*			m_pCombinedMatrixPtr[BONE_END];


private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBeater* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END