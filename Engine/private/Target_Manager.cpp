#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

ID3D11ShaderResourceView * CTarget_Manager::Get_SRV(const _tchar * pTargetTag)
{
	CRenderTarget*	pTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pTarget)
		return nullptr;

	return pTarget->Get_SRV();
}

HRESULT CTarget_Manager::NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Rect::Create(pDevice, pDeviceContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

#endif // _DEBUG

	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, CRenderTarget::TEXTURETYPE eTextureType, _uint iMipLevel)
{
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iWidth, iHeight, eFormat, vClearColor, eTextureType, iMipLevel);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pRenderTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar * pMRTTag)
{
	if (nullptr == pDeviceContext)
		return E_FAIL;

	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRenderTargets[8];

	_uint iNumTargets = 0;

	ID3D11DepthStencilView* pDepth = m_pDepthStencilView;

	CRenderTarget::TEXTURETYPE eType = CRenderTarget::TEXTURETYPE_2D;

	for (auto& pTarget : *pMRTList)
	{
		eType = pTarget->Get_TextureType();
		if (CRenderTarget::TEXTURETYPE_DEPTH == eType)
		{
			pDepth = pTarget->Get_DSV();
			if (!lstrcmp(TEXT("MRT_Deferred"), pMRTTag))
				m_pBackUpDepthStencilView = pDepth;
		}
		pTarget->Clear();
		pRenderTargets[iNumTargets++] = pTarget->Get_RTV();
		pDeviceContext->GenerateMips(pTarget->Get_SRV());

	}
	if (!lstrcmp(TEXT("MRT_Effect"), pMRTTag))
	{
		//memcpy_s(pDepth, sizeof(ID3D11DepthStencilView), m_pBackUpDepthStencilView, sizeof(ID3D11DepthStencilView));
		if(nullptr!=m_pBackUpDepthStencilView)
			pDepth = m_pBackUpDepthStencilView;
	}

	
	pDeviceContext->OMSetRenderTargets(iNumTargets, pRenderTargets, pDepth);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT_Arr(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag, _uint iTargetIndex, _uint iRTVIndex)
{
	if (nullptr == pDeviceContext)
		return E_FAIL;

	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRenderTargets;
	ID3D11DepthStencilView*		pDsv = nullptr;

	_uint iNumTargets = 1;


	for (auto& pTarget : *pMRTList)
	{
		if (0 == iRTVIndex)
			pTarget->Clear_Array();
		/*for (_uint i = 0; i < 6; ++i)*/
		pRenderTargets = pTarget->Get_Arr_RTV()[iRTVIndex];

		pDsv = pTarget->Get_ArrDSV()[iRTVIndex];

	}

	pDeviceContext->OMSetRenderTargets(iNumTargets, &pRenderTargets, pDsv);
	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* pDeviceContext)
{
	if (nullptr == pDeviceContext)
		return E_FAIL;
	//if (nullptr != m_pBackUpDepthStencilView)
	//{
	//	
	//	memmove(m_pDepthStencilView, m_pBackUpDepthStencilView, sizeof(ID3D11DepthStencilView));
	//	
	//	m_pBackUpDepthStencilView = nullptr;
	//}
	pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	ID3D11ShaderResourceView*		SRV[8] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, 8, SRV);
	
	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug_Buffer_Desc(const _tchar * pTargetTag, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	if (FAILED(pRenderTarget->Ready_Debug_Desc(fStartX, fStartY, fSizeX, fSizeY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTarget_Manager::Render_Debug_Buffer(const _tchar * pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_float4x4		ViewMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render_Debug_Buffer(m_pVIBuffer);
	}

	return S_OK;
}

#endif // _DEBUG

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pRenderTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pRenderTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	//Safe_Release(m_pDepthStencilView);
	//Safe_Release(m_pBackBufferView);

	for (auto& Pair : m_MRTs)
	{
		for (auto& pTarget : Pair.second)
			Safe_Release(pTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG
}
