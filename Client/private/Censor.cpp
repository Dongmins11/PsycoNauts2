#include "stdafx.h"
#include "..\public\Censor.h"
#include "GameInstance.h"
#include "Sequence.h"
#include "Selector.h"
#include "Spawn_Censor.h"
#include "Idle_Censor.h"
#include "Dead_Censor.h"
#include "Run_Censor.h"
#include "Walk_Censor.h"
#include "Hit_Censor.h"
#include "Attack_Censor.h"
#include "Jump_Censor.h"
#include "Chase_Censor.h"
#include "Censor_AttTrigger.h"
#include "CollisionObject.h"
#include "Monster_HpBar.h"
#include "QuestManager.h"
#include "Camera_Fly.h"
#include "Player.h"
CCensor::CCensor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CCensor::CCensor(const CCensor & rhs)
	: CEnemy(rhs)
{
}

HRESULT CCensor::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CCensor::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float4x4		WorldMatrix;

	if (nullptr != pArg)
	{
		CENSORINFO tagInfo;
		ZeroMemory(&tagInfo, sizeof(CENSORINFO));
		tagInfo = *(CENSORINFO*)pArg;
		WorldMatrix = tagInfo.WorldMatrix;
		m_iMeshType = tagInfo.iMeshType;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_IsGravity = true;
	m_pModelCom->Set_Animation(0);

	m_IsGravity = false;

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_pRoot = CSequence::Create();
	m_pSelector = CSelector::Create();
	m_pSpawn_Censor = CSpawn_Censor::Create(this);
	m_pIdle_Censor = CIdle_Censor::Create(this);
	m_pDead_Censor = CDead_Censor::Create(this);
	m_pWalk_Censor = CWalk_Censor::Create(this);
	m_pRun_Censor = CRun_Censor::Create(this);
	m_pHit_Censor = CHit_Censor::Create(this);
	m_pChase_Censor = CChase_Censor::Create(this);
	m_pJump_Censor = CJump_Censor::Create(this);
	m_pAttack_Censor = CAttack_Censor::Create(this);

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

	m_WeaponOffsetMatrix = m_pModelCom->Get_OffsetMatrix("Weapon");
	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_pCombinedMatrixptr = m_pModelCom->Get_CombinedMatrixPtr("Weapon");

	_vector WorldPos = m_pTransformCom->Get_Position();
	m_pCharacterController->Set_Position(_float3(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos)+0.8f, XMVectorGetZ(WorldPos)));
	if (0 == m_iMeshType)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_iHp = 100;
		CMonster_HpBar::MONSTERHPBARINFO tMonsterHpBarInfo;
		ZeroMemory(&tMonsterHpBarInfo, sizeof(CMonster_HpBar::MONSTERHPBARINFO));
		tMonsterHpBarInfo.m_pParentHp = &m_iHp;
		tMonsterHpBarInfo.m_pParentMatrix = &m_HpBarMatrix;
		pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterHpBar"), m_iCurrentScene, TEXT("Layer_CensorHpBar"), &m_pHpBar, &tMonsterHpBarInfo);

		RELEASE_INSTANCE(CGameInstance);
	}
	else
		m_iHp = 1;


	XMStoreFloat4x4(&m_WeaponMatrix, XMMatrixIdentity());
	m_pAttTrigger = CCensor_AttTrigger::Create(m_pDevice, m_pDeviceContext, &m_WeaponMatrix);

	

	return S_OK;
}

