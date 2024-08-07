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
class CKey_2D final : public CGameObject
{
private:
	explicit CKey_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CKey_2D(const CKey_2D& rhs);
	virtual ~CKey_2D() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void OnTriggerEnter(CCollisionObject& collision)override;

public:
	void	Set_IsPlayer_Collision(_bool bCheck) { m_IsPlayer_Collision = bCheck; }
	void	Set_TriggerRender(_bool bCheck) { m_IsTriggerRender = bCheck; }
	_bool	Get_Clear() { return m_IsClear; }


private:
	CVIBuffer_Rect*	m_pModelCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CBoxCollider*	m_pColliderCom = nullptr;

private:
	_bool			m_IsCollision = false;
	_bool			m_IsPlayer_Collision = false;
	_bool			m_IsTriggerRender = false;
	_bool			m_IsClear = false;
	_uint			m_iLayerIndex = 0;
	_float3			m_vInitialPos = _float3(0.f, 0.f, 0.f);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CKey_2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END