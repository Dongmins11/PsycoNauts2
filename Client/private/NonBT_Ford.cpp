#include "stdafx.h"
#include "NonBT_Ford.h"
#include "GameInstance.h"

CNonBT_Ford::CNonBT_Ford(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CNonBT_Ford::CNonBT_Ford(const CNonBT_Ford & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNonBT_Ford::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNonBT_Ford::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4 WorldMatrix;

		WorldMatrix = *(_float4x4*)pArg;

		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	}

	/*m_pTransformCom->Scaling(_float3(0.75f, 0.75f, 0.75f));*/

	m_pModelCom->Set_Animation(17);
	m_pTransformCom->Scaling(_float3(0.5f, 0.5f, 0.5f));
	
	m_pModelCom->Set_AnimSpeed(22, 0.75);
	m_pModelCom->Set_AnimBlendSpeed(22, 1.0);
	return S_OK;
}

_int CNonBT_Ford::Tick(_double TimeDelta)
{
	if (20 <= m_pModelCom->Get_AnimationCurrentKeyFrame() &&
		m_pModelCom->Get_AnimIndex() == 22)
	{
		m_pModelCom->Set_IsAnimLefp(true);
		m_pModelCom->Set_Animation(17);
	}

	m_pModelCom->Lerp_Update(TimeDelta);
	
	return 0;
}

_int CNonBT_Ford::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CNonBT_Ford::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

const _bool CNonBT_Ford::IsIdle() const
{
	if (m_pModelCom->Get_AnimIndex() == 17)
		return true;
	else
		return false;
}

void CNonBT_Ford::Set_EatMotion()
{
	m_pModelCom->Set_IsAnimLefp(true);
	m_pModelCom->Set_Animation(22);
}

HRESULT CNonBT_Ford::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_GoatFord0"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNonBT_Ford::SetUp_ConstantTable()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CNonBT_Ford * CNonBT_Ford::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNonBT_Ford*	pInstance = new CNonBT_Ford(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CNonBT_Ford");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNonBT_Ford::Clone(void * pArg, _uint iCurrentScene)
{
	CNonBT_Ford*	pInstance = new CNonBT_Ford(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CNonBT_Ford");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNonBT_Ford::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
