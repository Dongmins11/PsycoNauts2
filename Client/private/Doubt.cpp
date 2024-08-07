#include "stdafx.h"
#include "..\public\Doubt.h"
#include "GameInstance.h"
#include "Spawn_Doubt.h"
#include "Idle_Doubt.h"
#include "Move_Doubt.h"
#include "Chase_Doubt.h"
#include "Hit_Doubt.h"
#include "Attack_Doubt.h"
#include "Dead_Doubt.h"
#include "Sequence.h"
#include "Selector.h"
#include "Anvil.h"
#include "CollisionObject.h"
#include "QuestManager.h"
#include "Camera_Fly.h"

CDoubt::CDoubt(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CDoubt::CDoubt(const CDoubt & rhs)
	: CEnemy(rhs)
{
}

HRESULT CDoubt::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CDoubt::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float4x4		WorldMatrix;

	if (nullptr != pArg)
		WorldMatrix = *(_float4x4*)pArg;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pModelCom->Set_Animation(21);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_pCombinedMatrix = m_pModelCom->Get_CombinedMatrixPtr("PropHand");

	m_pAnvil = CAnvil::Create(m_pDevice, m_pDeviceContext, &m_PropHandMatrix);

	m_pRoot = CSequence::Create();
	m_pSpawnDoubt = CSpawn_Doubt::Create(this);
	m_pIdleDoubt = CIdle_Doubt::Create(this);
	m_pDeadDoubt = CDead_Doubt::Create(this);
	m_pAttackDoubt = CAttack_Doubt::Create(this);
	m_pMoveDoubt = CMove_Doubt::Create(this);
	m_pHitDoubt = CHit_Doubt::Create(this);
	m_pChaseDoubt = CChase_Doubt::Create(this);

	m_pRoot->AddChild(m_pSpawnDoubt);
	m_pRoot->AddChild(m_pHitDoubt);
	m_pRoot->AddChild(m_pChaseDoubt);
	m_pRoot->AddChild(m_pAttackDoubt);
	m_pRoot->AddChild(m_pIdleDoubt);
	m_pRoot->AddChild(m_pMoveDoubt);
	m_pRoot->AddChild(m_pDeadDoubt);

	_vector WorldPos = m_pTransformCom->Get_Position();
	WorldPos += XMVectorSet(0.f, 2.f, 0.f, 0.f);
	m_pCharacterController->Set_Position(_float3(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos), XMVectorGetZ(WorldPos)));
	return S_OK;
}

_int CDoubt::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (nullptr != m_pAnvil)
	{
		if (m_pAnvil->Get_IsDestroy())
			Safe_Release(m_pAnvil);
	}

	m_pTransformCom->Set_Velocity(XMVectorZero());

	if (true == m_pRoot->Invoke(TimeDelta, m_pModelCom, m_pTransformCom))
	{
		m_isDestroy = true;
		CQuestManager* pQManager = GET_INSTANCE(CQuestManager);
		pQManager->Battle_MonsterCnt(2, true);
		m_isDestroy = true;

		if (5 == pQManager->Battle_MonsterCnt(2, false))
		{
			pQManager->Clear_Quest(2);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_11);
			static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_CLOUD);
			RELEASE_INSTANCE(CGameInstance);
		}
		RELEASE_INSTANCE(CQuestManager)
	}
	m_pModelCom->Lerp_Update(TimeDelta);

	m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());

	m_pCharacterController->Update_Transform();

	if (!m_isDestroy)
	{
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3] += m_pTransformCom->Get_State(CTransform::STATE_UP);
		m_pHitColliderCom->Update_TransformPhysX(WorldMatrix);
	}

	Update_PropHandMatrix();

	if (nullptr != m_pAnvil)
		m_pAnvil->Tick(TimeDelta);

	return _int();
}

