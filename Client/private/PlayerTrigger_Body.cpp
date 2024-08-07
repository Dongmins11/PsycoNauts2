#include "stdafx.h"
#include "PlayerTrigger_Body.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player.h"

#include "EnviromentTrigger.h"
#include "Loboto.h"
#include "DoubleDoorBlocker.h"
#include "AnimatedShelv.h"
#include "Camera_Fly.h"
#include "FallObject.h"
#include "DemoRect.h"
#include "OfficeWalls.h"
#include "Not_Affected_Render.h"
#include "Static_Mesh.h"
#include "ZiantLoboto.h"
#include "DoorZipper.h"
#include "BlenderPath.h"
#include "Blender2.h"
#include "Boss_Dish.h"

#include "Player_CutScene.h"
#include "MoveNpc.h"
#include "PsyBLExit.h"
#include "Tuto_ScriptUI.h"
#include "Skill_UI.h"
#include "Skill_Info.h"

#include "Enemy.h"
#include "FadeInOut.h"
#include "Scene_Stage01.h"
#include "NpcLoboto.h"

CPlayerTrigger_Body::CPlayerTrigger_Body(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CPlayerTrigger(pDevice, pDeviceContext)
{
}

CPlayerTrigger_Body::CPlayerTrigger_Body(const CPlayerTrigger_Body & rhs)
	: CPlayerTrigger(rhs)
{
}

HRESULT CPlayerTrigger_Body::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerTrigger_Body::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_PLAYER_TRIGGER;
	m_eTriggerType = PlayerTriggerType::PTTYPE_CHECKTRIGGER;
	_float3 vColSize = { 0.4f, 1.f, 0.4f };
	m_vColPos = { 0.f,.3f,0.f };//objinfo.fCollider_Center;

	_float3 vObjScale = { 1.f,1.f,1.f };//GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(vColSize, m_vColPos, vObjScale)))
		return E_FAIL;


	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, XMMatrixTranslation(m_vColPos.x, m_vColPos.y, m_vColPos.z));

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	return S_OK;
}

_int CPlayerTrigger_Body::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Get_IsRender())
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}
	RELEASE_INSTANCE(CGameInstance);


	return _int();
}

_int CPlayerTrigger_Body::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Get_IsRender())
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (m_pColliderCom)
		m_pColliderCom->Update_Transform();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CPlayerTrigger_Body::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	//m_pRenderCollider->Render(COLOR_ORANGE);
#endif // _DEBUG

	return S_OK;
}

