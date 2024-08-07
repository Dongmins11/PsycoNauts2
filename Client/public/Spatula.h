#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CBoxCollider;
END

BEGIN(Client)
class CSequence;
class CIdle_Spatula;
class CAttack_Spatula;
class CIntro_Spatula;
class CTaunt_Spatula;
class CSpatula final :public CEnemy
{
protected:
	explicit CSpatula(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpatula(const CSpatula& rhs);
	virtual ~CSpatula() = default;

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
	_double		m_dLerpTime = 0.0;

public:
	void			LockBosses(_bool* _bArr);

private:
	CBoxCollider*				m_pAttColliderCom = nullptr;
	CSequence*					m_pRoot = nullptr;
	CAttack_Spatula*			m_pAttackSpatula = nullptr;
	CIdle_Spatula*				m_pIdleSpatula = nullptr;
	CTaunt_Spatula*				m_pTauntSpatula = nullptr;
	CIntro_Spatula*				m_pIntroSpatula = nullptr;
	CGameObject*				m_pParent = nullptr;
	_float4x4					m_BoneMatrix;
	_float4x4					m_PivotMatrix;
	_float4x4					m_OffsetMatrix;
	const _float4x4*			m_pCombinedMatrixPtr = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CSpatula* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END