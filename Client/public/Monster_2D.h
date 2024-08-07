#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CMonster_2D final : public CGameObject
{
private:
	explicit CMonster_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonster_2D(const CMonster_2D& rhs);
	virtual ~CMonster_2D() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	_int Update_Monster_Move(_double TimeDelta);

public:
	void Set_TriggerRender(_bool bCheck) { m_IsTriggerRender = bCheck; }

private:
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CBoxCollider*	m_pColliderCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CVIBuffer_Rect*	m_pModelCom = nullptr;

private:
	_bool			m_IsTriggerRender = false;
	_bool			m_IsLerp = false;
	_bool			m_IsLerpUpDown = false;
	_float3			m_vInitialPos = _float3(0.f, 0.f, 0.f);
	_vector			m_vSrcPos = XMVectorZero();
	_vector			m_vDstPos = XMVectorZero();
	_double			m_dTimeAcc = 0.0;
	_uint			m_iLayerIndex = 0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CMonster_2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END