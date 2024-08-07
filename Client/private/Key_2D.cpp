#include "stdafx.h"
#include "..\public\Key_2D.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player_2D.h"

CKey_2D::CKey_2D(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CKey_2D::CKey_2D(const CKey_2D & rhs)
	: CGameObject(rhs)
{
}

HRESULT CKey_2D::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKey_2D::NativeConstruct(void * pArg, _uint iCurrentScene)
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

	switch (m_iLayerIndex)
	{
	case 0:
		m_vInitialPos = _float3(2.59f, 1.7f, 42.89f);
		break;
	case 1:
		m_vInitialPos = _float3(1.92f, 1.7f, 45.83f);
		break;
	case 2:
		m_vInitialPos = _float3(2.936f, 1.7f, 44.435f);
		break;
	default:
		break;
	}
	m_pTransformCom->Set_State_Float3(CTransform::STATE_POSITION, m_vInitialPos);

	m_isRender = false;

	return S_OK;
}

_int CKey_2D::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();

	if (true == m_IsTriggerRender)
	{
		if (true == m_IsCollision)
			m_isRender = false;
		else
			m_isRender = true;
	}
	else
		m_isRender = false;

	return _int();
}

_int CKey_2D::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer_2D* pPlayer_2D = dynamic_cast<CPlayer_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Player_2D"), 0));
	if (nullptr == pPlayer_2D)
	{
		RELEASE_INSTANCE(CGameInstance);
		return -1;
	}

	if (true == m_IsPlayer_Collision)
	{
		for (m_iLayerIndex = 0; m_iLayerIndex < 3; m_iLayerIndex++)
			m_IsCollision = false;
	}

	RELEASE_INSTANCE(CGameInstance);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CKey_2D::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if(m_isRender)
		m_pModelCom->Render(28);
	return S_OK;
}

void CKey_2D::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_2D_PLAYER == collision.Get_Object()->Get_ObjectLayerType())
	{
		m_IsCollision = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->StopSound(CHANNELID::PLAYER);
		pGameInstance->PlaySoundOnce(TEXT("Key.wav"), CHANNELID::PLAYER);
		RELEASE_INSTANCE(CGameInstance);
		for (m_iLayerIndex = 0; m_iLayerIndex < 3; m_iLayerIndex++)
		{
			if (2 == m_iLayerIndex)
				m_IsClear = true;
		}
		//OutputDebugString(TEXT("충돌"));
	}
}

HRESULT CKey_2D::SetUp_Component()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE02, TEXT("Layer_Key_2D"));

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STAGE02, TEXT("Prototype_Component_Texture_Key_2D"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.pGameObject = this;						// Trigger ?
	tColliderDesc.isTrigger = true;

	m_eLayerType = LayerType::LAYERTYPE_2D_KEY;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(0.1f, 0.1f, 0.001f);	// scale이 지름값을 가지므로 scale * 2 해야한다.

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_matrix smatPviot = XMMatrixTranslation(0.f, 0.05f, 0.f);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CKey_2D::SetUp_ConstantTable()
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
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelCom, 0);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CKey_2D * CKey_2D::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CKey_2D* pInstance = new CKey_2D(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CKey_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKey_2D::Clone(void * pArg, _uint iSceneIndex)
{
	CKey_2D* pInstance = new CKey_2D(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CKey_2D");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CKey_2D::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
}
