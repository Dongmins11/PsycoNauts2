#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

#define SPINNERSPEED 150.0
#define TEXTSCALE 2.5f
#define MAXFRAME 5.f

BEGIN(Client)
class CLoading_UI final : public CGameObject
{
private:
	explicit CLoading_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLoading_UI(const CLoading_UI& rhs);
	virtual ~CLoading_UI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_IsLoad(_bool bLoad) { m_IsLoad = bLoad; }

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pTextureMixCom = nullptr;

	CVIBuffer_Rect*		m_pModelBGCom = nullptr;
	CVIBuffer_Rect*		m_pModelSpiralCom = nullptr;
	CVIBuffer_Rect*		m_pModelMixCom = nullptr;
	CVIBuffer_Rect*		m_pModelSpinnerCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:
	_bool				m_IsLoad = false;

	_float				m_fTimeAcc = 0.f;	// Shader
	_float				m_fTemp = 0.f;
	_double				m_dTimeAcc = 0.f;	// Spinner
	_double				m_dTemp = 0.0;

	_float				m_fX, m_fY, m_fSpinnerSize;
	_float4x4			m_ProjMatrix;

public:
	static CLoading_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END

