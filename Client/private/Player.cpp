
#include "stdafx.h"
#include "..\Public\Player.h"
#include "Player_AnimState.h"
#include "GameInstance.h"
#include "Player_LeftHand.h"
#include "Player_RightHand.h"

#include "PlayerTrigger_Parkour.h"
#include "PlayerTrigger_Body.h"
#include "Camera_Fly.h"

#include "CollisionObject.h"
#include "Tuto_ScriptUI.h"
#include "QuestUI.h"
#include "QuestManager.h"

#include "PlayerHPBar.h"
#include "RailSlide.h"

#include "Boss_Dish.h"
CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)

{
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(SetUp_MyBoneMatrices("Lf_ball")))
		return E_FAIL;

	m_isRender = false;
	//m_pNavigationCom->Set_CurrentIndex(0);
	m_pModelCom->Set_Animation(0);



	m_StrLayerName = TEXT("Player");
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_eTextureType = TEXTRUETYPE_MOUSE;

	//m_pCharacterController->Set_Position(_float3(-151.f, 5.f, 107.f));
	m_pCharacterController->Set_Position(_float3(5.f, 5.f, 2.f));
	//m_pCharacterController->Set_Position(_float3(239.f, 4.8f, 190.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(239.f, 4.8f, 190.f)), 1.f));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*For.Layer_GameObject_LeftHand*/
	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_LeftHand"), SCENE_STATIC, TEXT("Layer_LeftHand"), (CGameObject**)&m_pLeftHand, nullptr)))
		return E_FAIL;
	Safe_AddRef(m_pLeftHand);
	/*For.Layer_GameObject_RightHand*/
	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_RightHand"), SCENE_STATIC, TEXT("Layer_RightHand"), (CGameObject**)&m_pRightHand, nullptr)))
		return E_FAIL;
	Safe_AddRef(m_pRightHand);
	/*For.Trigger_ForPlayerHead*/
	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Player_ParkourTrigger"), SCENE_STATIC, TEXT("Layer_PlayerTrigger"), (CGameObject**)&m_pTriggerParkour, nullptr)))
		return E_FAIL;
	Safe_AddRef(m_pTriggerParkour);
	/*For.Trigger_ForPlayerBody*/
	if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Player_BodyTrigger"), SCENE_STATIC, TEXT("Layer_PlayerTrigger"), (CGameObject**)&m_pTriggerBody, nullptr)))
		return E_FAIL;
	Safe_AddRef(m_pTriggerBody);

	m_pTriggerParkour->Set_Matrix(m_pTransformCom->Get_WorldMatrixPtr());
	m_pTriggerBody->Set_Matrix(m_pTransformCom->Get_WorldMatrixPtr());

	Safe_Release(pGameInstance);

	m_pAnimState = CPlayer_AnimState::Create(this, m_pTransformCom, m_pModelCom, nullptr, nullptr, m_pCharacterController, m_pLeftHand, m_pRightHand, m_pTriggerParkour, m_iCurrentScene);
	if (nullptr == m_pAnimState)
		return E_FAIL;


	for (_uint i = 0; i < 3; ++i)
	{
		XMStoreFloat4x4(&m_OldWolrd[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_OldView[i], XMMatrixIdentity());
	}

	m_ArrKey[0] = 0.f;
	m_ArrKey[1] = 0.f;
	m_ArrKey[2] = 0.f;
	return S_OK;
}

