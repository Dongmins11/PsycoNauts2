#include "stdafx.h"
#include "..\public\Judge.h"
#include "GameInstance.h"
#include "Sequence.h"
#include "Selector.h"
#include "Spawn_Judge.h"
#include "Chase_Judge.h"
#include "Stolen_Judge.h"
#include "Idle_Judge.h"
#include "Idle_Judge_UA.h"
#include "Hit_Judge.h"
#include "Hit_Judge_UA.h"
#include "Dead_Judge.h"
#include "Dead_Judge_UA.h"
#include "Walk_Judge.h"
#include "Walk_Judge_UA.h"
#include "Run_Judge.h"
#include "Run_Judge_UA.h"
#include "Jump_Judge.h"
#include "Jump_Judge_UA.h"
#include "Attack_Judge.h"
#include "Attack_Judge_UA.h"
#include "Judge_Weapon.h"
#include "Judge_Weaponhead.h"
#include "Judge_AttTrigger.h"
#include "JudgeWeapon_AttTrigger.h"
#include "CollisionObject.h"
#include "BossHpBar.h"

CJudge::CJudge(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CJudge::CJudge(const CJudge & rhs)
	: CEnemy(rhs)
{
}

HRESULT CJudge::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CJudge::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float4x4 WorldMatrix;

	if (nullptr != pArg)
	{
		JUDGEINFO tagJudgeInfo;
		ZeroMemory(&tagJudgeInfo, sizeof(JUDGEINFO));

		tagJudgeInfo = *(JUDGEINFO*)pArg;
		m_eType = tagJudgeInfo.eMeshType;
		WorldMatrix = tagJudgeInfo.WorldMatrix;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);


	m_pCombinedMatrix[BONE_PROPHAND] = m_pModelCom->Get_CombinedMatrixPtr("Weapon");
	m_OffSetMatrix[BONE_PROPHAND] = m_pModelCom->Get_OffsetMatrix("Weapon");
	m_PivotMatrix[BONE_PROPHAND] = m_pModelCom->Get_PivotMatrix();

	m_pCombinedMatrix[BONE_WEAPONHEAD] = m_pModelCom->Get_CombinedMatrixPtr("WeaponCollider");
	m_OffSetMatrix[BONE_WEAPONHEAD] = m_pModelCom->Get_OffsetMatrix("WeaponCollider");
	m_PivotMatrix[BONE_WEAPONHEAD] = m_pModelCom->Get_PivotMatrix();


	m_pModelCom->Set_Animation(51);
	m_pModelCom->Set_AnimSpeed(51, 0.5);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_pRoot = CSequence::Create();
	m_pArmedSeq = CSequence::Create();
	m_pUnArmedSeq = CSequence::Create();
	m_pSelector = CSelector::Create();
	m_pArmedMove = CSelector::Create();
	m_pUnArmedMove = CSelector::Create();
	m_pSpawnJudge = CSpawn_Judge::Create(this);
	m_pChaseJudge = CChase_Judge::Create(this);
	m_pStolenJudge = CStolen_Judge::Create(this);
	m_pIdleJudge = CIdle_Judge::Create(this);
	m_pIdleJudgeUA = CIdle_Judge_UA::Create(this);
	m_pHitJudge = CHit_Judge::Create(this);
	m_pHitJudgeUA = CHit_Judge_UA::Create(this);
	m_pDeadJudge = CDead_Judge::Create(this);
	m_pDeadJudgeUA = CDead_Judge_UA::Create(this);
	m_pWalkJudge = CWalk_Judge::Create(this);
	m_pWalkJudgeUA = CWalk_Judge_UA::Create(this);
	m_pRunJudge = CRun_Judge::Create(this);
	m_pRunJudgeUA = CRun_Judge_UA::Create(this);
	m_pJumpJudge = CJump_Judge::Create(this);
	m_pJumpJudgeUA = CJump_Judge_UA::Create(this);
	m_pAttackJudge = CAttack_Judge::Create(this);
	m_pAttackJudgeUA = CAttack_Judge_UA::Create(this);

	m_pRoot->AddChild(m_pSpawnJudge);
	m_pRoot->AddChild(m_pStolenJudge);
	m_pRoot->AddChild(m_pSelector);
	m_pSelector->AddChild(m_pArmedSeq);
	m_pArmedSeq->AddChild(m_pHitJudge);
	m_pArmedSeq->AddChild(m_pChaseJudge);
	m_pArmedSeq->AddChild(m_pAttackJudge);
	m_pArmedSeq->AddChild(m_pIdleJudge);
	m_pArmedSeq->AddChild(m_pArmedMove);
	m_pArmedMove->AddChild(m_pRunJudge);
	m_pArmedMove->AddChild(m_pWalkJudge);
	m_pArmedSeq->AddChild(m_pJumpJudge);
	m_pArmedSeq->AddChild(m_pDeadJudge);
	m_pSelector->AddChild(m_pUnArmedSeq);
	m_pUnArmedSeq->AddChild(m_pHitJudgeUA);
	m_pUnArmedSeq->AddChild(m_pChaseJudge);
	m_pUnArmedSeq->AddChild(m_pAttackJudgeUA);
	m_pUnArmedSeq->AddChild(m_pIdleJudgeUA);
	m_pUnArmedSeq->AddChild(m_pUnArmedMove);
	m_pUnArmedMove->AddChild(m_pRunJudgeUA);
	m_pUnArmedMove->AddChild(m_pWalkJudgeUA);
	m_pUnArmedSeq->AddChild(m_pJumpJudgeUA);
	m_pUnArmedSeq->AddChild(m_pDeadJudgeUA);

	_vector WorldPos = m_pTransformCom->Get_Position();
	m_pCharacterController->Set_Position(_float3(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos), XMVectorGetZ(WorldPos)));

	CJudge_Weapon::WEAPONINFO tWeaponInfo;
	ZeroMemory(&tWeaponInfo, sizeof(CJudge_Weapon::WEAPONINFO));

	tWeaponInfo.pParentMatrix = &m_BoneMatirx[BONE_PROPHAND];
	tWeaponInfo.iWeaponType = m_eType;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeWeapon"), m_iCurrentScene, TEXT("Layer_JudgeWeapon"), &m_pWeapon, &tWeaponInfo);
	Safe_AddRef(m_pWeapon);

	CJudge_Weaponhead::WEAPONINFO tWeaponheadInfo;
	ZeroMemory(&tWeaponheadInfo, sizeof(CJudge_Weaponhead::WEAPONINFO));

	tWeaponheadInfo.pParentMatrix = &m_BoneMatirx[BONE_WEAPONHEAD];
	tWeaponheadInfo.iWeaponType = m_eType;
	tWeaponheadInfo.pParent = this;

	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeWeaponhead"), m_iCurrentScene, TEXT("Layer_JudgeWeaponhead"), &m_pWeaponhead, &tWeaponheadInfo);
	Safe_AddRef(m_pWeaponhead);

	XMStoreFloat4x4(&m_BoneMatirx[BONE_ROOT], XMMatrixIdentity());
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeAttTrigger"), m_iCurrentScene, TEXT("Layer_JudgeAttTrigger"), &m_pAttTrigger, &m_BoneMatirx[BONE_ROOT]);
	Safe_AddRef(m_pAttTrigger);
	
	XMStoreFloat4x4(&m_BoneMatirx[BONE_WEAPONHEAD], XMMatrixIdentity());
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeWeaponAttTrigger"), m_iCurrentScene, TEXT("Layer_JudgeWeaponAttTrigger"), &m_pWeaponAttTrigger, &m_BoneMatirx[BONE_WEAPONHEAD]);
	Safe_AddRef(m_pWeaponAttTrigger);

	m_pCharacterController->Update_Transform();

	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Judge_Trail"), m_iCurrentScene, TEXT("Layer_Judge_Trail"), &m_pTrail, nullptr)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
	Safe_AddRef(m_pTrail);
	
	if (FAILED(pGameInstance->Effect_ParentBoneMatrix(m_iCurrentScene, TEXT("Layer_Judge_Trail"), m_pTransformCom, m_pModelCom, "Weapon")))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		m_iCurrentScene, TEXT("Layer_JudgeSpawnPaper"),
		SCENE_STATIC, TEXT("Effect_JudgeSpawnPaper"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	if (FAILED(pGameInstance->Effect_Call_WorldPos(m_iCurrentScene, TEXT("Layer_JudgeSpawnPaper"), m_pTransformCom->Get_Position() + XMVectorSet(0.f, 2.f, 0.f, 0.f), true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
	

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_PointParticle"),
		m_iCurrentScene, TEXT("Layer_JudgeSpawnSmoke"),
		SCENE_STATIC, TEXT("Effect_JudgeSpawnSmoke"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	if (FAILED(pGameInstance->Effect_Call_WorldPos(m_iCurrentScene, TEXT("Layer_JudgeSpawnSmoke"), m_pTransformCom->Get_Position() + XMVectorSet(-1.f, 2.f, 0.f, 0.f), true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
	
	if (TYPE_DEFAULT == m_eType)
	{
		CBossHpBar::MONSTERHPBARINFO tHpBarInfo;
		ZeroMemory(&tHpBarInfo, sizeof(CBossHpBar::MONSTERHPBARINFO));

		tHpBarInfo.m_pParentMatrix = &m_HpBarMatrix;
		tHpBarInfo.m_pParentHp = &m_iHp;

		pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_BossHpBar"), m_iCurrentScene, TEXT("Layer_HpBar"), &m_pHpBar, &tHpBarInfo);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CJudge::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (m_IsHit)
	{
		m_pAttackJudge->Reset_AttackState();
		m_pAttackJudgeUA->Reset_AttackState();
	}

	if (m_IsAttack[ARMED])
	{
		m_IsAttack[ARMED] = false;
	}
	if (m_IsAttack[UNARMED])
	{
		m_IsAttack[UNARMED] = false;
	}

	m_pTransformCom->Set_Velocity(XMVectorZero());

	if (!m_pCharacterController->IsDown())
		m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -4.9f);

	if (true == m_pRoot->Invoke(TimeDelta, m_pModelCom, m_pTransformCom, m_pCharacterController))
		m_IsReadyDead = true;

	m_pModelCom->Lerp_Update(TimeDelta);

	if(19!=m_pModelCom->Get_AnimIndex()&&
		61!=m_pModelCom->Get_AnimIndex())
	m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());

	m_pCharacterController->Update_Transform();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] += m_pTransformCom->Get_State(CTransform::STATE_UP);
	m_pHitColliderCom->Update_TransformPhysX(WorldMatrix);

	if (m_IsAttack[ARMED])
	{
		if (nullptr != m_pWeaponAttTrigger)
		{
			((CJudgeWeapon_AttTrigger*)m_pWeaponAttTrigger)->Attack(true);
			((CJudgeWeapon_AttTrigger*)m_pWeaponAttTrigger)->Set_ColliderDir(XMLoadFloat4(&m_vColliderDir));
		}
	}
	else
	{
		if (nullptr != m_pWeaponAttTrigger)
			((CJudgeWeapon_AttTrigger*)m_pWeaponAttTrigger)->Attack(false);
	}
	if (m_IsAttack[UNARMED])
	{
		if (nullptr != m_pAttTrigger)
			((CJudge_AttTrigger*)m_pAttTrigger)->Attack(true);
		//여기에 보스주변으로 종이날라다니는 이펙트 보스기준 주변 범위공격임.
		//여기에 토네이도 같은 이펙트랑 종이이펙트 같이 넣으면 좋을 듯?
	}
	else
	{
		if (nullptr != m_pAttTrigger)
			((CJudge_AttTrigger*)m_pAttTrigger)->Attack(false);
	}

	Update_BoneMatrix();

	if (m_IsTrailRender)
	{
		//트레일 Update돌리기
		//m_BoneMatirx[BONE_PROPHAND] 손위치 
		//m_BoneMatirx[BONE_WEAPONHEAD]  무기 대가리 위치
		if(nullptr != m_pTrail)
			m_pTrail->Set_IsRender(true);
	}
	else
	{
		if (nullptr != m_pTrail)
			m_pTrail->Set_IsRender(false);
	}

	return _int();
}

_int CJudge::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_IsReadyDead)
	{
		//if (!m_IsReadyDestroy)
		//{
		//	m_IsReadyDestroy = true;
		//}
		//else
		//{
			if (nullptr != m_pWeapon)
			{
				m_pWeapon->Set_IsDestroy(true);
				Safe_Release(m_pWeapon);
				m_pWeapon = nullptr;
			}

			if (nullptr != m_pWeaponhead)
			{
				m_pWeaponhead->Set_IsDestroy(true);
				Safe_Release(m_pWeaponhead);
				m_pWeaponhead = nullptr;
			}

			if (nullptr != m_pTrail)
			{
				m_pTrail->Set_IsDestroy(true);
				Safe_Release(m_pTrail);
				m_pTrail = nullptr;
			}

			if (nullptr != m_pAttTrigger)
			{
				m_pAttTrigger->Set_IsDestroy(true);
				Safe_Release(m_pAttTrigger);
				m_pAttTrigger = nullptr;
			}
			if (nullptr != m_pWeaponAttTrigger)
			{
				m_pWeaponAttTrigger->Set_IsDestroy(true);
				Safe_Release(m_pWeaponAttTrigger);
				m_pWeaponAttTrigger = nullptr;
			}

			m_isDestroy = true;
		//}
	}

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CJudge::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_bool		bArr[3] = { true, false, false };
	m_pModelCom->Render_IsEmissiveMapping(0, bArr);

	//if(m_IsTrailRender)
	//	//트레일렌더
	return S_OK;
}

void CJudge::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_SKILL)
	{
		if (collision.Get_Object()->Get_IsPulling())
		{
			if (nullptr != m_pWeapon)
			{
				if (!((CJudge_Weapon*)m_pWeapon)->IsDrop() &&
					!((CJudge_Weapon*)m_pWeapon)->IsRotate())
				{
					if (nullptr != m_pTrail)
						m_pTrail->Set_IsRender(false);
					Attacked(CEnemy::ATTACKED::FW);
				}
			}
		}
		Damaged(10);
	}
	else if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PICK)
	{
		if (collision.Get_Object()->Get_IsThrowing())
		{
			if (nullptr != m_pTrail)
				m_pTrail->Set_IsRender(false);
			Attacked(CEnemy::ATTACKED::TO);
			Damaged(5);
		}
	}
}