_int CCensor::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
	{
		m_dAccTime += TimeDelta;

		if (20.0 < m_dAccTime)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->IsDefault()&&
				0==pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Judge")))
			{
				_float4x4 SpawnMatrix;
				CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
				CCharacterController* pCCT = (CCharacterController*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Controller"));
				_vector vPos = pCCT->Get_FootPos();
				_uint iRand = rand() % 4;


				switch(iRand) //플레이어 기준 스폰위치
				{
				case 0: //정방향
					vPos += pTransform->Get_State(CTransform::STATE_LOOK)*3.f;
					m_pCharacterController->Set_Position(_float3(XMVectorGetX(vPos), XMVectorGetY(vPos)+1.5f, XMVectorGetZ(vPos)));
					break;
				case 1: //역방향
					vPos += pTransform->Get_State(CTransform::STATE_LOOK)*-3.f;
					m_pCharacterController->Set_Position(_float3(XMVectorGetX(vPos), XMVectorGetY(vPos) + 1.5f, XMVectorGetZ(vPos)));
					break;
				case 2: //오른쪽
					vPos += pTransform->Get_State(CTransform::STATE_RIGHT)*3.f;
					m_pCharacterController->Set_Position(_float3(XMVectorGetX(vPos), XMVectorGetY(vPos) + 1.5f, XMVectorGetZ(vPos)));
					break;
				case 3: //왼쪽
					vPos += pTransform->Get_State(CTransform::STATE_RIGHT)*-3.f;
					m_pCharacterController->Set_Position(_float3(XMVectorGetX(vPos), XMVectorGetY(vPos) + 1.5f, XMVectorGetZ(vPos)));
					break;
				}
				m_pDead_Censor->Reset_DeadState();
				m_isRender = true;
				m_dAccTime = 0.0;
				m_iHp = 1;
			}
			
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	else
	{
		if (m_IsHit)
			m_pAttack_Censor->Reset_AttackState();

		if (m_IsSwing)
			m_IsSwing = false;

		Update_WeaponMatrix();

		m_pTransformCom->Set_Velocity(XMVectorZero());

		if (!m_pCharacterController->IsDown() && m_IsGravity)
			m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -0.1f);

		if (true == m_pRoot->Invoke(TimeDelta, m_pModelCom, m_pTransformCom, m_pCharacterController))
		{
			if (0 == m_iMeshType)
			{
				if (SCENE::SCENE_STAGE01 == m_iCurrentScene)
				{
					CQuestManager* pQManager = GET_INSTANCE(CQuestManager);
					pQManager->Battle_MonsterCnt(2, true);
					RELEASE_INSTANCE(CQuestManager);
				}
				m_isDestroy = true;
			}
			else
			{
				m_isRender = false;
			}

		}

		_matrix	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3] += XMVectorSet(0.f, 2.f, 0.f, 0.f);
		XMStoreFloat4x4(&m_HpBarMatrix, WorldMatrix);

		m_pModelCom->Lerp_Update(TimeDelta);

		m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());

		m_pCharacterController->Update_Transform();

		if (!m_isDestroy) {
			WorldMatrix = m_pTransformCom->Get_WorldMatrix();
			WorldMatrix.r[3] += XMVectorSet(0.f, 0.5f, 0.f, 0.f);
			m_pHitColliderCom->Update_TransformPhysX(WorldMatrix);
		}

		if (nullptr != m_pAttTrigger)
			m_pAttTrigger->Tick(TimeDelta);

		if (m_IsSwing)
			((CCensor_AttTrigger*)m_pAttTrigger)->Attack(true);
		else
			((CCensor_AttTrigger*)m_pAttTrigger)->Attack(false);
	}

	return _int();
}

_int CCensor::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_isRender)
	{

		if (nullptr != m_pAttack_Censor->Get_Donut())
		{
			if (m_pAttack_Censor->Get_Donut()->Get_IsDestroy())
				m_pAttack_Censor->Clear_Effect();
		}

		if (nullptr != m_pAttTrigger)
			m_pAttTrigger->LateTick(TimeDelta);

		if (true != m_isDestroy)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}

	return 0;
}

HRESULT CCensor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;
	_bool		bArr[4] = { true, false, false, false };
	m_pModelCom->Render_IsEmissiveMapping(0, bArr);
	return S_OK;
}

