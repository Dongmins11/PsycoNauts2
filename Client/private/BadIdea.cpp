#include "stdafx.h"
#include "..\public\BadIdea.h"
#include "CollisionObject.h"
#include "GameInstance.h"
#include "Spawn_BadIdea.h"
#include "Idle_BadIdea.h"
#include "Spawn_BadIdea.h"
#include "Dead_BadIdea.h"
#include "Run_BadIdea.h"
#include "Walk_BadIdea.h"
#include "Chase_BadIdea.h"
#include "Jump_BadIdea.h"
#include "Attack_BadIdea.h"
#include "Hit_BadIdea.h"
#include "Sequence.h"
#include "Selector.h"
#include "Bulb.h"
#include "BadIdea_AttTrigger.h"
#include "Monster_HpBar.h"
#include "Camera_Fly.h"
CBadIdea::CBadIdea(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CBadIdea::CBadIdea(const CBadIdea & rhs)
	: CEnemy(rhs)
{
}

HRESULT CBadIdea::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBadIdea::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float4x4		WorldMatrix;

	if (nullptr != pArg)
		WorldMatrix = *(_float4x4*)pArg;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pModelCom->Set_Animation(16);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_pRoot = CSequence::Create();
	m_pSelector = CSelector::Create();
	m_pSpawnBadIdea = CSpawn_BadIdea::Create(this);
	m_pIdleBadIdea = CIdle_BadIdea::Create(this);
	m_pDeadBadIdea = CDead_BadIdea::Create(this);
	m_pRunBadIdea = CRun_BadIdea::Create(this);
	m_pWalkBadIdea = CWalk_BadIdea::Create(this);
	m_pHitBadIdea = CHit_BadIdea::Create(this);
	m_pChaseBadIdea = CChase_BadIdea::Create(this);
	m_pJumpBadIdea = CJump_BadIdea::Create(this);
	m_pAttackBadIdea = CAttack_BadIdea::Create(this);
	//
	m_pRoot->AddChild(m_pSpawnBadIdea);
	m_pRoot->AddChild(m_pHitBadIdea);
	m_pRoot->AddChild(m_pChaseBadIdea);
	m_pRoot->AddChild(m_pAttackBadIdea);
	m_pRoot->AddChild(m_pIdleBadIdea);
	m_pRoot->AddChild(m_pSelector);
	m_pSelector->AddChild(m_pRunBadIdea);
	m_pSelector->AddChild(m_pWalkBadIdea);
	m_pRoot->AddChild(m_pJumpBadIdea);
	m_pRoot->AddChild(m_pDeadBadIdea);

	_vector WorldPos = m_pTransformCom->Get_Position();
	m_pCharacterController->Set_Position(_float3(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos) + 2.f, XMVectorGetZ(WorldPos)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	XMStoreFloat4x4(&m_AttackTriggerMatrix, XMMatrixIdentity());
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_BadIdeaAttTrigger"), m_iCurrentScene, TEXT("Layer_AttTrigger"), &m_pAttTrigger, &m_AttackTriggerMatrix);

	CMonster_HpBar::MONSTERHPBARINFO tMonsterHpBarInfo;
	ZeroMemory(&tMonsterHpBarInfo, sizeof(CMonster_HpBar::MONSTERHPBARINFO));
	tMonsterHpBarInfo.m_pParentHp = &m_iHp;
	tMonsterHpBarInfo.m_pParentMatrix = &m_HpBarMatrix;
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterHpBar"), m_iCurrentScene, TEXT("Layer_CensorHpBar"), &m_pHpBar, &tMonsterHpBarInfo);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CBadIdea::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pTransformCom->Set_Velocity(XMVectorZero());

	if (!m_pCharacterController->IsDown())
		m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -0.5f);

	if (true == m_pRoot->Invoke(TimeDelta, m_pModelCom, m_pTransformCom))
		m_IsReadyDead = true;

	m_pModelCom->Lerp_Update(TimeDelta);

	m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());

	m_pCharacterController->Update_Transform();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] += m_pTransformCom->Get_State(CTransform::STATE_UP);
	WorldMatrix.r[3] += m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	XMStoreFloat4x4(&m_AttackTriggerMatrix, WorldMatrix);

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] += XMVectorSet(0.f, 2.3f, 0.f, 0.f);
	WorldMatrix.r[3] += m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	XMStoreFloat4x4(&m_HpBarMatrix, WorldMatrix);

	if (!m_isDestroy)
	{
		WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
		m_pHitColliderCom->Update_TransformPhysX(WorldMatrix);
	}
	
	if (m_IsBite)
	{
		((CBadIdea_AttTrigger*)m_pAttTrigger)->Attack(true);
		m_IsBite = false;
	}
	else
		((CBadIdea_AttTrigger*)m_pAttTrigger)->Attack(false);

	return _int();
}

