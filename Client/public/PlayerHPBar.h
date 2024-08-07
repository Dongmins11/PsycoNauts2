#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPlayerHPBar final : public CGameObject
{
public:
	explicit CPlayerHPBar(ID3D11Device* pDevice, ID3D11DeviceContext *pDeviceContext);
	explicit CPlayerHPBar(const CPlayerHPBar& rhs);
	virtual ~CPlayerHPBar() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	void Set_HpIndex(_uint iIndex) { m_iHpDefaultIndex = iIndex; }
	void Set_HpBackUpIndex(_uint iIndex) { m_iHpBackUpIndex = iIndex; }
	void Set_IsDecrease(_bool IsDecrease) { m_IsDecrease = IsDecrease; }
	void Set_IsIncrease(_bool IsIncrease) { m_IsIncrease = IsIncrease; }
	void Set_IsDefault(_bool IsDefault) { m_IsDefault = IsDefault; }
	void Set_IsRenderCheck(_bool bCheck) { m_IsRenderCheck = bCheck; }

private:
	class CRenderer*		m_pRendererCom = nullptr;
	class CTexture*			m_pTextureCom = nullptr;
	class CVIBuffer_Rect*	m_pVIBufferBGCom = nullptr;
	class CVIBuffer_Rect*	m_pVIBufferDefaultCom = nullptr;
	class CVIBuffer_Rect*	m_pVIBufferBackUpCom = nullptr;

private:
	_bool				m_IsPhaseNum3 = false;
	_bool				m_IsScene3 = false;
	_bool				m_IsScene5 = false;
	_bool				m_IsRenderCheck = true;		// Render?
	_bool				m_IsDecrease = false;		// Yellow
	_bool				m_IsIncrease = false;		// Green
	_bool				m_IsDefault = false;		// Pink

	_uint				m_iHpDefaultIndex = 4;		// Pink
	_uint				m_iHpBackUpIndex = 4;		// 잔상
	_int				m_iLayerIndex = 0;			// 위치설정을 위한 LayerIndex

	_float				m_fTimeBackUpAcc = 0.f;		// Pink->Yellow or Pink->Green
	_float				m_fTimeDefaultAcc = 0.f;	// Green->Pink
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;
	_tchar*				m_szBuffer[3] = {};

public:
	static CPlayerHPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END
