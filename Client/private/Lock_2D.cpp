#include "stdafx.h"
#include "..\public\Lock_2D.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player_2D.h"
#include "Key_2D.h"

CLock_2D::CLock_2D(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CLock_2D::CLock_2D(const CLock_2D & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLock_2D::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLock_2D::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;


	m_fRangeForFrustum = 1.f;

	m_pTransformCom->Scaling(_float3(0.2f, 0.2f, 0.1f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pPlaneTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_Plane_2D"), TEXT("Com_Transform"), 0);
	if (nullptr == pPlaneTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(pPlaneTransform->Get_State(CTransform::STATE_RIGHT)) * 0.25f);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(pPlaneTransform->Get_State(CTransform::STATE_UP)) * 0.25f);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(pPlaneTransform->Get_State(CTransform::STATE_LOOK)) * 0.1f);
	RELEASE_INSTANCE(CGameInstance);

	_matrix RotMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.f), XMConvertToRadians(-90.f), XMConvertToRadians(0.f));

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(m_pTransformCom->Get_Scale());
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(m_pTransformCom->Get_Scale());
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(m_pTransformCom->Get_Scale());

	vRight = XMVector3TransformNormal(vRight, RotMatrix);
	vUp = XMVector3TransformNormal(vUp, RotMatrix);
	vLook = XMVector3TransformNormal(vLook, RotMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	m_vInitialPos = _float3(3.07f, 1.7f, 43.976f);

	m_pTransformCom->Set_State_Float3(CTransform::STATE_POSITION, m_vInitialPos);

	m_isRender = false;

	return S_OK;
}

_int CLock_2D::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (true == m_IsTriggerRender)
		m_isRender = true;
	else
		m_isRender = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CKey_2D* pKey = dynamic_cast<CKey_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Key_2D"), 2));
	if (true == pKey->Get_Clear())
	{
		m_IsDisolveCheck = true;
		m_iTextureIndex = 1;
	}

	RELEASE_INSTANCE(CGameInstance);
	
	return _int();
}

_int CLock_2D::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CLock_2D::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (m_isRender)
		m_pModelCom->Render(28);

	return S_OK;
}


HRESULT CLock_2D::SetUp_Component()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STAGE02, TEXT("Prototype_Component_Texture_Lock_2D"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLock_2D::SetUp_ConstantTable()
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
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelCom, m_iTextureIndex);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLock_2D * CLock_2D::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLock_2D* pInstance = new CLock_2D(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CLock_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLock_2D::Clone(void * pArg, _uint iSceneIndex)
{
	CLock_2D* pInstance = new CLock_2D(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CLock_2D");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CLock_2D::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

}
