#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{

public:
	enum TEXTURETYPE { TEXTURETYPE_2D, TEXTURETYPE_CUBE, TEXTURETYPE_DEPTH, TEXTURETYPE_END };
public:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() {
		return m_pRTV;
	}

	ID3D11ShaderResourceView* Get_SRV() {
		return m_pSRV;
	}
	ID3D11RenderTargetView** Get_Arr_RTV() {
		return m_ArrRTV;
	}
	TEXTURETYPE	Get_TextureType() {
		return m_eTextureType;
	}
	ID3D11DepthStencilView*	Get_DSV() {
		return m_pDSV;
	}
	ID3D11DepthStencilView**	Get_ArrDSV() {
		return m_ArrDSV;
	}
public:
	HRESULT NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, TEXTURETYPE eTextureType = TEXTURETYPE_2D, _uint iMipLevel = 1);
	HRESULT Clear();
	HRESULT Clear_Array();


#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Desc(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer(class CVIBuffer* pVIBuffer);
#endif // _DEBUG

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

private:
	ID3D11Texture2D*			m_pTexture = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
	ID3D11RenderTargetView*		m_ArrRTV[6];
	ID3D11DepthStencilView*		m_ArrDSV[6];
	_float4						m_vClearColor;

	ID3D11Texture2D*			m_pTexture_Dep = nullptr;
	ID3D11DepthStencilView*		m_pDSV = nullptr;
	TEXTURETYPE					m_eTextureType = TEXTURETYPE_END;
	D3D11_VIEWPORT				m_ViewPort;
#ifdef _DEBUG
private:
	_float4x4					m_TransformMatrix;
#endif // _DEBUG


public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, TEXTURETYPE eTextureType = TEXTURETYPE_2D, _uint iMipLevel = 1);
	virtual void Free() override;
};

END