void CJudge::OnCollisionEnter(CCollisionObject & collision)
{
}

void CJudge::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CJudge::OnTriggerExit(CCollisionObject & collision)
{
}

void CJudge::Update_LookVector(_double TimeDelta)
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

void CJudge::SetOn_RotateWeapon(_bool _bRotate)
{
	if(nullptr != m_pWeapon)
		((CJudge_Weapon*)m_pWeapon)->SetOn_Rotate(_bRotate);
}

void CJudge::Set_Armed(_bool _bArmed)
{
	m_IsArmed = _bArmed;
}

void CJudge::SetOn_Drop(_bool _bDrop)
{
	if (nullptr != m_pWeapon)
		((CJudge_Weapon*)m_pWeapon)->Set_Drop(_bDrop);
}

void CJudge::Destroy_Weapon()
{
	if (nullptr != m_pWeapon&&
		nullptr!= m_pWeaponhead)
	{
		m_pWeapon->Set_IsDestroy(true);
		Safe_Release(m_pWeapon);
		m_pWeapon = nullptr;
		Safe_Release(m_pWeaponhead);
		m_pWeaponhead = nullptr;
		Set_Armed(false);
	}
}

const _bool& CJudge::IsArmed() const
{
	return m_IsArmed;
}

const _float4x4 * CJudge::Get_PropHandMatrixPtr() const
{
	return &m_BoneMatirx[BONE_PROPHAND];
}

