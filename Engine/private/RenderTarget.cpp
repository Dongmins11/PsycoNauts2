#include "..\Public\RenderTarget.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderTarget::NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, TEXTURETYPE eTextureType, _uint iMipLevel)
{
	if (nullptr == m_pDevice)
		return E_FAIL;
	for (_uint i = 0; i < 6; ++i)
	{
		m_ArrRTV[i] = nullptr;
		m_ArrDSV[i] = nullptr;
	}

	m_vClearColor = vClearColor;
	m_eTextureType = eTextureType;

	if (TEXTURETYPE_2D == m_eTextureType || TEXTURETYPE_DEPTH ==m_eTextureType)
	{
		D3D11_TEXTURE2D_DESC			TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		TextureDesc.Width = iWidth;
		TextureDesc.Height = iHeight;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = eFormat;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture)))
			return E_FAIL;

		D3D11_RENDER_TARGET_VIEW_DESC			TargetViewDesc;
		ZeroMemory(&TargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		TargetViewDesc.Format = eFormat;
		TargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		TargetViewDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, &TargetViewDesc, &m_pRTV)))
			return E_FAIL;

		D3D11_SHADER_RESOURCE_VIEW_DESC			ShaderResourceViewDesc;
		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		ShaderResourceViewDesc.Format = eFormat;
		ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceViewDesc.Texture2D.MipLevels = 1;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &ShaderResourceViewDesc, &m_pSRV)))
			return E_FAIL;
	}
	else if (TEXTURETYPE_CUBE == eTextureType)
	{
		D3D11_TEXTURE2D_DESC			TextureDesc;
		//ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = 512;
		TextureDesc.Height = 512;
		TextureDesc.MipLevels = iMipLevel;
		TextureDesc.ArraySize = 6;
		TextureDesc.Format = eFormat;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
		TextureDesc.CPUAccessFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, NULL, &m_pTexture)))
			return E_FAIL;

		D3D11_RENDER_TARGET_VIEW_DESC TargetViewDesc;
		ZeroMemory(&TargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		TargetViewDesc.Format = eFormat;
		TargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

		TargetViewDesc.Texture2DArray.ArraySize = 1;
		TargetViewDesc.Texture2DArray.MipSlice = 0;

		for (int i = 0; i < 6; i++)
		{
			TargetViewDesc.Texture2DArray.FirstArraySlice = i;
			if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, &TargetViewDesc, &m_ArrRTV[i])))
				return E_FAIL;
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = TextureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = -1;
		if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pSRV)))
			return E_FAIL;


		D3D11_TEXTURE2D_DESC	tDsvTex;
		ZeroMemory(&tDsvTex, sizeof(D3D11_TEXTURE2D_DESC));

		tDsvTex.Width = 512;
		tDsvTex.Height = 512;
		tDsvTex.MipLevels = iMipLevel;
		tDsvTex.ArraySize = 6;
		tDsvTex.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tDsvTex.SampleDesc.Quality = 0;
		tDsvTex.SampleDesc.Count = 1;
		tDsvTex.Usage = D3D11_USAGE_DEFAULT;
		tDsvTex.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tDsvTex.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		tDsvTex.CPUAccessFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&tDsvTex, NULL, &m_pTexture_Dep)))
			return E_FAIL;

		D3D11_DEPTH_STENCIL_VIEW_DESC	tDsvDesc;
		ZeroMemory(&tDsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

		tDsvDesc.Format = tDsvTex.Format;
		tDsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		tDsvDesc.Texture2DArray.FirstArraySlice = 0;
		tDsvDesc.Texture2DArray.ArraySize = 1;
		tDsvDesc.Texture2DArray.MipSlice = 0;

		for (int i = 0; i < 6; i++)
		{
			tDsvDesc.Texture2DArray.FirstArraySlice = i;
			if (FAILED(m_pDevice->CreateDepthStencilView(m_pTexture_Dep, &tDsvDesc, &m_ArrDSV[i])))
				return E_FAIL;
		}


	}
	if (TEXTURETYPE_DEPTH == eTextureType)
	{
		D3D11_TEXTURE2D_DESC	TextureDesc_Depth;
		ZeroMemory(&TextureDesc_Depth, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc_Depth.Width = iWidth;
		TextureDesc_Depth.Height = iHeight;
		TextureDesc_Depth.MipLevels = 1;
		TextureDesc_Depth.ArraySize = 1;
		TextureDesc_Depth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		TextureDesc_Depth.SampleDesc.Quality = 0;
		TextureDesc_Depth.SampleDesc.Count = 1;

		TextureDesc_Depth.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc_Depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		TextureDesc_Depth.CPUAccessFlags = 0;
		TextureDesc_Depth.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc_Depth, nullptr, &m_pTexture_Dep)))
			return E_FAIL;

		/* RenderTarget */
		/* ShaderResource */
		/* DepthStencil */

		if (FAILED(m_pDevice->CreateDepthStencilView(m_pTexture_Dep, nullptr, &m_pDSV)))
			return E_FAIL;

		//D3D11_SHADER_RESOURCE_VIEW_DESC			ShaderResourceViewDesc_Depth;
		//ZeroMemory(&ShaderResourceViewDesc_Depth, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		//ShaderResourceViewDesc_Depth.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		//ShaderResourceViewDesc_Depth.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//ShaderResourceViewDesc_Depth.Texture2D.MipLevels = 1;
		//ShaderResourceViewDesc_Depth.Texture2D.MostDetailedMip = 0;

		//if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture_DSV, &ShaderResourceViewDesc_Depth, &m_pSRV_Detph)))
		//	return E_FAIL;
	}
	m_ViewPort.Width = iWidth;
	m_ViewPort.Height = iHeight;
	m_ViewPort.TopLeftX = 0.f;
	m_ViewPort.TopLeftY = 0.f;
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;
	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	//if(m_vClearColor.x == 1.f)
	//{
	//	if (FAILED(SaveWICTextureToFile(m_pDeviceContext, m_pTexture, GUID_ContainerFormatPng, TEXT("../Bin/Diffuse.Png"))))
	//		return E_FAIL;
	//}
	//else
	//{
	//	if (FAILED(SaveWICTextureToFile(m_pDeviceContext, m_pTexture, GUID_ContainerFormatPng, TEXT("../Bin/Normal.Png"))))
	//		return E_FAIL;
	//}


	m_pDeviceContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
	//if (TEXTURETYPE_CUBE == m_eTextureType)
	//{
	//	m_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH, 1.f, 0.f);
	//	//m_pDeviceContext->RSSetViewports(1,&m_ViewPort);
	//}	else
	if (TEXTURETYPE_DEPTH == m_eTextureType)
		m_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pDeviceContext->RSSetViewports(1, &m_ViewPort);

	return S_OK;
}

