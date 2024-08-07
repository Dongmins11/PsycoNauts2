#include "..\Public\Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Light.h"
#include "Target_Manager.h"
#include "PipeLine.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::NativeConstruct(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	m_pVIBuffer = CVIBuffer_Rect::Create(pDevice, pDeviceContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

	m_TransformMatrix._11 = Viewport.Width;
	m_TransformMatrix._22 = Viewport.Height;

	m_TransformMatrix._41 = 0.0f;
	m_TransformMatrix._42 = 0.0f;

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)));

	lstrcpy(m_szDiffuse, TEXT("Target_Diffuse"));

	return S_OK;
}

HRESULT CLight_Manager::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render()
{
	CTarget_Manager*	pTarget_Manager = GET_INSTANCE(CTarget_Manager);

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", pTarget_Manager->Get_SRV(m_szDiffuse))))
		return E_FAIL;
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_RomTexture", pTarget_Manager->Get_SRV(TEXT("Target_Rom")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthShadowTexture", pTarget_Manager->Get_SRV(TEXT("Target_DepthShadow")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_EmissiveTexture", pTarget_Manager->Get_SRV(TEXT("Target_Emissive")))))
			return E_FAIL;

	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal_Origin")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_Depth_Origin")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_RomTexture", pTarget_Manager->Get_SRV(TEXT("Target_Rom_Origin")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthShadowTexture", pTarget_Manager->Get_SRV(TEXT("Target_DepthShadow_Origin")))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_EmissiveTexture", pTarget_Manager->Get_SRV(TEXT("Target_Emissive_Origin")))))
			return E_FAIL;
		//if (FAILED(m_pVIBuffer->Set_TextureValue("g_ShadowMapTexture", pTarget_Manager->Get_SRV(TEXT("Target_ShadowMap_Origin")))))
		//	return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_ShadowMapTexture", pTarget_Manager->Get_SRV(TEXT("Target_ShadowMap")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthBlurTexture", pTarget_Manager->Get_SRV(TEXT("Target_DepthBlur")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DOFTexture", pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_SSAOTexture", pTarget_Manager->Get_SRV(TEXT("Target_UpSample_4")))))
		return E_FAIL;
	//if (FAILED(m_pVIBuffer->Set_TextureValue("g_PreFilterTexture", pTarget_Manager->Get_SRV(TEXT("Target_PreFilter")))))
	//	return E_FAIL;
	//if (FAILED(m_pVIBuffer->Set_TextureValue("g_IrradianceTexture", pTarget_Manager->Get_SRV(TEXT("Target_Irradiance")))))
	//	return E_FAIL;
	//if (FAILED(m_pVIBuffer->Set_TextureValue("g_BRDF_LUTTexture", pTarget_Manager->Get_SRV(TEXT("Target_BRDF_LUT")))))
	//	return E_FAIL;
	//if (FAILED(m_pVIBuffer->Set_TextureValue("g_TestTexture", pTarget_Manager->Get_SRV(TEXT("Target_Test")))))
	//	return E_FAIL;
	//if (FAILED(m_pVIBuffer->Set_TextureValue("g_ShadowTexture", pTarget_Manager->Get_SRV(TEXT("Target_Shadow")))))
	//	return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4	ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4	ViewMatrixInverse, ProjMatrixInverse;
	_float4		vCamPosition;

	XMStoreFloat4x4(&ViewMatrixInverse, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::D3DTS_VIEW))));
	m_pVIBuffer->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInverse, sizeof(_float4x4));

	XMStoreFloat4x4(&ProjMatrixInverse, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::D3DTS_PROJ))));
	m_pVIBuffer->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInverse, sizeof(_float4x4));

	XMStoreFloat4(&vCamPosition, pPipeLine->Get_CamPosition());
	m_pVIBuffer->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	if (FAILED(m_pVIBuffer->Set_RawValue("g_bEmissive", &m_bEmissive, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_bShadow", &m_bShadow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_bAO", &m_bAO, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fShadeAmbient", &m_fShadeAmbient, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fShadowBias", &m_fShadowBias, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fPercentLimitMax", &m_fPercentLimitMax, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fOffset", &m_fLightOffset, sizeof(_float))))
		return E_FAIL;

	_uint LightCount = 0;
	for (auto& pLight : m_Lights)
	{
		if (true == m_bIsCurling)
		{
			_uint Index = pLight->Get_CurIndex();
			if (nullptr != pLight && 0 != LightCount &&m_iLightCurling == Index || m_iLightCurling + 1 == Index)
				pLight->Render(m_pVIBuffer);
			if (nullptr != pLight && 0 == LightCount)
				pLight->Render(m_pVIBuffer);

			++LightCount;
		}
		else
			pLight->Render(m_pVIBuffer);
	}

	RELEASE_INSTANCE(CPipeLine);

	RELEASE_INSTANCE(CTarget_Manager);

	return S_OK;
}

void CLight_Manager::Set_LightDesc(_uint iIndex, LIGHTDESC & tLightDesc)
{
	if (0 > iIndex || m_Lights.size() <= (size_t)iIndex)
		return;

	auto iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		iter++;

	(*iter)->Set_LightDesc(tLightDesc);
}

void CLight_Manager::Set_LightRender(_bool bCheck, _uint iIndex)
{
	if (m_Lights.size() <= (size_t)iIndex)
		return;

	auto iter = m_Lights.begin();
	
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	(*iter)->Set_LightRender(bCheck);
}

void CLight_Manager::Set_CurIndex(_uint LayerIndex, _uint curindex)
{
	auto iter = m_Lights.begin();

	for (_uint i = 0; i < LayerIndex; ++i)
		++iter;

	(*iter)->Set_CurIndex(curindex);
}

_uint CLight_Manager::Get_Lights_Size()
{
	if (m_Lights.empty())
		return 0;

	return (_uint)m_Lights.size();
}

void CLight_Manager::Lights_Clear()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
	m_iLightCurling = 0;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	Safe_Release(m_pVIBuffer);


}