_int CPlayer::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	static _uint i = 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_pAnimState->Get_LedgeCheck())
		m_pTransformCom->Set_Velocity(XMVectorZero());

	Update_PlayerQuestKeyDown();

	_float4x4		WorldMatrix;
	WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WorldMatrix._42 += 1.f;
	if (pGameInstance->Key_Down('B'))
	{
		m_pCharacterController->Set_Position(_float3(5.f, 5.f, 2.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(5.f, 5.f, 2.f)), 1.f));
	}
	if (pGameInstance->Key_Down('N'))
	{
		//m_pCharacterController->Set_Position(_float3(47.8f, 1.f, 20.6f));
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(47.8f, 1.f, 20.6f)), 1.f));
		m_pCharacterController->Set_Position(_float3(-154.f, 22.0f, 157.464f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(-154, 22.0f, 157.464f)), 1.f));
	}
	if (pGameInstance->Key_Down('M'))
	{
		m_pCharacterController->Set_Position(_float3(40.f, 0.190f, 43.583f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(40.f, 0.190f, 43.583f)), 1.f));
	}
	if (pGameInstance->Key_Down(VK_ADD))
	{
		m_pCharacterController->Set_Position(_float3(287.5f, 19.4f, 195.3f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(287.5f, 19.4f, 195.3f)), 1.f));
		m_pTransformCom->Set_Velocity(XMVectorZero());

		/* Test용 함수(Stage03- 중보->요리대회 이동시 호출함수) */
		Setting_CookingContestUI();
	}

	if (pGameInstance->Key_Down(VK_DIVIDE))
	{
		m_isSlider = !m_isSlider;
	}


	//if (pGameInstance->Key_Down(VK_HOME))
	//{
	//	_tchar szBuff[260] = TEXT("");
	//	swprintf_s(szBuff, 260, TEXT("Position- X: %.2f, Y: %.2f, Z: %.2f\n"), XMVectorGetX(m_pTransformCom->Get_Position()), XMVectorGetY(m_pTransformCom->Get_Position()), XMVectorGetZ(m_pTransformCom->Get_Position()));
	//	OutputDebugString(szBuff);
	//}
	if (pGameInstance->Key_Down('J'))
	{
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Judge"), pGameInstance->Get_CurrentSceneIndex(), TEXT("a"), &WorldMatrix);
		//전구몬스터
	}
	if (pGameInstance->Key_Down('K'))
	{
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_BossGoat"), pGameInstance->Get_CurrentSceneIndex(), TEXT("a"), &WorldMatrix);
		//전구몬스터
	}
	//if (pGameInstance->Key_Down('K'))
	//{
	//	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Judge"), pGameInstance->Get_CurrentSceneIndex(), TEXT("a"), &WorldMatrix);
	//	//보스몬스터
	//}
	//if (pGameInstance->Key_Down('L'))
	//{
	//	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Doubt"), pGameInstance->Get_CurrentSceneIndex(), TEXT("a"), &WorldMatrix);
	//	//날파리몬스터
	//}
	//if (pGameInstance->Key_Down('J'))
	//{
	//	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("a"), &WorldMatrix);
	//	//도장맨
	//}
	//if (pGameInstance->Key_Down('L'))
	//{
	//	/* 구름 생성 막을것임 */
	//	_float3 vpos;
	//	XMStoreFloat3(&vpos, m_pTransformCom->Get_Position());
	//	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), m_iCurrentScene, TEXT("Layer_Cloud"), &XMVectorSet(vpos.x, vpos.y + 2.f, vpos.z, 1.f));
	//}
	//if (pGameInstance->Key_Down('L'))
	//{
	//	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Doubt0"), m_iCurrentScene, TEXT("a"), &WorldMatrix);
	//	//근육맨
	//}
	//if (pGameInstance->Key_Down('K'))
	//{
	//	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_ToothFairy"), m_iCurrentScene, TEXT("a"), &WorldMatrix);
	//	//이빨요정
	//}
	//if (pGameInstance->Key_Down('L'))
	//{
	//	pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorBurly"), m_iCurrentScene, TEXT("a"), &WorldMatrix);
	//	//근육맨 
	//}


	if (GetKeyState('G') & 0x8000)
	{
		m_bIsAnimUpdate = false;

	}
	if (GetKeyState('H') & 0x8000)
	{
		m_bIsAnimUpdate = true;

	}


	/*if (pGameInstance->Key_Down(VK_NUMPAD0))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(0);
	}

	if (pGameInstance->Key_Down(VK_NUMPAD1))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(1,2);
	}

	if (pGameInstance->Key_Down(VK_NUMPAD2))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(3);
	}

	if (pGameInstance->Key_Down(VK_NUMPAD3))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(5,8);
	}

	if (pGameInstance->Key_Down(VK_NUMPAD4))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(9);

	}

	if (pGameInstance->Key_Down(VK_NUMPAD5))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(10);

	}

	if (pGameInstance->Key_Down(VK_NUMPAD6))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(11,14);
	}

	if (pGameInstance->Key_Down(VK_ADD))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(25);
	}

	if (pGameInstance->Key_Down(VK_DIVIDE))
	{
	((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(26,29);
	}*/


	if (pGameInstance->Key_Down(VK_NUMPAD7))
	{
		_uint BossDishArg = CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_DISH;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Dish"), SCENE_STAGE05, TEXT("Boss_Dish_One"), &BossDishArg);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_BOSSDISH_POSE);
	}
	if (pGameInstance->Key_Down(VK_NUMPAD8))
	{
		_uint BossDishArg = CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_SOUP;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Dish"), SCENE_STAGE05, TEXT("Boss_Dish_One"), &BossDishArg);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_BOSSDISH_POSE);

	}
	if (pGameInstance->Key_Down(VK_NUMPAD9))
	{
		_uint BossDishArg = CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_COOKIE;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Dish"), SCENE_STAGE05, TEXT("Boss_Dish_One"), &BossDishArg);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_BOSSDISH_POSE);
	}

	if (pGameInstance->Key_Down(VK_ADD))
	{
		m_pCharacterController->Set_Position(_float3(239.f, 4.8f, 190.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(239.f, 4.8f, 190.f)), 1.f));
		m_bClimbingCheck = false;
	}

	if (false == m_pAnimState->Get_JumpCheck() && false == m_pAnimState->Get_DoubleJumpCheck())
		Player_Sliding(TimeDelta);

	if (true == m_bIsAnimUpdate)
	{
		m_pAnimState->Player_Hit(&m_bHitCheck, TimeDelta);
		m_pAnimState->Update(TimeDelta);
	}

	if (FAILED(Bone_MatrixCalculation()))
		return -1;

	if (false == m_pAnimState->Get_LedgeCheck())
		m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());
	if (false == m_pAnimState->Get_LedgeCheck())
		m_pCharacterController->Update_Transform();

	static_cast<CPlayerTrigger*>(m_pTriggerParkour)->Set_Position();
	static_cast<CPlayerTrigger*>(m_pTriggerBody)->Set_Position();

	if (true == m_bIsAlphaDown || true == m_bIsAlphaUp)
		Set_Alpha(TimeDelta);

	Update_HpBar(TimeDelta);

	//m_pCharacterController->Get_Desc().

	if (m_isInteraction)
	{
		m_isInteraction = false;

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HitEffect"),
			SCENE_STATIC, TEXT("Effect_HitEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_HitEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		_vector vPos = XMLoadFloat3(&m_vTargetEffectPos) - m_pTransformCom->Get_Position();
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HitEffect"), TEXT("Com_Option"), vPos);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HitEffect"), TEXT("Com_Option"), 0.1f);

	}

	if (pGameInstance->Key_Down(VK_NUMPAD3))
	{
		static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Stage3_CUT(CCamera_Fly::STAGE3_CUT_START);
		static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_STAGE3);
	}

	if (pGameInstance->Key_Down(VK_NUMPAD2))
	{
		static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Stage3_CUT(CCamera_Fly::STAGE3_CUT_18);
		static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_STAGE3);
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

