#include "stdafx.h"
#include "..\public\Censor_Burly.h"
#include "GameInstance.h"
#include "Sequence.h"
#include "Selector.h"
#include "Spawn_CensorB.h"
#include "Idle_CensorB.h"
#include "Dead_CensorB.h"
#include "Run_CensorB.h"
#include "Walk_CensorB.h"
#include "Hit_CensorB.h"
#include "Attack_CensorB.h"
#include "Jump_CensorB.h"
#include "Chase_CensorB.h"
#include "CollisionObject.h"
#include "CensorB_AttTriggerL.h"
#include "CensorB_AttTriggerR.h"
#include "Monster_HpBar.h"
#include "Camera_Fly.h"
CCensor_Burly::CCensor_Burly(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CCensor_Burly::CCensor_Burly(const CCensor_Burly & rhs)
	: CEnemy(rhs)
{
}

HRESULT CCensor_Burly::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CCensor_Burly::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float4x4		WorldMatrix;

	if (nullptr != pArg)
		WorldMatrix = *(_float4x4*)pArg;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_pModelCom->Set_Animation(39);

	m_pModelCom->Set_AnimSpeed(23, 1.0);
	m_pModelCom->Set_AnimSpeed(11, 3.0);
	m_pModelCom->Set_AnimBlendSpeed(21, 0.25);
	/*m_pModelCom->Set_AnimBlendSpeed(11, 20.0);*/

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_pRoot = CSequence::Create();
	m_pSelector = CSelector::Create();
	m_pSpawn_Censor = CSpawn_CensorB::Create(this);
	m_pIdle_Censor = CIdle_CensorB::Create(this);
	m_pDead_Censor = CDead_CensorB::Create(this);
	m_pWalk_Censor = CWalk_CensorB::Create(this);
	m_pRun_Censor = CRun_CensorB::Create(this);
	m_pHit_Censor = CHit_CensorB::Create(this);
	m_pChase_Censor = CChase_CensorB::Create(this);
	m_pJump_Censor = CJump_CensorB::Create(this);
	m_pAttack_Censor = CAttack_CensorB::Create(this);

	m_pRoot->AddChild(m_pSpawn_Censor);
	m_pRoot->AddChild(m_pHit_Censor);
	m_pRoot->AddChild(m_pChase_Censor);
	m_pRoot->AddChild(m_pAttack_Censor);
	m_pRoot->AddChild(m_pIdle_Censor);
	m_pRoot->AddChild(m_pSelector);
	m_pSelector->AddChild(m_pRun_Censor);
	m_pSelector->AddChild(m_pWalk_Censor);
	m_pRoot->AddChild(m_pJump_Censor);
	m_pRoot->AddChild(m_pDead_Censor);

	_vector WorldPos = m_pTransformCom->Get_Position();
	m_pCharacterController->Set_Position(_float3(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos), XMVectorGetZ(WorldPos)));

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_pCombinedMatrixPtr[HAND_R] = m_pModelCom->Get_CombinedMatrixPtr("RightHand");
	m_pCombinedMatrixPtr[HAND_L] = m_pModelCom->Get_CombinedMatrixPtr("LeftHand");
	m_OffSetMatrix[HAND_R] = m_pModelCom->Get_OffsetMatrix("RightHand");
	m_OffSetMatrix[HAND_L] = m_pModelCom->Get_OffsetMatrix("LeftHand");

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	XMStoreFloat4x4(&m_AttMatrix[HAND_L], XMMatrixIdentity());
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_CensorBAttTriggerL"), m_iCurrentScene, TEXT("Layer_AttTriggerL"), &m_pAttTriggerL, &m_AttMatrix[HAND_L]);

	XMStoreFloat4x4(&m_AttMatrix[HAND_R], XMMatrixIdentity());
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_CensorBAttTriggerR"), m_iCurrentScene, TEXT("Layer_AttTriggerR"), &m_pAttTriggerR, &m_AttMatrix[HAND_R]);

	CMonster_HpBar::MONSTERHPBARINFO tMonsterHpBarInfo;
	ZeroMemory(&tMonsterHpBarInfo, sizeof(CMonster_HpBar::MONSTERHPBARINFO));
	tMonsterHpBarInfo.m_pParentHp = &m_iHp;
	tMonsterHpBarInfo.m_pParentMatrix = &m_HpBarMatrix;
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterHpBar"), m_iCurrentScene, TEXT("Layer_CensorHpBar"), &m_pHpBar, &tMonsterHpBarInfo);


	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_CencerB_Left_Trail"), m_iCurrentScene, TEXT("Layer_CencerB_Left_Trail"), &m_pLeftHandTrail, nullptr)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
	Safe_AddRef(m_pLeftHandTrail);

	if (FAILED(pGameInstance->Effect_ParentBoneMatrix(m_iCurrentScene, TEXT("Layer_CencerB_Left_Trail"), m_pTransformCom, m_pModelCom, "LeftHand")))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_CencerB_Right_Trail"), m_iCurrentScene, TEXT("Layer_CencerB_Right_Trail"), &m_pRightHandTrail, nullptr)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
	Safe_AddRef(m_pRightHandTrail);

	if (FAILED(pGameInstance->Effect_ParentBoneMatrix(m_iCurrentScene, TEXT("Layer_CencerB_Right_Trail"), m_pTransformCom, m_pModelCom, "RightHand")))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	_vector vPos = XMVectorZero();
	vPos = XMVectorSetW(vPos, 1.f);
	vPos = XMVectorSetY(vPos, 1.5f);
	/* For. SpawnSmokeCencerB */
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		m_iCurrentScene, TEXT("Layer_SpawnSmokeCencerB"),
		SCENE_STATIC, TEXT("Effect_SpawnSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Effect_Call_WorldPos(m_iCurrentScene, TEXT("Layer_SpawnSmokeCencerB"), m_pTransformCom->Get_Position() + vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		m_iCurrentScene, TEXT("Layer_StarSpownRectCencerB"),
		SCENE_STATIC, TEXT("Effect_StarSpownRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Effect_Call_WorldPos(m_iCurrentScene, TEXT("Layer_StarSpownRectCencerB"), m_pTransformCom->Get_Position() + vPos, true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CCensor_Burly::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (m_IsHit)
		m_pAttack_Censor->Reset_AttackState();

	if (m_IsAttack[HAND_R])
		m_IsAttack[HAND_R] = false;

	if (m_IsAttack[HAND_L])
		m_IsAttack[HAND_L] = false;

	Update_AttMatrix();

	m_pTransformCom->Set_Velocity(XMVectorZero());

	if (!m_pCharacterController->IsDown() && m_IsGravity)
		m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -0.04f);

	if (true == m_pRoot->Invoke(TimeDelta, m_pModelCom, m_pTransformCom, m_pCharacterController))
	{
		m_pHitColliderCom->Remove_Actor();
		m_IsReadyDead = true;
	}

	m_pModelCom->Lerp_Update(TimeDelta);

	if (m_IsLeftHandTrail)
	{
		//왼손 트레일 업데이트함수
		m_pLeftHandTrail->Set_IsRender(true);
	}
	else
	{
		m_pLeftHandTrail->Set_IsRender(false);
	}

	if (m_IsRightHandTrail)
	{
		//오른손 트레일 업데이트 함수
		m_pRightHandTrail->Set_IsRender(true);
	}
	else
	{
		m_pRightHandTrail->Set_IsRender(false);
	}

	_matrix	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] += XMVectorSet(0.f, 2.5f, 0.f, 0.f);
	XMStoreFloat4x4(&m_HpBarMatrix, WorldMatrix);

	m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());

	m_pCharacterController->Update_Transform();

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f);

	if (!m_isDestroy||!m_IsReadyDead)
	{
		m_pHitColliderCom->Update_TransformPhysX(WorldMatrix);

		if (m_IsAttack[HAND_L])
			((CCensorB_AttTriggerL*)m_pAttTriggerL)->Attack(true);
		else
			((CCensorB_AttTriggerL*)m_pAttTriggerL)->Attack(false);

		if (m_IsAttack[HAND_R])
			((CCensorB_AttTriggerR*)m_pAttTriggerR)->Attack(true);
		else
			((CCensorB_AttTriggerR*)m_pAttTriggerR)->Attack(false);
	}

	return _int();
}

