#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CSphereCollider;
END


BEGIN(Client)
class CSkill_PisBlast final : public CGameObject
{
private:
	CSkill_PisBlast(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSkill_PisBlast(const CSkill_PisBlast& rhs);
	virtual ~CSkill_PisBlast() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSphereCollider*		m_pColliderCom = nullptr;

	CGameObject*			m_pEffect = nullptr;
private:
	_bool					m_bDeadCheck = false;
	_double					m_lfAgeDeadTime = 0.0;
	_double					m_TimeAcc = 0.0;
	_float					m_fHeight = 0.f;

private: /* For. Effect */
	_bool					m_isEffectCreate = false;
	_float3					m_vTargetEffectPos;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_TargetBoneMatrices(const char* pBoneName);

public:
	static CSkill_PisBlast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END