_int CPlayer::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	if (!m_isRender)
		return 0;
	if (true == m_bIsAnimUpdate)
	{
		if (false == m_bIsRenderChange)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PLAYER, this);
		else
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

		}
	}

	return 0;
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render_Change()))
		return E_FAIL;

	//#ifdef _DEBUG
	//	m_pAABBCom->Render();
	//	m_pOBBCom->Render();
	//#endif // _DEBUG

	return S_OK;
}

void CPlayer::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_MONSTER)
	{
		OutputDebugStringW(TEXT("Col-Trigger-Static충돌:HandRight\n"));

		if (collision.Get_ObjectTransform())
		{
			if (!m_isInteraction)
			{
				m_isInteraction = true;
				_float fRandRight = GET_RANDOMFLOAT(-0.5f, 0.5f);
				_float fRandUp = GET_RANDOMFLOAT(0.5f, 1.5f);
				XMStoreFloat3(&m_vTargetEffectPos, ((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_Position() +
					((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_State(CTransform::STATE_UP) * fRandUp +
					((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_State(CTransform::STATE_RIGHT) * fRandRight);
			}
		}


		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Set_Stiffness(true);
		RELEASE_INSTANCE(CGameInstance);
		OutputDebugString(TEXT("Hit Monster!!\n"));
	}
}

void CPlayer::OnControllerCollisionHit(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PICK)
	{
		if (collision.Get_ObjectTransform())
		{
			((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->SetGravity(true);
			((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->Add_Force(m_pTransformCom->Get_State(CTransform::STATE_LOOK)*30.f, PxForceMode::eFORCE);
			((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->SetRestitution(.5f);
			((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->Add_Force((m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_UP))*3.f, PxForceMode::eIMPULSE);
		}
		OutputDebugString(TEXT("Hit PickObject!!\n"));
	}

	// *** 수정
	//if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_MONSTER_FRUIT)
	//{
	//	if (collision.Get_ObjectTransform())
	//	{
	//		((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->SetGravity(true);
	//		((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->Add_Force(m_pTransformCom->Get_State(CTransform::STATE_LOOK)*30.f, PxForceMode::eFORCE);
	//		((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->SetRestitution(.5f);
	//		((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->Add_Force((m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_UP))*3.f, PxForceMode::eIMPULSE);
	//	}
	//	OutputDebugString(TEXT("Hit FRUITObject!!\n"));
	//}

	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_STATIC)
	{

	}

	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_MONSTER)
	{
		OutputDebugString(TEXT("Hit Monster Object!!\n"));
		m_lfMoveValue = 0.5;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Set_Stiffness(true);
		RELEASE_INSTANCE(CGameInstance);
	}

	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_RAIL)
	{
		m_pAnimState->Set_RailSlideCheck(true);
		m_pAnimState->Set_CollisionObject(collision.Get_Object());
		m_vHitPos = collision.Get_HitPosFloat3();
	}
}

void CPlayer::SetUp_PlayerPosition(SCENE eScene)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (eScene)
	{
	case SCENE_DEMO:
	{
		m_pCharacterController->Set_Position(_float3(5.f, 5.f, 2.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(5.f, 5.f, 2.f)), 1.f));

		break;
	}
	case SCENE_STAGE01:
	{
		//m_pTransformCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f,0.f));
		m_pCharacterController->Set_Position(_float3(62.026f, 2.73f, 60.372f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(62.026f, 2.73f, 60.372f)), 1.f));

		//m_pCharacterController->Update_Transform();
		break;
	}
	case SCENE_STAGE02:
	{
		//m_pTransformCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f,0.f));
		m_pCharacterController->Set_Position(_float3(18.f, 2.f, 6.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(18.f, 2.f, 6.f)), 1.f));

		//m_pCharacterController->Update_Transform();
		break;
	}
	case SCENE_STAGE03:
	{
		//m_pTransformCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f,0.f));
		if (m_iCookingStage_PhaseNum == 0)
		{
			m_pCharacterController->Set_Position(_float3(41.f, 2.f, 12.f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(41.f, 2.f, 12.f)), 1.f));
		}
		else
		{
			m_pCharacterController->Set_Position(_float3(236.62f, 3.15f, 177.274f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(236.62f, 3.15f, 177.274f)), 1.f));
		}
		//m_pCharacterController->Update_Transform();
		break;
	}
	case SCENE_STAGE04:
	{
		//m_pTransformCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f,0.f));
		m_pCharacterController->Set_Position(_float3(24.f, 2.f, 34.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(24.f, 2.f, 34.f)), 1.f));

		//m_pCharacterController->Update_Transform();
		break;
	}
	case SCENE_STAGE05:
	{
		//m_pTransformCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f,0.f));
		m_pCharacterController->Set_Position(_float3(40.f, 0.095f, 25.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(40.f, 0.095f, 25.f)), 1.f));

		//m_pCharacterController->Update_Transform();
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Player_Sliding(_double TimeDelta)
{
	_float3 vCurrentVec, vNextPos;
	_float3 vVelocity;
	if (m_isSlider)
	{
		if (!m_isSlider)
			return;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
		// raycasting 의 시작지점 ( 지금 player의 발쪽에 아마 되어있을 가능성이 높음 아마 몸통부분으로 수정해줘야할듯 )
		vCurrentVec = m_pTransformCom->Get_Velocity();
		vCurrentVec.y = 0;


		// 만약에 아무키도 누르지않은상태(Move값이 0일때 처리)
		if (XMVector3Equal(XMLoadFloat3(&vCurrentVec), XMVectorZero()))
		{
			// 플레이어가 현재 바라보고있는 Look을받아서 CurrentVec에 저장
			vCurrentVec = STOREFLOAT3(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			vCurrentVec.y = 0;
			// 이동값이 있을경우엔 Get_Velocity를 그대로사용
		}
		// 내 현재 좌표 + 한틱 이동한 만큼의 값 더해(한틱 앞에있는 포지션값)
		XMStoreFloat3(&tDescIn.vOrigin, m_pTransformCom->Get_Position() + XMVector3Normalize(XMLoadFloat3(&vCurrentVec))*m_pTransformCom->Get_TransFormDescSpeed() * TimeDelta);// ;																																			  // raycasting 방향 ( 플레이어의 look방향이 기본(벽탈때 보통 플레이어 기준 앞쪽을 바라보기 때문)
																																												// 레이 Direction을 -Up
		XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(-m_pTransformCom->Get_State(CTransform::STATE_UP)));
		// raycast 길이
		tDescIn.fDistance = 5.0f;
		// 어떤 오브젝트와 부딪힐 것인지에 대한 정보 (전부, 그리고 static만 부딪힌다(현재 네비메쉬만 static으로 되어있음)
		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
		// 어떤 레이어오브젝트랑만 충돌할 것인지 ( 네비메쉬의 layerType을 Plane으로 설정해서 Plane만 체킹해줌)
		// 어떤레이어로 설정하고 싶으면 Set_ShapeLayer() 함수 사용
		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PLANE);
		// 부딪힌 오브젝트를 받아오기 위한 변수
		CGameObject* pHitObject = nullptr;
		// Out에 받아오기 때문에 주소값 던져줌
		tDescOut.ppHitObject = &pHitObject;
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (pHitObject)
			{
				vVelocity = STOREFLOAT3(XMLoadFloat3(&tDescOut.vHitPos) - m_pTransformCom->Get_Position());
				XMStoreFloat4(&m_vUpRayDirection, XMVector4Normalize(XMLoadFloat3(&tDescOut.vHitPos) - m_pTransformCom->Get_Position()));
				// 좌표 세팅
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tDescOut.vHitPos));
				// 원래 있었던 이동값을 다 0으로 초기화
				m_pTransformCom->Set_Velocity(XMVectorZero());
				// 
				m_pCharacterController->Move(TimeDelta * 3.0, vVelocity);
				m_pColliderCom->Update_TransformPhysX();
				m_pColliderCom->Update_Transform();
			}
		}
		RELEASE_INSTANCE(CGameInstance);

	}
}