_int CCensor_Burly::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_IsReadyDead)
	{
		if (nullptr != m_pLeftHandTrail)
		{
			m_pLeftHandTrail->Set_IsDestroy(true);
			Safe_Release(m_pLeftHandTrail);
			m_pLeftHandTrail = nullptr;
		}

		if (nullptr != m_pRightHandTrail)
		{
			m_pRightHandTrail->Set_IsDestroy(true);
			Safe_Release(m_pRightHandTrail);
			m_pRightHandTrail = nullptr;
		}

		if (m_pAttTriggerL)
		{
			m_pAttTriggerL->Set_IsDestroy(true);
			m_pAttTriggerL = nullptr;
		}
		if (m_pAttTriggerR)
		{
			m_pAttTriggerR->Set_IsDestroy(true);
			m_pAttTriggerR = nullptr;
		}
		if (m_pHpBar)
		{
			m_pHpBar->Set_IsDestroy(true);
			m_pHpBar = nullptr;
		}
		m_isDestroy = true;
	}

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CCensor_Burly::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_bool		bArr[3] = { true, false, false };
	m_pModelCom->Render_IsEmissiveMapping(0, bArr);

	return S_OK;
}

void CCensor_Burly::OnTriggerEnter(CCollisionObject & collision)
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

void CCensor_Burly::OnCollisionEnter(CCollisionObject & collision)
{
}

