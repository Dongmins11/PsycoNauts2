#include "stdafx.h"
#include "..\public\GoatOtto.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Sequence.h"
#include "Taunt_GoatO.h"
#include "Attack_GoatO.h"
#include "Idle_GoatO.h"
#include "Hit_GoatO.h"
#include "Dead_GoatO.h"
#include "Spawn_GoatO.h"
#include "Boss_Goat.h"
#include "AttTrigger_GoatO.h"

CGoatOtto::CGoatOtto(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CGoatOtto::CGoatOtto(const CGoatOtto & rhs)
	: CEnemy(rhs)
{
}

HRESULT CGoatOtto::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CGoatOtto::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CBoss_Goat::BOSSGOATINFO tBossGoatInfo;
		ZeroMemory(&tBossGoatInfo, sizeof(CBoss_Goat::BOSSGOATINFO));
		tBossGoatInfo = *(CBoss_Goat::BOSSGOATINFO*)pArg;
		m_pParent = tBossGoatInfo.pParent;
		m_pTransformCom->Set_WorldMatrix(tBossGoatInfo.WorldMatrix);
	}

	m_pModelCom->Set_Animation(8);
	m_pModelCom->Set_AnimSpeed(2, 3.0);
	m_pModelCom->Set_AnimSpeed(1, 1.5);
	m_pRoot = CSequence::Create();
	m_pAttackGoat = CAttack_GoatO::Create(this);
	m_pDeadGoat = CDead_GoatO::Create(this);
	m_pHitGoat = CHit_GoatO::Create(this);
	m_pIdleGoat = CIdle_GoatO::Create(this);
	m_pTauntGoat = CTaunt_GoatO::Create(this);
	m_pSpawnGoat = CSpawn_GoatO::Create(this);

	m_pRoot->AddChild(m_pSpawnGoat);
	m_pRoot->AddChild(m_pHitGoat);
	m_pRoot->AddChild(m_pTauntGoat);
	m_pRoot->AddChild(m_pAttackGoat);
	m_pRoot->AddChild(m_pIdleGoat);
	m_pRoot->AddChild(m_pDeadGoat);

	m_pCombinedMatrixPtr = m_pModelCom->Get_CombinedMatrixPtr("MouthPoint");
	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_OffsetMatrix = m_pModelCom->Get_OffsetMatrix("MouthPoint");

	m_pAttTrigger = CAttTrigger_GoatO::Create(m_pDevice, m_pDeviceContext, &m_BoneMatrix);

	m_iHp = 200;

	return S_OK;
}

_int CGoatOtto::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pModelCom->Lerp_Update(TimeDelta);

	if (true == m_pRoot->Invoke(TimeDelta, m_pModelCom, m_pTransformCom, nullptr))
		m_isDestroy = true;

	XMStoreFloat4x4(&m_BoneMatrix, XMLoadFloat4x4(m_pCombinedMatrixPtr)*XMLoadFloat4x4(&m_PivotMatrix)*m_pTransformCom->Get_WorldMatrix());

	m_pHitColliderCom->Update_TransformPhysX(XMLoadFloat4x4(&m_BoneMatrix));

	m_pAttTrigger->Tick(TimeDelta);

	return _int();
}

_int CGoatOtto::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pAttTrigger->LateTick(TimeDelta);

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CGoatOtto::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CGoatOtto::Reset()
{
	m_pHitGoat->ResetHit();
	m_pAttackGoat->Reset_AttackState();
}

void CGoatOtto::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_SKILL)
	{
		m_pHitGoat->ResetHit();
		Attacked(CEnemy::ATTACKED::B);
		Damaged(10.f);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		((CBoss_Goat*)pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_FinalBoss"), 0))->Damaged(2);

		/* For. Layer_BlueSpark 捞棋飘 积己 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"),
			SCENE_STATIC, TEXT("Effect_FoodHitEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"), collision.Get_ObjectTransform()->Get_Position(), true)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_LocalBillboard(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"), TEXT("Com_Option"), true);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_FoodDieStar"),
			SCENE_STATIC, TEXT("Effect_Boss_FoodDieStar"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_FoodDieStar"), collision.Get_ObjectTransform()->Get_Position(), true)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

	}
	else if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_BOSS_DISH)
	{
		m_pHitGoat->ResetHit();
		Attacked(CEnemy::ATTACKED::B);
		Damaged(200.f);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		/* For. Layer_BlueSpark 捞棋飘 积己 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"),
			SCENE_STATIC, TEXT("Effect_FoodHitEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"), collision.Get_ObjectTransform()->Get_Position(), true)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pGameInstance->Setting_Effect_LocalBillboard(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FoodHitEffect"), TEXT("Com_Option"), true);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_FoodDieStar"),
			SCENE_STATIC, TEXT("Effect_Boss_FoodDieStar"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_FoodDieStar"), collision.Get_ObjectTransform()->Get_Position(), true)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

	}
}

void CGoatOtto::OnCollisionEnter(CCollisionObject & collision)
{
}

void CGoatOtto::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CGoatOtto::OnTriggerExit(CCollisionObject & collision)
{
}

void CGoatOtto::Update_LookVector(_double TimeDelta)
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

void CGoatOtto::Update_BoneMatrix()
{
}

void CGoatOtto::LockBosses(_bool * _bArr)
{
	static_cast<CBoss_Goat*>(m_pParent)->Lock_Boss(_bArr);
}

HRESULT CGoatOtto::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_GoatOtto0"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
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
	tBoxColliderDesc.vScale = { 7.f,7.f,7.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pHitColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pHitColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER);

	return S_OK;
}

HRESULT CGoatOtto::SetUp_ConstantTable()
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

CGoatOtto * CGoatOtto::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CGoatOtto*	pInstance = new CGoatOtto(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg, SCENE_STATIC)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGoatOtto::Clone(void * pArg, _uint iCurrentScene)
{
	return nullptr;
}

void CGoatOtto::Free()
{
	__super::Free();
	Safe_Release(m_pAttTrigger);
	Safe_Release(m_pRoot);
	Safe_Release(m_pSpawnGoat);
	Safe_Release(m_pAttackGoat);
	Safe_Release(m_pDeadGoat);
	Safe_Release(m_pHitGoat);
	Safe_Release(m_pIdleGoat);
	Safe_Release(m_pTauntGoat);
	if (m_pHitColliderCom)
	{
		m_pHitColliderCom->Remove_Actor();
		Safe_Release(m_pHitColliderCom);
	}
}