void CPlayerTrigger_Body::OnTriggerEnter(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	SCENE  eScene = (SCENE)pGameInstance->Get_CurrentSceneIndex();
	RELEASE_INSTANCE(CGameInstance);

	if (eScene == SCENE_DEMO)
	{
		switch (collision.Get_Object()->Get_ObjectLayerType())
		{
		case LayerType::LAYERTYPE_TRIGGER:
		{
			/* 수정 필요 */
			if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_LOBOTO)
			{
				TriggerEnter_ForLoboEvent(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CURLING)
			{
				TriggerEnter_MapCurling(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CLOSE)
			{
				TriggerEnter_MapDeleteCurling(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_TUTOMSG)
			{
				TriggerEnter_TutoUIMsg(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_STATE0_END)
			{
				TriggerEnter_Stage0_End(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_STATE0_NPC)
			{
				TriggerEnter_Stage0_NPCMOVE(collision);
			}
			break;
		}
		default:
			break;
		}
	}
	if (eScene == SCENE_STAGE01)
	{
		switch (collision.Get_Object()->Get_ObjectLayerType())
		{
		case LayerType::LAYERTYPE_TRIGGER:
		{
			
			 if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CURLING)
			{
				TriggerEnter_Map1Curling(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CLOSE)
			{
				TriggerEnter_Map1End(collision); /* 이빨맵 컷 */
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_MONSTER_CREATE)
			{
				TriggerEnter_Stage1_Monster(collision); /* 이빨맵 컷 */
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_SKILL_INFO)
			{
				TriggerEnter_Stage1_SKILL_InfoUI(collision); /* 스테이지1 스킬 사용 가능 UI*/
			}
		break;
		}
		case LayerType::LAYERTYPE_MONSTER_ATTACK:
		{
			Player_HitEntry(collision);
			break;
		}
		case LayerType::LAYERTYPE_MONSTER:
		{
			Player_AttackEntry(collision);
			break;
		}
		default:
			break;
		}
	}
	if (eScene == SCENE_STAGE02)
	{
		switch (collision.Get_Object()->Get_ObjectLayerType())
		{
		case LayerType::LAYERTYPE_TRIGGER:
		{
			if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_MONSTER_CREATE)
			{
				TriggerEnter_Stage2_Monster(collision); /* 이빨맵 컷 */
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_MOUSECUTSCNE)
			{
				TriggerEnter_MouseCutScene(collision); /* 쥐 컷신 */
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_2DMAP)
			{
				TriggerEnter_2DMAP(collision); /* 2D MAP */
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_SKILL_INFO)
			{
				TriggerEnter_Stage2_SKILL_InfoUI(collision); /* SKILL_INFO */
			}
			break;
		}
		case LayerType::LAYERTYPE_MONSTER_ATTACK:
		{
			Player_HitEntry(collision);
			break;
		}
		case LayerType::LAYERTYPE_MONSTER:
		{
			Player_AttackEntry(collision);
			break;
		}
		}
	}
	if (eScene == SCENE_STAGE03)
	{
		switch (collision.Get_Object()->Get_ObjectLayerType())
		{
		case LayerType::LAYERTYPE_TRIGGER:
		{
			if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_BLENDERPATH)
			{
				TriggerEnter_BlenderPath(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CLIMBING)
			{
				TriggerEnter_Stage3_Climbing(collision); /* 믹서기 쪽 그물망 타고 올라가는쪽 트리거 충돌 */
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CLIMBINGEND)
			{
				TriggerEnter_Stage3_ClimbingEND(collision); /* 믹서기 쪽 그물망 타고 올라가는쪽 트리거 충돌 */
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_SLIDE)
			{
				TriggerEnter_Stage3_GroundSlide(collision);
			}
			else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_DEAD)
			{
				TriggerEnter_Stage3_Dead(collision);
			}
			break;
		}
		case LayerType::LAYERTYPE_MONSTER_ATTACK:
		{
			Player_HitEntry(collision);
			break;
		}
		case LayerType::LAYERTYPE_MONSTER:
		{
			Player_AttackEntry(collision);
			break;
		}
		}
	}
	if (eScene == SCENE_STAGE04)
	{
		switch (collision.Get_Object()->Get_ObjectLayerType())
		{
		case LayerType::LAYERTYPE_MONSTER_ATTACK:
		{
			Player_HitEntry(collision);
			break;
		}
		case LayerType::LAYERTYPE_MONSTER:
		{
			Player_AttackEntry(collision);
			break;
		}
		}
	}
	if (eScene == SCENE_STAGE05)
	{
		switch (collision.Get_Object()->Get_ObjectLayerType())
		{
		case LayerType::LAYERTYPE_BOSS_DISH:
		{
			TriggerEnter_Stage5_DishIsIn(collision);
			break;
		}
		case LayerType::LAYERTYPE_MONSTER_ATTACK:
		{
			Player_HitEntry(collision);
			break;
		}
		case LayerType::LAYERTYPE_MONSTER:
		{
			Player_AttackEntry(collision);
			break;
		}
		}
	}
}
//
//void CPlayerTrigger_Parkour::OnTriggerStay(CCollisionObject & collision)
//{
//	//switch (collision.Get_Object()->Get_ObjectLayerType())
//	//{
//	//case LayerType::LAYERTYPE_STATIC:
//	//case LayerType::LAYERTYPE_PICK:
//	//{
//	//	if(!collision.Get_Object()->Get_IsRender())
//	//		collision.Get_Object()->Set_IsRender(true);
//	//	break;
//	//}
//	//default:
//	//	break;
//	//}
//}
//
void CPlayerTrigger_Body::OnTriggerExit(CCollisionObject & collision)
{
	switch (collision.Get_Object()->Get_ObjectLayerType())
	{
	case LayerType::LAYERTYPE_MONSTER:
	{
		Player_AttackExit();
		break;
	}
	case LayerType::LAYERTYPE_TRIGGER:
	{
		if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_BLENDERPATH)
		{
			TriggerExit_BlenderPath(collision);
		}
		else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CLIMBING)
		{
			TriggerExit_Climbing(collision);
		}
		else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_CLIMBINGEND)
		{
			TriggerExit_Stage3_ClimbingEND(collision);
		}
		else if (static_cast<CEnviromentTrigger*>(collision.Get_Object())->Get_EnviromentTriggerType() == EnviromentTriggerType::ETTYPE_SLIDE)
		{
			TriggerExit_Stage3_GroundSlide(collision);
		}
		break;
	}
	case LayerType::LAYERTYPE_BOSS_DISH:
	{
		TriggerExit_Stage5_DishIsOut(collision);
		break;
	}
	}
}