_int CDoubt::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;
	if (nullptr != m_pAnvil)
		m_pAnvil->LateTick(TimeDelta);

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CDoubt::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (nullptr != m_pAnvil)
	{
		if (FAILED(m_pAnvil->Render()))
			return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CDoubt::OnTriggerEnter(CCollisionObject & collision)
{
	if (!m_IsHit)
	{
		if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PICK)
		{
			if (collision.Get_Object()->Get_IsThrowing())
			{
				Damaged(101);
			}
		}
		else if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_SKILL)
		{
			Damaged(101);
		}
	}
}

void CDoubt::OnCollisionEnter(CCollisionObject & collision)
{
}

void CDoubt::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CDoubt::OnTriggerExit(CCollisionObject & collision)
{
}

void CDoubt::Update_LookVector(_double TimeDelta)
{
	if (!m_pModelCom->Get_IsAnimLerp())
	{
		if (1.0 > m_dLerpTime)
			m_dLerpTime += TimeDelta;
		else
			m_dLerpTime = 0.0;
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vAimLook = XMVector4Normalize(XMVectorLerp(vLook, XMLoadFloat4(&Get_TargetDir()), (_float)m_dLerpTime));
		_vector vRight = XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vAimLook));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vAimLook);
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	}
	else
		m_dLerpTime = 0.0;
}

_bool CDoubt::Is_AttackSure()
{
	return !m_IsUnArmed;
}

void CDoubt::Set_Armed(_bool _bArmed)
{
	if (true == _bArmed&&
		!m_IsActionDrop)
	{
		m_IsActionDrop = true;
		m_pAnvil->Set_Drop(true);
	}
	m_IsUnArmed = _bArmed;
}

void CDoubt::Update_PropHandMatrix()
{
	XMStoreFloat4x4(&m_PropHandMatrix, XMLoadFloat4x4(m_pCombinedMatrix)*XMMatrixScaling(0.01f, 0.01f, 0.01f)*XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f))*m_pTransformCom->Get_WorldMatrix());
}


HRESULT CDoubt::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Doubt0"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_CCharacterController */

	CCharacterController::DESC tCharacterControllerDesc;
	tCharacterControllerDesc.eClimbingMode = PxCapsuleClimbingMode::eEASY;
	tCharacterControllerDesc.fHeight = 0.5f;
	tCharacterControllerDesc.fRadius = 0.5f;
	tCharacterControllerDesc.fStepOffset = 0.f;
	tCharacterControllerDesc.fContactOffset = tCharacterControllerDesc.fRadius * 0.01f;
	tCharacterControllerDesc.fStaticFriction = 0.5f;
	tCharacterControllerDesc.fDynamicFriction = 0.5f;
	tCharacterControllerDesc.fRestitution = 0.01f;
	tCharacterControllerDesc.vPosition = { 0.f, 2.f, 0.f };
	tCharacterControllerDesc.pGameObject = this;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_ChracterController"), TEXT("Com_Controller"), (CComponent**)&m_pCharacterController, &tCharacterControllerDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = false;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = { 2.f,2.f,2.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pHitColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pHitColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER);

	return S_OK;
}

HRESULT CDoubt::SetUp_ConstantTable()
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

CDoubt * CDoubt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDoubt*	pInstance = new CDoubt(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDoubt::Clone(void * pArg, _uint iCurrentScene)
{
	CDoubt*	pInstance = new CDoubt(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoubt::Free()
{
	__super::Free();
	Safe_Release(m_pAnvil);
	if (m_pHitColliderCom)
	{
		m_pHitColliderCom->Remove_Actor();
		Safe_Release(m_pHitColliderCom);
	}
	Safe_Release(m_pRoot);
	Safe_Release(m_pSpawnDoubt);
	Safe_Release(m_pIdleDoubt);
	Safe_Release(m_pDeadDoubt);
	Safe_Release(m_pAttackDoubt);
	Safe_Release(m_pMoveDoubt);
	Safe_Release(m_pHitDoubt);
	Safe_Release(m_pChaseDoubt);
}
