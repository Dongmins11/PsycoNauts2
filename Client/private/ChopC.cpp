#include "stdafx.h"
#include "..\public\ChopC.h"
#include "GameInstance.h"

CChopC::CChopC(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CChopC::CChopC(const CChopC & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChopC::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CChopC::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	return S_OK;
}

_int CChopC::Tick(_double TimeDelta)
{
	if (2.f < m_fTimeAcc)
	{
		m_isDestroy = true;
		m_isRender = false;
		return 0;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));

	if (1.0f < m_fTimeAcc && !m_isParticleCreate)
	{
		m_isParticleCreate = true;

		if (FAILED(Lading_Effect(pTransform)))
			return -1;
	}


	if (1.3f < m_fTimeAcc && !m_isCreate)
	{
		m_isCreate = true;

		if (FAILED(Impact_Effect(pTransform)))
			return -1;
	}


	m_pTransformCom->Set_WorldMatrix(pTransform->Get_WorldFloat4x4());
	RELEASE_INSTANCE(CGameInstance);

	m_fTimeAcc += static_cast<_float>(TimeDelta) * 5.0f;
	return _int();
}

_int CChopC::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CChopC::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(4);
	return S_OK;
}

HRESULT CChopC::Lading_Effect(CTransform* pTransform)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//세번째 공격//
	/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCRending2"),
		SCENE_STATIC, TEXT("Effect_ChopCRending2"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, pTransform, TEXT("Layer_ChopCRending2"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCRending2"), TEXT("Com_Option"), 0.8f);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CChopC::Impact_Effect(CTransform* pTransform)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//세번째 공격//
		/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCImpact"),
		SCENE_STATIC, TEXT("Effect_ChopCImpact"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, pTransform, TEXT("Layer_ChopCImpact"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCImpact"), TEXT("Com_Option"), 2.f);
	_float3 vPos = _float3(0.f, 0.7f, 0.f);
	XMStoreFloat3(&vPos, XMLoadFloat3(&vPos) + pTransform->Get_State(CTransform::STATE_LOOK) * 2.f - pTransform->Get_State(CTransform::STATE_RIGHT) * 0.5f);
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCImpact"), TEXT("Com_Option"), 0.8f);
	pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCImpact"), TEXT("Com_Option"), XMLoadFloat3(&vPos));

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCSmoke"),
		SCENE_STATIC, TEXT("Effect_Smoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	
	/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, pTransform, TEXT("Layer_ChopCSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_ChopCSmoke"), TEXT("Com_Option"), 0.5f);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CChopC::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_ChopC"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	/* Com_Noise */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"), TEXT("Com_Noise"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;


	return S_OK;
}

HRESULT CChopC::SetUp_ConstantTable()
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

	_float3 vColor = _float3(0.8f, 0.25f, 0.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CChopC * CChopC::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CChopC*	pInstance = new CChopC(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CChopC");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChopC::Clone(void * pArg, _uint iCurrentScene)
{
	CChopC*	pInstance = new CChopC(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CChopC");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChopC::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);
}
