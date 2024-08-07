#include "stdafx.h"
#include "..\public\PurseRing.h"
#include "GameInstance.h"

CPurseRing::CPurseRing(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPurseRing::CPurseRing(const CPurseRing & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPurseRing::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CPurseRing::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_vector vPos = *(_vector*)pArg;

	vPos =  XMVectorSetW(vPos, 1.f);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.3f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

_int CPurseRing::Tick(_double TimeDelta)
{
	m_pTransformCom->RotationPerSec(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
	m_TimeAcc += TimeDelta * 2.26;

	return _int();
}

_int CPurseRing::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CPurseRing::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_PURSERING);
	return S_OK;
}

HRESULT CPurseRing::SetUp_Component()
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
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_PulseRing"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPurseRing::SetUp_ConstantTable()
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

	_float fTimeAcc = static_cast<_float>(m_TimeAcc);
	m_pModelCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));

	_float fFadeSet = 1.7f;
	m_pModelCom->Set_RawValue("g_fFadeSet", &fFadeSet, sizeof(_float));

	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pAlphaTexture->Get_SRV(1));

	_float3 vColor = _float3(1.f, 0.f, 0.0f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));

	_float3	vEndColor = _float3(0.5f, 0.f, 0.0f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CPurseRing * CPurseRing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPurseRing*	pInstance = new CPurseRing(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPurseRing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPurseRing::Clone(void * pArg, _uint iCurrentScene)
{
	CPurseRing*	pInstance = new CPurseRing(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CPurseRing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPurseRing::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAlphaTexture);
}
