#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CBoxCollider;
END

BEGIN(Client)
class CAnvil final :public CGameObject
{
private:
	CAnvil(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAnvil(const CAnvil& rhs);
	virtual ~CAnvil() = default;

public:
	virtual HRESULT NativeConstruct(void* _pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

public:
	void Set_Drop(_bool _bDrop);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;
	_bool					m_IsDrop = false;
	
private:
	const	_float4x4*		m_pParentMatrix = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CAnvil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* _pArg);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END