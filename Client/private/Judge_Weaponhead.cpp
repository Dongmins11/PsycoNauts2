#include "stdafx.h"
#include "..\public\Judge_Weaponhead.h"
#include "Judge.h"
#include "GameInstance.h"
#include "CollisionObject.h"

CJudge_Weaponhead::CJudge_Weaponhead(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CJudge_Weaponhead::CJudge_Weaponhead(const CJudge_Weaponhead & rhs)
	: CGameObject(rhs)
{
}

HRESULT CJudge_Weaponhead::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		ZeroMemory(&m_tWeaponInfo, sizeof(WEAPONINFO));
		m_tWeaponInfo = *(WEAPONINFO*)_pArg;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	return S_OK;
}

HRESULT CJudge_Weaponhead::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CJudge_Weaponhead::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	_matrix WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f))*XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f))*XMLoadFloat4x4(m_tWeaponInfo.pParentMatrix);
	if (m_pBoxCollider)
	{
		if (m_isPicking)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CJudge*>(m_tWeaponInfo.pParent)->Destroy_Weapon();
			CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
			_vector vPos = pPlayerTrans->Get_Position() - pPlayerTrans->Get_State(CTransform::STATE_UP) * 0.7f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pBoxCollider->SetGravity(false);
			RELEASE_INSTANCE(CGameInstance);
		}
		else
		{
			m_pBoxCollider->SetGravity(true);
		}
		if (!m_isThrowing && !m_isPicking)
		{
			m_pBoxCollider->Reset_Power();
			m_pBoxCollider->Update_TransformPhysX(WorldMatrix);
		}
		else {
			if(!m_isDestroy)
				m_pBoxCollider->Update_TransformPhysX();
		}
	}

	return 0;
}

_int CJudge_Weaponhead::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy&&
		m_isThrowing||
		m_isPicking)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	m_pBoxCollider->Update_Transform();

	return 0;
}

HRESULT CJudge_Weaponhead::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CJudge_Weaponhead::Setting_Throwing(_fvector vDir)
{
	m_isThrowing = true;
	m_pBoxCollider->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
}

void CJudge_Weaponhead::OnTriggerEnter(CCollisionObject & collision)
{
	if (m_isThrowing)
	{
		m_isInteraction = true;
		//해당위치에 터지는 이펙트
		Set_IsDestroy(true);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		/* WhiteStarRect */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"),
			SCENE_STATIC, TEXT("Effect_WhiteStarRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. WhiteStarRect 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_WhiteStarRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		_float3 vPos = _float3(0.f, 1.f, 0.f);
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), XMLoadFloat3(&vPos));
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), 0.5f);

		/* Smoke_Gray */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"),
			SCENE_STATIC, TEXT("Effect_Smoke_Gray"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. Smoke_Gray 이펙트 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_Smoke_Gray"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), 0.8f);

		/* For. Layer_BlueSpark 이펙트 생성 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"),
			SCENE_STATIC, TEXT("Effect_BlueSpark"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. Layer_BlueSpark 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_BlueSpark"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 2.f);

		/* For. Layer_GrabBrokenRect 이펙트 생성 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"),
			SCENE_STATIC, TEXT("Effect_GrabBrokenRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 1.5f);

		/* For. Layer_GrabBrokenRect 이펙트 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_GrabBrokenRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"), TEXT("Com_Option"), 0.8f);

		RELEASE_INSTANCE(CGameInstance);

	}
}

void CJudge_Weaponhead::OnCollisionEnter(CCollisionObject & collision)
{
	if (m_isThrowing)
	{
		m_isInteraction = true;
		//해당위치에 터지는 이펙트
		Set_IsDestroy(true);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		/* WhiteStarRect */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"),
			SCENE_STATIC, TEXT("Effect_WhiteStarRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. WhiteStarRect 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_WhiteStarRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		_float3 vPos = _float3(0.f, 1.f, 0.f);
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), XMLoadFloat3(&vPos));
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), 0.5f);

		/* Smoke_Gray */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"),
			SCENE_STATIC, TEXT("Effect_Smoke_Gray"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. Smoke_Gray 이펙트 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_Smoke_Gray"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), 0.8f);

		/* For. Layer_BlueSpark 이펙트 생성 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"),
			SCENE_STATIC, TEXT("Effect_BlueSpark"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. Layer_BlueSpark 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_BlueSpark"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 2.f);

		/* For. Layer_GrabBrokenRect 이펙트 생성 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"),
			SCENE_STATIC, TEXT("Effect_GrabBrokenRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 1.5f);

		/* For. Layer_GrabBrokenRect 이펙트 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_GrabBrokenRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"), TEXT("Com_Option"), 0.8f);

		RELEASE_INSTANCE(CGameInstance);

	}
}

void CJudge_Weaponhead::OnControllerCollisionHit(CCollisionObject & collision)
{
	if (m_isThrowing)
	{
		m_isInteraction = true;
		//해당위치에 터지는 이펙트
		Set_IsDestroy(true);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		/* WhiteStarRect */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"),
			SCENE_STATIC, TEXT("Effect_WhiteStarRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. WhiteStarRect 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_WhiteStarRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		_float3 vPos = _float3(0.f, 1.f, 0.f);
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), XMLoadFloat3(&vPos));
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), 0.5f);

		/* Smoke_Gray */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"),
			SCENE_STATIC, TEXT("Effect_Smoke_Gray"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. Smoke_Gray 이펙트 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_Smoke_Gray"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), 0.8f);

		/* For. Layer_BlueSpark 이펙트 생성 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"),
			SCENE_STATIC, TEXT("Effect_BlueSpark"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		/* For. Layer_BlueSpark 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_BlueSpark"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), true);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 2.f);

		/* For. Layer_GrabBrokenRect 이펙트 생성 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"),
			SCENE_STATIC, TEXT("Effect_GrabBrokenRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 1.5f);

		/* For. Layer_GrabBrokenRect 이펙트 상속 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_GrabBrokenRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"), TEXT("Com_Option"), 0.8f);

		RELEASE_INSTANCE(CGameInstance);

	}
}

void CJudge_Weaponhead::OnTriggerExit(CCollisionObject & collision)
{
}

HRESULT CJudge_Weaponhead::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_tchar szModelPrototypeTag[MAX_PATH] = TEXT("");
	swprintf_s(szModelPrototypeTag, MAX_PATH, TEXT("Prototype_Component_Model_JudgeWeaponhead%d"), m_tWeaponInfo.iWeaponType);

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, szModelPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Collider */
	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = false;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = true;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = { 2.f,2.f,2.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pBoxCollider, &tBoxColliderDesc)))
		return E_FAIL;

	m_pBoxCollider->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_PICK);

	m_pBoxCollider->Update_TransformPhysX();

	return S_OK;
}

HRESULT CJudge_Weaponhead::SetUp_ConstantTable()
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


CJudge_Weaponhead * CJudge_Weaponhead::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CJudge_Weaponhead*	pInstance = new CJudge_Weaponhead(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CJudge_Weaponhead");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJudge_Weaponhead::Clone(void * pArg, _uint iSceneIndex)
{
	CJudge_Weaponhead*	pInstance = new CJudge_Weaponhead(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CJudge_AttTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJudge_Weaponhead::Free()
{
	__super::Free();
	if (m_pBoxCollider)
		m_pBoxCollider->Remove_Actor();
	Safe_Release(m_pBoxCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
