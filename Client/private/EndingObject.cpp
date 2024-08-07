#include "stdafx.h"
#include "..\public\EndingObject.h"
#include "GameInstance.h"


CEndingObject::CEndingObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CEndingObject::CEndingObject(const CEndingObject & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEndingObject::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEndingObject::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;


	m_pLayerTag = (_tchar*)pArg;
	
	if (FAILED(SetUp_Component(SCENE_STATIC, m_pLayerTag)))
		return E_FAIL;
	
	m_iAnimationIndex = 1;
	m_pModelCom->Set_Animation(m_iAnimationIndex);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	return S_OK;
}

_int CEndingObject::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pModelCom->Set_Animation(m_iAnimationIndex);
	m_pModelCom->Lerp_Update(TimeDelta);
	return 0;
}

_int CEndingObject::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	if (!m_isRender)
		return 0;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CEndingObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;


	return S_OK;
}

HRESULT CEndingObject::SetUp_Component(_uint iSceneIndex, const _tchar * pPrototypeTag)
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

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_pLayerTag,TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	

	return S_OK;
}

HRESULT CEndingObject::SetUp_ConstantTable()
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

	m_pModelCom->Render(0);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEndingObject * CEndingObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEndingObject*	pInstance = new CEndingObject(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CEndingObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEndingObject::Clone(void * pArg, _uint iCurrentScene)
{
	CEndingObject*	pInstance = new CEndingObject(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CEndingObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEndingObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
