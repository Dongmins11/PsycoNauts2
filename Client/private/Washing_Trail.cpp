#include "stdafx.h"
#include "..\public\Washing_Trail.h"
#include "GameInstance.h"

CWashing_Trail::CWashing_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CWashing_Trail::CWashing_Trail(const CWashing_Trail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWashing_Trail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWashing_Trail::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	return S_OK;
}

_int CWashing_Trail::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	

	return 0;
}

_int CWashing_Trail::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_RECT_EFFECT, this);

	return 0;
}

HRESULT CWashing_Trail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBuffer->Render(0);
	return S_OK;
}

HRESULT CWashing_Trail::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	/* Com_Option */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_EffectOption"), TEXT("Com_Option"), (CComponent**)&m_pOptionCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWashing_Trail::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pVIBuffer->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pVIBuffer, 70);

	_float3 vColor = _float3(1.f, 0.5f, 0.f);
	m_pVIBuffer->Set_RawValue("g_vColor", &vColor, sizeof(_float3));

	_float3 vEndColor = _float3(1.f, 0.1f, 0.f);
	m_pVIBuffer->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWashing_Trail * CWashing_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWashing_Trail*	pInstance = new CWashing_Trail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CWashing_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWashing_Trail::Clone(void * pArg, _uint iCurrentScene)
{
	CWashing_Trail*	pInstance = new CWashing_Trail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CWashing_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWashing_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pOptionCom);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pRendererCom);
}
