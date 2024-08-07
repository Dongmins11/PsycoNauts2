#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CLock_2D final : public CGameObject
{
private:
	explicit CLock_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLock_2D(const CLock_2D& rhs);
	virtual ~CLock_2D() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void	Set_TriggerRender(_bool bCheck) { m_IsTriggerRender = bCheck; }
	void	Set_DisolveCheck(_bool bCheck) { m_IsDisolveCheck = bCheck; }
	_bool	Get_DisolveCheck() { return m_IsDisolveCheck; }

private:
	CVIBuffer_Rect*	m_pModelCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;

private:
	_uint			m_iTextureIndex = 0;
	_bool			m_IsCollision = false;
	_bool			m_IsPlayer_Collision = false;
	_bool			m_IsTriggerRender = false;
	_bool			m_IsDisolveCheck = false;
	_float3			m_vInitialPos = _float3(0.f, 0.f, 0.f);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CLock_2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END