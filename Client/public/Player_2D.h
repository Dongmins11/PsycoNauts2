#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPlayer_2D final : public CGameObject
{
private:
	explicit CPlayer_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer_2D(const CPlayer_2D& rhs);
	virtual ~CPlayer_2D() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void OnTriggerEnter(CCollisionObject& collision)override;

private:
	void Dissolve(_double TimeDelta);
	void Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed);

public:
	_bool	Get_CollisionCheck() { return m_IsCollision; }
	void	Set_TriggerRender(_bool bCheck) { m_IsTriggerRender = bCheck; }

private:
	CVIBuffer_Rect*	m_pModelCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTexture*		m_pDissolveTextureCom = nullptr;
	CBoxCollider*	m_pColliderCom = nullptr;

private:
	_float3			m_vInitialPos = _float3(0.f, 0.f, 0.f);
	_bool			m_IsKeyMove = false;
	_bool			m_IsCollision = false;
	_bool			m_IsCollisionDissolve = false;
	_bool			m_IsTriggerRender = false;
	_double			m_dCollisionDissolveAcc = 0.0;

private:
	/*For.Dissolve*/
	_bool					m_IsChain = false;
	_bool					m_IsLock = false;
	_float					m_fDissolveTime = 0.f;
	_double					m_dDissolveSpeed = 1.4f;
	_bool					m_bIsDissolve = false;
	_bool					m_bBackUpDissolve = false;
	_uint					m_iTextureIndex = 0;
	_float					m_fDissolveStart = 0.1f;
	_float3					m_vBackUpScale;
	_float3					m_vDissolveScale;
	_float					m_fDissolveScaleSpeed = 0.5f;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CPlayer_2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END