void CCensor::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_SKILL)
	{
		if (collision.Get_Object()->Get_IsPulling())
		{
			Attacked(CEnemy::ATTACKED::FW);
			Damaged(20);
			m_pAttack_Censor->Clear_Effect();
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
			m_pAttack_Censor->Clear_Effect();
		}
	}
}

void CCensor::OnCollisionEnter(CCollisionObject & collision)
{
}

void CCensor::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CCensor::OnTriggerExit(CCollisionObject & collision)
{
}

void CCensor::Set_Gravity(_bool _IsGravity)
{
	m_IsGravity = _IsGravity;
}

void CCensor::Update_LookVector(_double TimeDelta)
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

void CCensor::Swing(_bool _IsSwing)
{
	m_IsSwing = _IsSwing;
}

void CCensor::Update_WeaponMatrix()
{
	XMStoreFloat4x4(&m_WeaponMatrix, XMLoadFloat4x4(&m_WeaponOffsetMatrix)*XMLoadFloat4x4(m_pCombinedMatrixptr)*XMLoadFloat4x4(&m_PivotMatrix)*m_pTransformCom->Get_WorldMatrix());
}


HRESULT CCensor::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	_tchar szModelPrototypeTag[MAX_PATH] = TEXT("");
	swprintf_s(szModelPrototypeTag, MAX_PATH, TEXT("Prototype_Component_Model_Censor%d"), m_iMeshType);
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
	if (0 == m_iMeshType)
	{
		tCharacterControllerDesc.fHeight = 0.5f;
		tCharacterControllerDesc.fRadius = 0.5f;
		tCharacterControllerDesc.fStepOffset = 0.0f;
		tCharacterControllerDesc.fContactOffset = tCharacterControllerDesc.fRadius * 0.01f;
		tCharacterControllerDesc.fStaticFriction = 0.5f;
		tCharacterControllerDesc.fDynamicFriction = 0.5f;
		tCharacterControllerDesc.fRestitution = 0.f;
		tCharacterControllerDesc.vPosition = { 0.f, 1.f, 0.f };
		tCharacterControllerDesc.pGameObject = this;
	}
	else
	{
		tCharacterControllerDesc.fHeight = 0.25f;
		tCharacterControllerDesc.fRadius = 0.25f;
		tCharacterControllerDesc.fStepOffset = 0.0f;
		tCharacterControllerDesc.fContactOffset = tCharacterControllerDesc.fRadius * 0.01f;
		tCharacterControllerDesc.fStaticFriction = 0.5f;
		tCharacterControllerDesc.fDynamicFriction = 0.5f;
		tCharacterControllerDesc.fRestitution = 0.f;
		tCharacterControllerDesc.vPosition = { 0.f, 1.f, 0.f };
		tCharacterControllerDesc.pGameObject = this;
	}

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
	if(0==m_iMeshType)
		tBoxColliderDesc.vScale = { 2.f,2.f,2.f }; //MULTI_FLOAT3(vColSize, 2.0);
	else
		tBoxColliderDesc.vScale = { 2.f,2.f,2.f };
	

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pHitColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pHitColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER);

	return S_OK;
}

HRESULT CCensor::SetUp_ConstantTable()
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

CCensor * CCensor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCensor*	pInstance = new CCensor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCensor::Clone(void * pArg, _uint iCurrentScene)
{
	CCensor*	pInstance = new CCensor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCensor::Free()
{
	__super::Free();
	Safe_Release(m_pRoot);
	Safe_Release(m_pSelector);
	Safe_Release(m_pSpawn_Censor);
	Safe_Release(m_pIdle_Censor);
	Safe_Release(m_pDead_Censor);
	Safe_Release(m_pWalk_Censor);
	Safe_Release(m_pRun_Censor);
	Safe_Release(m_pRun_Censor);
	Safe_Release(m_pHit_Censor);
	Safe_Release(m_pChase_Censor);
	Safe_Release(m_pJump_Censor);
	Safe_Release(m_pAttack_Censor);
	Safe_Release(m_pAttTrigger);
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