_int CBadIdea::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_IsReadyDead)
	{
		m_pAttTrigger->Set_IsDestroy(true);
		m_pAttTrigger = nullptr;

		m_pHpBar->Set_IsDestroy(true);
		m_pHpBar = nullptr;

		m_isDestroy = true;
	}

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CBadIdea::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CBadIdea::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_SKILL)
	{
		if (collision.Get_Object()->Get_IsPulling())
		{
			Attacked(CEnemy::ATTACKED::FW);
			Damaged(20);
		}
		else
			Damaged(10);
	}
	else if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PICK)
	{
		if (collision.Get_Object()->Get_IsThrowing())
		{
			Attacked(CEnemy::ATTACKED::TO);
			Damaged(10);
		}
	}
}

void CBadIdea::OnCollisionEnter(CCollisionObject & collision)
{
}

void CBadIdea::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CBadIdea::OnTriggerExit(CCollisionObject & collision)
{
}

void CBadIdea::Update_LookVector(_double TimeDelta)
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

void CBadIdea::Bite(_bool _IsBite)
{
	m_IsBite = _IsBite;
}


HRESULT CBadIdea::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_BadIdea0"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
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
	tCharacterControllerDesc.fHeight = 0.3f;
	tCharacterControllerDesc.fRadius = 0.5f;
	tCharacterControllerDesc.fStepOffset = 0.f;
	tCharacterControllerDesc.fContactOffset = tCharacterControllerDesc.fRadius * 0.01f;
	tCharacterControllerDesc.fStaticFriction = 0.5f;
	tCharacterControllerDesc.fDynamicFriction = 0.5f;
	tCharacterControllerDesc.fRestitution = 0.f;
	tCharacterControllerDesc.vPosition = { 0.f, 1.f, 0.f };
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
	tBoxColliderDesc.vScale = { 2.f,1.f,2.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pHitColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pHitColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER);

	return S_OK;
}

HRESULT CBadIdea::SetUp_ConstantTable()
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
	_bool		bShadow = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_SHADOW);

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
	if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
		return E_FAIL;
	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBadIdea * CBadIdea::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBadIdea*	pInstance = new CBadIdea(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBadIdea::Clone(void * pArg, _uint iCurrentScene)
{
	CBadIdea*	pInstance = new CBadIdea(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBadIdea::Free()
{
	__super::Free();
	Safe_Release(m_pRoot);
	Safe_Release(m_pSelector);
	Safe_Release(m_pSpawnBadIdea);
	Safe_Release(m_pIdleBadIdea);
	Safe_Release(m_pDeadBadIdea);
	Safe_Release(m_pRunBadIdea);
	Safe_Release(m_pWalkBadIdea);
	Safe_Release(m_pHitBadIdea);
	Safe_Release(m_pChaseBadIdea);
	Safe_Release(m_pJumpBadIdea);
	Safe_Release(m_pAttackBadIdea);
	if (m_pHpBar)
	{
		m_pHpBar->Set_IsDestroy(true);
	}
	if (m_pHitColliderCom)
	{
		m_pHitColliderCom->Remove_Actor();
		Safe_Release(m_pHitColliderCom);
	}
}
