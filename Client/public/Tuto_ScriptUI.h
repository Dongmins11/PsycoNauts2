#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CTuto_ScriptUI final : public CGameObject
{
private:
	explicit CTuto_ScriptUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTuto_ScriptUI(const CTuto_ScriptUI& rhs);
	virtual ~CTuto_ScriptUI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void SetUp_ScriptUI(_uint iNumber);
	void SetUp_ScriptUI(_uint iStartNumber,_uint iEndNumber);
private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

	CVIBuffer_Rect*		m_pBackground_BufferCom = nullptr;
	CVIBuffer_Rect*		m_pScript_BufferCom = nullptr;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	HRESULT SetUp_ScriptPresenterArray(const _tchar * pFilePath);
	HRESULT SetUp_ScriptPositionArray(const _tchar * pFilePath);
private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

	_double				m_lfTimeAcc = 0.0;
	_double				m_lfTimeMax = 2.0;

	_int				m_iCurrentPresenter = 0;
	_int				m_iEndPresenter = 0;
	_uint				m_iScriptPresenterArr[30] = { 0 };
	_float4				m_fImagePosition[30] = {};

	_bool				m_isSettingFin = false;
	_bool				m_isLerpScript = false;
private:
	
public:
	static CTuto_ScriptUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END

