#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTexture;
class CEffect_Option;
END

class CCencerCSW final :public CGameObject
{
private:
	explicit CCencerCSW(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCencerCSW(const CCencerCSW& rhs);
	virtual ~CCencerCSW() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pAlphaTexture = nullptr;
	CEffect_Option*			m_pOptionCom = nullptr;

	_float					m_fTimeAcc = 0.0f;

	/* »ó¼Ó */
	_float4x4 m_ParentMat;

private:
	HRESULT Call_Inheritance();

	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CCencerCSW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
