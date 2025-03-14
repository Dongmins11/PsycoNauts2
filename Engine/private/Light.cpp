#include "..\Public\Light.h"
#include "VIBuffer_Rect.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight::NativeConstruct(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect * pVIBuffer)
{
	if (nullptr == pVIBuffer)
		return E_FAIL;

	if (false == m_bIsRender)
		return S_OK;

	_uint			iPassIndex = 4;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.iType)
	{
		iPassIndex = 4;

		if (FAILED(pVIBuffer->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else if (LIGHTDESC::TYPE_POINT == m_LightDesc.iType)
	{
		iPassIndex = 3;

		if (FAILED(pVIBuffer->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pVIBuffer->Set_RawValue("g_fRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pVIBuffer->Set_RawValue("g_fPower", &m_LightDesc.fPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pVIBuffer->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

	}
	else if (LIGHTDESC::TYPE_SPOT == m_LightDesc.iType)
	{
		iPassIndex = 5;

		if (FAILED(pVIBuffer->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pVIBuffer->Set_RawValue("g_fRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pVIBuffer->Set_RawValue("g_fOuterConeAngle", &m_LightDesc.fOuterConeAngle, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pVIBuffer->Set_RawValue("g_fInnerConeAngle", &m_LightDesc.fInnerConeAngle, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pVIBuffer->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}


	if (FAILED(pVIBuffer->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pVIBuffer->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pVIBuffer->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pVIBuffer->Render(iPassIndex);

	return S_OK;
}


CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(LightDesc)))
	{
		MSGBOX("Failed to Created CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