void CPlayerTrigger_Body::OnControllerCollisionHit(CCollisionObject & collision)
{
}


void CPlayerTrigger_Body::Set_Position(_fvector vPos)
{
	if (!m_pTransformCom || !m_pPlayerTransform)
		return;

	_vector vec = XMVector3TransformCoord(XMLoadFloat3(&m_vColPos), XMLoadFloat4x4(m_pPlayerTransform));
	_matrix matTrigger = XMMatrixTranslationFromVector(XMLoadFloat3(&m_vColPos));
	_float4x4 TriggerMatrix = STOREMATRIX(matTrigger * XMLoadFloat4x4(m_pPlayerTransform));
	m_pTransformCom->Set_WorldMatrix(TriggerMatrix);

#ifdef _DEBUG
	m_pRenderCollider->Update(matTrigger * XMLoadFloat4x4(m_pPlayerTransform));
#endif // _DEBUG

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();
}

void CPlayerTrigger_Body::Set_Matrix(const _float4x4 * pMatPtr)
{
	m_pPlayerTransform = pMatPtr;
}

HRESULT CPlayerTrigger_Body::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	m_vColPos = vColPos;
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom, nullptr)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	_float vLength = GET_FLOAT3LENGTH(vColSize);
	if (vLength > 0.f)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		tColliderDesc.isVisualization = false;
		tColliderDesc.isTrigger = true;
		tColliderDesc.pGameObject = this;

		tColliderDesc.isSceneQuery = true;

		CSphereCollider::DESC tBoxColliderDesc;
		tBoxColliderDesc.tColliderDesc = tColliderDesc;
		tBoxColliderDesc.vScale = vColSize;

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
			return E_FAIL;

		m_pColliderCom->Set_ShapeLayer(m_eLayerType);

		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}

#ifdef _DEBUG
	CCollider::COLLIDERDESC		ColliderDesc;
	//_float3 vColSize = { 0.5f, 0.4f, 1.f };
	//m_vColPos = { 0.f,.4f,0.f };//objinfo.fCollider_Center;

	ColliderDesc.vCenter = m_vColPos;//_float3(0.f, 0.f, 0.f);
	ColliderDesc.vExtents = _float3(0.2f, 0.5f, 0.2f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pRenderCollider, &ColliderDesc)))
		return E_FAIL;
#endif

	return S_OK;
}

