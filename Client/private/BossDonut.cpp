#include "stdafx.h"
#include "..\public\BossDonut.h"
#include "GameInstance.h"
#include "Boss_Goat.h"

CBossDonut::CBossDonut(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBossDonut::CBossDonut(const CBossDonut & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossDonut::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CBossDonut::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_pOptionCom->Set_ParentTransform(false, true, (CTransform*)pArg);
	}

	return S_OK;
}

_int CBossDonut::Tick(_double TimeDelta)
{
	//if (0.7f <= m_fTimeAcc)
	//{
	//	m_isDestroy = true;
	//	m_isRender = false;
	//	return 0;
	//}

	m_fTimeAcc += (_float)TimeDelta * 8.f;

	//if (0.35f <= m_fTimeAcc)
	//{
	//	m_isFadeOn = true;
	//	m_fFadeSet -= static_cast<_float>(TimeDelta) * 0.5f;
	//}

	if (FAILED(Call_Inheritance()))
		return -1;

	if (!m_isInteraction)
	{
		m_isInteraction = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RepeatParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Effect_Env_Boss_Particle"),
			SCENE_STATIC, TEXT("Effect_Env_Boss_Particle"), &m_pEffect)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		Safe_AddRef(m_pEffect);

		if(FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true ,m_pTransformCom, TEXT("Layer_Effect_Env_Boss_Particle"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	return _int();
}

_int CBossDonut::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_isDestroy)
	{
		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
	}

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CBossDonut::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_isDestroy)
	{
		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
	}

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_DONUTRING);
	return S_OK;
}

HRESULT CBossDonut::Call_Inheritance()
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_pOptionCom)
		return E_FAIL;
	/* 可记 矫累 */

	//LocalMatrix.r[3] -= m_pOptionCom->Get_ParentMatrix().r[2] * 0.03;
	/* 可记 场 */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float4x4 WorldMatrix = dynamic_cast<CBoss_Goat*>(pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_FinalBoss"), 0))->Get_BeaterMatrix();

	WorldMatrix._11 *= 2.5f;
	WorldMatrix._12 *= 2.5f;
	WorldMatrix._13 *= 2.5f;

	WorldMatrix._21 *= 10.f;
	WorldMatrix._22 *= 10.f;
	WorldMatrix._23 *= 10.f;

	WorldMatrix._31 *= 2.5f;
	WorldMatrix._32 *= 2.5f;
	WorldMatrix._33 *= 2.5f;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBossDonut::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_DonutRing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Option */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_EffectOption"), TEXT("Com_Option"), (CComponent**)&m_pOptionCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossDonut::SetUp_ConstantTable()
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

	_float fTimeAcc = -m_fTimeAcc;
	m_pModelCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));

	m_pModelCom->Set_RawValue("g_isFadeIn", &m_isFadeOn, sizeof(_bool));

	_float fDonutY = 1.f;
	m_pModelCom->Set_RawValue("g_fDonutY", &fDonutY, sizeof(_float));

	m_pModelCom->Set_RawValue("g_fFadeSet", &m_fFadeSet, sizeof(_float));

	_float fAlphaDown = 0.6f;
	m_pModelCom->Set_RawValue("g_fAlphaDown", &fAlphaDown, sizeof(_float));

	_float3 vColor = _float3(1.f, 0.3f, 1.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));

	_float3	vEndColor = _float3(0.5f, 0.1f, 0.5f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBossDonut * CBossDonut::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossDonut*	pInstance = new CBossDonut(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBossDonut");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossDonut::Clone(void * pArg, _uint iCurrentScene)
{
	CBossDonut*	pInstance = new CBossDonut(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBossDonut");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossDonut::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pOptionCom);

	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}
