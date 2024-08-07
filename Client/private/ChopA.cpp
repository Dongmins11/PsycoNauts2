#include "stdafx.h"
#include "..\public\ChopA.h"
#include "GameInstance.h"

CChopA::CChopA(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CChopA::CChopA(const CChopA & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChopA::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CChopA::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;
	
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	
	return S_OK;
}

_int CChopA::Tick(_double TimeDelta)
{
	if (2.0f < m_fTimeAcc)
	{
		m_isDestroy = true;
		m_isRender = false;
		return 0;
	}

	m_fTimeAcc += static_cast<_float>(TimeDelta) * 15.0f;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_LeftHand"), TEXT("Com_Transform"));

	_matrix LocalMat = XMMatrixIdentity();
	LocalMat.r[3] = XMVectorSet(-0.5f, 0.f, 0.f, 1.f);
	_float4x4 WorldMat;
	XMStoreFloat4x4(&WorldMat, LocalMat * XMLoadFloat4x4(&pTransform->Get_WorldFloat4x4()));
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CChopA::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CChopA::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(4);
	return S_OK;
}

HRESULT CChopA::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_ChopA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	/* Com_Noise */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"), TEXT("Com_Noise"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChopA::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pModelCom->Set_TextureValue("g_NoiseMapTexture", m_pNoiseTexture->Get_SRV(4));
	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pAlphaTexture->Get_SRV(5));
	m_pModelCom->Set_TextureValue("g_FadeMapTexture", m_pAlphaTexture->Get_SRV(68));
	m_pModelCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	//_float3 vColor = _float3(1.f, 0.046665f, 0.f);
	_float3 vColor = _float3(0.8f, 0.25f, 0.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CChopA * CChopA::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CChopA*	pInstance = new CChopA(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CChopA");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChopA::Clone(void * pArg, _uint iCurrentScene)
{
	CChopA*	pInstance = new CChopA(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CChopA");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChopA::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);
}
