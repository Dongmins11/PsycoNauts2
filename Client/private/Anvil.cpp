#include "stdafx.h"
#include "..\public\Anvil.h"
#include "GameInstance.h"
#include "Doubt.h"
#include "CollisionObject.h"

CAnvil::CAnvil(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CAnvil::CAnvil(const CAnvil & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnvil::NativeConstruct(void* _pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != _pArg)
		m_pParentMatrix = (_float4x4*)_pArg;

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	return S_OK;
}

_int CAnvil::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_IsDrop)
	{
		_float4x4 CalculateMatrix;
		XMStoreFloat4x4(&CalculateMatrix, XMLoadFloat4x4(m_pParentMatrix));
		m_pTransformCom->Set_WorldMatrix(CalculateMatrix);
		m_pBoxCollider->SetGravity(false);
		m_pBoxCollider->Sleep();
	}
	else
	{
		m_pBoxCollider->SetGravity(true);
		m_pBoxCollider->Update_TransformPhysX();
	}
	
	return 0;
}

_int CAnvil::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_isInteraction)
	{
		m_isInteraction = false;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_AnvilSmoke"),
			SCENE_STATIC, TEXT("Effect_Smoke_Bulb"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_AnvilSmoke"), m_pTransformCom->Get_Position(), true)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		RELEASE_INSTANCE(CGameInstance);
	}


	if (m_IsDrop)
		m_pBoxCollider->Update_Transform();

	return 0;
}

HRESULT CAnvil::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CAnvil::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_TRIGGER == collision.Get_Object()->Get_ObjectLayerType())
	{
		if (m_IsDrop)
			m_isDestroy = true;
		//여기서 이펙트처리해도될듯?
	}
}

void CAnvil::OnCollisionEnter(CCollisionObject & collision)
{
	if (!m_isInteraction)
		m_isInteraction = true;
}

void CAnvil::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CAnvil::OnTriggerExit(CCollisionObject & collision)
{
}

void CAnvil::Set_Drop(_bool _bDrop)
{
	m_IsDrop = _bDrop;
	if (_bDrop)
		m_isThrowing = true;
}

HRESULT CAnvil::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Anvil0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;
	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = true;

	tColliderDesc.isSceneQuery = true;
	tColliderDesc.isTrigger = false;

	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pBoxCollider, &tBoxColliderDesc)))
		return E_FAIL;

	m_pBoxCollider->Update_TransformPhysX();

	m_pBoxCollider->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER_ATTACK);

	return S_OK;
}

HRESULT CAnvil::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	_float fLightRange = 30.f;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
	{
		m_vRGBA = m_pRendererCom->Get_RimLightColorFloat4();
		m_fRimPower = m_pRendererCom->Get_RimLightPower();
	}
	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &XMLoadFloat4x4(&WorldMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&ViewMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&ProjMatrix), sizeof(_float4x4));

	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float4(-1.f, -1.f, -1.f, 0.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CAnvil * CAnvil::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* _pArg)
{
	CAnvil*	pInstance = new CAnvil(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSGBOX("Failed to Created : CAnvil");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAnvil::Clone(void * pArg, _uint iSceneIndex)
{
	return nullptr;
}

void CAnvil::Free()
{
	__super::Free();
	if (m_pBoxCollider)
		m_pBoxCollider->Remove_Actor();
	Safe_Release(m_pBoxCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
