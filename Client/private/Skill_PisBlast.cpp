#include "stdafx.h"
#include "..\public\Skill_PisBlast.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "CollisionObject.h"

CSkill_PisBlast::CSkill_PisBlast(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSkill_PisBlast::CSkill_PisBlast(const CSkill_PisBlast & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkill_PisBlast::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkill_PisBlast::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(SetUp_TargetBoneMatrices("goggles1")))
		return E_FAIL;

	m_pTransformCom->Scaling(_float3(5.f, 5.f, 5.f));

	return S_OK;
}

_int CSkill_PisBlast::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pTransformCom->Go_Straight(TimeDelta * 8.f);

	m_lfAgeDeadTime += TimeDelta;

	m_TimeAcc += TimeDelta;

	m_isPulling = true;

	m_pColliderCom->Update_TransformPhysX();

	m_pColliderCom->Update_Transform();

	if (!m_isInteraction)
	{
		m_isInteraction = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_FireImpactRect"),
			SCENE_STATIC, TEXT("Effect_Repeat_FireImpactRect"), &m_pEffect)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		Safe_AddRef(m_pEffect);

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransformCom, TEXT("Layer_Repeat_FireImpactRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_isEffectCreate)
	{
		m_isEffectCreate = false;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PsiBlastHitEffect2"),
			SCENE_STATIC, TEXT("Effect_HitEffect2"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PsiBlastHitEffect2"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		_vector vPos = XMLoadFloat3(&m_vTargetEffectPos) - m_pTransformCom->Get_Position();
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PsiBlastHitEffect2"), TEXT("Com_Option"), vPos);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PsiBlastHitEffect2"), TEXT("Com_Option"), 0.3);

		RELEASE_INSTANCE(CGameInstance);
	}


	if (true == m_bDeadCheck)
		m_isDestroy = true;

	if (3.0 <= m_lfAgeDeadTime)
		m_isDestroy = true;

	if (m_isDestroy && nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
	return _int();
}

_int CSkill_PisBlast::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return _int();
}

HRESULT CSkill_PisBlast::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_BRAINSTORM);


	return S_OK;
}

void CSkill_PisBlast::OnTriggerEnter(CCollisionObject & collision)
{
	if (false == m_bDeadCheck)
		m_bDeadCheck = true;

	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_MONSTER)
	{
		OutputDebugStringW(TEXT("Col-Trigger-StaticÃæµ¹:HandRight\n"));

		if (collision.Get_ObjectTransform())
		{
			if (!m_isEffectCreate)
			{
				m_isEffectCreate = true;
				_float fRandRight = GET_RANDOMFLOAT(-0.5f, 0.5f);
				_float fRandUp = GET_RANDOMFLOAT(0.5f, 1.5f);
				XMStoreFloat3(&m_vTargetEffectPos, ((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_Position() +
					((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_State(CTransform::STATE_UP) * fRandUp +
					((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_State(CTransform::STATE_RIGHT) * fRandRight);
			}
		}
		OutputDebugString(TEXT("Hit Monster!!\n"));
	}
}

HRESULT CSkill_PisBlast::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_BrainStorm"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = true;

	CSphereCollider::DESC tSphereColliderDesc;
	tSphereColliderDesc.tColliderDesc = tColliderDesc;
	tSphereColliderDesc.vScale = { 0.35f,0.35f,0.35f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tSphereColliderDesc)))
		return E_FAIL;
	m_eLayerType = LayerType::LAYERTYPE_PLAYER_SKILL;

	m_pColliderCom->Set_ShapeLayer(m_eLayerType);

	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();
	
	return S_OK;
}

HRESULT CSkill_PisBlast::SetUp_ConstantTable()
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

	_float fTimeAcc = (_float)m_TimeAcc;
	m_pModelCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(1.f, 1.f, 0.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(1.f, 0.25f, 0.f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	_bool	isDissolve = false;
	if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &isDissolve, sizeof(_bool))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CSkill_PisBlast::SetUp_TargetBoneMatrices(const char * pBoneName)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4				OffsetMatrix;
	_float4x4				PivotMatrix;
	const _float4x4*		pCombinedMatrix = nullptr;
	const _float4x4*		pTargetWorldMatrix;
	_matrix					CamWorldMat;

	CGameObject* pCameraFly = pGameInstance->Get_GameObject(m_iCurrentScene, L"Layer_Camera", 0);
	if (nullptr == pCameraFly)
	{
		MSGBOX("Failed to Finder CamFly : Function CSkill_PisBlast : SetUp_TargetBoneMatrices");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	CTransform*	pTargetTransform = (CTransform*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTargetTransform)
	{
		MSGBOX("Failed to Finder pTargetTransform : Function CSkill_PisBlast : SetUp_TargetBoneMatrices");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	CModel*		pModel = (CModel*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Model"), 0);
	if (nullptr == pModel)
	{
		MSGBOX("Failed to Finder pModel : Function CSkill_PisBlast : SetUp_TargetBoneMatrices");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	OffsetMatrix = pModel->Get_OffsetMatrix(pBoneName);
	PivotMatrix = pModel->Get_PivotMatrix();
	pCombinedMatrix = pModel->Get_CombinedMatrixPtr(pBoneName);
	pTargetWorldMatrix = pTargetTransform->Get_WorldMatrixPtr();

	CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();

	if (nullptr != pCombinedMatrix || nullptr != pTargetWorldMatrix)
	{
		_float4x4		TransformationMatrix;
		_vector			vPosition;
		_float4			vPositionFloat4;
		_vector			vUp, vLook;

		_matrix TargetTransformationMatrix = XMLoadFloat4x4(&OffsetMatrix) * XMLoadFloat4x4(pCombinedMatrix) * XMLoadFloat4x4(&PivotMatrix) * XMLoadFloat4x4(pTargetWorldMatrix);
		XMStoreFloat4x4(&TransformationMatrix, (m_pTransformCom->Get_WorldMatrix() * TargetTransformationMatrix));

		vUp = *(_vector*)&TransformationMatrix.m[1];
		vLook = *(_vector*)&TransformationMatrix.m[2];
		vPosition = *(_vector*)&TransformationMatrix.m[3];
		vPosition = vPosition + XMVector3Normalize(pTargetTransform->Get_State(CTransform::STATE_LOOK));
		XMStoreFloat4(&vPositionFloat4, vPosition);
		vPositionFloat4.y += 0.5f;

		_matrix	ViewMatrix =  pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPositionFloat4));
		_vector CamLook = *(_vector*)&CamWorldMat.r[2];
		_vector CamUp = *(_vector*)&CamWorldMat.r[1];
		CamLook = XMVector3Normalize(CamLook) + (XMVector3Normalize(CamUp) * 0.4f);
		//_vector CamLook = *(_vector*)&ViewMatrix.r[2];
		m_pTransformCom->Set_MyLook(CamLook);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CSkill_PisBlast * CSkill_PisBlast::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSkill_PisBlast*	pInstance = new CSkill_PisBlast(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CSkill_PisBlast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkill_PisBlast::Clone(void * pArg, _uint iSceneIndex)
{
	CSkill_PisBlast*	pInstance = new CSkill_PisBlast(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CSkill_PisBlast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_PisBlast::Free()
{
	__super::Free();

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}
