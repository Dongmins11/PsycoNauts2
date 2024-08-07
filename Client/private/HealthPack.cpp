#include "stdafx.h"
#include "..\public\HealthPack.h"
#include "GameInstance.h"
#include "Right_Hand_Anim.h"
#include "CollisionObject.h"
#include "Player.h"

CHealthPack::CHealthPack(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CHealthPack::CHealthPack(const CHealthPack & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHealthPack::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHealthPack::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	_vector vPostion;
	if (nullptr != pArg)
	{
		vPostion = *(_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPostion);
	}

	m_pModelCom->Set_Animation(0);

	m_iRenderPass = 0;

	m_isRender = true;

	return S_OK;
}

_int CHealthPack::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (true == m_bItemPickUpCheck)
	{
		m_isDestroy =true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
		
		/* For. HealStarRect */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HealPackRect"),
			SCENE_STATIC, TEXT("Effect_HealPackRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, false, pTransform, TEXT("Layer_HealPackRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		_vector vPos = XMVectorZero();
		vPos = XMVectorSetW(vPos, 1.f);
		vPos = XMVectorSetY(vPos, 1.5f);
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HealPackRect"), TEXT("Com_Option"), vPos);
		
		/* For. HealStarRect */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HealStarRect"),
			SCENE_STATIC, TEXT("Effect_HealStarRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		vPos = XMVectorSetY(vPos, 0.7f);
		
		if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HealStarRect") , m_pTransformCom->Get_Position()  + vPos, true)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if (!m_isRender)
		return 0;


	m_pModelCom->Update(TimeDelta);

	if (nullptr != m_pColliderCom)
	{
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}

#ifdef _DEBUG
	m_pRenderColider->Update(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG

	return _int();
}

_int CHealthPack::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CHealthPack::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(m_iRenderPass)))
		return E_FAIL;


#ifdef _DEBUG
	m_pRenderColider->Render();
#endif // _DEBUG

	return S_OK;
}

void CHealthPack::OnTriggerEnter(CCollisionObject & collision)
{
	OutputDebugString(TEXT("HealthPack Trigger Enter \n"));
	m_bItemPickUpCheck = true;
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsHeal(true);

	if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_HEALPACK)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (FAILED(pGameInstance->PlaySoundOnce(TEXT("HealthPack.wav"), CHANNELID::PLAYER_HEALPACK)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	

	RELEASE_INSTANCE(CGameInstance);
}

void CHealthPack::OnCollisionEnter(CCollisionObject & collision)
{
	OutputDebugString(TEXT("HealthPack OnCollisionEnter \n"));
}

void CHealthPack::OnControllerCollisionHit(CCollisionObject & collision)
{
	OutputDebugString(TEXT("HealthPack OnControllerCollisionHit \n"));
}

void CHealthPack::OnTriggerExit(CCollisionObject & collision)
{
}


void CHealthPack::Bone_MatrixCalculation()
{
	if (nullptr != m_pCombinedMatrix)
	{
		_float4x4		TransformationMatrix;
		_matrix			RootTransformationMatrix;
		_vector			RootScale;
		_vector			RootRight, RootUp, RootLook;

		_matrix TargetTransformationMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedMatrix) * XMLoadFloat4x4(&m_PivotMatrix) * m_pTransformCom->Get_WorldMatrix();
		RootTransformationMatrix = TargetTransformationMatrix;

		RootScale = XMVectorSet(XMVectorGetX(XMVector3Length(RootTransformationMatrix.r[0])), XMVectorGetY(XMVector3Length(RootTransformationMatrix.r[1])), XMVectorGetZ(XMVector3Length(RootTransformationMatrix.r[2])), 1.f);

		RootRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(RootScale);
		RootUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(RootScale);
		RootLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(RootScale);

		RootTransformationMatrix.r[0] = RootRight;
		RootTransformationMatrix.r[1] = RootUp;
		RootTransformationMatrix.r[2] = RootLook;

		XMStoreFloat4x4(&TransformationMatrix, RootTransformationMatrix);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, RootTransformationMatrix.r[3]);
	}

}


HRESULT CHealthPack::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_HealthPack"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* For.Com_Render */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
	ColliderDesc.vExtents = _float3(1.f, 1.0f, 1.f);
	ColliderDesc.fRadius = 0.4f;

	_float PxSphereColiderSize =0.f;
	PxSphereColiderSize = ColliderDesc.fRadius;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_RenderColider"), (CComponent**)&m_pRenderColider, &ColliderDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;

	tColliderDesc.isSceneQuery = true;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(PxSphereColiderSize, PxSphereColiderSize, PxSphereColiderSize);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_ITEM;
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);

	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();


	return S_OK;
}

HRESULT CHealthPack::SetUp_BoneMatrices(const char * pBoneName)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*	pTargetTransform = (CTransform*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTargetTransform)
		return E_FAIL;

	CModel*		pModel = (CModel*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Model"), 0);
	if (nullptr == pModel)
		return E_FAIL;

	m_OffsetMatrix = pModel->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix = pModel->Get_PivotMatrix();
	m_pCombinedMatrix = pModel->Get_CombinedMatrixPtr(pBoneName);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHealthPack::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 TransPoseWorldMat, ViewTransPoseMat, ProjectionTransPoseMat;

	XMStoreFloat4x4(&TransPoseWorldMat, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjectionTransPoseMat,XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pModelCom->Set_RawValue("g_WorldMatrix", &TransPoseWorldMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjectionTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

CHealthPack * CHealthPack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHealthPack*	pInstance = new CHealthPack(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CHealthPack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHealthPack::Clone(void * pArg, _uint iSceneIndex)
{
	CHealthPack*	pInstance = new CHealthPack(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CHealthPack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHealthPack::Free()
{
	__super::Free();


	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRenderColider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
