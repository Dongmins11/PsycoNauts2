#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CCookingProgressBarUI final : public CGameObject
{
private:
	explicit CCookingProgressBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCookingProgressBarUI(const CCookingProgressBarUI& rhs);
	virtual ~CCookingProgressBarUI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	void Set_IsFinished(_bool isFinished) { m_isFinished = isFinished; }
	void Set_MaxTime(_double TimeAcc) { m_lfTimeMax = TimeAcc; }
	void Set_TimeAcc(_double TimeAcc) { m_lfTimeAcc = TimeAcc; }
public:
	_bool Get_IsFinished() const { return m_isFinished; }
	
private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferBackgroundCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	_bool				m_isFinished = false;
	
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

	_uint				m_iCurrentImgIndex = 0;

	_uint				m_iLoadingTextureIndex = 1;
	_double				m_lfTimeAcc = 0.0;
	_double				m_lfTimeMax = 0.0;
private:
public:
	static CCookingProgressBarUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END