void CCensor_Burly::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CCensor_Burly::OnTriggerExit(CCollisionObject & collision)
{
}

void CCensor_Burly::Set_Gravity(_bool _IsGravity)
{
	m_IsGravity = _IsGravity;
}

void CCensor_Burly::SetOnTrail_LeftHand(_bool IsOnOff)
{
	m_IsLeftHandTrail = IsOnOff;
}

void CCensor_Burly::SetOnTrail_RightHand(_bool IsOnOff)
{
	m_IsRightHandTrail = IsOnOff;
}

void CCensor_Burly::Update_AttMatrix()
{
	XMStoreFloat4x4(&m_AttMatrix[HAND_R], XMLoadFloat4x4(&m_OffSetMatrix[HAND_R])*XMLoadFloat4x4(m_pCombinedMatrixPtr[HAND_R])*XMLoadFloat4x4(&m_PivotMatrix)*m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_AttMatrix[HAND_L], XMLoadFloat4x4(&m_OffSetMatrix[HAND_L])*XMLoadFloat4x4(m_pCombinedMatrixPtr[HAND_L])*XMLoadFloat4x4(&m_PivotMatrix)*m_pTransformCom->Get_WorldMatrix());
}

void CCensor_Burly::Update_LookVector(_double TimeDelta)
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

void CCensor_Burly::Attack(HANDTYPE eHandType, _bool _IsAttack)
{
	m_IsAttack[eHandType] = _IsAttack;
}


HRESULT CCensor_Burly::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_CensorBurly0"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
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
	tCharacterControllerDesc.fHeight = 0.5f;
	tCharacterControllerDesc.fRadius = 0.5f;
	tCharacterControllerDesc.fStepOffset = 0.3f;
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
	tBoxColliderDesc.vScale = { 2.f,2.f,2.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pHitColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pHitColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER);

	return S_OK;
}

HRESULT CCensor_Burly::SetUp_ConstantTable()
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

CCensor_Burly * CCensor_Burly::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCensor_Burly*	pInstance = new CCensor_Burly(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCensor_Burly::Clone(void * pArg, _uint iCurrentScene)
{
	CCensor_Burly*	pInstance = new CCensor_Burly(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCensor_Burly::Free()
{
	__super::Free();
	Safe_Release(m_pRoot);
	Safe_Release(m_pSelector);
	Safe_Release(m_pSpawn_Censor);
	Safe_Release(m_pIdle_Censor);
	Safe_Release(m_pDead_Censor);
	Safe_Release(m_pWalk_Censor);
	Safe_Release(m_pRun_Censor);
	Safe_Release(m_pHit_Censor);
	Safe_Release(m_pChase_Censor);
	Safe_Release(m_pJump_Censor);
	Safe_Release(m_pAttack_Censor);
	Safe_Release(m_pHitColliderCom);

	if (m_pHpBar)
	{
		m_pHpBar->Set_IsDestroy(true);
	}

	if (nullptr != m_pRightHandTrail)
		Safe_Release(m_pRightHandTrail);

	if (nullptr != m_pLeftHandTrail)
		Safe_Release(m_pLeftHandTrail);

}