HRESULT CRenderTarget::Clear_Array()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	for (_uint i = 0; i < 6; ++i)
	{
		m_pDeviceContext->ClearRenderTargetView(m_ArrRTV[i], (_float*)&m_vClearColor);
		m_pDeviceContext->ClearDepthStencilView(m_ArrDSV[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug_Desc(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	m_TransformMatrix._11 = fSizeX;
	m_TransformMatrix._22 = fSizeY;

	m_TransformMatrix._41 = fStartX + fSizeX * 0.5f - (Viewport.Width * 0.5f);
	m_TransformMatrix._42 = -(fStartY + fSizeY * 0.5f) + (Viewport.Height * 0.5f);

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)));

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug_Buffer(CVIBuffer * pVIBuffer)
{
	if (nullptr == pVIBuffer)
		return E_FAIL;

	pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4));
	pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pSRV);

	pVIBuffer->Render(0);

	return S_OK;
}

#endif // _DEBUG

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, TEXTURETYPE eTextureType, _uint iMipLevel)
{
	CRenderTarget*	pInstance = new CRenderTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(iWidth, iHeight, eFormat, vClearColor, eTextureType, iMipLevel)))
	{
		MSGBOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);
	Safe_Release(m_pTexture);


	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	for (_uint i = 0; i < 6; ++i)
	{
		if (TEXTURETYPE_CUBE ==m_eTextureType)
		{
			Safe_Release(m_ArrRTV[i]);
			Safe_Release(m_ArrDSV[i]);
		}
	}
	

	Safe_Release(m_pDSV);
	Safe_Release(m_pTexture_Dep);
}