void CPlayerTrigger_Body::TriggerEnter_ForLoboEvent(CCollisionObject& collision)
{
	// 여기서 트리거에 체킹됐을때 해당 트리거의 값을 바꿔줌
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	_tchar szBuff[MAX_PATH];
	wsprintf(szBuff, TEXT("OnTriggerEnter - Loboto%d\n"), TriggerNum);
	OutputDebugStringW(szBuff);
	/*Fovy Increase*/

	if(2 ==TriggerNum)
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(15);

	if (5 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(25);
		LoboEvent5_FovyIncrease();

	}
	/*Fovy Decrease*/
	else if (6 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(26, 29);
		LoboEvent6_FovyDecrease();

	}

	CLoboto* pLoboto = (CLoboto*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_Loboto"), 0);
	if (!pLoboto)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	pLoboto->SetUpPointNum(TriggerNum);
	if (TriggerNum != 3 /*&& TriggerNum != 6*/)
		pLoboto->Set_Move(true);
	/*Cinema*/
	if (3 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(16, 22);
		LoboEvent3_Cinema();
		pGameInstance->Set_ShadowLightVector(_float3(0.014f, -1.0f, 0.0f), _float3(-85.388f, 27.881f, 89.664f));

	}
	/*Look Fix*/
	else if (4 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(23, 24);
		LoboEvent4_LookFix();
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_MapCurling(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	_tchar szBuff[MAX_PATH];
	wsprintf(szBuff, TEXT("OnTriggerEnter - MapCurling%d\n"), TriggerNum);
	OutputDebugStringW(szBuff);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	if (!pPlayer)
		return;

	pPlayer->Set_CurlingIndex(TriggerNum);
	pGameInstance->Light_Curling(TriggerNum);

	if (2 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(10);
		dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_WalkTiggerCheck(false);
		pGameInstance->Set_ShadowLightVector(_float3(0.0f, -0.998f, 0.068f), _float3(-50.436f, 40.486f, 70.227f));

	}
	if(4 ==TriggerNum)
		pGameInstance->Set_ShadowLightVector(_float3(0.0f, -0.999f, 0.053f), _float3(-119.905f, 36.386f, 104.963f));
	if (5 == TriggerNum)
	{

		//COfficeWalls* pOfficeWalls_Left = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 0));
		//if (nullptr == pOfficeWalls_Left)
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}
		//pOfficeWalls_Left->Set_RectMove(COfficeWalls::RECTMOVE_NORMAL);

		//COfficeWalls* pOfficeWalls_Right = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 1));
		//pOfficeWalls_Right->Set_RectMove(COfficeWalls::RECTMOVE_NORMAL);
		//if (nullptr == pOfficeWalls_Right)
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}
		//CDemoRect* pDemoRect = dynamic_cast<CDemoRect*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_DemoRect"), 0));
		//if (nullptr == pDemoRect)
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}

		//pDemoRect->Set_RectMove(CDemoRect::RECTMOVE_NORMAL);
	}
	if (6 == TriggerNum)
	{
		Parkour6_FovyIncrease();
		pGameInstance->Set_ShadowLightVector(_float3(-0.996f, -0.075f, -0.041f), _float3(-130.888f, 10.865f, 113.824f));
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_MapDeleteCurling(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	_tchar szBuff[MAX_PATH];
	wsprintf(szBuff, TEXT("OnTriggerEnter - MapDeleteCurling%d\n"), TriggerNum);
	OutputDebugStringW(szBuff);
	CDoubleDoorBlocker* pDoubleDoor = (CDoubleDoorBlocker*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_DoubleDoorBlocker"), TriggerNum);

	if (pDoubleDoor)
	{
		if (false == pDoubleDoor->Get_AnimationStartCheck())
			pDoubleDoor->AnimationStart();
	}

	if (1 == TriggerNum) /* 상자 많은곳 */
	{
		_uint Count = pGameInstance->Get_LayerSize(SCENE_DEMO, TEXT("Layer_AnimatedShelv"));
		for (_uint i = 0; i < Count; ++i)
		{
			CAnimatedShelv* pAnimatedShelv = (CAnimatedShelv*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_AnimatedShelv"), i);
			if (pAnimatedShelv)
				pAnimatedShelv->Set_AnimatedShelvStart();
		}

		pGameInstance->Set_ShadowLightVector(_float3(0.0f, -0.998f, 0.068f), _float3(-50.436f, 40.486f, 70.227f));
	}
	if (4 == TriggerNum)
	{
		CZiantLoboto* pZiantLoboto = (CZiantLoboto*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_ZiantLoboto"),0);
		if (nullptr == pZiantLoboto)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pZiantLoboto->Set_Active();

		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_STAGE0_END);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Stage0_End_State(CCamera_Fly::STAGE0_END_LOOK_DOOR);

		((CPlayer_CutScene*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_Player_CutScene"), 0))->Set_Active();
		dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsAnimUpdate(false);

	}
	RELEASE_INSTANCE(CGameInstance);
}
void CPlayerTrigger_Body::TriggerEnter_TutoUIMsg(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_int TriggerNum = (_int)pTriggerObj->Get_TriggerNum();
	_tchar szBuff[MAX_PATH];
	wsprintf(szBuff, TEXT("OnTriggerEnter - TutoUI%d\n"), TriggerNum);
	OutputDebugStringW(szBuff);
	if (m_iTutoUICnt >= TriggerNum)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	m_iTutoUICnt = TriggerNum;
	pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_TutoMessageUI"), TriggerNum)->Set_IsRender(true);

	if (0 == TriggerNum)
	{
		dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_WalkTiggerCheck(true);
		dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_SliderTriggerCheck(true);
	}
	if (1 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(5, 8);
		pGameInstance->Set_ShadowLightVector(_float3(0.f, -0.998f, -0.023f), _float3(-7.155f,37.769f, 55.299f));
	}
	if(2==TriggerNum)
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(11, 14);

	if (5 == TriggerNum)
		dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_SliderTriggerCheck(false);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage0_End(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	if (nullptr == pTriggerObj)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_int TriggerNum = (_int)pTriggerObj->Get_TriggerNum();
	_tchar szBuff[MAX_PATH];
	wsprintf(szBuff, TEXT("OnTriggerEnter - FallObject%d\n"), TriggerNum);
	OutputDebugStringW(szBuff);
	if (0 == TriggerNum)
	{
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_PARKOUR_END);

	}
	else if (1 == TriggerNum)
	{
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Camera_Shaking(true, 1.f,false);

		_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_DEMO, TEXT("Layer_FallObject"));
		for (_uint i = 0; i < iLayerSize; ++i)
		{
			CFallObject* pFallObject = dynamic_cast<CFallObject*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_FallObject"), i));
			if (nullptr == pFallObject)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pFallObject->Set_Falling();
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage0_NPCMOVE(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("OnTriggerEnter - ETTYPE_NPC 트리거가 Enter됌\n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	if (0 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(0);
		CLoboto* pLoboto = (CLoboto*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_Loboto"), 0);
		pLoboto->Set_MoveStartLoboto(true);
		for (_uint i = 0; i < 4; ++i)
			static_cast<CMoveNpc*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveNpc"), i))->Set_MoveStart(true);
	}
	if (1 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(1, 2);
		static_cast<CMoveNpc*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveNpc"), 3))->Set_GoDoor(true);
	}
	if (2 == TriggerNum)
	{
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(3);
		static_cast<CMoveNpc*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveNpc"), 0))->Set_GoDoor(true);
		static_cast<CMoveNpc*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveNpc"), 1))->Set_GoDoor(true);
	}
	if (3 == TriggerNum)
		static_cast<CMoveNpc*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveNpc"), 2))->Set_GoDoor(true);
	
	if(4 == TriggerNum)
		((CTuto_ScriptUI*)(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_NPC_ScriptUI"), 0)))->SetUp_ScriptUI(9);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage1_Monster(CCollisionObject & collision)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	CNpcLoboto* pNpcLoboto = static_cast<CNpcLoboto*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_QuestNpc"), 2));

	if (!pNpcLoboto->IsReady())
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE01, TEXT("Layer_Censor"));
	if (0 == TriggerNum)
	{
		
	}

	else if (1 == TriggerNum)
	{
		if (!m_IsGenerateCensor)
		{
			m_IsGenerateCensor = true;

			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			WorldMatrix._41 = 46.651f;
			WorldMatrix._42 = 4.f;
			WorldMatrix._43 = 1.402f;

			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Spawner"), &WorldMatrix);

			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			WorldMatrix._11 = 0.987f;
			WorldMatrix._13 = -0.161f;
			WorldMatrix._31 = 0.161f;
			WorldMatrix._33 = 0.987f;
			WorldMatrix._41 = 40.116f;
			WorldMatrix._42 = 4.f;
			WorldMatrix._43 = 2.497f;

			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Spawner"), &WorldMatrix);

			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			WorldMatrix._11 = 0.928f;
			WorldMatrix._13 = 0.373f;
			WorldMatrix._31 = -0.373f;
			WorldMatrix._33 = 0.928f;
			WorldMatrix._41 = 52.261f;
			WorldMatrix._42 = 4.f;
			WorldMatrix._43 = 2.415f;

			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Spawner"), &WorldMatrix);

			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE::CAMERATYPE_MONSTER);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_StaticMonsterType(CCamera_Fly::STATICMONSTER::MONSTER_CENSOR);
		}
		else if (m_IsGenerateCensor &&
			0 == iLayerSize &&
			!m_IsGenerateDoubt)
		{
			m_IsGenerateDoubt = true;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			WorldMatrix._11 = -0.993f;
			WorldMatrix._13 = 0.120f;
			WorldMatrix._31 = -0.120f;
			WorldMatrix._33 = -0.993f;
			WorldMatrix._41 = 46.f;
			WorldMatrix._42 = 2.f;
			WorldMatrix._43 = 42.f;

			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Doubt0"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Doubt"), &WorldMatrix);

			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			WorldMatrix._11 = -0.993f;
			WorldMatrix._13 = 0.120f;
			WorldMatrix._31 = -0.120f;
			WorldMatrix._33 = -0.993f;
			WorldMatrix._41 = 49.f;
			WorldMatrix._42 = 2.f;
			WorldMatrix._43 = 42.f;

			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Doubt0"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Doubt"), &WorldMatrix);

			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE::CAMERATYPE_MONSTER);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_StaticMonsterType(CCamera_Fly::STATICMONSTER::MONSTER_REGRET);
		}
	}


	RELEASE_INSTANCE(CGameInstance);
}
 
void CPlayerTrigger_Body::TriggerEnter_Stage2_Monster(CCollisionObject & collision)
{
	/*if (20 < m_iGenerateCount)
		return;*/

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	_float4x4 WorldMatrix;
	_matrix	  FixMatrix = pTransform->Get_WorldMatrix();
	FixMatrix.r[0] *= -1.f;
	FixMatrix.r[2] *= -1.f;
	FixMatrix.r[3] += XMVectorSet(0.f, 2.f, 0.f, 0.f) + pTransform->Get_State(CTransform::STATE_LOOK);

	XMStoreFloat4x4(&WorldMatrix, FixMatrix);
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (0 == TriggerNum)
	{
		if (0 == pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CensorBurly")))
		{
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorBurly"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CensorBurly"), &WorldMatrix);
			FixMatrix.r[3] += pTransform->Get_State(CTransform::STATE_RIGHT)*2.f;
			XMStoreFloat4x4(&WorldMatrix, FixMatrix);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorBurly"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CensorBurly"), &WorldMatrix);
		}
	}

	else if (1 == TriggerNum)
	{
		if (0 == pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BadIdea")))
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_BadIdeaSpawner"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Spawner"), &WorldMatrix);
	}

	else if (2 == TriggerNum)
	{
		if (0 >= pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BadIdea")))
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_BadIdeaSpawner"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Spawner"), &WorldMatrix);
	}

	else if (3 == TriggerNum)
	{
		if (0 >= pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CensorBurly")))
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorBurly"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CensorBurly"), &WorldMatrix);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Map1Curling(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("OnTriggerEnter - ETTYPE_CURLING 트리거가 Enter됌\n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	CNot_Affected_Render* pNot_Affected_Render= (CNot_Affected_Render*)pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Not_AFFECTED_Render"), 1);
	if (!pPlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	pPlayer->Set_CurlingIndex(TriggerNum);
	if (1 == TriggerNum)
	{
		pNot_Affected_Render->Set_Rander_Trigger(false);
		if (false == m_IsSkillRBUse)
		{
			CSkill_Info* pSkill_Info = dynamic_cast<CSkill_Info*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_Info"), 2));
			if (nullptr == pSkill_Info)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_Info->Set_IsRender(true);
			m_IsSkillRBUse = true;
		}
	}

	if (2 == TriggerNum)
	{
		pNot_Affected_Render->Set_Rander_Trigger(true);
		pPlayer->Set_CurlingIndex(TriggerNum);
		pPlayer->Set_Delete_CurlingIndex(0);
		//static_cast<CDoorZipper*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_DoorZipper"), 0))->Set_Delete();

		//_float fWeight[4] = { 0.5f,0.03f,1.f,1.f };
		//CRenderer* pRenderer =dynamic_cast<CRenderer*>(pPlayer->Get_Components(TEXT("Com_Renderer")));
		//pRenderer->Set_BloomEffectWeight(fWeight);
		//pGameInstance->Set_ShadeAmbient(0.2f);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE::CAMERATYPE_MONSTER);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_StaticMonsterType(CCamera_Fly::STATICMONSTER::MONSTER_DENTURE);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Map1End(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("OnTriggerEnter - ETTYPE_CLOSE 트리거가 Enter됌\n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();

	static_cast<CPsyBLExit*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_PsyBlExit1"), 0))->Set_Move(true);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage1_SKILL_InfoUI(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("OnTriggerEnter - Stage1_SKILL_InfoUI됌\n"));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum(); /* 인덱스 몇번쨰 트리거랑 충돌 되었는가 받아올것 */

	if (0 == TriggerNum && false == m_IsSkillRBCheck)
	{
		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_UI->Set_SkillRenderSet(3, true);
		m_IsSkillRBCheck = true;

		CSkill_Info* pSkill_Info = dynamic_cast<CSkill_Info*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_Info"), 1));
		if (nullptr == pSkill_Info)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_Info->Set_IsRender(true);

	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_MouseCutScene(CCollisionObject & collision)
{
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_int TriggerNum = (_int)pTriggerObj->Get_TriggerNum();

	if (m_iCutSceneCnt >= TriggerNum)
		return;

	m_iCutSceneCnt = TriggerNum;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pCameraFly = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Camera", 0);

	dynamic_cast<CCamera_Fly*>(pCameraFly)->Set_CameraType(CCamera_Fly::CAMERTYPE_HAROLDMOUSE);
	dynamic_cast<CCamera_Fly*>(pCameraFly)->Set_HarOldMouse(CCamera_Fly::HAROLDMOUSE_START);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_2DMAP(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_Is2DMapEnter)
	{
		static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsAnimUpdate(false);

		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_STAGE2_2D);

		m_Is2DMapEnter = true;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage2_SKILL_InfoUI(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum(); /* 인덱스 몇번쨰 트리거랑 충돌 되었는가 받아올것 */

	if (1 == TriggerNum && false == m_IsSkillQCheck)
	{
		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_UI->Set_SkillRenderSet(4, true);
		m_IsSkillQCheck = true;

		CSkill_Info* pSkill_Info = dynamic_cast<CSkill_Info*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_Info"), 0));
		if (nullptr == pSkill_Info)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_Info->Set_IsRender(true);

	}
}

void CPlayerTrigger_Body::TriggerEnter_BlenderPath(CCollisionObject & collision)
{
	//OutputDebugStringW(TEXT("OnTriggerEnter - BlenderPath IN Enter됌\n"));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	static_cast<CBlenderPath*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_BlenderPath"), 0))->Set_PlayerBenderEnter();
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerExit_BlenderPath(CCollisionObject & collision)
{
	//OutputDebugStringW(TEXT("OnTriggerEnter - BlenderPath OUT Enter됌\n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	static_cast<CBlenderPath*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_BlenderPath"), 0))->Set_PlayerBenderExit();
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage3_Climbing(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("OnTriggerEnter - Climbing Enter됌\n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());

	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	_matrix TriggerWorldMat = pTriggerObj->Get_WorldMatrix();

	/*_vector vRight = pTriggerObj->Get_WorldMatrix().r[0];
	_vector vUp = pTriggerObj->Get_WorldMatrix().r[1];
	_vector vLook = pTriggerObj->Get_WorldMatrix().r[2];
	_vector vPosition = pTriggerObj->Get_WorldMatrix().r[3];*/

	/* 해당 객체의 클라이밍이 가능하게 진행을 할것입니다. */
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Climbing_TriggerCheck(true, TriggerNum);
	static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum))->Set_Climbing(true, TriggerWorldMat);
	static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum))->Set_Move(true);
	
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerExit_Climbing(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("OnTriggerEnter - Climbing Exitr됌\n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	_matrix TriggerWorldMat = pTriggerObj->Get_WorldMatrix();

	/* 해당 객체의 클라이밍이 불가능하게 진행을 할것입니다. */
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Climbing_TriggerCheck(false,-1);
	static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum))->Set_Climbing(false, TriggerWorldMat);
	static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum))->Set_Move(false);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage3_ClimbingEND(CCollisionObject & collision)
{
	//OutputDebugStringW(TEXT("OnTriggerEnter - Climbing마지막 점프 들어감 \n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Climbing_TriggerEndCheck(true);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerExit_Stage3_ClimbingEND(CCollisionObject & collision)
{
	//OutputDebugStringW(TEXT("OnTriggerEnter - Climbing마지막 점프 나감 \n"));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_Climbing_TriggerEndCheck(false);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage3_GroundSlide(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsSlide(true);
	_uint TriggerNum = pTriggerObj->Get_TriggerNum();


	if (0 == TriggerNum)
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->IsLook_Fix(true, _float3(-1.f, -1.f, -0.8f));
	else if (1 == TriggerNum)
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->IsLook_Fix(true, _float3(-0.3f, -0.2f, 0.6f));
	else if (2 == TriggerNum)
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->IsLook_Fix(true, _float3(1.1f, -0.7f, 0.6f));

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerExit_Stage3_GroundSlide(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsSlide(false);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage3_Dead(CCollisionObject & collision)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEnviromentTrigger* pTriggerObj = static_cast<CEnviromentTrigger*>(collision.Get_Object());
	_uint TriggerNum = pTriggerObj->Get_TriggerNum();
	if (1 == TriggerNum)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);

		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_PLAYER_DEAD);

		pGameInstance->StopSound(PLAYER_DEAD);

		pGameInstance->PlaySoundOnce(TEXT("PlayerDead.wav"),PLAYER_DEAD);

		Safe_Release(pGameInstance);
	}
	else if (0 == TriggerNum)
	{
		static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsDead(true);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::TriggerEnter_Stage5_DishIsIn(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_BOSS_DISH)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
		CGameObject* pDishObject = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Boss_Dish_One", 0);
		if (nullptr == pPlayerObject || nullptr == pDishObject)
		{
			Safe_Release(pGameInstance);
			return;
		}
		Safe_Release(pGameInstance);

		dynamic_cast<CPlayer*>(pPlayerObject)->Set_Boss_DishCheck(true, dynamic_cast<CBoss_Dish*>(pDishObject)->Get_DishType());
	}

}

void CPlayerTrigger_Body::TriggerExit_Stage5_DishIsOut(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_BOSS_DISH)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
		if (nullptr == pPlayerObject)
		{
			Safe_Release(pGameInstance);
			return;
		}

		//여기서 모션변경체크를진행해줌
		_uint iCheckOffIndex = (_uint)CBoss_Dish::BOSS_FOODTYPE_END;
		dynamic_cast<CPlayer*>(pPlayerObject)->Set_Boss_DishCheck(false, iCheckOffIndex);

		Safe_Release(pGameInstance);
	}
}



