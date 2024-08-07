#pragma once

#include "Base.h"
#include "RenderTarget.h"
BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)

private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(const _tchar* pTargetTag);

public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, CRenderTarget::TEXTURETYPE eTextureType = CRenderTarget::TEXTURETYPE::TEXTURETYPE_2D, _uint iMipLevel = 1);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag);
	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT Begin_MRT_Arr(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag, _uint iTargetIndex, _uint iRTVIndex);
	HRESULT End_MRT(ID3D11DeviceContext* pDeviceContext);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Buffer_Desc(const _tchar* pTargetTag, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer(const _tchar* pMRTTag);
	_bool	Get_IsRender() const { return m_isRender; }
	void	Set_IsRender(_bool isRender) { m_isRender = isRender; }
#endif // _DEBUG

private:
	map<const _tchar*, class CRenderTarget*>				m_RenderTargets;
	typedef map<const _tchar*, class CRenderTarget*>		RENDERTARGETS;

	ID3D11RenderTargetView*				m_pBackBufferView = nullptr;
	ID3D11DepthStencilView*				m_pDepthStencilView = nullptr;
	ID3D11DepthStencilView*				m_pBackUpDepthStencilView = nullptr;

private:
	map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const _tchar*, list<class CRenderTarget*>>	MRTS;

#ifdef _DEBUG
private:
	class CVIBuffer_Rect*					m_pVIBuffer = nullptr;
	_bool									m_isRender = true;
	_float4x4								m_ProjMatrix;
#endif // _DEBUG


private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pRenderTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);



public:
	virtual void Free() override;
};

END