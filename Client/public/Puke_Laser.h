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
class CPuke_Laser final :public CGameObject
{
private:
	CPuke_Laser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPuke_Laser(const CPuke_Laser& rhs);
	virtual ~CPuke_Laser() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

private:
	void Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	const _float4x4*		m_pPivotMatrix = nullptr;

private:
	_float					m_fDissolveTime = 0.f;
	_double					m_dDissolveSpeed = 1.4f;
	_bool					m_bIsDissolve = false;
	_bool					m_bBackUpDissolve = false;
	_float					m_fDissolveStart = 0.1f;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CPuke_Laser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END