HRESULT CPlayer::Render_Change()
{
	if (false == m_bIsRenderChange)
	{
		if (true == m_bIsAlphaDown || true == m_bIsAlphaUp)
		{
			if (1 == m_iRenderPass)
				return S_OK;
			else
				m_iRenderPass = 7;

		}
		if (FAILED(m_pModelCom->Render(m_iRenderPass)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBufferCom->Render(28)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Bone_MatrixCalculation()
{
	if (nullptr != m_pMyCombinedMatrix && true == m_pAnimState->Get_ColiderEnable())
	{
		_matrix ColiderMatrix = XMLoadFloat4x4(&m_MyOffsetMatrix) * XMLoadFloat4x4(m_pMyCombinedMatrix) * XMLoadFloat4x4(&m_MyPivotMatrix) * XMLoadFloat4x4(&m_pTransformCom->Get_WorldFloat4x4());
		XMStoreFloat4x4(&m_ColiderMatirx, ColiderMatrix);
		m_pBoneMatrix = &m_ColiderMatirx;

		m_isPulling = true;
		m_pColliderCom->Add_Actor();
		m_pColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pBoneMatrix));
		m_pColliderCom->Update_Transform(m_pBoneMatrix);
	}

	if (false == m_pAnimState->Get_ColiderEnable())
	{
		m_isPulling = false;
		m_pColliderCom->Remove_Actor();
	}

	return S_OK;
}

void CPlayer::Setting_CookingContestUI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_CookingUI_Background"));
	for (_uint i = 0; i<iLayerSize; ++i)
		pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_CookingUI_Background"), i)->Set_IsRender(true);

	iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"));
	for (_uint i = 0; i<iLayerSize; ++i)
		pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), i)->Set_IsRender(true);

	pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_TimerUI"), 0)->Set_IsRender(true);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Setting_PlayerQuest(_uint iQuestNum, _bool isFirst)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	for (_uint i = 0; i < 3; ++i)
	{
		if (m_iCurrentQuestArr[i] == -1)
		{
			m_iCurrentQuestArr[i] = iQuestNum;
			CQuestUI* pUI = (CQuestUI*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestUI"), i);
			pUI->Set_RenderPass(iQuestNum);

			if (i == iSelectQuestArrNumber)
				pUI->Set_UIState(CQuestUI::UISTATE_SELECTED);
			else
				pUI->Set_UIState(CQuestUI::UISTATE_IDLE);


			break;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Reset_PlayerQuest(_uint iQuestNum)
{
	m_iCurrentQuestArr[iQuestNum] = -1;
	Sort_PlayerQuest(iQuestNum);
}

void CPlayer::Set_GrabSuccessCheck(_bool bSuccessCheck)
{
	if (nullptr == m_pAnimState)
		return;

	m_pAnimState->Set_GrabSuccess(true);
}

_bool CPlayer::Get_HitCheck() const
{
	if (nullptr == m_pAnimState)
		return false;

	return m_pAnimState->Get_HitCheck();
}

_bool CPlayer::Get_HitingCheck() const
{
	if (nullptr == m_pAnimState)
		return false;

	return m_pAnimState->Get_HitingCheck();
}

_bool CPlayer::IsJumping()
{
	return m_pAnimState->Check_JumpAnimstate();
}

void CPlayer::Set_TrampolineJump()
{
	m_pAnimState->Set_TrampolineTriggerCheck(true);
}

_bool CPlayer::Get_AttackCheck() const
{
	return m_pAnimState->Get_AttackCheck();
}

void CPlayer::Set_AttackMoveSpeed(_double lfAttackMoveSpeed)
{
	if (nullptr == m_pAnimState)
		return;

	m_pAnimState->Set_AttackMoveSpeed(lfAttackMoveSpeed);
}


void CPlayer::Set_DashAttackMoveSpeed(_double lfDashAttackMoveSpeed)
{
	if (nullptr == m_pAnimState)
		return;

	m_pAnimState->Set_DashAttackMoveSpeed(lfDashAttackMoveSpeed);
}

void CPlayer::Set_Alpha(_double TimeDelta)
{
	if (true == m_bIsAlphaDown)
	{
		m_fDecreaseAlpha += (_float)(TimeDelta*m_dAlphaSpeed);
		if (1 < m_fDecreaseAlpha)
			m_fDecreaseAlpha = 1.0;
	}
	if (true == m_bIsAlphaUp)
	{
		m_fDecreaseAlpha -= (_float)(TimeDelta*m_dAlphaSpeed);
		if (0 > m_fDecreaseAlpha)
		{
			m_fDecreaseAlpha = 0.0;
			m_bIsAlphaUp = false;
			m_bIsAlphaDown = false;
			m_iRenderPass = m_iRenderPassBackUp;
		}
	}
}

_int CPlayer::Update_HpBar(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_IsDecrease || m_IsIncrease)
		//m_dDecreaseTimeAcc += TimeDelta;
		m_dDecreaseTimeAcc += 0.016;

	if (m_IsDefault)
		//	m_dIncreaseTimeAcc += TimeDelta;
		m_dIncreaseTimeAcc += 0.016;

	if (!m_IsDecrease && m_IsGetDamaged)
	{
		m_iCurrentHp -= m_iSettingHp;
		m_IsDecrease = true;
		m_IsGetDamaged = false;

		if (0 > m_iCurrentHp)
			m_iCurrentHp = 0;

		_uint iHPUILayerIndex = m_iCurrentHp / 4;
		_uint iHPUICount = m_iCurrentHp % 4;

		if (0 > iHPUICount)
			--iHPUILayerIndex;

		if (0 > iHPUILayerIndex)
			iHPUILayerIndex = 0;

		CPlayerHPBar* pPlayerHpUI = (CPlayerHPBar*)pGameInstance->Get_GameObject(m_iHpCreateScene, TEXT("Layer_PlayerHPBar"), iHPUILayerIndex);
		if (nullptr == pPlayerHpUI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		m_iBackUpLayerIndex = iHPUILayerIndex;
		pPlayerHpUI->Set_HpIndex(iHPUICount);
		pPlayerHpUI->Set_IsDecrease(m_IsDecrease);
	}

	if (!m_IsIncrease && m_IsGetHealPack && m_iBackUpHp != m_iMaxHp)
	{
		m_iBackUpHp += m_iSettingHp;
		m_IsIncrease = true;
		m_IsGetHealPack = false;

		if (m_iMaxHp < m_iBackUpHp)
			m_iBackUpHp = m_iMaxHp;

		_uint iHPUILayerIndex = m_iBackUpHp / 4;
		_uint iHPUICount = m_iBackUpHp % 4;

		if (iHPUILayerIndex > m_iBackUpLayerIndex && 0 == iHPUICount)
		{
			iHPUICount = 4;
			iHPUILayerIndex = m_iBackUpLayerIndex;
		}

		CPlayerHPBar* pPlayerHpUI = (CPlayerHPBar*)pGameInstance->Get_GameObject(m_iHpCreateScene, TEXT("Layer_PlayerHPBar"), iHPUILayerIndex);
		if (nullptr == pPlayerHpUI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}

		m_iBackUpLayerIndex = iHPUILayerIndex;
		pPlayerHpUI->Set_HpBackUpIndex(iHPUICount);
		pPlayerHpUI->Set_IsIncrease(m_IsIncrease);
	}


	if (1.0 <= m_dDecreaseTimeAcc)
	{
		if (m_IsDecrease)
		{
			m_iBackUpHp -= m_iSettingHp;

			if (0 > m_iBackUpHp)
				m_iBackUpHp = 0;

			_uint iBackUpHPUILayerIndex = m_iBackUpHp / 4;
			_uint iBackUpHPUICount = m_iBackUpHp % 4;

			if (0 > iBackUpHPUICount)
				--iBackUpHPUILayerIndex;

			if (0 > iBackUpHPUILayerIndex)
				iBackUpHPUILayerIndex = 0;

			CPlayerHPBar* pPlayerHpUI = (CPlayerHPBar*)pGameInstance->Get_GameObject(m_iHpCreateScene, TEXT("Layer_PlayerHPBar"), iBackUpHPUILayerIndex);
			if (nullptr == pPlayerHpUI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			m_IsDecrease = false;
			pPlayerHpUI->Set_HpBackUpIndex(iBackUpHPUICount);
			m_dDecreaseTimeAcc = 0.0;
		}
		else if (m_IsIncrease)
		{
			m_iCurrentHp += m_iSettingHp;

			if (m_iMaxHp < m_iCurrentHp)
				m_iCurrentHp = m_iMaxHp;

			_uint iBackUpHPUILayerIndex = m_iCurrentHp / 4;
			_uint iBackUpHPUICount = m_iCurrentHp % 4;

			if (iBackUpHPUILayerIndex > m_iBackUpLayerIndex && 0 == iBackUpHPUICount)
			{
				iBackUpHPUICount = 4;
				iBackUpHPUILayerIndex = m_iBackUpLayerIndex;
			}

			CPlayerHPBar* pPlayerHpUI = (CPlayerHPBar*)pGameInstance->Get_GameObject(m_iHpCreateScene, TEXT("Layer_PlayerHPBar"), iBackUpHPUILayerIndex);
			if (nullptr == pPlayerHpUI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			m_IsDefault = true;
			pPlayerHpUI->Set_HpIndex(iBackUpHPUICount);
			pPlayerHpUI->Set_IsDefault(m_IsDefault);
			m_IsIncrease = false;
			m_dDecreaseTimeAcc = 1.0;
		}
	}

	if (1 <= m_dIncreaseTimeAcc)
	{
		m_IsDefault = false;
		m_dDecreaseTimeAcc = 0.0;
		m_dIncreaseTimeAcc = 0.0;
	}
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

void CPlayer::Set_HpCreate(SCENE iScene)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iHpCreateScene = iScene;

	switch (iScene)
	{
	case SCENE_DEMO:
		for (_uint i = 0; i < HPCOUNT_DEMO; i++)
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), iScene, TEXT("Layer_PlayerHPBar"));
		m_iMaxHp = 4 * HPCOUNT_DEMO;
		break;
	case SCENE_STAGE01:
		for (_uint i = 0; i < HPCOUNT_STAGE1; i++)
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), iScene, TEXT("Layer_PlayerHPBar"));
		m_iMaxHp = 4 * HPCOUNT_STAGE1;
		break;
	case SCENE_STAGE02:
		for (_uint i = 0; i < HPCOUNT_STAGE2; i++)
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), iScene, TEXT("Layer_PlayerHPBar"));
		m_iMaxHp = 4 * HPCOUNT_STAGE2;
		break;
	case SCENE_STAGE03:
		for (_uint i = 0; i < HPCOUNT_STAGE3; i++)
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), iScene, TEXT("Layer_PlayerHPBar"));
		m_iMaxHp = 4 * HPCOUNT_STAGE3;
		break;
	case SCENE_STAGE04:
		for (_uint i = 0; i < HPCOUNT_STAGE4; i++)
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), iScene, TEXT("Layer_PlayerHPBar"));
		m_iMaxHp = 4 * HPCOUNT_STAGE4;
		break;
	case SCENE_STAGE05:
		for (_uint i = 0; i < HPCOUNT_STAGE5; i++)
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), iScene, TEXT("Layer_PlayerHPBar"));
		m_iMaxHp = 4 * HPCOUNT_STAGE5;
		break;
	}

	m_iCurrentHp = m_iMaxHp;
	m_iBackUpHp = m_iMaxHp;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_HpRender(SCENE iScene, _bool bCheck)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iScene)
	{
	case SCENE_DEMO:
		for (_uint i = 0; i < HPCOUNT_DEMO; i++)
		{
			CPlayerHPBar* pHpBar = dynamic_cast<CPlayerHPBar*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_PlayerHPBar"), i));
			pHpBar->Set_IsRenderCheck(bCheck);
		}
		break;
	case SCENE_STAGE01:
		for (_uint i = 0; i < HPCOUNT_STAGE1; i++)
		{
			CPlayerHPBar* pHpBar = dynamic_cast<CPlayerHPBar*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_PlayerHPBar"), i));
			pHpBar->Set_IsRenderCheck(bCheck);
		}
		break;
	case SCENE_STAGE02:
		for (_uint i = 0; i < HPCOUNT_STAGE2; i++)
		{
			CPlayerHPBar* pHpBar = dynamic_cast<CPlayerHPBar*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_PlayerHPBar"), i));
			pHpBar->Set_IsRenderCheck(bCheck);
		}
		break;
	case SCENE_STAGE03:
		for (_uint i = 0; i < HPCOUNT_STAGE3; i++)
		{
			CPlayerHPBar* pHpBar = dynamic_cast<CPlayerHPBar*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_PlayerHPBar"), i));
			pHpBar->Set_IsRenderCheck(bCheck);
		}
		break;
	case SCENE_STAGE04:
		for (_uint i = 0; i < HPCOUNT_STAGE4; i++)
		{
			CPlayerHPBar* pHpBar = dynamic_cast<CPlayerHPBar*>(pGameInstance->Get_GameObject(SCENE_STAGE04, TEXT("Layer_PlayerHPBar"), i));
			pHpBar->Set_IsRenderCheck(bCheck);
		}
		break;
	case SCENE_STAGE05:
		for (_uint i = 0; i < HPCOUNT_STAGE5; i++)
		{
			CPlayerHPBar* pHpBar = dynamic_cast<CPlayerHPBar*>(pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_PlayerHPBar"), i));
			pHpBar->Set_IsRenderCheck(bCheck);
		}
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_RailValues(_double dStartSpeed, _double dDefaultSpeed, _double dFastSpeed, _double dSlowSpeed, _double dNonFixJumpPower, _double dRailCollisionCheckTerm, _double dJumpCorrection, _double dStartJumpAccMax, _double dRayTrans, _double dJumpPower, _double dJumpFallSpeed, _double dLookSpeed, _double dMoveSpeed, _double dAdjustSpeed)
{
	m_pAnimState->Set_RailValues(dStartSpeed, dDefaultSpeed, dFastSpeed, dSlowSpeed, dNonFixJumpPower, dRailCollisionCheckTerm, dJumpCorrection, dStartJumpAccMax, dRayTrans, dJumpPower, dJumpFallSpeed, dLookSpeed, dMoveSpeed, dAdjustSpeed);

}
void CPlayer::Set_CollsionObject(CGameObject * pCollisionObject)
{
	m_pAnimState->Set_CollisionObject(pCollisionObject);
}
void CPlayer::Sort_PlayerQuest(_int iNumber)
{
	if (iNumber != 2)
	{
		if (iNumber == 0)
		{
			for (_uint i = 0; i < 2; ++i)
			{
				m_iCurrentQuestArr[i] = m_iCurrentQuestArr[i + 1];
			}
			m_iCurrentQuestArr[2] = -1;
		}
		else if (iNumber == 1)
		{
			m_iCurrentQuestArr[1] = m_iCurrentQuestArr[2];
			m_iCurrentQuestArr[2] = -1;
		}
	}
}
void CPlayer::Update_PlayerQuestKeyDown()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(VK_TAB))
	{
		Tap_Quest();
	}

	if (pGameInstance->Key_Down('1'))
	{
		OutputDebugStringW(TEXT("Clear Quest0"));
		CQuestManager::Get_Instance()->Clear_Quest(0);
	}
	if (pGameInstance->Key_Down('2'))
	{
		OutputDebugStringW(TEXT("Clear Quest1"));

	}
	if (pGameInstance->Key_Down('3'))
	{
		OutputDebugStringW(TEXT("Clear Quest2"));
		CQuestManager::Get_Instance()->Clear_Quest(2);
	}

	if (pGameInstance->Key_Down(VK_LEFT))
	{
		if (iSelectQuestArrNumber > 0 && m_iCurrentQuestArr[iSelectQuestArrNumber - 1] != -1)
			--iSelectQuestArrNumber;
		Move_QuestSelecting();
	}
	else if (pGameInstance->Key_Down(VK_RIGHT))
	{
		if (iSelectQuestArrNumber < 2 && m_iCurrentQuestArr[iSelectQuestArrNumber + 1] != -1)
			++iSelectQuestArrNumber;
		Move_QuestSelecting();
	}
	RELEASE_INSTANCE(CGameInstance);
}
void CPlayer::Move_QuestSelecting()
{
	CQuestManager::Get_Instance()->Setting_SelectedQuest(iSelectQuestArrNumber);
}
void CPlayer::Tap_Quest()
{
	CQuestManager::Get_Instance()->Setting_TabQuest(iSelectQuestArrNumber);
}
void CPlayer::Set_SliderTriggerCheck(_bool IsSlider)
{
	if (nullptr == m_pAnimState)
		return;

	m_pAnimState->Set_SliderTriggerCheck(IsSlider);
}

