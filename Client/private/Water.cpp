#include "stdafx.h"
#include "..\public\Water.h"
#include "GameInstance.h"

CWater::CWater(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CWater::CWater(const CWater & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWater::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CWater::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(_float3(2.7f, 2.7f, 2.7f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(283.04f, 13.7f, 202.26f, 1.f));
	return S_OK;
}

_int CWater::Tick(_double TimeDelta)
{
	if (!m_isInteraction)
	{
		m_isInteraction = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			SCENE_STAGE03, TEXT("Layer_Repeat_WaterVaporRect"),
			SCENE_STATIC, TEXT("Effect_Repeat_WaterVaporRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(pGameInstance->Effect_Call_WorldPos(SCENE_STAGE03, TEXT("Layer_Repeat_WaterVaporRect"), XMVectorSet(283.04f, 14.f, 202.26f, 1.f), true)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	m_fTimeAcc += (_float)TimeDelta * 0.01f;

	return _int();
}

_int CWater::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_RECT_EFFECT, this);

	return 0;
}

HRESULT CWater::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_WATER);
	return S_OK;
}

HRESULT CWater::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Water"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	/* Com_Noise */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"), TEXT("Com_Noise"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::SetUp_ConstantTable()
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

	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pAlphaTexture->Get_SRV(6));
	m_pModelCom->Set_TextureValue("g_NoiseMapTexture", m_pNoiseTexture->Get_SRV(4));
	m_pModelCom->Set_TextureValue("g_FadeMapTexture", m_pAlphaTexture->Get_SRV(68));
	m_pModelCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(0.7f, 0.6f, 0.7f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(0.1f, 0.6f, 0.6f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWater * CWater::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWater*	pInstance = new CWater(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CWater");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWater::Clone(void * pArg, _uint iCurrentScene)
{
	CWater*	pInstance = new CWater(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CWater");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWater::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);
}