const _float4x4 * CJudge::Get_WeaponMatrixPtr() const
{
	return &m_BoneMatirx[BONE_WEAPONHEAD];
}

void CJudge::Attack(ARMEDTYPE eType, _bool IsAttack)
{
	m_IsAttack[eType] = IsAttack;
}

void CJudge::Render_Trail(_bool _IsTrail)
{
	m_IsTrailRender = _IsTrail;
}

CJudge::TYPE CJudge::Get_Type()
{
	return m_eType;
}


HRESULT CJudge::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_tchar szModelPrototypeTag[MAX_PATH] = TEXT("");
	swprintf_s(szModelPrototypeTag, MAX_PATH, TEXT("Prototype_Component_Model_Judge%d"), m_eType);

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, szModelPrototypeTag, TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
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
	tCharacterControllerDesc.fHeight = 1.f;
	tCharacterControllerDesc.fRadius = 0.7f;
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
	tBoxColliderDesc.vScale = { 2.f,2.f,2.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pHitColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pHitColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER);

	return S_OK;
}

HRESULT CJudge::SetUp_ConstantTable()
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

void CJudge::Update_BoneMatrix()
{
	XMStoreFloat4x4(&m_BoneMatirx[BONE_PROPHAND], XMLoadFloat4x4(m_pCombinedMatrix[BONE_PROPHAND])*XMLoadFloat4x4(&m_PivotMatrix[BONE_PROPHAND])*m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_BoneMatirx[BONE_WEAPONHEAD], XMLoadFloat4x4(m_pCombinedMatrix[BONE_WEAPONHEAD])*XMLoadFloat4x4(&m_PivotMatrix[BONE_WEAPONHEAD])*m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_BoneMatirx[BONE_ROOT], m_pTransformCom->Get_WorldMatrix());

	if (15 == m_pModelCom->Get_AnimIndex())
	{
		_vector vCurPos = XMLoadFloat4((_float4*)m_BoneMatirx[BONE_WEAPONHEAD].m[3]);
		_vector vCalculateDir = vCurPos - XMLoadFloat3(&m_vPrePos);
		XMStoreFloat3(&m_vPrePos, vCurPos);
		vCalculateDir = XMVector3Normalize(vCalculateDir);
		XMStoreFloat4(&m_vColliderDir, XMVectorSet(XMVectorGetX(vCalculateDir), XMVectorGetY(vCalculateDir), XMVectorGetZ(vCalculateDir), 1.f));
	}
	else if (14 == m_pModelCom->Get_AnimIndex())
	{
		_vector vCurPos = XMLoadFloat4((_float4*)m_BoneMatirx[BONE_WEAPONHEAD].m[3]);
		XMStoreFloat3(&m_vPrePos, vCurPos);
	}
}