_bool CPlayer::Get_SliderTriggerCheck() const
{
	if (nullptr == m_pAnimState)
		return false;

	return m_pAnimState->Get_SliderTriggerCheck();
}

void CPlayer::Set_WalkTiggerCheck(_bool IsWalk)
{
	if (nullptr == m_pAnimState)
		return;

	m_pAnimState->Set_WalkingCheck(IsWalk);
}

_bool CPlayer::Get_WalkTriggerCheck() const
{
	if (nullptr == m_pAnimState)
		return false;

	return m_pAnimState->Get_WalkingCheck();
}


HRESULT CPlayer::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CModel::NODENAMEDESC tNodeDesc;
	ZeroMemory(&tNodeDesc, sizeof(CModel::NODENAMEDESC));

	tNodeDesc.szFinderBoneName = "Raz_LOBO_WholeBody.ao";
	tNodeDesc.szBottomBoneName = "hipRoot";
	tNodeDesc.szRootBoneName = "Root";

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &tNodeDesc)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(0.0f, 0.75f, 0.0f);
	ColliderDesc.vExtents = _float3(0.35f, 0.75f, 0.35f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;



	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(0.0f, 1.0f, 0.0f);
	ColliderDesc.vExtents = _float3(0.4f, 1.0f, 0.4f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For. CCT*/
	CCharacterController::DESC tCharacterControllerDesc;
	tCharacterControllerDesc.fHeight = m_fHeight = 0.7f;
	tCharacterControllerDesc.fRadius = 0.35f;
	tCharacterControllerDesc.fStepOffset = 0.2f;
	tCharacterControllerDesc.fContactOffset = 0.01f;
	tCharacterControllerDesc.fSlopeLimit = 20.f;
	tCharacterControllerDesc.fStaticFriction = 0.5f;
	tCharacterControllerDesc.fDynamicFriction = 0.5f;
	tCharacterControllerDesc.fRestitution = 0.f;
	tCharacterControllerDesc.vPosition = { 0.f, 0.f, 0.f };
	tCharacterControllerDesc.pGameObject = this;
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_ChracterController"), TEXT("Com_Controller"), (CComponent**)&m_pCharacterController, &tCharacterControllerDesc)))
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
	tSphereColliderDesc.vScale = { 0.6f,0.6f,0.6f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tSphereColliderDesc)))
		return E_FAIL;
	m_eLayerType = LayerType::LAYERTYPE_PLAYER_SKILL;

	m_pColliderCom->Set_ShapeLayer(m_eLayerType);

	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();


	////Skill_Render_Buffer/////

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	////Skill_Render_Texture/////

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Raz"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;
	_float fLightRange = 30.f;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
	{
		//m_vRGBA = m_pRendererCom->Get_RimLightColorFloat4();
		//m_fRimPower = m_pRendererCom->Get_RimLightPower();
	}

	//if (GetKeyState('Y') & 0x8000)
	//{
	//	m_ArrKey[0] -= 0.5f;
	//}
	//if (GetKeyState('U') & 0x8000)
	//{
	//	m_ArrKey[0] += 0.5f;
	//}
	//if (GetKeyState('H') & 0x8000)
	//{
	//	m_ArrKey[1] -= 0.5f;
	//}
	//if (GetKeyState('J') & 0x8000)
	//{
	//	m_ArrKey[1] += 0.5f;
	//}
	//if (GetKeyState('C') & 0x8000)
	//{
	//	m_ArrKey[2] -= 0.5f;
	//}
	//if (GetKeyState('V') & 0x8000)
	//{
	//	m_ArrKey[2] += 0.5f;
	//}
	_bool		bMotionBlur = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR);
	_bool		bShadow = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_SHADOW);
	//_bool		bRimlight = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_RIMLIGHT);
	_bool		bIsMapping = true;

	if (true == m_bIsAlphaDown || true == m_bIsAlphaUp)
		bShadow = false;
	if (false == m_bIsRenderChange)
	{

		_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
		XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
		XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

		m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

		if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float3(0.079f, -0.997f, 0.0f), sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &m_bRimLightEnable, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
		//	return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_bIsNormalMapping", &bIsMapping, sizeof(_bool))))
			return E_FAIL;

		_float2 vRimRatio = m_pRendererCom->Get_RimRatio();

		if (FAILED(m_pModelCom->Set_RawValue("g_vRimRatio", &vRimRatio, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_vOffsetRGBA", &m_vOffsetRGB, sizeof(_float3))))
			return E_FAIL;
		//_float fRimWidth = m_pRendererCom->Get_RimWidth();
		//if (FAILED(m_pModelCom->Set_RawValue("g_fRimWidth", &fRimWidth, sizeof(_float))))
		//	return E_FAIL;

		_matrix PlayerWorld = XMMatrixIdentity();
		PlayerWorld.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vPos = XMVector3TransformCoord(XMVectorSet(m_ArrKey[0], m_ArrKey[1], m_ArrKey[2], 1.f), PlayerWorld);
		_float3 vPosFloat;
		XMStoreFloat3(&vPosFloat, vPos);

		_float3 vLightLook, vLightPos;
		vLightLook = pGameInstance->Get_ShadowLightLook();
		vLightPos = pGameInstance->Get_ShadowLightPos();

		_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

		if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
			return E_FAIL;

		if (true == m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR))
		{
			m_pModelCom->Set_RawValue("g_OldWorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldWolrd[2])), sizeof(_float4x4));
			m_pModelCom->Set_RawValue("g_OldViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldView[2])), sizeof(_float4x4));

		}
		m_OldWolrd[0] = m_pTransformCom->Get_WorldFloat4x4();
		XMStoreFloat4x4(&m_OldView[0], pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));

		m_OldWolrd[1] = m_OldWolrd[0];
		m_OldView[1] = m_OldView[0];

		m_OldWolrd[2] = m_OldWolrd[1];
		m_OldView[2] = m_OldView[1];

		if (true == m_bIsAlphaDown || true == m_bIsAlphaUp)
		{
			if (FAILED(m_pModelCom->Set_RawValue("g_fAlpha", &m_fDecreaseAlpha, sizeof(_float))))
				return E_FAIL;
		}
	}
	else
	{
		_matrix	WolrdMatrix = m_pTransformCom->Get_WorldMatrix();
		_vector vPos = WolrdMatrix.r[3];
		_float4 vPosition;
		XMStoreFloat4(&vPosition, vPos);
		vPosition.y += 0.7f;
		vPos = XMLoadFloat4(&vPosition);
		WolrdMatrix.r[3] = vPos;


		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(WolrdMatrix), sizeof(_matrix));
		m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
		m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)), sizeof(_matrix));


		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, m_eTextureType)))
			return E_FAIL;

		m_iRenderPass = 0;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::SetUp_HeightOnNavigation()
{
	//if (nullptr == m_pTransformCom || nullptr == m_pNavigationCom)
	//	return E_FAIL;
	//
	//_float3 vPos;
	//XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//vPos.y = m_pNavigationCom->OnNavigationGetY(XMLoadFloat3(&vPos));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
	return S_OK;
}

HRESULT CPlayer::SetUp_MyBoneMatrices(const char * pBoneName)
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("Failed to CPlayer : Funtion SetUp_MyBoneMatrices");
		return E_FAIL;
	}

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	m_MyOffsetMatrix = m_pModelCom->Get_OffsetMatrix(pBoneName);
	m_MyPivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_pMyCombinedMatrix = m_pModelCom->Get_CombinedMatrixPtr(pBoneName);
	m_MyWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void* pArg, _uint iSceneIndex)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
	Safe_Release(m_pLeftHand);
	Safe_Release(m_pRightHand);
	Safe_Release(m_pTriggerParkour);
	Safe_Release(m_pTriggerBody);

	Safe_Release(m_pCharacterController);
	Safe_Release(m_pAnimState);

	Safe_Release(m_pOBBCom);
	Safe_Release(m_pAABBCom);

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);

}
