#include "stdafx.h"
#include "..\public\CencerDonut.h"
#include "GameInstance.h"

CCencerDonut::CCencerDonut(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CCencerDonut::CCencerDonut(const CCencerDonut & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCencerDonut::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CCencerDonut::NativeConstruct(void * pArg, _uint iCurrentScene)
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

_int CCencerDonut::Tick(_double TimeDelta)
{
	if (14.f <= m_fTimeAcc)
	{
		m_isDestroy = true;
		m_isRender = false;
		return 0;
	}

	m_fTimeAcc += static_cast<_float>(TimeDelta) * 5.f;

	if (11.5f <= m_fTimeAcc)
	{
		m_isFadeOn = true;
		m_fFadeSet -= static_cast<_float>(TimeDelta) * 5.f;
	}

	if (FAILED(Call_Inheritance()))
		return -1;

	return _int();
}

_int CCencerDonut::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CCencerDonut::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_DONUTRING);
	return S_OK;
}

HRESULT CCencerDonut::Call_Inheritance()
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_pOptionCom)
		return E_FAIL;
	/* 可记 矫累 */
	_vector vScale = XMVectorSet(1.3f + m_fTimeAcc * 0.04f, 4.3f + m_fTimeAcc * 0.04f, 1.3f + m_fTimeAcc * 0.04f, 0.f);
	_matrix LocalMatrix = XMMatrixIdentity();

	_vector vRight = LocalMatrix.r[0];
	_vector vUp = LocalMatrix.r[1];
	_vector vLook = LocalMatrix.r[2];

	vRight = XMVector3Normalize(vRight) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(vUp) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	LocalMatrix.r[0] = vRight;
	LocalMatrix.r[1] = vUp;
	LocalMatrix.r[2] = vLook;
	LocalMatrix.r[3] = XMVectorSetY(LocalMatrix.r[3], 1.0f);
	/* 可记 场 */
	_float4x4 WorldMatrix;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!m_isOnce)
	{
		if (m_pOptionCom->Get_NonBillboard())
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentMatrix());
		else
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentBillboardMatrix(pGameInstance->Get_CamPosition()));
	}

	if (m_pOptionCom->Get_Once())
		m_isOnce = true;

	RELEASE_INSTANCE(CGameInstance);
	XMStoreFloat4x4(&WorldMatrix, LocalMatrix * XMLoadFloat4x4(&m_ParentMat));
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

HRESULT CCencerDonut::SetUp_Component()
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

HRESULT CCencerDonut::SetUp_ConstantTable()
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

	m_pModelCom->Set_RawValue("g_fFadeSet", &m_fFadeSet, sizeof(_float));

	_float3 vColor = _float3(0.9f, 1.f, 1.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));

	_float3	vEndColor = _float3(0.8f, 0.95f, 0.97f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CCencerDonut * CCencerDonut::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCencerDonut*	pInstance = new CCencerDonut(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CCencerDonut");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCencerDonut::Clone(void * pArg, _uint iCurrentScene)
{
	CCencerDonut*	pInstance = new CCencerDonut(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CCencerDonut");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCencerDonut::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pOptionCom);
}