CJudge * CJudge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CJudge*	pInstance = new CJudge(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJudge::Clone(void * pArg, _uint iCurrentScene)
{
	CJudge*	pInstance = new CJudge(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJudge::Free()
{
	Safe_Release(m_pRoot);
	Safe_Release(m_pSelector);
	Safe_Release(m_pArmedMove);
	Safe_Release(m_pUnArmedMove);
	Safe_Release(m_pArmedSeq);
	Safe_Release(m_pUnArmedSeq);
	Safe_Release(m_pSpawnJudge);
	Safe_Release(m_pChaseJudge);
	Safe_Release(m_pStolenJudge);
	Safe_Release(m_pIdleJudge);
	Safe_Release(m_pIdleJudgeUA);
	Safe_Release(m_pHitJudge);
	Safe_Release(m_pHitJudgeUA);
	Safe_Release(m_pDeadJudge);
	Safe_Release(m_pDeadJudgeUA);
	Safe_Release(m_pWalkJudge);
	Safe_Release(m_pWalkJudgeUA);
	Safe_Release(m_pRunJudge);
	Safe_Release(m_pRunJudgeUA);
	Safe_Release(m_pJumpJudge);
	Safe_Release(m_pJumpJudgeUA);
	Safe_Release(m_pAttackJudge);
	Safe_Release(m_pAttackJudgeUA);

	if (nullptr != m_pHitColliderCom)
		m_pHitColliderCom->Remove_Actor();

	Safe_Release(m_pHitColliderCom);

	if (nullptr != m_pWeapon)
	{
		m_pWeapon->Set_IsDestroy(true);
		Safe_Release(m_pWeapon);
		m_pWeapon = nullptr;
	}

	if (nullptr != m_pWeaponhead)
	{
		m_pWeaponhead->Set_IsDestroy(true);
		Safe_Release(m_pWeaponhead);
		m_pWeaponhead = nullptr;
	}

	if (nullptr != m_pTrail)
	{
		m_pTrail->Set_IsDestroy(true);
		Safe_Release(m_pTrail);
		m_pTrail = nullptr;
	}

	if (nullptr != m_pAttTrigger)
	{
		m_pAttTrigger->Set_IsDestroy(true);
		Safe_Release(m_pAttTrigger);
		m_pAttTrigger = nullptr;
	}
	if (nullptr != m_pWeaponAttTrigger)
	{
		m_pWeaponAttTrigger->Set_IsDestroy(true);
		Safe_Release(m_pWeaponAttTrigger);
		m_pWeaponAttTrigger = nullptr;
	}
	__super::Free();
}
