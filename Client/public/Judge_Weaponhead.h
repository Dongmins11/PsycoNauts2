#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CSphereCollider;
END

BEGIN(Client)
class CJudge_Weaponhead final :public CGameObject
{
public:
	typedef struct tagWeaponInfo
	{
		CGameObject*		pParent;
		const	_float4x4*		pParentMatrix;
		_uint	iWeaponType;
	}WEAPONINFO;

private:
	CJudge_Weaponhead(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CJudge_Weaponhead(const CJudge_Weaponhead& rhs);
	virtual ~CJudge_Weaponhead() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	virtual void Setting_Throwing(_fvector vDir)override;

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;

private:
	WEAPONINFO				m_tWeaponInfo;
	_float4x4	m_NewPivotMatrix;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CJudge_Weaponhead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END