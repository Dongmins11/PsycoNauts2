#include "stdafx.h"
#include "..\public\TumbleAttack.h"
#include "GameInstance.h"

CTumbleAttack::CTumbleAttack(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTumbleAttack::CTumbleAttack(const CTumbleAttack & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTumbleAttack::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CTumbleAttack::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;


	return S_OK;
}

_int CTumbleAttack::Tick(_double TimeDelta)
{
	if (2.0f < m_fTimeAcc)
	{
		m_isDestroy = true;
		m_isRender = false;
		return 0;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_Stiffness())
	{
		m_fTimeAcc = 0.f;
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (1.f < m_fTimeAcc && !m_isEffectCreate)
	{
		m_isEffectCreate = true;
		/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueTumbleAttack"),
			SCENE_STATIC, TEXT("Effect_BlueTumbleAttack"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, pTransform, TEXT("Layer_BlueTumbleAttack"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		_vector vPos = pTransform->Get_State(CTransform::STATE_LOOK) * 2.2f + pTransform->Get_State(CTransform::STATE_UP) * 0.02f;
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueTumbleAttack"), TEXT("Com_Option"), vPos);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueTumbleAttack"), TEXT("Com_Option"), 0.8f);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_PointParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_TumbleAttackPoint"),
			SCENE_STATIC, TEXT("Effect_TumbleAttackPoint"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, pTransform, TEXT("Layer_TumbleAttackPoint"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		vPos = pTransform->Get_State(CTransform::STATE_LOOK) * 2.2f + pTransform->Get_State(CTransform::STATE_UP) * 0.5f + pTransform->Get_State(CTransform::STATE_RIGHT) * -1.f;
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_TumbleAttackPoint"), TEXT("Com_Option"), vPos);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_TumbleAttackPoint"), TEXT("Com_Option"), 2.5f);
		
	}


	m_fTimeAcc += static_cast<_float>(TimeDelta) * 10.0f;


	_matrix LocalMat = XMMatrixIdentity();
	LocalMat.r[3] = XMVectorSet(-0.5f, 0.f, 0.f, 1.f);
	_float4x4 WorldMat;
	XMStoreFloat4x4(&WorldMat, LocalMat * XMLoadFloat4x4(&pTransform->Get_WorldFloat4x4()));
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CTumbleAttack::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CTumbleAttack::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(4);
	return S_OK;
}

HRESULT CTumbleAttack::SetUp_Component()
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

HRESULT CTumbleAttack::SetUp_ConstantTable()
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

CTumbleAttack * CTumbleAttack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTumbleAttack*	pInstance = new CTumbleAttack(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CTumbleAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTumbleAttack::Clone(void * pArg, _uint iCurrentScene)
{
	CTumbleAttack*	pInstance = new CTumbleAttack(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CTumbleAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTumbleAttack::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);
}