void CPlayerTrigger_Body::LoboEvent3_Cinema()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_LOBOTO_TALK);
	((CDemoRect*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_DemoRect"), 0))->Set_IsRender(true);
	m_pRendererCom->Set_IsSSAA(false);
	m_pRendererCom->Set_RenderOption(CRenderer::RENDEROPTION_SHADOW, false);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::LoboEvent4_LookFix()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->IsLook_Fix(true, _float3(-1.f, 0.f, 0.f));
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::LoboEvent5_FovyIncrease()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(110.f, 0.8f);
	//COfficeWalls* pOfficeWalls_Left = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 0));
	//if (nullptr == pOfficeWalls_Left)
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return;
	//}
	//pOfficeWalls_Left->Set_RectMove(COfficeWalls::RECTMOVE_SPEEDUP);

	//COfficeWalls* pOfficeWalls_Right = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 1));
	//if (nullptr == pOfficeWalls_Right)
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return;
	//}
	//pOfficeWalls_Right->Set_RectMove(COfficeWalls::RECTMOVE_SPEEDUP);

	//CDemoRect* pDemoRect = dynamic_cast<CDemoRect*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_DemoRect"), 0));
	//if (nullptr == pDemoRect)
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return;
	//}
	//pDemoRect->Set_RectMove(CDemoRect::RECTMOVE_SPEEDUP);
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayerTrigger_Body::LoboEvent6_FovyDecrease()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(70.f, 1.1f);
	RELEASE_INSTANCE(CGameInstance);
}
void CPlayerTrigger_Body::Parkour6_FovyIncrease()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(110.f, 0.5f);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_PARKOUR);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DownDistance(6.f);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->IsLook_Fix(true, _float3(-1.f, 0.f, 0.f));
	m_pRendererCom->Set_IsSSAA(true);
	m_pRendererCom->Set_RenderOption(CRenderer::RENDEROPTION_SHADOW, true);

	RELEASE_INSTANCE(CGameInstance);
}
void CPlayerTrigger_Body::Player_HitEntry(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("Player_HitEntry\n"));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

	if (false == pPlayer->Get_HitingCheck())
	{
		pPlayer->Set_HitCheck(true);

		pPlayer->Set_HitCamShaking(true);

		if (true == collision.Get_Object()->Get_IsPulling() && false == pPlayer->Get_IsKnockBack())
			pPlayer->Set_IsKnockBack(true);
	}

	RELEASE_INSTANCE(CGameInstance);
}
void CPlayerTrigger_Body::Player_AttackExit()
{
	OutputDebugStringW(TEXT("Player_HitExit\n"));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	//pPlayer->Set_HitCheck(false);

	pPlayer->Set_AttackMoveSpeed(1.0);
	pPlayer->Set_DashAttackMoveSpeed(1.0);

	RELEASE_INSTANCE(CGameInstance);
}
void CPlayerTrigger_Body::Player_AttackEntry(CCollisionObject & collision)
{
	OutputDebugStringW(TEXT("Player_AttackEntry\n"));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	CTransform* pEnemyTrans = (CTransform*)(((CEnemy*)collision.Get_Object()->Get_Components(L"Com_Transform")));
	CTransform*	pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, L"Layer_Player", L"Com_Transform", 0);

	_float3	vLook;
	XMStoreFloat3(&vLook, pEnemyTrans->Get_State(CTransform::STATE_POSITION) - pPlayerTrans->Get_State(CTransform::STATE_POSITION));
	vLook.y = 0;
	if (true == pPlayer->Get_AttackCheck())
		pPlayerTrans->Set_MyLook(XMLoadFloat3(&vLook));

	pPlayer->Set_AttackMoveSpeed(0.1);
	pPlayer->Set_DashAttackMoveSpeed(0.0);

	RELEASE_INSTANCE(CGameInstance);
}
CPlayerTrigger_Body * CPlayerTrigger_Body::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerTrigger_Body*	pInstance = new CPlayerTrigger_Body(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayerTrigger_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerTrigger_Body::Clone(void * pArg, _uint iCurrentScene = 0)
{
	CPlayerTrigger_Body*	pInstance = new CPlayerTrigger_Body(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlayerTrigger_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerTrigger_Body::Free()
{
	__super::Free();
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();
#ifdef _DEBUG
	Safe_Release(m_pRenderCollider);
#endif
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
