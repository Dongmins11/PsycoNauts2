#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CLock_Object final : public CGameObject
{
private:
	explicit CLock_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLock_Object(const CLock_Object& rhs);
	virtual ~CLock_Object() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void Dissolve(_double TimeDelta);
	void Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed);

public:
	_bool Get_DissolveStart() { return m_IsDisolveStart; }
	void Set_DissolveStart(_bool bCheck) { m_IsDisolveStart = bCheck; }
	void Set_EmissiveEnd(_bool bCheck) { m_IsEmissiveEnd = bCheck; }

private:
	CModel*			m_pModelCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;

private:
	_bool		m_IsEmissiveEnd = false;
	_bool		m_IsDisolveStart = false;
	_uint		m_iRenderPassArr[2] = { 2, 0 };
	_bool		m_isEmissiveActive[2] = { false, true };

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
	static CLock_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END