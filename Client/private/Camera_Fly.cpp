#include "stdafx.h"
#include "..\public\Camera_Fly.h"

#include "GameInstance.h"
#include "Player.h"
#include "Loboto.h"
#include "Player_CutScene.h"
#include "HaroldMouse.h"
#include "CollisionObject.h"
#include "FadeInOut.h"
#include "DoorZipper.h"
#include "Boss_Dish.h"
#include "Boss_Food_Projectile.h"
#include "Boss_Goat.h"
#include "Player_LeftHand.h"
#include "Player_RightHand.h"
#include "Zip.h"
#include "GiantCloche.h"
#include "FruitSpawner.h"
#include "Stage3BossDish.h"
#include "CookingEnd.h"
#include "Scene_Stage03.h"
#include "Enemy.h"
#include "MonsterInfo.h"
#include "NonBT_Otto.h"
#include "NonBT_Hollis.h"
#include "NonBT_Ford.h"
#include "Censor.h"
#include "BossHpBar.h"

// 2D Map
#include "Lock_2D.h"
#include "Player_2D.h"
#include "Monster_2D.h"
#include "Key_2D.h"
#include "Plane_2D.h"
#include "Lock_Object.h"
#include "Skill_UI.h"
#include "Skill_Info.h"

CCamera_Fly::CCamera_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{

}

CCamera_Fly::CCamera_Fly(const CCamera_Fly & rhs)
	: CCamera(rhs),
	m_RotationSpeed(_float2(0.4f, 0.2f)),
	m_eCameraType(CAMERATYPE_INDEPENDENT)
{

}

HRESULT CCamera_Fly::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Fly::NativeConstruct(void * pArg, _uint iSceneIndex)
{

	if (FAILED(__super::NativeConstruct(pArg, iSceneIndex)))
		return E_FAIL;
	memcpy(&m_CameraDesc, pArg, sizeof(CAMEREADESC));

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	POINT		ptMouse = { g_iWinCX >> 1, g_iWinCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);

	SetCursorPos(ptMouse.x, ptMouse.y);
	m_StrLayerName = TEXT("Camera");
	XMStoreFloat3(&m_vBackUpLook, m_pTransform->Get_State(CTransform::STATE_LOOK));
	XMStoreFloat3(&m_vBackUpDestLook, m_pTransform->Get_State(CTransform::STATE_LOOK));

	m_vDependentValue = _float4(0.8f, 0.1f, 4.f, 3.f);
	m_CameraDesc.fFovy = XMConvertToRadians(70.f);
	m_eLobotoTalk = LOBOTOTALK_START;
	m_eJudge = JUDGE_START;
	m_eJudgeDead = JUDGEDEAD_START;
	m_eHarOldMouse = HAROLDMOUSE_START;
	m_eDishThrow = BOSS_DISH_THROW_CHECKING;
	m_eBossDish = BOSS_DISH_POS_START;
	m_eBossHit = BOSS_DISH_HIT_START;
	m_eBossIntro = BOSS_INTRO_START;
	m_eMap_2D = MAP_2D_START;
	m_Ratio = 0.4f;


	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();
	return S_OK;
}

_int CCamera_Fly::Tick(_double TimeDelta)
{
	//if (1 == m_iCountt)
	//{
	//	m_iCountt = 0;
	//	return 0;
	//}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_Stiffness())
	{
		m_isShaking = true;
	}
	else if (!pGameInstance->Get_Stiffness() && m_isShaking)
	{
		m_isShaking = false;
		m_isCollisionShaking = false;
		Set_Camera_Shaking(true, 0.15);

		switch (m_iCurrentScene)
		{
		case Client::SCENE_DEMO:
			Set_Fovy(40.f, 10.0, 60.f, true, true);
			break;
		case Client::SCENE_STAGE01:
			Set_Fovy(40.f, 10.0, 70.f, true, true);
			break;
		case Client::SCENE_STAGE02:
			Set_Fovy(40.f, 10.0, 70.f, true, true);
			break;
		case Client::SCENE_STAGE03:
			Set_Fovy(50.f, 10.0, 80.f, true, true);
			break;
		case Client::SCENE_STAGE04:
			Set_Fovy(40.f, 10.0, 70.f, true, true);
			break;
		case Client::SCENE_STAGE05:
			Set_Fovy(40.f, 10.0, 70.f, true, true);
			break;
		}
	}

	if (m_isCollisionShaking)
	{
		m_isCollisionShaking = false;
		Set_Camera_Shaking(true, 0.15);
	}

	_long		MouseMove = 0;

	/*���콺 ��� ����*/
	if (GetKeyState('C') & 0x8000)
		m_bIsMouseFix = false;
	if (GetKeyState('H') & 0x8000)
		m_bIsMouseFix = true;

	if (true == m_bIsMouseFix)
	{
		POINT        ptMouse = { (g_iWinCX >> 1) + m_vFixAdjustPos.x, (g_iWinCY >> 1) + m_vFixAdjustPos.y };
		ClientToScreen(g_hWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
	}

	if (GetKeyState(VK_F9) & 0x8000)
		m_bMouseMove = false;
	if (GetKeyState(VK_F10) & 0x8000)
		m_bMouseMove = true;
	//if (GetKeyState('Z') & 0x8000)
	//{
	//	Set_Camera_Shaking(true, 1.f);
	//}
	//if (GetKeyState('X') & 0x8000)
	//{
	//	m_bIsShaking = true;
	//}
	//if (GetKeyState(VK_F7) & 0x8000)
	//	dynamic_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 1.f), CFadeInOut::INOUT_OUT);
	//if (GetKeyState(VK_F8) & 0x8000)
	//	dynamic_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 1.f), CFadeInOut::INOUT_IN);



	Lerp_Values(TimeDelta);

	if (CAMERATYPE_DEPENDENT == m_eCameraType)
		m_pTransform->Set_MyLook(XMLoadFloat3(&m_vBackUpDestLook));
	if (true == m_bMouseMove)
	{
		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE::MMS_X))
			m_pTransform->RotationPerSec(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), TimeDelta*MouseMove*(_double)m_vMouseMoveSpeed.x);

		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE::MMS_Y))
		{
			if (false == m_bIsStuck)
			{
				if (true == IsMoveY(MouseMove))
					m_pTransform->RotationPerSec(m_pTransform->Get_State(CTransform::STATE_RIGHT), TimeDelta*MouseMove*(_double)m_vMouseMoveSpeed.y);
			}
			else
			{
				m_pTransform->RotationPerSec(m_pTransform->Get_State(CTransform::STATE_RIGHT), TimeDelta*MouseMove*(_double)m_vMouseMoveSpeed.y);
			}
		}
	}
	//if (GetKeyState('M') & 0x8000)
	//	Set_CameraType(CAMERATYPE_PARKOUR_END);

	if (GetKeyState('X') & 0x8000)
	{
		Set_CameraType(CAMERATYPE_DEPENDENT);
		m_dTimeAcc = 0.0;
		m_Ratio = 0.4f;

	}
	if (CAMERATYPE_DEPENDENT == m_eCameraType)
	{
		Update_Dependent(TimeDelta, m_vDependentValue.x, m_vDependentValue.y, m_vDependentValue.z, m_vDependentValue.w, _float4(0.f, 0.5f, 1.f, 1.f));
	}
	else if (CAMERATYPE_INDEPENDENT == m_eCameraType)
	{
		Update_Independent(TimeDelta);
	}
	else if (CAMERATYPE_TEST == m_eCameraType)
	{
		m_CameraDesc.fFovy = XMConvertToRadians(40.f);
		Test_Update_Dependent(TimeDelta, 0.5f, 0.4f, 3.f, 1.f, _float4(0.f, 0.5f, 0.3f, 1.f));
	}
	else if (CAMERATYPE_LOBOTO_TALK == m_eCameraType)
	{
		Update_Loboto_Talk(TimeDelta);
	}
	else if (CAMERTYPE_PARKOUR == m_eCameraType)
	{
		Update_Parkour(TimeDelta, m_vDependentValue.x, m_vDependentValue.y, m_vDependentValue.z, m_vDependentValue.w, _float4(0.f, 0.5f, 1.f, 1.f));

	}
	else if (CAMERATYPE_PARKOUR_END == m_eCameraType)
	{
		Update_ParkourEnd(TimeDelta, m_vDependentValue.x, m_vDependentValue.y, m_vDependentValue.z, m_vDependentValue.w, _float4(0.f, 0.5f, 1.f, 1.f));
	}
	else if (CAMERATYPE_STAGE0_END == m_eCameraType)
	{
		Update_Stage0_End(TimeDelta);
	}
	else if (CAMERATYPE_PLAYER_DEAD == m_eCameraType)
	{
		Update_Player_Dead(TimeDelta);
	}
	else if (CAMERATYPE_JUDGE == m_eCameraType)
	{
		Update_Judge_Spawn(TimeDelta);
	}
	else if (CAMERATYPE_JUDGE_DEAD == m_eCameraType)
	{
		Update_Judge_Dead(TimeDelta);
	}
	else if (CAMERTYPE_HAROLDMOUSE == m_eCameraType)
	{
		Update_HarOldMouse(TimeDelta);
	}
	else if (CAMERTYPE_CLOUD == m_eCameraType)
	{
		Update_Cloud_CutScene(TimeDelta);
	}
	else if (CAMERTYPE_STAGE2_2D == m_eCameraType)
	{
		Update_Stage2_2D(TimeDelta);
	}
	else if (CAMERATYPE_BOSSDISH_THROW == m_eCameraType)
	{
		m_bMouseMove = false;
		Update_Boss_Throw_Dish(TimeDelta);
	}
	else if (CAMERTYPE_BOSSDISH_POSE == m_eCameraType)
	{
		Update_Boss_Dish(TimeDelta);
	}
	else if (CAMERTYPE_BOSSDISH_HIT == m_eCameraType)
	{
		Update_Boss_Hit(TimeDelta);
	}
	else if (CAMERTYPE_BOSSINTRO == m_eCameraType)
	{
		Update_Boss_Intro(TimeDelta);
	}
	else if (CAMERTYPE_STAGE3 == m_eCameraType)
	{
		Update_Stage3_CUT_SCENE(TimeDelta);
	}
	else if (CAMERATYPE_MONSTER == m_eCameraType)
	{
		Update_StaticMonsterView(TimeDelta);
	}

	RELEASE_INSTANCE(CGameInstance);

	if (GetKeyState('O') & 0x8000)
		m_bIsStay = false;

	if (GetKeyState('P') & 0x8000)
		m_bIsStay = true;


	if (1 == m_iCountt)
	{
		_uint i = 0;
	}
	Shaking(TimeDelta);

	if (CAMERATYPE_DEPENDENT == m_eCameraType)
		Camera_Collide();

	_matrix		ViewMatrix = m_pTransform->Get_WorldMatrixInverse();
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, ViewMatrix);

	_matrix ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix);

	return __super::Tick(TimeDelta);
}

_int CCamera_Fly::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_Fly::Render()
{
	return S_OK;
}

_float4x4 CCamera_Fly::Get_LightView(_float3 vLightDir, _float3 vLightPos)
{
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

	m_pTransform->Set_MyLook(XMLoadFloat3(&vLightDir));
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vLightPos));

	_float4x4 View;
	XMStoreFloat4x4(&View, m_pTransform->Get_WorldMatrixInverse());

	_float4x4 WorldMatrixFloat;

	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	m_pTransform->Set_WorldMatrix(WorldMatrixFloat);
	return View;
}

_matrix CCamera_Fly::Get_View()
{
	_matrix		ViewMatrix = m_pTransform->Get_WorldMatrixInverse();
	return ViewMatrix;
}

_matrix CCamera_Fly::Get_ReflectionView(_float fHeight)
{
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

	//m_pTransform->Set_MyLook(XMLoadFloat3(&vLightDir));0
	_float3 vPos = STOREFLOAT3(m_pTransform->Get_State(CTransform::STATE_POSITION));

	_float vPosY = vPos.y;
	//vPosY -= (vPosY - fHeight)*2.f;
	vPosY = -vPosY + (fHeight)*2.f;
	vPos.y = vPosY;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));

	_float3 vLook = STOREFLOAT3(m_pTransform->Get_State(CTransform::STATE_LOOK));

	_float vLookY = vLook.y;
	vLookY *= -1.f;
	vLook.y = vLookY;

	m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));

	_matrix View = m_pTransform->Get_WorldMatrixInverse();

	_float4x4 WorldMatrixFloat;

	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	m_pTransform->Set_WorldMatrix(WorldMatrixFloat);
	return View;
}

void CCamera_Fly::Set_CameraType(CAMERATYPE eCameraType)
{
	m_eCameraType = eCameraType;

	switch (eCameraType)
	{
	case Client::CCamera_Fly::CAMERATYPE_INDEPENDENT:
		break;
	case Client::CCamera_Fly::CAMERATYPE_DEPENDENT:
		m_Ratio = 0.4f;
		m_bMouseMove = true;
		m_bLookFix = false;
		m_bIsMouseFix = true;

		Set_Default_Value();
		break;
	case Client::CCamera_Fly::CAMERATYPE_TEST:
		break;
	case Client::CCamera_Fly::CAMERATYPE_LOBOTO_TALK:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pPlayer->Set_IsAnimUpdate(false);
		RELEASE_INSTANCE(CGameInstance);

	}
	break;
	case Client::CCamera_Fly::CAMERTYPE_LOOKFIX:
		break;
	case Client::CCamera_Fly::CAMERTYPE_PARKOUR:
		m_Ratio = 0.0f;
		m_bIsLerp = true;
		break;
	case Client::CCamera_Fly::CAMERATYPE_PARKOUR_END:
		m_Ratio = 0.05f;
		//m_bMouseMove = false;
		m_pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vBackUpDestLook));
		break;
	case Client::CCamera_Fly::CAMERATYPE_STAGE0_END:
		m_bMouseMove = false;
		m_dStage0_End_TimeAcc = 0.0;
		m_bIsLerp = true;
		break;
	case Client::CCamera_Fly::CAMERATYPE_END:
		break;
	case Client::CCamera_Fly::CAMERATYPE_PLAYER_DEAD:
		break;
	case Client::CCamera_Fly::CAMERATYPE_JUDGE:
	{
		m_eJudge = JUDGE_START;
		m_Ratio = 0.0f;
		m_bIsLerp = true;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pPlayer->Set_IsAnimUpdate(false);
		RELEASE_INSTANCE(CGameInstance);

	}
	break;
	case Client::CCamera_Fly::CAMERATYPE_JUDGE_DEAD:
	{
		m_eJudgeDead = JUDGEDEAD_START;
		m_Ratio = 0.0f;
		m_bIsLerp = true;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pPlayer->Set_IsAnimUpdate(false);
		RELEASE_INSTANCE(CGameInstance);

	}
	break;
	case Client::CCamera_Fly::CAMERTYPE_HAROLDMOUSE:
	{
		m_Ratio = 0.0f;
		m_bIsLerp = true;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pPlayer->Set_IsAnimUpdate(false);
		RELEASE_INSTANCE(CGameInstance);
	}
	break;
	case Client::CCamera_Fly::CAMERTYPE_CLOUD:
	{
		m_Ratio = 0.0f;
		m_bIsLerp = true;
		m_OnesCheck = true;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (m_CloudScene != CLOUD_11 || m_CloudScene != CLOUD_16)
			pPlayer->Set_IsAnimUpdate(false);

		//if(m_CloudScene == CLOUD_11)
		//	pPlayer->Set_IsAnimUpdate(true);

		RELEASE_INSTANCE(CGameInstance);
	}
	break;
	default:
		break;
	}
}

void CCamera_Fly::Set_Camera_Transform_Ptr(CAMERATYPE eCameraType, const _float4x4 * pWorldMatrix)
{
	if (nullptr != pWorldMatrix)
	{
		if (nullptr != m_pTargetObjectMatrix)
			m_pTargetObjectMatrix = nullptr;

		m_eCameraType = eCameraType;
		m_pTargetObjectMatrix = pWorldMatrix;
	}
}

void CCamera_Fly::Set_DependentType_Value(_float4 vValue, _float fIntervalZ, _double dLerpSpeed)
{
	m_bLerpOnce = true;
	m_vDestDependentValue = vValue;
	m_fDestDependentIntervalZ = fIntervalZ;
	m_dDependentLerpSpeed = dLerpSpeed;
}

void CCamera_Fly::Set_DependentType_Value_Default(_double dLerpSpeed)
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (SCENE_STAGE05 == pGameInstance->Get_CurrentSceneIndex())
		m_vDestDependentValue = _float4(0.8f, 0.1f, 7.4f, 4.5f);
	else
		m_vDestDependentValue = _float4(0.8f, 0.1f, 4.f, 3.f);
	Safe_Release(pGameInstance);


	m_bLerpOnce = true;
	m_fDestDependentIntervalZ = 2.f;
	m_dDependentLerpSpeed = dLerpSpeed;
}

void CCamera_Fly::Set_Fovy(_float fAngle, _double dRatioSpeed, _float dBackUpFovy, _bool IsShaking, _bool IsRecovery)
{
	m_fSourFovy = m_CameraDesc.fFovy;
	if (IsShaking)
		m_fBackUpFovy = dBackUpFovy;
	else
		m_fBackUpFovy = m_fSourFovy;
	m_fDestFovy = XMConvertToRadians(fAngle);
	m_dFovyLerpSpeed = dRatioSpeed;
	m_bIsChangeFovy = true;
	m_bIsFovyReverse = IsRecovery;
}

void CCamera_Fly::Set_InterverZ(_bool bCheck, _float fValueZ, _double lfLerpSpeed)
{
	m_IsLerpZCheck = true;
	m_fLerpInterverStartZ = fValueZ;
	m_lfZInterverSpeed = lfLerpSpeed;
}

void CCamera_Fly::Set_Stage3UI_Render_Check(_bool bCheck)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint  Count = 0;

	Count = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_CookingUI_Background"));
	for (_uint i = 0; i < Count; ++i)
		pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_CookingUI_Background"), i)->Set_IsRender(bCheck);
	/* ------------------------------------------------------------------------------------------------ */
	
	Count = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"));
	for (_uint i = 0; i < Count; ++i)
		pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), i)->Set_IsRender(bCheck);
	/* ------------------------------------------------------------------------------------------------ */

	Count = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"));
	for (_uint i = 0; i < Count; ++i)
		pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_CookingUI_FruitImage"), i)->Set_IsRender(bCheck);
	/* ------------------------------------------------------------------------------------------------ */

	Count = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_TimerUI"));
	for (_uint i = 0; i < Count; ++i)
		pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_TimerUI"), i)->Set_IsRender(bCheck);
	/* ------------------------------------------------------------------------------------------------ */

	Count = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_ProgressBarUI"));
	for (_uint i = 0; i < Count; ++i)
		pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_ProgressBarUI"), i)->Set_IsRender(bCheck);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCamera_Fly::SetUp_Component()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&_float4(m_CameraDesc.vEye, 1.f)));

	_vector		vLook = XMLoadFloat4(&_float4(m_CameraDesc.vAt, 1.f)) - XMLoadFloat4(&_float4(m_CameraDesc.vEye, 1.f));
	m_pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	_vector		vRight;
	vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));

	_vector		vUp;
	vUp = XMVector3Cross(vLook, vRight);
	m_pTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	//tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = false;

	tColliderDesc.isSceneQuery = true;


	tColliderDesc.pGameObject = this;




	CSphereCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(0.1f, 0.1f, 0.1f);


	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_CAMERA;
	_matrix smatPviot = XMMatrixTranslation(0.f, 0.f, 0.f);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	//m_pColliderCom->Update_TransformPhysX();
	//m_pColliderCom->Update_Transform();

	m_isActorActive = false;
	SettingActorActive();

	return S_OK;
}

void CCamera_Fly::Camera_Collide()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
	CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
	// raycasting �� �������� ( ���� player�� ���ʿ� �Ƹ� �Ǿ����� ���ɼ��� ���� �Ƹ� ����κ����� ����������ҵ� )
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == pPlayerTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	_float3 vPlayerPos = pPlayerTransform->Get_State_Float3(CTransform::STATE_POSITION);
	vPlayerPos.y += m_fBias;


	tDescIn.vOrigin = vPlayerPos;// +XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP))*0.35f);// ;
								 // raycasting ���� ( �÷��̾��� look������ �⺻(��Ż�� ���� �÷��̾� ���� ������ �ٶ󺸱� ����)
	XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(m_pTransform->Get_Position() - XMLoadFloat3(&vPlayerPos)));
	// raycast ����
	tDescIn.fDistance = XMVectorGetX(XMVector3Length(m_pTransform->Get_Position() - XMLoadFloat3(&vPlayerPos)));
	// � ������Ʈ�� �ε��� �������� ���� ���� (����, �׸��� static�� �ε�����(���� �׺�޽��� static���� �Ǿ�����)
	tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
	// � ���̾������Ʈ���� �浹�� ������ ( �׺�޽��� layerType�� Plane���� �����ؼ� Plane�� üŷ����)
	// ����̾�� �����ϰ� ������ Set_ShapeLayer() �Լ� ���
	tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PLANE);
	// �ε��� ������Ʈ�� �޾ƿ��� ���� ����
	CGameObject* pHitObject = nullptr;
	// Out�� �޾ƿ��� ������ �ּҰ� ������
	tDescOut.ppHitObject = &pHitObject;
	if (pGameInstance->Raycast(tDescIn, tDescOut))
	{
		//�÷��̾� ������� �ϱ� 
		_float fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPlayerPos) - XMLoadFloat3(&tDescOut.vHitPos)));
		if (false == m_bIsPlayerAlphaChange && 1.f > fDis)
		{
			dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsAlphaDown(4.0);
			m_bIsPlayerAlphaChange = true;
			//dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsRender(false);

		}
		if (true == m_bIsPlayerAlphaChange && 1.f < fDis)
		{
			dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsAlphaUp(4.);
			m_bIsPlayerAlphaChange = false;
			//dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsRender(true);

		}
		_float3 vHitPos = tDescOut.vHitPos;
		//vHitPos.y += m_fBias_Down;
		m_pTransform->Set_State_Float3(CTransform::STATE_POSITION, vHitPos);
	}
	else
	{
		if (true == m_bIsPlayerAlphaChange)
		{
			dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsAlphaUp(4.);
			m_bIsPlayerAlphaChange = false;
			//dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsRender(true);

		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Fly::Update_Dependent(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (true == m_bLookFix)
	{
		m_pTransform->Set_MyLook(XMLoadFloat3(&m_vLookFixDir));
		m_Ratio = 0.1f;

		if (true == m_bIsLimitTime)
		{
			if (m_dLimitTime > m_dLookFixTimeAcc)
				m_dLookFixTimeAcc += TimeDelta;
			else
			{
				m_dLimitTime = 0.0;
				m_dLookFixTimeAcc = 0.0;
				m_bLookFix = false;
				m_bIsLimitTime = false;
			}
		}
	}

	_matrix PlayerWorldMatrix = pTransform->Get_WorldMatrix();
	_matrix CameraWorldMatrix = m_pTransform->Get_WorldMatrix();
	_vector vSourPos, vDestPos;
	_vector vSourLook, vDestLook;
	_float4 vPos;
	vPos = Get_StandardPos();
	CameraWorldMatrix.r[3] = XMLoadFloat4(&vPos);
	vDestPos = XMVector3TransformCoord(Compute_Pos_Dependent(TimeDelta, fMaxDot, fMinDot, fUpDis, fDownDis, vInterval, vPos), CameraWorldMatrix);
	vSourPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

	vDestLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	XMStoreFloat3(&m_vBackUpDestLook, vDestLook);

	vSourLook = XMLoadFloat3(&m_vBackUpLook);
	_float3 vMyPos, vMyLook;
	m_fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPos) - vDestPos));

	_float3 vDir;

	if (true == m_bIsLerp)
	{
		Lerp_Once(TimeDelta, vSourPos, vDestPos, vSourLook, vDestLook, &vMyLook, &vMyPos);
		XMStoreFloat3(&vDir, XMVector3Normalize(XMLoadFloat3(&vMyPos) - XMLoadFloat4(&vPos)));
		XMStoreFloat3(&vMyPos, XMLoadFloat4(&vPos) + XMLoadFloat3(&vDir)*m_fLength);
		//if (false == m_bLookFix)
		//{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vMyPos));
		m_pTransform->Set_MyLook(XMLoadFloat3(&vDir)*-1.f);
		if (true == m_bIsShaking)
		{
			_float fRandom[3];
			fRandom[0] = (rand() % 10 - 5)*0.005f;
			fRandom[1] = (rand() % 10 - 5)*0.005f;
			fRandom[2] = (rand() % 10 - 5)*0.005f;

			vDir.x += fRandom[0];
			vDir.y += fRandom[1];
			vDir.z += fRandom[2];

			m_pTransform->Set_MyLook(XMLoadFloat3(&vDir)*-1.f);
		}
		//}
		//else
		//{
		//	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vMyPos));
		//}
		//if (true == m_bParkourEnd)
		//{
		//	m_dTimeAcc += TimeDelta;
		//	_vector vLook = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&vDir), m_dTimeAcc);
		//	m_pTransform->Set_MyLook(vLook);
		//	if (m_dTimeAcc > 1.f)
		//		m_bParkourEnd = false;
		//}
	}
	else
		m_pTransform->Set_State(CTransform::STATE_POSITION, vDestPos);

	//m_vBackUpLook = vDir;
	XMStoreFloat3(&m_vBackUpLook, m_pTransform->Get_State(CTransform::STATE_LOOK));

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Fly::Test_Update_Dependent(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval)
{
	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pTargetObjectMatrix)
	{
		_matrix TargetMatrix = XMLoadFloat4x4(m_pTargetObjectMatrix);

		_vector vTargetPos = TargetMatrix.r[3];
		_vector vTargetLook = TargetMatrix.r[2];
		_vector   vTargetRight = TargetMatrix.r[0];

		_float4 TargetPos;

		vTargetPos += XMVector4Normalize(vTargetLook) * -0.135f;
		vTargetPos -= XMVector4Normalize(vTargetRight) * 0.046f;

		XMStoreFloat4(&TargetPos, vTargetPos);

		TargetPos.y += 0.54f;

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&TargetPos));

		m_pTransform->Set_MyLook(vTargetLook);
	}

	RELEASE_INSTANCE(CGameInstance);
}


void CCamera_Fly::Update_Independent(_double TimeDelta)
{
	if (!m_isRender)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransform->Go_Straight(TimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransform->Go_Backward(TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransform->Go_Left(TimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransform->Go_Right(TimeDelta);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Fly::Update_Loboto_Talk(_double TimeDelta)
{
	switch (m_eLobotoTalk)
	{
	case Client::CCamera_Fly::LOBOTOTALK_START:
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-82.065f, 2.039f, 76.768f, 1.f));
		m_pTransform->Set_MyLook(XMVectorSet(0.0f, -0.473f, 0.908f, 0.f));
		m_bMouseMove = false;

		m_LerpSpeed = 2.f;
		m_bIsLerp = true;
		m_eLobotoTalk = LOBOTOTALK_TALK;
		break;
	case Client::CCamera_Fly::LOBOTOTALK_TALK:
	{
		if (true == m_bIsLerp)
		{
			_float3 vPos;
			Lerp_Once(TimeDelta, XMVectorSet(-82.065f, 2.5f, 76.768f, 1.f), XMVectorSet(-82.065f, 2.5f, 80.536f, 1.f), &vPos);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
		}
		else
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CModel* pLobotoModel = dynamic_cast<CModel*>(pGameInstance->Get_Components(SCENE_DEMO, TEXT("Layer_Loboto"), TEXT("Com_Model")));
			if (nullptr == pLobotoModel)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			if (true == pLobotoModel->Get_AnimNextFinish(4))
			{
				m_eLobotoTalk = LOBOTOTALK_RUN;
				m_bIsLerp = true;
				m_LerpSpeed = 1.f;
			}
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	}
	case Client::CCamera_Fly::LOBOTOTALK_RUN:
	{
		if (true == m_bIsLerp)
		{
			_float3 vLook;
			Lerp_Once(TimeDelta, XMVectorSet(0.0f, -0.473f, 0.908f, 0.f), XMVectorSet(0.0f, 0.f, 0.908f, 0.f), &vLook);
			m_pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vLook));
		}
		else
		{

			m_eLobotoTalk = LOBOTOTALK_FINISH;
			m_bIsLerp = true;
			m_LerpSpeed = 2.f;

		}
		break;
	}
	case Client::CCamera_Fly::LOBOTOTALK_FINISH:
		if (m_dLobotoRunFinishTime > m_dLobotoTimeAcc)
		{
			m_dLobotoTimeAcc += TimeDelta;
		}
		else
		{
			m_pTransform->Set_MyLook(XMVectorSet(0.0f, 0.f, 0.908f, 0.f));
			m_vBackUpDestLook = m_vBackUpLook = _float3(0.0f, 0.f, 0.908f);
			m_bIsLerp = true;
			m_bMouseMove = true;
			m_dLobotoTimeAcc = 0.0f;
			m_LerpSpeed = 1.0;
			m_eLobotoTalk = LOBOTOTALK_END;
			Set_CameraType(CAMERATYPE_DEPENDENT);
			//IsLook_Fix(true, _float3(0.063f, 0.003f, 0.998f), true, 1.f);
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
			if (nullptr == pPlayer)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pPlayer->Set_IsAnimUpdate(true);
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	case Client::CCamera_Fly::LOBOTOTALK_END:
		Set_CameraType(CAMERATYPE_DEPENDENT);
		break;
	default:
		break;
	}
}

void CCamera_Fly::Update_Judge_Spawn(_double TimeDelta)
{
	switch (m_eJudge)
	{
	case Client::CCamera_Fly::JUDGE_START:
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(41.730f, 6.584f, 7.229f, 1.f));
		m_pTransform->Set_MyLook(XMVectorSet(0.347f, -0.173f, 0.922f, 0.f));
		m_bMouseMove = false;

		m_LerpSpeed = 0.5f;
		m_bIsLerp = true;
		m_eJudge = JUDGE_CAMERA_1;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		pPlayer->Set_HpRender(SCENE_STAGE03, false);
		((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_HpBar"), 0))->Set_IsRender(false);
		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_UI->Set_AllSkillRenderSet(false);

		//SOUND
		if (FAILED(pGameInstance->PlayBGM(TEXT("MiddleBossBackGraound.wav"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);

		break;
	}
	case Client::CCamera_Fly::JUDGE_CAMERA_1:
	{
		if (true == m_bIsLerp)
		{
			_float3 vPos;
			_float3 vLook;
			Lerp_Once(TimeDelta, XMVectorSet(41.730f, 6.584f, 7.229f, 1.f), XMVectorSet(42.986f, 2.436f, 13.386f, 1.f), XMVectorSet(0.347f, -0.173f, 0.922f, 0.f), XMVectorSet(0.165f, 0.255f, 0.953f, 0.f), &vLook, &vPos);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			m_eJudge = JUDGE_CAMERA_2;
			m_bIsLerp = true;
			m_LerpSpeed = 0.75f;
		}
		break;
	}
	case Client::CCamera_Fly::JUDGE_CAMERA_2:
	{
		if (true == m_bIsLerp)
		{
			_float3 vPos;
			_float3 vLook;
			Lerp_Once(TimeDelta, XMVectorSet(42.986f, 2.436f, 13.386f, 1.f), XMVectorSet(43.736f, 2.329f, 16.929f, 1.f), XMVectorSet(0.165f, 0.255f, 0.953f, 0.f), XMVectorSet(0.334f, 0.412f, 0.848f, 0.f), &vLook, &vPos);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			m_eJudge = JUDGE_CAMERA_3;
			m_bIsLerp = true;
			m_LerpSpeed = 0.75f;
		}
		break;
	}
	case Client::CCamera_Fly::JUDGE_CAMERA_3:
		if (true == m_bIsLerp)
		{
			_float3 vLook;
			Lerp_Once(TimeDelta, XMVectorSet(0.334f, 0.412f, 0.848f, 0.f), XMVectorSet(0.399f, 0.474f, 0.785f, 0.f), &vLook);
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			//IsLook_Fix(true, _float3(0.063f, 0.003f, 0.998f), true, 1.f);
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_MonsterUI"));
			if (0 == iLayerSize)
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
				if (nullptr == pPlayer)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				pPlayer->Set_IsAnimUpdate(true);

				m_eJudge = JUDGE_END;
				m_bIsLerp = true;
				m_bMouseMove = true;
				m_LerpSpeed = 0.75f;
				Set_CameraType(CAMERATYPE_DEPENDENT);

				pPlayer->Set_HpRender(SCENE_STAGE03, true);
				((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_HpBar"), 0))->Set_IsRender(true);

				CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Skill_UI"), 0));
				if (nullptr == pSkill_UI)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				pSkill_UI->Set_AllSkillRenderSet(true);
			}
			RELEASE_INSTANCE(CGameInstance);

		}
		break;
	case Client::CCamera_Fly::JUDGE_END:
		Set_CameraType(CAMERATYPE_DEPENDENT);
		break;
	default:
		break;
	}
}

void CCamera_Fly::Update_Judge_Dead(_double TimeDelta)
{
	switch (m_eJudgeDead)
	{
	case Client::CCamera_Fly::JUDGEDEAD_START:
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(44.539f, 2.227f, 21.052f, 1.f));
		m_pTransform->Set_MyLook(XMVectorSet(0.247f, 0.070f, 0.966f, 0.f));
		m_bMouseMove = false;
		Set_Far(70.f);
		m_LerpSpeed = 0.15f;
		m_bIsLerp = true;
		m_eJudgeDead = JUDGEDEAD_1;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		pPlayer->Set_HpRender(SCENE_STAGE03, false);
		((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_HpBar"), 0))->Set_IsRender(false);
		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_UI->Set_AllSkillRenderSet(false);
		RELEASE_INSTANCE(CGameInstance);

		break;
	}
	case Client::CCamera_Fly::JUDGEDEAD_1:
	{
		if (true == m_bIsLerp)
		{
			_float3 vLook;

			Lerp_Once(TimeDelta, XMVectorSet(0.247f, 0.070f, 0.966f, 0.f), XMVectorSet(0.508f, -0.015f, 0.861f, 0.f), &vLook);
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			m_eJudgeDead = JUDGEDEAD_2;
			m_bIsLerp = true;
			m_LerpSpeed = 0.5f;
		}
		break;
	}
	case Client::CCamera_Fly::JUDGEDEAD_2:
	{
		if (true == m_bIsLerp)
		{
			_float3 vLook;

			Lerp_Once(TimeDelta, XMVectorSet(0.508f, -0.015f, 0.861f, 0.f), XMVectorSet(0.659f, 0.180f, 0.730f, 0.f), &vLook);
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			m_eJudgeDead = JUDGEDEAD_3;
			m_bIsLerp = true;
			m_LerpSpeed = 0.5f;
		}
		break;
	}
	case Client::CCamera_Fly::JUDGEDEAD_3:
		if (true == m_bIsLerp)
		{
			_float3 vLook;

			Lerp_Once(TimeDelta, XMVectorSet(0.659f, 0.180f, 0.730f, 0.f), XMVectorSet(0.602f, 0.539f, 0.589f, 0.f), &vLook);
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_MonsterUI"));
			if (0 == iLayerSize)
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
				if (nullptr == pPlayer)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
				((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_OUT);
				RELEASE_INSTANCE(CGameInstance);
				pPlayer->Set_IsAnimUpdate(true);

				m_eJudgeDead = JUDGEDEAD_4;
				m_bIsLerp = true;
				m_bMouseMove = true;
				m_LerpSpeed = 0.75f;
			}
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	case Client::CCamera_Fly::JUDGEDEAD_4:
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
		{
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CCharacterController* pPlayer_Controller = dynamic_cast<CCharacterController*>(pGameInstnace->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Controller")));
			pPlayer_Controller->Set_Position(_float3(236.62f, 3.15f, 177.274f));

			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_IN);
			m_Ratio = 0.0;
			m_eJudgeDead = JUDGEDEAD_END;
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
			if (nullptr == pPlayer)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pPlayer->Setting_CookingContestUI();
			CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGameInstnace->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Renderer")));
			pRenderer->Set_RenderOption(CRenderer::RENDEROPTION_SHADOW, false);
			m_eJudgeDead = JUDGEDEAD_END;

			pPlayer->Set_HpRender(SCENE_STAGE03, true);

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstnace->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_AllSkillRenderSet(true);

			pGameInstnace->StopAll();

			pGameInstnace->StopSound(STAGE3ONES);

			pGameInstnace->PlaySoundOnce(TEXT("Stage3Start.wav"), STAGE3ONES);

			pGameInstnace->SettingVolume(0.6f, STAGE3ONES);

			pGameInstnace->PlayBGM(TEXT("Stage3BackGroundSound.wav"));

			RELEASE_INSTANCE(CGameInstance);
			Set_Far(200.f);
		}
		break;
	}
	case Client::CCamera_Fly::JUDGEDEAD_5:
	{
	}
	case Client::CCamera_Fly::JUDGEDEAD_END:
	{
		Set_CameraType(CAMERATYPE_DEPENDENT);
		break;
	}
	default:
		break;
	}
}

void CCamera_Fly::Update_HarOldMouse(_double TimeDelta)
{
	switch (m_eHarOldMouse)
	{
	case Client::CCamera_Fly::HAROLDMOUSE_START:
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-9.384f, 2.093f, 48.577f, 1.f));
		m_pTransform->Set_MyLook(XMVectorSet(-0.057f, 0.222f, 0.973f, 0.f));
		m_bMouseMove = false;

		m_LerpSpeed = 0.3f;
		m_bIsLerp = true;

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STATIC, TEXT("Layer_Player"));
		if (1 == iLayerSize)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
			CCharacterController* pPlayer_Controller = dynamic_cast<CCharacterController*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Controller")));
			CTransform* pPlayer_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

			dynamic_cast<CPlayer_CutScene*>(pGameInstance->Get_GameObject(SCENE_STAGE02, L"Layer_Player_CutScene", 0))->Set_Enable_Stage2(true);
			CTransform* pPlayer_CutScene_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_Player_CutScene"), TEXT("Com_Transform")));
			CModel* pPlayer_CutScene_Model = dynamic_cast<CModel*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_Player_CutScene"), TEXT("Com_Model")));

			if (nullptr == pPlayer || nullptr == pPlayer_Controller ||
				nullptr == pPlayer_CutScene_Transform || nullptr == pPlayer_CutScene_Model)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			//Player
			pPlayer->Set_IsAnimUpdate(false);
			pPlayer_Controller->Set_FootPosition(_float3(-14.218f, 1.144f, 41.891f));
			pPlayer_Transform->Set_MyLook(XMVectorSet(-0.826f, 0.f, -0.564f, 0.f));

			//CutScene
			pPlayer_CutScene_Transform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-9.839f, 1.126f, 49.142f, 1.f));
			pPlayer_CutScene_Transform->Set_MyLook(XMVectorSet(0.355f, 0.f, 0.935f, 0.f));
			pPlayer_CutScene_Model->Set_Animation(3);

			pPlayer->Set_HpRender(SCENE_STAGE02, false);

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_AllSkillRenderSet(false);

			if (true == pPlayer_CutScene_Model->Get_IsAnimFinishied(3))
			{
				if (FAILED(pGameInstance->PlaySoundOnce(TEXT("CutSceneMouseSound.wav"), CHANNELID::MOUSE_CUTSCENE)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				m_eHarOldMouse = HAROLDMOUSE_CAM1;
			}

		}

		RELEASE_INSTANCE(CGameInstance);

		break;
	}
	case Client::CCamera_Fly::HAROLDMOUSE_CAM1:
	{
		if (true == m_bIsLerp)
		{
			_float3 vLook;
			Lerp_Once(TimeDelta, XMVectorSet(-0.057f, 0.222f, 0.973f, 0.f), XMVectorSet(-0.826f, 0.f, -0.564f, 0.f), &vLook);
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			m_pTransform->Set_MyLook(XMVectorSet(-0.826f, 0.f, -0.564f, 0.f));
			m_eHarOldMouse = HAROLDMOUSE_CAM2;
			m_bIsLerp = true;
			m_LerpSpeed = 0.5f;
		}
		break;
	}
	case Client::CCamera_Fly::HAROLDMOUSE_CAM2:
	{
		if (true == m_bIsLerp)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CTransform* pPlayer_CutScene_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_Player_CutScene"), TEXT("Com_Transform")));
			CModel* pPlayer_CutScene_Model = dynamic_cast<CModel*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_Player_CutScene"), TEXT("Com_Model")));

			CCharacterController* pMouse_Controller = dynamic_cast<CCharacterController*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_HaroldMouse"), TEXT("Com_Controller")));
			CTransform* pMouse_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_HaroldMouse"), TEXT("Com_Transform")));
			CModel* pMouse_Model = dynamic_cast<CModel*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_HaroldMouse"), TEXT("Com_Model")));


			if (nullptr == pPlayer_CutScene_Transform || nullptr == pPlayer_CutScene_Model ||
				nullptr == pMouse_Controller || nullptr == pMouse_Transform || nullptr == pMouse_Model)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			_vector vLook = XMVectorSet(-0.942f, -0.186f, -0.281f, 0.f);
			_vector vStartPos = XMVectorSet(-21.6609859f, 1.14059436f, 40.2531700f, 1.f);
			_vector vEndPos = XMVectorSet(-15.850f, 1.14059436f, 40.2531700f, 1.f);
			_float3	vLerpMousePosition;


			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-13.818f, 2.722f, 41.487f, 1.f));
			m_pTransform->Set_MyLook(vLook);

			pPlayer_CutScene_Model->Set_Animation(0);
			pPlayer_CutScene_Transform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-14.218f, 1.144f, 41.891f, 0.f));
			pPlayer_CutScene_Transform->Set_MyLook(pMouse_Transform->Get_State(CTransform::STATE_POSITION) - pPlayer_CutScene_Transform->Get_State(CTransform::STATE_POSITION));

			Lerp_Once(TimeDelta, vStartPos, vEndPos, &vLerpMousePosition);

			pMouse_Model->Set_Animation(9);
			pMouse_Controller->Set_FootPosition(vLerpMousePosition);
			pMouse_Transform->Set_MyLook(XMVectorSet(1.3f, 0.f, -0.0235003084f, 0.f));

			RELEASE_INSTANCE(CGameInstance);
		}
		else
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CModel* pMouse_Model = dynamic_cast<CModel*>(pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_HaroldMouse"), TEXT("Com_Model")));
			if (nullptr == pMouse_Model)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if (false == m_bModelLerpCheck)
			{
				pMouse_Model->Set_Animation(10);
				pMouse_Model->Set_IsAnimLefp(true);
				m_bModelLerpCheck = true;
			}

			if (true == pMouse_Model->Get_IsAnimFinishied(10)) /*�㰡�����°� Ȯ�����Ŀ� �����ɷγѾ�ڴ�*/
			{
				pMouse_Model->Set_Animation(6);
				pMouse_Model->Set_IsAnimLefp(true);
				m_pTransform->Set_MyLook(XMVectorSet(-0.942f, -0.186f, -0.281f, 0.f));
				m_eHarOldMouse = HAROLDMOUSE_CAM3;
				m_bIsLerp = true;
				m_bModelLerpCheck = false;
				m_LerpSpeed = 0.2f;
				Set_Fovy(30.f);
			}

			RELEASE_INSTANCE(CGameInstance);
		}


		break;
	}
	case Client::CCamera_Fly::HAROLDMOUSE_CAM3:
	{
		if (true == m_bIsLerp)
		{
			_float3 vPosition;
			_float3 vLook;

			Lerp_Once(TimeDelta, XMVectorSet(-16.128f, 1.296f, 39.655f, 1.f), XMVectorSet(-15.781f, 1.296f, 39.406f, 1.f), &vPosition);

			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPosition));
			m_pTransform->Set_MyLook(XMVectorSet(0.559f, 0.258f, 0.788f, 0.f));
		}
		else
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-15.618f, 5.f, 39.891f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.559f, 0.258f, 0.788f, 0.f));
			m_eHarOldMouse = HAROLDMOUSE_CAM4;
			m_bIsLerp = true;
			m_LerpSpeed = 0.2f;
		}
		break;
	}
	case Client::CCamera_Fly::HAROLDMOUSE_CAM4:
	{
		if (true == m_bIsLerp)
		{
			_float3 vPosition;

			Lerp_Once(TimeDelta, XMVectorSet(-13.911f, 2.061f, 42.361f, 1.f), XMVectorSet(-13.257f, 2.061f, 42.165f, 1.f), &vPosition);
			m_pTransform->Set_MyLook(XMVectorSet(-0.648f, -0.036f, -0.761f, 0.f));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPosition));
		}
		else
		{
			m_pTransform->Set_MyLook(XMVectorSet(-0.648f, 0.026f, -0.761f, 0.f));
			m_eHarOldMouse = HAROLDMOUSE_CAM5;
			m_bIsLerp = true;
			m_LerpSpeed = 0.5f;
		}
		break;
	}
	case Client::CCamera_Fly::HAROLDMOUSE_CAM5:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		dynamic_cast<CPlayer_CutScene*>(pGameInstance->Get_GameObject(SCENE_STAGE02, L"Layer_Player_CutScene", 0))->Set_Enable_Stage2(false);
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		pPlayer->Set_IsAnimUpdate(true);

		pPlayer->Set_HpRender(SCENE_STAGE02, true);

		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_UI->Set_AllSkillRenderSet(true);

		CSkill_Info* pSkill_Info = dynamic_cast<CSkill_Info*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_Info"), 1));
		if (nullptr == pSkill_Info)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_Info->Set_IsRender(true);

		m_bIsLerp = true;
		m_bMouseMove = true;

		m_LerpSpeed = 0.5f;

		Set_CameraType(CAMERATYPE_DEPENDENT);
		m_eHarOldMouse = HAROLDMOUSE_END;
		Set_Fovy(70.f);

		RELEASE_INSTANCE(CGameInstance);
	}
	case Client::CCamera_Fly::HAROLDMOUSE_END:
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
		if (nullptr == pPlayer)
			break;

		pPlayer->Set_HpRender(SCENE_STAGE02, true);

		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_UI->Set_AllSkillRenderSet(true);
		RELEASE_INSTANCE(CGameInstance);
		Set_Fovy(70.f);
		Set_CameraType(CAMERATYPE_DEPENDENT);
		break;
	}
	default:
		break;
	}

}

void CCamera_Fly::Update_Parkour(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (true == m_bLookFix)
	{
		m_pTransform->Set_MyLook(XMLoadFloat3(&m_vLookFixDir));

		if (true == m_bIsLimitTime)
		{
			if (m_dLimitTime > m_dLookFixTimeAcc)
				m_dLookFixTimeAcc += TimeDelta;
			else
			{
				m_dLimitTime = 0.0;
				m_dLookFixTimeAcc = 0.0;
				m_bLookFix = false;
				m_bIsLimitTime = false;
			}
		}
	}

	_matrix PlayerWorldMatrix = pTransform->Get_WorldMatrix();
	_matrix CameraWorldMatrix = m_pTransform->Get_WorldMatrix();
	_vector vSourPos, vDestPos;

	_float4 vPos;
	vPos = Get_StandardPos();
	vPos.x = -155.f;

	vDestPos = XMLoadFloat4(&vPos);
	vSourPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

	if (true == m_bIsLerp)
	{
		_float3 vOutPos;
		Lerp_Once(TimeDelta, vSourPos, vDestPos, &vOutPos);

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vOutPos));
	}

	m_bMouseMove = false;
	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Fly::Update_ParkourEnd(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (GetKeyState('W') & 0x8000 || GetKeyState('D') & 0x8000)
	{
		m_dTimeAcc += TimeDelta*0.05;

	}
	if (GetKeyState('S') & 0x8000 || GetKeyState('A') & 0x8000)
	{
		m_dTimeAcc -= TimeDelta*0.05;
	}
	_vector vDir = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.2f, 0.f, 1.f, 0.f), (_float)m_dTimeAcc);
	m_vDependentValue.w = (1 - m_dTimeAcc)*m_vDependentValue.w + 3.f*(_float)m_dTimeAcc;
	if (0.98f <= XMVectorGetX(XMVector3Dot(m_pTransform->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.2f, 0.f, 1.f, 0.f))))
	{
		m_dTimeAcc = 0.0;
		Set_CameraType(CAMERATYPE_DEPENDENT);
		//m_bMouseMove = false;
		XMStoreFloat3(&m_vBackUpDestLook, m_pTransform->Get_State(CTransform::STATE_LOOK));
		m_vDependentValue.w = 3.f;
		m_Ratio = 0.4f;
		m_bParkourEnd = true;

		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	m_pTransform->Set_MyLook(vDir);


	_matrix CameraWorldMatrix = m_pTransform->Get_WorldMatrix();
	_vector vSourPos, vDestPos;

	_float4 vPos;
	vPos = Get_StandardPos();
	CameraWorldMatrix.r[3] = XMLoadFloat4(&vPos);
	vDestPos = XMVector3TransformCoord(Compute_Pos_Dependent(TimeDelta, fMaxDot, fMinDot, fUpDis, m_vDependentValue.w, vInterval, vPos), CameraWorldMatrix);
	vSourPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

	_float3 vMyPos;

	if (true == m_bIsLerp)
	{
		_float3 vOutPos;
		Lerp_Once(TimeDelta, vSourPos, vDestPos, &vOutPos);

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vOutPos));
	}
	m_bMouseMove = false;
	XMStoreFloat3(&m_vBackUpLook, m_pTransform->Get_State(CTransform::STATE_LOOK));
	m_vBackUpDestLook = m_vBackUpLook;
	RELEASE_INSTANCE(CGameInstance);
	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	//if (nullptr == pTransform)
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return;
	//}

	//if (true == m_bLookFix)
	//{
	//	m_pTransform->Set_MyLook(XMLoadFloat3(&m_vLookFixDir));
	//	m_Ratio = 0.1f;

	//	if (true == m_bIsLimitTime)
	//	{
	//		if (m_dLimitTime > m_dLookFixTimeAcc)
	//			m_dLookFixTimeAcc += TimeDelta;
	//		else
	//		{
	//			m_dLimitTime = 0.0;
	//			m_dLookFixTimeAcc = 0.0;
	//			m_bLookFix = false;
	//			m_bIsLimitTime = false;
	//		}
	//	}
	//}
	//if (GetKeyState('W') & 0x8000 || GetKeyState('D') & 0x8000)
	//{
	//	m_Ratio = 0.001f;

	//}
	//if (GetKeyState('S') & 0x8000 || GetKeyState('A') & 0x8000)
	//{
	//	m_Ratio = 0.05f;

	//}
	//m_vDependentValue.w = (1 - m_dTimeAcc)*m_vDependentValue.w + 3.f*m_dTimeAcc;
	//if(0.99f<= XMVectorGetX(XMVector3Dot(m_pTransform->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.f, 0.f, 1.f, 0.f))))
	//{
	//	Set_CameraType(CAMERATYPE_DEPENDENT);
	//	m_bMouseMove = false;
	//	XMStoreFloat3(&m_vBackUpDestLook , m_pTransform->Get_State(CTransform::STATE_LOOK));
	//	m_vDependentValue.w = 3.f;
	//	m_Ratio = 0.4f;
	//	return;
	//}
	//_matrix PlayerWorldMatrix = pTransform->Get_WorldMatrix();
	//_matrix CameraWorldMatrix = m_pTransform->Get_WorldMatrix();
	//_vector vSourPos, vDestPos;
	//_vector vSourLook, vDestLook;
	//_float4 vPos;
	//vPos = Get_StandardPos();
	//CameraWorldMatrix.r[3] = XMLoadFloat4(&vPos);
	//vDestPos = XMVector3TransformCoord(Compute_Pos_Dependent(TimeDelta, fMaxDot, fMinDot, fUpDis, m_vDependentValue.w, vInterval, vPos), CameraWorldMatrix);
	//vSourPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

	//vDestLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	//XMStoreFloat3(&m_vBackUpDestLook, vDestLook);

	//vSourLook = XMLoadFloat3(&m_vBackUpLook);
	//_float3 vMyPos, vMyLook;
	//m_fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPos) - vDestPos));

	//_float3 vDir;

	//if (true == m_bIsLerp)
	//{
	//	Lerp_Once(TimeDelta, vSourPos, vDestPos, vSourLook, vDestLook, &vMyLook, &vMyPos);
	//	XMStoreFloat3(&vDir, XMVector3Normalize(XMLoadFloat3(&vMyPos) - XMLoadFloat4(&vPos)));
	//	XMStoreFloat3(&vMyPos, XMLoadFloat4(&vPos) + XMLoadFloat3(&vDir)*m_fLength);
	//	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vMyPos));
	//	m_pTransform->Set_MyLook(XMLoadFloat3(&vDir)*-1.f);

	//}
	//else
	//	m_pTransform->Set_State(CTransform::STATE_POSITION, vDestPos);

	//XMStoreFloat3(&m_vBackUpLook, m_pTransform->Get_State(CTransform::STATE_LOOK));

	//RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Fly::Update_Stage0_End(_double TimeDelta)
{
	switch (m_eStage0_End)
	{
	case Client::CCamera_Fly::STAGE0_END_LOOK_DOOR:
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-155.72f, 23.797f, 152.788f, 1.f));
		m_pTransform->Set_MyLook(XMVectorSet(-0.071f, -0.039f, 0.997f, 0.f));

		break;
	case Client::CCamera_Fly::STAGE0_END_PUSHBACK:
		m_dStage0_End_TimeAcc += TimeDelta*3.0;
		if (1.f < m_dStage0_End_TimeAcc)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-155.646f, 23.838f, 151.74f, 1.f));
			m_dStage0_End_TimeAcc = 0.0;
			m_bIsLerp = false;
		}
		if (true == m_bIsLerp)
		{
			_vector vPos = XMVectorLerp(XMVectorSet(-155.72f, 23.797f, 152.788f, 1.f), XMVectorSet(-155.646f, 23.838f, 151.74f, 1.f), (_float)m_dStage0_End_TimeAcc);
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
		}
		break;
	case Client::CCamera_Fly::STAGE0_END_PUNCH:
	{
		_vector vPos;
		_vector vLook;
		if (1.f < m_dStage0_End_TimeAcc)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-157.013f, 23.576f, 152.184f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.305f, 0.048f, 0.951f, 0.f));
			m_dStage0_End_TimeAcc = 0.0;
			m_bIsLerp = false;

		}
		if (true == m_bIsLerp)
		{
			m_dStage0_End_TimeAcc += TimeDelta*0.5;
			vPos = XMVectorLerp(m_pTransform->Get_Position(), XMVectorSet(-157.013f, 23.576f, 152.184f, 1.f), (_float)m_dStage0_End_TimeAcc);
			vLook = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.305f, 0.048f, 0.951f, 0.f), (_float)m_dStage0_End_TimeAcc);
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
			m_pTransform->Set_MyLook(vLook);
		}
		else
		{
			vPos = m_pTransform->Get_Position();

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CTransform* pPlayer_CutScene_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_DEMO, TEXT("Layer_Player_CutScene"), TEXT("Com_Transform")));
			if (nullptr == pPlayer_CutScene_Transform)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			vPos += XMVector3Normalize(pPlayer_CutScene_Transform->Get_State(CTransform::STATE_LOOK))*0.005f;
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
			RELEASE_INSTANCE(CGameInstance);
		}

		break;
	}
	case Client::CCamera_Fly::STAGE0_END_PUNCHTWO:
	{
		_vector vPos;
		_vector vLook;
		if (1.f < m_dStage0_End_TimeAcc)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-156.373f, 23.362f, 147.542f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.567f, 0.073f, 0.82f, 0.f));
			m_dStage0_End_TimeAcc = 0.0;
			m_bIsLerp = false;

		}
		if (true == m_bIsLerp)
		{
			m_dStage0_End_TimeAcc += TimeDelta*0.5;
			vPos = XMVectorLerp(m_pTransform->Get_Position(), XMVectorSet(-156.373f, 23.362f, 147.542f, 1.f), (_float)m_dStage0_End_TimeAcc);
			vLook = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.567f, 0.073f, 0.82f, 0.f), (_float)m_dStage0_End_TimeAcc);
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
			m_pTransform->Set_MyLook(vLook);
		}

		vPos = m_pTransform->Get_Position();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CTransform* pPlayer_CutScene_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_DEMO, TEXT("Layer_Player_CutScene"), TEXT("Com_Transform")));
		if (nullptr == pPlayer_CutScene_Transform)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		vPos += XMVector3Normalize(pPlayer_CutScene_Transform->Get_State(CTransform::STATE_LOOK))*0.005f;
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
		RELEASE_INSTANCE(CGameInstance);

		break;
	}
	case Client::CCamera_Fly::STAGE0_END_FINISH:
		break;
	case Client::CCamera_Fly::STAGE0_END_END:
		break;
	default:
		break;
	}
}

void CCamera_Fly::Update_Player_Dead(_double TimeDelta)
{
}

void CCamera_Fly::Update_Cloud_CutScene(_double TimeDelta)
{
	switch (m_CloudScene)
	{
	case Client::CCamera_Fly::CLOUD_START: /* ���� (****** �ָԹ��̶� ��ȭ�ϰ� ����Ʈ ������  ****** ) */
	{
		m_OnesCheck = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		m_bMouseMove = false;
		pPlayer->Set_IsAnimUpdate(false);
		pPlayer->Set_HpRender(SCENE_STAGE01, false);


		RELEASE_INSTANCE(CGameInstance);

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(62.064f, 2.735f, 61.401f, 1.f));
		_vector vLook = XMVectorSet(62.066f, 3.153f, 63.863f, 1.f) - m_pTransform->Get_Position();
		m_pTransform->Set_MyLook(vLook);

		XMStoreFloat3(&m_CameraINFO[CAMAERA_LOOK], XMVectorSet(-0.847f, 0.526f, -0.083f, 0.f));
		XMStoreFloat3(&m_CameraINFO[CAMAERA_POSTION], XMVectorSet(64.684f, 2.535f, 63.746f, 1.f));

		XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK], m_pTransform->Get_State(CTransform::STATE_LOOK));
		XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));

		m_Ratio += TimeDelta;
		if (1.5 <= m_Ratio)
		{
			m_Ratio = 0.0;
			m_CloudScene = CLOUD_1;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_1:
	{
		if (m_OnesCheck)
		{
			Set_Fovy(90.f, 1.f);
			m_OnesCheck = false;
		}
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION]), XMLoadFloat3(&m_CameraINFO[CAMAERA_POSTION]), m_Ratio);
		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_CameraINFO[CAMAERA_LOOK]), m_Ratio);

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
		m_pTransform->Set_MyLook(vLook);

		if (1.0 <= m_Ratio)
		{
			/* ����  �ٷ� �� �ɺ������ �ö󰡼� �����ϰ��� */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CTransform* pNpcTransform = (CTransform*)pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_QuestNpc"), 0)->Get_Components(TEXT("Com_Transform"));
			_float3 vpos;
			XMStoreFloat3(&vpos, pNpcTransform->Get_Position());
			pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE01, TEXT("Layer_Cloud"), &XMVectorSet(vpos.x, vpos.y + 1.5f, vpos.z, 1.f));
			RELEASE_INSTANCE(CGameInstance);
			m_OnesCheck = true;
			m_Ratio = 0.0;
			m_CloudScene = CLOUD_2;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_2:
	{
		/* ó�� ���� FadeOUt �� ���� ������ 1�� ��ü�� �̵��� �������� */
		break;
	}
	case Client::CCamera_Fly::CLOUD_3:
	{
		if (m_OnesCheck)
		{
			m_Ratio = 0.0;
			Set_Fovy(100.f, 1.f);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(59.13f, 10.36f, 58.38f, 1.f));
			_vector CurrentLook = XMVectorSet(61.62f, 8.33f, 59.05f, 0.f); /* 1���� ���� ���� ��ġ */
			_vector SettingLook = CurrentLook - m_pTransform->Get_Position();

			m_pTransform->Set_MyLook(SettingLook); /* ���� ������ �Ĵٺ����ְ� */

												   /* ���������� ���� ��ġ */
			_vector SettingNextLook = XMVectorSet(0.981, -0.192f, -0.007f, 0.f);


			XMStoreFloat3(&m_CameraINFO[CAMAERA_LOOK], XMVector3Normalize(SettingNextLook));
			XMStoreFloat3(&m_CameraINFO[CAMAERA_POSTION], XMVectorSet(59.02f, 13.74f, 55.52f, 1.f));
			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK], m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));
			m_OnesCheck = false;
		}

		m_Ratio += TimeDelta * 0.5;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION]), XMLoadFloat3(&m_CameraINFO[CAMAERA_POSTION]), m_Ratio);
		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_CameraINFO[CAMAERA_LOOK]), m_Ratio);

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
		m_pTransform->Set_MyLook(vLook);


		if (1.0 <= m_Ratio)
		{
			/* ��ǥ�� �ѹ��� ��� �÷��� �����̴�. */
			m_OnesCheck = true;
			m_Ratio = 0.0;
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
			m_pTransform->Set_MyLook(vLook);
			m_CloudScene = CLOUD_4;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_4: /* ��ü������ ��ġ �а� ���°� */
	{
		if (m_OnesCheck)
		{

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			RELEASE_INSTANCE(CGameInstance);

			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(54.403f, 18.179f, 24.021f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.020f, -0.298f, 0.954f, 0.f));
			m_OnesCheck = false;
			m_Ratio = 0.0;
			//Set_Fovy(70.f, 1.f);

			/* ���� ��ġ: �� �������� ���� ���� �ҵ� */
			//XMStoreFloat3(&m_CameraINFO[CAMAERA_LOOK], XMVectorSet(-0.983f, -0.174f, 0.063f, 0.f));
			//XMStoreFloat3(&m_CameraINFO[CAMAERA_POSTION], XMVectorSet(58.392f, 16.591f, 30.406f, 1.f));

			XMStoreFloat3(&m_CameraINFO[CAMAERA_LOOK], XMVectorSet(-0.995f, -0.095f, 0.042f, 0.f));
			XMStoreFloat3(&m_CameraINFO[CAMAERA_POSTION], XMVectorSet(54.935f, 16.132f, 31.588f, 1.f));

			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK], m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_5: // �� �������� ���� ���� �ҵ�
	{
		m_Ratio += TimeDelta*0.3;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION]), XMLoadFloat3(&m_CameraINFO[CAMAERA_POSTION]), m_Ratio);
		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_CameraINFO[CAMAERA_LOOK]), m_Ratio);

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
		m_pTransform->Set_MyLook(vLook);

		/* ���� ���� ���� ���İ��� ���ϰ��̴�. 7�� ������ */
		if (1.0 <= m_Ratio)
		{
			Set_Fovy(40.f, 1.f);
			m_CloudScene = CLOUD_6;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_6: /* ����ִ� ������ */
	{
		m_Ratio = 0.0;
		m_OnesCheck = true;
		break;
	}
	case Client::CCamera_Fly::CLOUD_7: /* ���۰� ������ �ǰ� 4�ʰ� ������ �Ҹ��� �ʴϴ�. */
	{
		Set_Fovy(70.f, 0.5f);

		m_Ratio += TimeDelta*0.5;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/* ���� �̵��� ���ļ� CatmullRom ������ ��Ű�鼭 ���� �ձ��� �̵��ϰ� �Ұ��� */
			//XMStoreFloat3(&m_CameraINFO[CAMAERA_LOOK], XMVectorSet(-1.0f, 0.030f, 0.008f, 0.f));
			//XMStoreFloat3(&m_CameraINFO[CAMAERA_POSTION], XMVectorSet(34.960f, 8.932f, 32.394f, 1.f));
			XMStoreFloat3(&m_CameraINFO[CAMAERA_LOOK], XMVectorSet(-0.734f, -0.679f, -0.007f, 0.f));
			XMStoreFloat3(&m_CameraINFO[CAMAERA_POSTION], XMVectorSet(34.205f, 6.869f, 62.822f, 1.f));
			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK], m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));
			m_Ratio = 0.0;
			m_CloudScene = CLOUD_8;
			m_OnesCheck = true;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_8: /*������ ���� */
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;



		/*_vector vPostion = XMVectorCatmullRom(XMVectorSet(m_CameraCurrentINFO[CAMAERA_POSTION].x + 3.f, m_CameraCurrentINFO[CAMAERA_POSTION].y, m_CameraCurrentINFO[CAMAERA_POSTION].z,1.f)
		,XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION])
		,XMLoadFloat3(&m_CameraINFO[CAMAERA_POSTION])
		,XMVectorSet(m_CameraCurrentINFO[CAMAERA_POSTION].x - 3.f, m_CameraCurrentINFO[CAMAERA_POSTION].y, m_CameraCurrentINFO[CAMAERA_POSTION].z, 1.f)
		,m_Ratio);*/


		_vector vPostion = XMVectorCatmullRom(XMVectorSet(m_CameraCurrentINFO[CAMAERA_POSTION].x + 80.f, m_CameraCurrentINFO[CAMAERA_POSTION].y, m_CameraCurrentINFO[CAMAERA_POSTION].z, 1.f)
			, XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION])
			, XMLoadFloat3(&m_CameraINFO[CAMAERA_POSTION])
			, XMVectorSet(m_CameraCurrentINFO[CAMAERA_POSTION].x - 80.f, m_CameraCurrentINFO[CAMAERA_POSTION].y, m_CameraCurrentINFO[CAMAERA_POSTION].z, 1.f)
			, m_Ratio);


		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_CameraINFO[CAMAERA_LOOK]), m_Ratio);

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
		m_pTransform->Set_MyLook(vLook);

		if (1.0 <= m_Ratio)
		{
			m_Ratio = 0.0;
			m_CloudScene = CLOUD_9;
			m_OnesCheck = true;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_9:
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/* ȭ�� ����  */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_OUT);
			RELEASE_INSTANCE(CGameInstance);
			m_Ratio = 0.0;
			m_CloudScene = CLOUD_10;
			m_OnesCheck = true;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_10: /* ���� �� �ٽ� ȭ���� Ű�ڴ�.  ��������� */
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_IN);
			CPlayer* pPlayer = nullptr;
			pPlayer = (CPlayer*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

			if (nullptr == pPlayer)
				break;

			m_pTransform->Set_MyLook(XMVectorSet(0.0f, 0.f, 0.908f, 0.f));
			m_vBackUpDestLook = m_vBackUpLook = _float3(0.0f, 0.f, 0.908f);
			m_LerpSpeed = 1.0;

			//m_bIsMouseFix = true;
			pPlayer->Set_IsAnimUpdate(true);

			pPlayer->Set_HpRender(SCENE_STAGE01, true);

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_SkillRenderSet(2, true);

			CSkill_Info* pSkill_Info = dynamic_cast<CSkill_Info*>(pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_Info"), 0));
			if (nullptr == pSkill_Info)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_Info->Set_IsRender(true);


			m_bIsLerp = true;
			m_bMouseMove = true;
			RELEASE_INSTANCE(CGameInstance);
			Set_CameraType(CAMERATYPE_DEPENDENT);

			m_OnesCheck = true;


			/* ������ Ÿ�� �ٴ� ���Ѱ� �����ϰڽ��ϴ�. */
			for (_uint i = 0; i < 5; ++i)
			{
				switch (i)
				{
				case 0:
					pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE01, TEXT("Layer_Cloud"), &XMVectorSet(42.06f, 8.10f, 17.72f, 1.f));
					break;
				case 1:
					pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE01, TEXT("Layer_Cloud"), &XMVectorSet(52.47f, 8.10f, 23.44f, 1.f));
					break;
				case 2:
					pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE01, TEXT("Layer_Cloud"), &XMVectorSet(46.28f, 6.80f, 35.08f, 1.f));
					break;
				case 3:
					pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE01, TEXT("Layer_Cloud"), &XMVectorSet(32.02f, 9.97f, 47.45f, 1.f));
					break;
				case 4:
					pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE01, TEXT("Layer_Cloud"), &XMVectorSet(47.24f, 6.94f, 8.43f, 1.f));
					break;
				}

			}

			m_CloudScene = CLOUD_END;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_11: /* ## ���͸� �� ������� �ҷ��ָ� �ɵ�  :: ȭ�� ���� �� ��ǥ �̵�  (****** ���� �� ��� ****** )*/
	{
		if (m_OnesCheck)
		{ /* �� ��Ҵ� ȭ�� ���� */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = nullptr;
			pPlayer = (CPlayer*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
			pPlayer->Set_IsAnimUpdate(true);
			m_OnesCheck = false;
			m_bMouseMove = false;
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
			pPlayer->Set_HpRender(SCENE_STAGE01, false);
			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_AllSkillRenderSet(false);
			RELEASE_INSTANCE(CGameInstance);
		}

		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/* ��ǥ �̵��ض� */
			m_OnesCheck = true;
			m_Ratio = 0.0;
			m_CloudScene = CLOUD_12;
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(46.684f, 16.445f, 31.610f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.959f, -0.281f, -0.030f, 0.f));
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_12: /* ## ���� óġ��  ���۷� ȭ�� �ѱ�  :: 1���� ȭ�鸸 �ѱ� */
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
		{
			/* ȭ�鸸 1���Ŀ� ������ */
			m_Ratio = 0.0;
			//CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			//((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			//RELEASE_INSTANCE(CGameInstance);
			m_OnesCheck = true;
			m_CloudScene = CLOUD_13;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_13: /* ## �������� ���İ� 1�� ������ ���� */
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio && true == m_OnesCheck)
		{
			/* ���İ��� 1�� ������ ����  */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CZip* pZip = static_cast<CZip*>(pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Zip"), 0));
			if (nullptr == pZip)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pZip->End_Blending(true);
			RELEASE_INSTANCE(CGameInstance);
			m_Ratio = 0.0;
			m_OnesCheck = false;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_14: /* ## ���İ��� 1�� �Ǿ����� Zip���� �θ�   */
	{
		if (m_OnesCheck)
		{
			m_Ratio = 0.0;
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
			RELEASE_INSTANCE(CGameInstance);
			m_OnesCheck = false;
		}

		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/* 1�ʰ� ������ �������� �ѱ�� ȭ���� Ű�鼭 �÷��̾� ī�޶� ����  */
			m_Ratio = 0.0;
			//CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			//((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_IN);
			//RELEASE_INSTANCE(CGameInstance);
			m_OnesCheck = true;
			m_CloudScene = CLOUD_15;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_15: /* ## �׳� �Ѱ� �޾Ƽ� 1���� ȭ���� �ٽ� Ų��. ��������� ���� �� ������ ȭ�� �ƾ� �� */
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/* 1�ʰ� ������ ȭ���� Ű�鼭 ��ǥ�� �̵� */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = nullptr;
			pPlayer = (CPlayer*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

			if (nullptr == pPlayer)
				break;

			m_pTransform->Set_MyLook(XMVectorSet(0.0f, 0.f, 0.908f, 0.f));
			m_vBackUpDestLook = m_vBackUpLook = _float3(0.0f, 0.f, 0.908f);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);

			pPlayer->Set_HpRender(SCENE_STAGE01, false);
			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_SkillRenderSet(2, true);
			pSkill_UI->Set_SkillRenderSet(3, true);
			pPlayer->Set_HpRender(SCENE_STAGE01, true);
			RELEASE_INSTANCE(CGameInstance);

			m_OnesCheck = true;

			m_LerpSpeed = 1.0;
			pPlayer->Set_IsAnimUpdate(true);
			m_bIsLerp = true;
			m_bMouseMove = true;
			Set_CameraType(CAMERATYPE_DEPENDENT);
			m_CloudScene = CLOUD_END;
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_16: /* �浹�� ������ zip�� ������  (****** Zip ������ ������  ****** )*/
	{
		if (m_OnesCheck)
		{
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = nullptr;
			pPlayer = (CPlayer*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

			if (nullptr == pPlayer)
				break;
			pPlayer->Set_IsAnimUpdate(true);
			RELEASE_INSTANCE(CGameInstance);

			/* �ѹ��� �����Ͽ� ������ �ƾ��� ��� �����ٰ��� */
			XMStoreFloat3(&m_CameraINFO[CAMAERA_LOOK], XMVectorSet(-1.0f, 0.030f, 0.008f, 0.f));
			XMStoreFloat3(&m_CameraINFO[CAMAERA_POSTION], XMVectorSet(34.960f, 8.932f, 32.394f, 1.f));
			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK], m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));
			m_Ratio = 0.0;
			m_OnesCheck = false;
			m_bMouseMove = false;
		}

		m_Ratio += TimeDelta;

		_vector vPostion = XMVectorCatmullRom(XMVectorSet(m_CameraCurrentINFO[CAMAERA_POSTION].x + 3.f, m_CameraCurrentINFO[CAMAERA_POSTION].y, m_CameraCurrentINFO[CAMAERA_POSTION].z, 1.f)
			, XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_POSTION])
			, XMLoadFloat3(&m_CameraINFO[CAMAERA_POSTION])
			, XMVectorSet(m_CameraCurrentINFO[CAMAERA_POSTION].x - 3.f, m_CameraCurrentINFO[CAMAERA_POSTION].y, m_CameraCurrentINFO[CAMAERA_POSTION].z, 1.f)
			, m_Ratio);

		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_CameraINFO[CAMAERA_LOOK]), m_Ratio);

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
		m_pTransform->Set_MyLook(vLook);

		if (1.0 <= m_Ratio)
		{
			m_CloudScene = CLOUD_17;
			m_OnesCheck = true;
			m_Ratio = 0.0;

			///* �����갡 ó���Ǹ鼭 ȭ���� �����̴�. */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			static_cast<CDoorZipper*>(pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_DoorZipper"), 0))->Setting_Dissolve();
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_17: /* ��� �Ѱ�  */
	{
		break;
	}
	case Client::CCamera_Fly::CLOUD_18: /* ���������� ȭ���� �ٽ� ���ش�. */
	{
		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;


		if (1.0 <= m_Ratio)
		{
			/* ȭ���� ���鼭 �ٽ� �÷��̾����� ���� */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = nullptr;
			pPlayer = (CPlayer*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

			if (nullptr == pPlayer)
				break;
			m_pTransform->Set_MyLook(XMVectorSet(0.0f, 0.f, 0.908f, 0.f));
			m_vBackUpDestLook = m_vBackUpLook = _float3(0.0f, 0.f, 0.908f);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			m_LerpSpeed = 1.0;

			//m_bIsMouseFix = true;
			pPlayer->Set_IsAnimUpdate(true);
			m_bIsLerp = true;
			m_bMouseMove = true;
			RELEASE_INSTANCE(CGameInstance);
			Set_CameraType(CAMERATYPE_DEPENDENT);

			m_OnesCheck = true;
			m_CloudScene = CLOUD_END;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstnace->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_SkillRenderSet(0, true);

			CSkill_Info* pSkill_Info = dynamic_cast<CSkill_Info*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_Info"), 3));
			if (nullptr == pSkill_Info)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_Info->Set_IsRender(true);
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	}
	case Client::CCamera_Fly::CLOUD_END:
	{
		Set_CameraType(CAMERATYPE_DEPENDENT);
		break;
	}
	}
}

void CCamera_Fly::Update_Boss_Throw_Dish(_double TimeDelta)
{
	switch (m_eDishThrow)
	{
	case Client::CCamera_Fly::BOSS_DISH_THROW_CHECKING:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STATIC, TEXT("Layer_Player"));
		if (1 == iLayerSize)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));

			XMStoreFloat4(&m_vBossBackUpPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat4(&m_vBossBackUpLook, m_pTransform->Get_State(CTransform::STATE_LOOK));

			m_LerpSpeed = 0.6f;
			m_bMouseMove = false;
			m_bIsLerp = true;

			//Player
			if (CBoss_Dish::BOSS_FOODTYPE_DISH == pPlayer->Get_DishType())
				m_eDishThrow = BOSS_DISH_THROW_DISH;
			else if (CBoss_Dish::BOSS_FOODTYPE_SOUP == pPlayer->Get_DishType())
				m_eDishThrow = BOSS_DISH_THROW_SOUP;
			else if (CBoss_Dish::BOSS_FOODTYPE_COOKIE == pPlayer->Get_DishType())
				m_eDishThrow = BOSS_DISH_THROW_COOKIE;
		}
		RELEASE_INSTANCE(CGameInstance);

		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THROW_DISH:
	{
		if (true == m_bIsLerp)
		{
			_float3 vLerpLook = {};
			_float3	vLerpPosition = {};
			Lerp_Once(TimeDelta, XMLoadFloat4(&m_vBossBackUpPosition), XMVectorSet(40.971f, 0.635f, 38.855f, 1.f), XMLoadFloat4(&m_vBossBackUpLook), XMVectorSet(0.392f, 0.257f, 0.883f, 0.f), &vLerpLook, &vLerpPosition);

			m_pTransform->Set_MyLook(XMLoadFloat3(&vLerpLook));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vLerpPosition));
		}
		else
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.971f, 0.635f, 38.855f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.392f, 0.257f, 0.883f, 0.f));
			m_LerpSpeed = 0.4f;
		}

		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THROW_SOUP:
	{
		if (true == m_bIsLerp)
		{
			_float3 vLerpLook = {};
			_float3	vLerpPosition = {};
			Lerp_Once(TimeDelta, XMLoadFloat4(&m_vBossBackUpPosition), XMVectorSet(39.083f, 1.005f, 38.119f, 1.f), XMLoadFloat4(&m_vBossBackUpLook), XMVectorSet(-0.183f, 0.219f, 0.958f, 0.f), &vLerpLook, &vLerpPosition);

			m_pTransform->Set_MyLook(XMLoadFloat3(&vLerpLook));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vLerpPosition));
		}
		else
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(39.083f, 1.005f, 38.119f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.183f, 0.219f, 0.958f, 0.f));
			m_LerpSpeed = 0.4f;
		}
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THROW_COOKIE:
	{
		if (true == m_bIsLerp)
		{
			_float3 vLerpLook = {};
			_float3	vLerpPosition = {};
			Lerp_Once(TimeDelta, XMLoadFloat4(&m_vBossBackUpPosition), XMVectorSet(40.893f, 2.692f, 35.964f, 1.f), XMLoadFloat4(&m_vBossBackUpLook), XMVectorSet(-0.074f, 0.078f, 0.994f, 0.f), &vLerpLook, &vLerpPosition);

			m_pTransform->Set_MyLook(XMLoadFloat3(&vLerpLook));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vLerpPosition));
		}
		else
		{
			m_pTransform->Set_MyLook(XMVectorSet(-0.074f, 0.078f, 0.994f, 0.f));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.893f, 2.692f, 35.964f, 1.f));
			m_LerpSpeed = 0.4f;
		}
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THOWCHECKING:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STATIC, TEXT("Layer_Player"));
		if (1 == iLayerSize)
		{
			/////////////////////////////
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));

			XMStoreFloat4(&m_vBossBackUpPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat4(&m_vBossBackUpLook, m_pTransform->Get_State(CTransform::STATE_LOOK));

			pPlayer->Set_KeyCheck(true);

			pPlayer->Set_HpRender(SCENE_STAGE05, false);

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_AllSkillRenderSet(false);

			((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_HpBar"), 0))->Set_IsRender(false);

			//Player
			if (CBoss_Dish::BOSS_FOODTYPE_DISH == pPlayer->Get_DishType())
			{
				m_eBossType = BOSS_TYPE_OTTO;
				m_eDishThrow = BOSS_DISH_THROW_OTTO;
			}
			else if (CBoss_Dish::BOSS_FOODTYPE_SOUP == pPlayer->Get_DishType())
			{
				m_eBossType = BOSS_TYPE_HOLLIS;
				m_eDishThrow = BOSS_DISH_THROW_HOLLIES;
			}
			else if (CBoss_Dish::BOSS_FOODTYPE_COOKIE == pPlayer->Get_DishType())
			{
				m_eBossType = BOSS_TYPE_GOAT;
				m_eDishThrow = BOSS_DISH_THROW_GOAT;
			}
		}
		RELEASE_INSTANCE(CGameInstance);
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THROW_OTTO:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pRightHand = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_RightHand", 0);
		if (nullptr == pRightHand)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		if (false == m_IsFirstCheck)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.484f, 1.137f, 33.951f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.203f, 0.005f, 0.979f, 0.f));
			m_IsFirstCheck = true;
			m_bIsLerp = true;
		}


		if (true == dynamic_cast<CPlayer_RightHand*>(pRightHand)->Get_DishGrabCheck())
		{
			_float4 vRightHandPosition = *(_float4*)dynamic_cast<CPlayer_RightHand*>(pRightHand)->Get_BoneMatrix()->m[3];
			_float4 vCamLook;
			_vector vLerpLook;

			XMStoreFloat4(&vCamLook, XMLoadFloat4(&vRightHandPosition) - m_pTransform->Get_State(CTransform::STATE_POSITION));
			vCamLook.y = 0.f;
			vLerpLook = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMLoadFloat4(&vCamLook), 0.1f);

			m_pTransform->Set_MyLook(vLerpLook);
		}

		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THROW_HOLLIES:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pLeftHand = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_LeftHand", 0);
		if (nullptr == pLeftHand)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);


		if (false == m_IsFirstCheck)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.484f, 1.137f, 33.951f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.203f, 0.005f, 0.979f, 0.f));
			m_IsFirstCheck = true;
			m_bIsLerp = true;
		}

		if (true == dynamic_cast<CPlayer_LeftHand*>(pLeftHand)->Get_DishGrabCheck())
		{
			_float4 vLeftHandPosition = *(_float4*)dynamic_cast<CPlayer_LeftHand*>(pLeftHand)->Get_BoneMatrix()->m[3];

			_float4 vCamLook;
			_vector vLerpLook;

			XMStoreFloat4(&vCamLook, XMLoadFloat4(&vLeftHandPosition) - m_pTransform->Get_State(CTransform::STATE_POSITION));
			vCamLook.y = 0.f;
			vLerpLook = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMLoadFloat4(&vCamLook), 0.1f);

			m_pTransform->Set_MyLook(vLerpLook);
		}
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THROW_GOAT:
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(41.303f, 3.468f, 50.145f, 1.f));
		m_pTransform->Set_MyLook(XMVectorSet(-0.066f, 0.331f, 0.941f, 1.f));
		Set_Fovy(50.f, 1.1f, 70.f);

		if (false == m_bBossSoundChecking)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->StopAll();

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("BossCut.wav"), CHANNELID::PLAYER_DODGE)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			m_bBossSoundChecking = true;
		}

		RELEASE_INSTANCE(CGameInstance);

		//�Ƹ��� ��Ʈ�� �ٷΰ����ҵ�??������ �ξ �ο��ϴ°�
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_THROW_END:
	{	
		m_LerpSpeed = 0.4f;
		m_IsFirstCheck = false;
		m_eDishThrow = BOSS_DISH_THROW_CHECKING;
		Set_CameraType(CAMERTYPE_BOSSDISH_HIT);

		//Set_CameraType(CAMERATYPE_DEPENDENT);
		//���� �´� �ƽ����� �Ѿ������ �� ����� �����帱������ ���� �÷��̾� ������ �Ʒ��γ������°�
		//Ȧ������ �ǵ����� ���Ŀ� �÷��̾� ������ �Ʒ��γ������°�
		//��Ʈ�� ���Ǹ��غ����ҵ� �������� ��������������
		break;
	}
	}

}

void CCamera_Fly::Update_Boss_Dish(_double TimeDelta)
{
	switch (m_eBossDish)
	{
	case Client::CCamera_Fly::BOSS_DISH_POS_START:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		CCharacterController* pPlayer_Controller = dynamic_cast<CCharacterController*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Controller")));
		CGameObject* pDishObject = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Boss_Dish_One", 0);
		CTransform* pDishTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STAGE05, L"Boss_Dish_One", L"Com_Transform", 0);
		if (nullptr == pDishObject || nullptr == pDishTrans)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		if (false == m_IsFirstPositionCheck)
		{
			pPlayer_Controller->Set_FootPosition(_float3(40.f, 0.095f, 25.f));
			pPlayer->Set_KeyCheck(true);
			m_IsFirstPositionCheck = true;
			Set_Fovy(40.f);
		}

		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.633f, 1.545f, 30.308f, 1.f));
		m_pTransform->Set_MyLook(pDishTrans->Get_State(CTransform::STATE_POSITION) - m_pTransform->Get_State(CTransform::STATE_POSITION));

		if (true == dynamic_cast<CBoss_Dish*>(pDishObject)->Get_CamSwitching())
		{
			Set_Fovy(70.f);
			m_IsFirstPositionCheck = false;
			m_eBossDish = BOSS_DISH_POS_CAM1;
		}

		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_POS_CAM1:
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		if (false == Get_IsInterverZCheck())
		{
			pPlayer->Set_KeyCheck(false);
			m_eBossDish = BOSS_DISH_POS_START;
			Set_InterverZ(true, 20.f, 0.5);
			Set_CameraType(CAMERATYPE_DEPENDENT);
		}
		break;
	}
	}
}

void CCamera_Fly::Update_Boss_Hit(_double TimeDelta)
{
	switch (m_eBossHit)
	{
	case Client::CCamera_Fly::BOSS_DISH_HIT_START:
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (BOSS_TYPE::BOSS_TYPE_OTTO == m_eBossType)
			m_eBossHit = BOSS_DISH_HIT_OTTO;
		else if (BOSS_TYPE::BOSS_TYPE_HOLLIS == m_eBossType)
			m_eBossHit = BOSS_DISH_HIT_HOLLIES;
		((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_HpBar"), 0))->Set_IsRender(false);
		RELEASE_INSTANCE(CGameInstance);
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_HIT_OTTO:
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(53.179f, 1.225f, 50.283f, 1.f));

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
		if (nullptr == pBoss)
		{
			Safe_Release(pGameInstance);
			return;
		}
		Safe_Release(pGameInstance);

		CModel* pOttoModel = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Model(CBoss_Goat::BOSS_O);
		//120
		if (nullptr != pOttoModel)
		{
			if (110 <= pOttoModel->Get_AnimationCurrentFrame(16))
			{
				if (false == m_IsFoveOneCheck)
				{
					Set_Fovy(40, 0.5f, 70.f);
					m_IsFoveOneCheck = true;
				}
			}

			//Set_Fovy(40, 0.5f, 70.f);

			if (false == m_IsFoveOneCheck)
			{
				_float4 vOttoMouthPositon = *(_float4*)dynamic_cast<CBoss_Goat*>(pBoss)->Get_MouthMatrix(CBoss_Goat::BOSS_O).m[3];

				_float4 vCamLook;
				_float4 vLerpLook;

				XMStoreFloat4(&vCamLook, XMLoadFloat4(&vOttoMouthPositon) - m_pTransform->Get_State(CTransform::STATE_POSITION));
				XMStoreFloat4(&vLerpLook, XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMLoadFloat4(&vCamLook), 0.01f));

				m_pTransform->Set_MyLook(XMLoadFloat4(&vLerpLook));
			}

			if (120 <= pOttoModel->Get_AnimationCurrentFrame(16))
				m_eBossHit = BOSS_DISH_HIT_OTTOTWO;
		}

		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_HIT_HOLLIES:
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
		if (nullptr == pBoss)
		{
			Safe_Release(pGameInstance);
			return;
		}
		Safe_Release(pGameInstance);

		CModel* pHolliesModel = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Model(CBoss_Goat::BOSS_H);

		if (nullptr != pHolliesModel)
		{

			if (false == m_IsFoveOneCheck)
			{
				m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.916f, 10.033f, 42.969f, 1.f));
				_float4 vHolliesMouthPositon = *(_float4*)dynamic_cast<CBoss_Goat*>(pBoss)->Get_MouthMatrix(CBoss_Goat::BOSS_H).m[3];

				_float4 vCamLook;
				_float4 vLerpLook;

				XMStoreFloat4(&vCamLook, XMLoadFloat4(&vHolliesMouthPositon) - m_pTransform->Get_State(CTransform::STATE_POSITION));
				XMStoreFloat4(&vLerpLook, XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMLoadFloat4(&vCamLook), 0.01f));

				m_pTransform->Set_MyLook(XMLoadFloat4(&vLerpLook));
			}

			if (110 <= pHolliesModel->Get_AnimationCurrentFrame(16))
			{
				if (false == m_IsFoveOneCheck)
				{
					Set_Fovy(45, 0.5f, 70.f);
					m_IsFoveOneCheck = true;
				}
			}
			//Set_Fovy(40, 0.5f, 70.f);


			if (120 <= pHolliesModel->Get_AnimationCurrentFrame(16))
				m_eBossHit = BOSS_DISH_HIT_HOLLIESTWO;

		}
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_HIT_OTTOTWO:
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE05, L"Layer_FinalBoss");
		if (1 <= iLayerSize)
		{
			CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
			if (nullptr == pBoss)
			{
				Safe_Release(pGameInstance);
				return;
			}
			CModel* pBeaterModel = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Model(CBoss_Goat::BOSS_B);

			if (nullptr != pBeaterModel)
			{
				_float4 vBonePosition;

				Bone_PositionGet("Rt_beater1", &vBonePosition, false);

				dynamic_cast<CBoss_Goat*>(pBoss)->Taunt_Boss(CBoss_Goat::BOSS_F,true);
				dynamic_cast<CBoss_Goat*>(pBoss)->Taunt_Boss(CBoss_Goat::BOSS_H, true);
				dynamic_cast<CBoss_Goat*>(pBoss)->Taunt_Boss(CBoss_Goat::BOSS_B,true);

				if (0 >= vBonePosition.w)
				{
					Safe_Release(pGameInstance);
					return;
				}

				if (5 <= pBeaterModel->Get_AnimationCurrentFrame(8) && 24 >= pBeaterModel->Get_AnimationCurrentFrame(8))
				{
					m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(60.125f, 29.953f, 37.028f, 1.f));
					m_pTransform->Set_MyLook(XMVectorSet(-0.138f, -0.528f, 0.838f, 0.f));
				}

				if (25 <= pBeaterModel->Get_AnimationCurrentFrame(8))
				{
					_float4 vCamLook;
					_vector vLerpLook;
					XMStoreFloat4(&vCamLook, XMLoadFloat4(&vBonePosition) - m_pTransform->Get_State(CTransform::STATE_POSITION));

					vLerpLook = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMLoadFloat4(&vCamLook), 0.005f);

					m_pTransform->Set_MyLook(vLerpLook);
					m_lfBossLerpTimeAcc = 0.0;
					m_IsBossLerpCheck = true;
				}

				if (70 <= pBeaterModel->Get_AnimationCurrentFrame(8) && 100 >= pBeaterModel->Get_AnimationCurrentFrame(8))
				{
					if (false == m_IsFovyFirstCheck)
					{
						Set_Fovy(25.f, 0.4f, 70.f);
						m_IsFovyFirstCheck = true;
					}
				}

				if (110 <= pBeaterModel->Get_AnimationCurrentFrame(8))
				{
					if (true == m_IsFovyFirstCheck)
					{
						m_lfBackUpRatio = m_Ratio;
						m_Ratio = 0.0;
						m_LerpSpeed = 0.15;
						m_IsFoveOneCheck = false;
						m_IsBossLerpCheck = false;
						m_IsFovyFirstCheck = false;
						m_bIsLerp = true;
						m_bMouseMove = false;
						m_eBossHit = BOSS_DISH_HIT_END;
					}
				}
			}
			else
			{
				if (false == m_IsBossLerpCheck)
				{
					m_lfBossLerpTimeAcc += TimeDelta * 0.01;

					if (1.0 <= m_lfBossLerpTimeAcc)
						m_lfBossLerpTimeAcc = 1.0;

					_vector vLerpPosition = XMVectorLerp(XMVectorSet(59.354f, 0.668f, 29.388f, 1.f), XMVectorSet(49.354f, 0.668f, 40.388f, 1.f), (_float)m_lfBossLerpTimeAcc);

					m_pTransform->Set_State(CTransform::STATE_POSITION, vLerpPosition);

					_float4 vCamLook;
					_float4 vLerpLook;
					XMStoreFloat4(&vCamLook, XMVectorSet(58.298f, 5.104f, 71.107f, 1.f) - m_pTransform->Get_State(CTransform::STATE_POSITION));
					m_pTransform->Set_MyLook(XMLoadFloat4(&vCamLook));

					if (1.0 <= m_lfBossLerpTimeAcc)
					{
						m_lfBossLerpTimeAcc = 0.0;
						m_IsBossLerpCheck = true;
					}

				}
			}
		}
		Safe_Release(pGameInstance);

		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_HIT_HOLLIESTWO:
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		_uint iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE05, L"Layer_FinalBoss");
		if (1 <= iLayerSize)
		{
			CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
			if (nullptr == pBoss)
			{
				Safe_Release(pGameInstance);
				return;
			}
			CModel* pSpatutlaModel = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Model(CBoss_Goat::BOSS_S);

			if (nullptr != pSpatutlaModel)
			{
				_float4 vBonePosition;

				if (false == m_bBoneFindCheck)
				{
					m_TargetOffsetMatrix = {};
					m_TargetPivotMatrix = {};
					m_pTargetCombinedMatrix = nullptr;
					m_pTargetWorldMatrix = nullptr;

					//CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
					if (nullptr == pBoss)
					{
						Safe_Release(pGameInstance);
						return;
					}
					CTransform* pBossTransform = nullptr;
					pBossTransform = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Transform(CBoss_Goat::BOSS_S);

					m_TargetOffsetMatrix = pSpatutlaModel->Get_OffsetMatrix("Rt_pointer1");
					m_TargetPivotMatrix = pSpatutlaModel->Get_PivotMatrix();
					m_pTargetCombinedMatrix = pSpatutlaModel->Get_CombinedMatrixPtr("Rt_pointer1");
					m_pTargetWorldMatrix = pBossTransform->Get_WorldMatrixPtr();

					dynamic_cast<CBoss_Goat*>(pBoss)->Taunt_Boss(CBoss_Goat::BOSS_F, true);
					dynamic_cast<CBoss_Goat*>(pBoss)->Taunt_Boss(CBoss_Goat::BOSS_B, true);
					dynamic_cast<CBoss_Goat*>(pBoss)->Taunt_Boss(CBoss_Goat::BOSS_S, true);

					m_bBoneFindCheck = true;
				}

				if (nullptr != m_pTargetWorldMatrix && nullptr != m_pTargetCombinedMatrix)
				{
					_float4x4 vBoneMat;
					_matrix BoneMatrix = XMLoadFloat4x4(&m_TargetOffsetMatrix) * XMLoadFloat4x4(m_pTargetCombinedMatrix) * XMLoadFloat4x4(&m_TargetPivotMatrix) * XMLoadFloat4x4(m_pTargetWorldMatrix);

					XMStoreFloat4x4(&vBoneMat, BoneMatrix);

					vBonePosition = *(_float4*)vBoneMat.m[3];
				}

				if (0 >= vBonePosition.w)
				{
					Safe_Release(pGameInstance);
					return;
				}

				_float4 vCamLook;
				_vector vLerpLook;
				m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(29.641f, 8.458f, 50.856f, 1.f));
				vBonePosition.y += 9.f;
				XMStoreFloat4(&vCamLook, XMLoadFloat4(&vBonePosition) - m_pTransform->Get_State(CTransform::STATE_POSITION));

				vLerpLook = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_LOOK), XMLoadFloat4(&vCamLook), 0.04f);

				m_pTransform->Set_MyLook(vLerpLook);
				m_lfBossLerpTimeAcc = 0.0;
				m_IsBossLerpCheck = true;


				if (10 <= pSpatutlaModel->Get_AnimationCurrentFrame(2) && 15 >= pSpatutlaModel->Get_AnimationCurrentFrame(2))
				{
					if (false == m_IsFovyFirstCheck)
					{
						Set_Fovy(40.f, 0.7f, 70.f);
						m_IsFovyFirstCheck = true;
					}
				}

				if (30 <= pSpatutlaModel->Get_AnimationCurrentFrame(2))
				{
					if (true == m_IsFovyFirstCheck)
					{
						m_lfBackUpRatio = m_Ratio;
						m_Ratio = 0.0;
						m_LerpSpeed = 0.15;

						m_IsFoveOneCheck = false;
						m_bBoneFindCheck = false;
						m_IsBossLerpCheck = false;
						m_IsFovyFirstCheck = false;

						m_bIsLerp = true;
						m_bMouseMove = false;

						m_eBossHit = BOSS_DISH_HIT_END;
					}
				}
			}
			else
			{
				if (false == m_IsBossLerpCheck)
				{
					m_lfBossLerpTimeAcc += (TimeDelta * 0.5);

					if (1.0 <= m_lfBossLerpTimeAcc)
						m_lfBossLerpTimeAcc = 1.0;

					_vector vLerpPosition = XMVectorLerp(XMVectorSet(30.916f, 10.033f, 42.969f, 1.f), XMVectorSet(33.916f, 10.033f, 40.969f, 1.f), (_float)m_lfBossLerpTimeAcc);

					m_pTransform->Set_State(CTransform::STATE_POSITION, vLerpPosition);

					_float4 vCamLook;
					_float4 vLerpLook;

					_float4 vHolliesMouthPositon = *(_float4*)dynamic_cast<CBoss_Goat*>(pBoss)->Get_MouthMatrix(CBoss_Goat::BOSS_H).m[3];

					XMStoreFloat4(&vCamLook, XMLoadFloat4(&vHolliesMouthPositon) - m_pTransform->Get_State(CTransform::STATE_POSITION));
					m_pTransform->Set_MyLook(XMLoadFloat4(&vCamLook));

					if (1.0 <= m_lfBossLerpTimeAcc)
					{
						m_lfBossLerpTimeAcc = 0.0;
						m_IsBossLerpCheck = true;
					}

				}
			}
		}
		Safe_Release(pGameInstance);

		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_HIT_END:
	{
		if (true == m_bIsLerp)
		{
			Set_Fovy(30.f);
			_float3 vPosition;
			_float3	vLook;

			Lerp_Once(TimeDelta, XMVectorSet(38.277f, 0.833f, 26.844f, 1.f), XMVectorSet(39.986f, 1.033f, 15.698f, 1.f), XMVectorSet(0.651f, 0.089f, -0.754f, 0.f), XMVectorSet(-0.012f, 0.116f, 0.993f, 0.f), &vLook, &vPosition);

			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPosition));
			m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
		}
		else
		{
			m_lfBossEndTimeAcc += TimeDelta;

			if (1.0 <= m_lfBossEndTimeAcc)
			{
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);

				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
				CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
				if (nullptr == pBoss)
				{
					Safe_Release(pGameInstance);
					return;
				}

				pPlayer->Set_HpRender(SCENE_STAGE05, true);

				CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_Skill_UI"), 0));
				if (nullptr == pSkill_UI)
				{
					Safe_Release(pGameInstance);
					return;
				}
				((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_HpBar"), 0))->Set_IsRender(true);
				pSkill_UI->Set_AllSkillRenderSet(true);
				Safe_Release(pGameInstance);


				dynamic_cast<CBoss_Goat*>(pBoss)->Taunt_Bosses(false);
				pPlayer->Set_KeyCheck(false);

				Set_Fovy(70.f);
				//m_Ratio = m_lfBackUpRatio;
				m_bIsLerp = true;
				m_lfBossEndTimeAcc = 0.0;
				m_eBossHit = BOSS_DISH_HIT_START;
				Set_CameraType(CCamera_Fly::CAMERATYPE::CAMERATYPE_DEPENDENT);
			}
		}
		break;
	}
	case Client::CCamera_Fly::BOSS_DISH_HIT_REALEND:
		Set_CameraType(CCamera_Fly::CAMERATYPE::CAMERATYPE_DEPENDENT);
	}

}

void CCamera_Fly::Update_Boss_Intro(_double TimeDelta)
{
	switch (m_eBossIntro)
	{
	case Client::CCamera_Fly::BOSS_INTRO_START:
	{

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->StopAll()))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(L"BossSpawnSound.wav", BGM)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}


		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));

		((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(2.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);

		pPlayer->Set_HpRender(SCENE_STAGE05, false);

		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			Safe_Release(pGameInstance);
			return;
		}
		pSkill_UI->Set_AllSkillRenderSet(false);
		((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_HpBar"), 0))->Set_IsRender(false);

		Safe_Release(pGameInstance);

		pPlayer->Set_KeyCheck(true);

		m_pTransform->Set_MyLook(XMVectorSet(-0.021f, 0.082f, 0.996f, 0.f));

		m_Ratio = 0.0;

		Set_Fovy(40.f);

		m_bMouseMove = false;

		m_eBossIntro = BOSS_INTRO_LOOP;

		break;
	}
	case Client::CCamera_Fly::BOSS_INTRO_LOOP:
	{
		if (false == m_bIntroCheck[0])
		{
			_vector vLerpPosition;

			m_Ratio += TimeDelta * 0.2;

			if (1.0 <= m_Ratio)
				m_Ratio = 1.0;

			vLerpPosition = XMVectorLerp(XMVectorSet(40.619f, 0.723f, 21.579f, 1.f), XMVectorSet(40.619f, 4.0f, 21.579f, 1.f), m_Ratio);

			m_pTransform->Set_State(CTransform::STATE_POSITION, vLerpPosition);

			if (1.0 <= m_Ratio)
			{
				m_bIntroCheck[0] = true;
				m_bIntroCheck[1] = true;
				m_Ratio = 0.0;
			}
		}
		if (true == m_bIntroCheck[1])
		{
			m_Ratio += TimeDelta;

			if (3.0 <= m_Ratio)
			{
				m_Ratio = 0.0;
				m_bIntroCheck[1] = false;
				m_eBossIntro = BOSS_INTRO_END;
			}
		}
		break;
	}
	case Client::CCamera_Fly::BOSS_INTRO_END:
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		((CBossHpBar*)pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_HpBar"), 0))->Set_IsRender(true);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
		((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.7, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
		
		pPlayer->Set_HpRender(SCENE_STAGE05, true);

		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE05, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			Safe_Release(pGameInstance);
			return;
		}
		pSkill_UI->Set_AllSkillRenderSet(true);
		
		Safe_Release(pGameInstance);

		pPlayer->Set_KeyCheck(false);

		Set_Fovy(70.f);
		
		dynamic_cast<CBoss_Goat*>(pBoss)->Set_Ready(CBoss_Goat::BOSS::BOSS_O);
		dynamic_cast<CBoss_Goat*>(pBoss)->Set_Ready(CBoss_Goat::BOSS::BOSS_H);
		dynamic_cast<CBoss_Goat*>(pBoss)->Set_Ready(CBoss_Goat::BOSS::BOSS_F);

		if (FAILED(pGameInstance->StopAll()))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlayBGM(L"Stage5BackGroundSound.wav")))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		Set_CameraType(CAMERATYPE_DEPENDENT);

		break;
	}
	}
}

void CCamera_Fly::Update_Stage2_2D(_double TimeDelta)
{
	switch (m_eMap_2D)
	{
	case Client::CCamera_Fly::MAP_2D_START:
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pSkill_UI->Set_AllSkillRenderSet(false);

		CPlayer_2D* pPlayer_2D = dynamic_cast<CPlayer_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Player_2D"), 0));
		if (nullptr == pPlayer_2D)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (_uint i = 0; i < 2; i++)
		{
			CMonster_2D* pMonster_2D = dynamic_cast<CMonster_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Monster_2D"), i));
			if (nullptr == pMonster_2D)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pMonster_2D->Set_TriggerRender(true);
		}

		for (_uint i = 0; i < 3; i++)
		{
			CKey_2D* pKey_2D = dynamic_cast<CKey_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Key_2D"), i));
			if (nullptr == pKey_2D)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pKey_2D->Set_TriggerRender(true);
		}

		pPlayer_2D->Set_TriggerRender(true);

		CPlane_2D* pPlane_2D = dynamic_cast<CPlane_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Plane_2D"), 0));
		if (nullptr == pPlane_2D)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		pPlane_2D->Set_TriggerRender(true);

		CLock_2D* pLock_2D = dynamic_cast<CLock_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Lock_2D"), 0));
		if (nullptr == pLock_2D)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pLock_2D->Set_TriggerRender(true);

		((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
		static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HpRender(SCENE_STAGE02, false);

		RELEASE_INSTANCE(CGameInstance);
		m_bMouseMove = false;
		m_eMap_2D = MAP_2D_1;
		break;
	}
	case Client::CCamera_Fly::MAP_2D_1:
	{
		m_Ratio += TimeDelta * 2.0;

		if (1.0 <= m_Ratio)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_IN);
			RELEASE_INSTANCE(CGameInstance);
			m_Ratio = 0.0;
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.485f, 10.297f, 44.619f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.03f, -1.f, 0.f, 0.f));

			m_bIsLerp = true;
			m_LerpSpeed = 0.75f;
			m_eMap_2D = MAP_2D_2;
		}
		break;
	}
	case Client::CCamera_Fly::MAP_2D_2:
	{
		if (true == m_bIsLerp)
		{
			_float3 vPos;
			Lerp_Once(TimeDelta, XMVectorSet(2.485f, 10.297f, 44.619f, 1.f), XMVectorSet(2.485f, 3.297f, 44.619f, 1.f), &vPos);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
		}
		else
		{
			m_eMap_2D = MAP_2D_3;
		}
		break;
	}
	case Client::CCamera_Fly::MAP_2D_3:
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.485f, 3.297f, 44.619f, 1.f));
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CKey_2D* pKey = dynamic_cast<CKey_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Key_2D"), 2));
		if (nullptr == pKey)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		CLock_Object* pLock_Obj = dynamic_cast<CLock_Object*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Lock_Object"), 1));
		if (nullptr == pLock_Obj)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pLock_Obj->Set_EmissiveEnd(true);

		if (true == pKey->Get_Clear())
		{
			m_eMap_2D = MAP_2D_4;
			m_bIsLerp = true;
		}

		RELEASE_INSTANCE(CGameInstance);
		break;
	}
	case Client::CCamera_Fly::MAP_2D_4:
	{
		if (true == m_bIsLerp)
		{
			_float3 vPos;
			Lerp_Once(TimeDelta, XMVectorSet(2.485f, 3.297f, 44.619f, 1.f), XMVectorSet(2.485f, 10.297f, 44.619f, 1.f), &vPos);
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
		}
		else
			m_eMap_2D = MAP_2D_5;
		break;
	}
	case Client::CCamera_Fly::MAP_2D_5:
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
		RELEASE_INSTANCE(CGameInstance);
		m_eMap_2D = MAP_2D_6;
		break;
	}
	case Client::CCamera_Fly::MAP_2D_6:
	{
		m_Ratio += TimeDelta * 2.0;

		if (1.0 <= m_Ratio)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_IN);
			RELEASE_INSTANCE(CGameInstance);
			m_Ratio = 0.0;
			m_eMap_2D = MAP_2D_7;
		}
		break;
	}
	case Client::CCamera_Fly::MAP_2D_7:
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CPlayer_2D* pPlayer_2D = dynamic_cast<CPlayer_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Player_2D"), 0));
		if (nullptr == pPlayer_2D)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		for (_uint i = 0; i < 2; i++)
		{
			CMonster_2D* pMonster_2D = dynamic_cast<CMonster_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Monster_2D"), i));
			if (nullptr == pMonster_2D)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pMonster_2D->Set_TriggerRender(false);
		}

		for (_uint i = 0; i < 3; i++)
		{
			CKey_2D* pKey_2D = dynamic_cast<CKey_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Key_2D"), i));
			if (nullptr == pKey_2D)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pKey_2D->Set_TriggerRender(false);
		}

		pPlayer_2D->Set_TriggerRender(false);

		CLock_2D* pLock_2D = dynamic_cast<CLock_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Lock_2D"), 0));
		if (nullptr == pLock_2D)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pLock_2D->Set_TriggerRender(false);

		CPlane_2D* pPlane_2D = dynamic_cast<CPlane_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Plane_2D"), 0));
		if (nullptr == pPlane_2D)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		pPlane_2D->Set_TriggerRender(false);

		CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Skill_UI"), 0));
		if (nullptr == pSkill_UI)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		for (_uint i = 0; i < 5; i++)
			pSkill_UI->Set_SkillRenderSet(i, true);

		for (_uint i = 0; i < 2; i++)
		{
			CLock_Object* pLock_Obj = dynamic_cast<CLock_Object*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Lock_Object"), i));
			if (nullptr == pLock_Obj)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pLock_Obj->Set_DissolveStart(true);
		}

		static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsAnimUpdate(true);
		static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HpRender(SCENE_STAGE02, true);

		Set_CameraType(CAMERATYPE_DEPENDENT);
		RELEASE_INSTANCE(CGameInstance);

		break;
	}
	}
}

void CCamera_Fly::Lerp_Values(_double TimeDelta)
{
	Lerp_Fovy(TimeDelta);
	Lerp_DependentValue(TimeDelta);
	//����
	Lerp_InterverZ(TimeDelta);
}

void CCamera_Fly::Update_Stage3_CUT_SCENE(_double TimeDelta)
{
	switch (m_Stage3Cut_Scene)
	{
	case Client::CCamera_Fly::STAGE3_CUT_START: /* ȭ�� Fade Out ���� ������ */
	{
		if (m_Stage3_OnesCheck)
		{
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
			RELEASE_INSTANCE(CGameInstance);

			m_Ratio = 0.0;
			m_Stage3_CameraCurrentINFO[CAMAERA_LOOK] = _float3(-0.047f, -0.654f, 0.755f);
			m_Stage3_CameraCurrentINFO[CAMAERA_POSTION] = _float3(235.560f, 18.170f, 162.786f);

			m_Stage3_CameraINFO[CAMAERA_LOOK] = _float3(-0.079, -0.219f, 0.972f);
			m_Stage3_CameraINFO[CAMAERA_POSTION] = _float3(236.445f, 7.043f, 179.107f);
			m_Stage3_OnesCheck = false;
			m_bMouseMove = false;
		}

		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/*  ���� ȭ���� ų���Դϴ�. */
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
			pPlayer->Set_HpRender(SCENE_STAGE03, false);

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_AllSkillRenderSet(false);
			RELEASE_INSTANCE(CGameInstance);

			Set_Stage3UI_Render_Check(false);
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_1;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_1: /* �ɸ��� ���� ������ ���� ���ٰ� + ȭ���� ���ֱ� �Ұ� */
	{
		if (m_Stage3_OnesCheck)
		{
			/* ���� �������� ����  */
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION]));
			//m_pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK]));
			m_pTransform->Set_MyLook(XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK]));
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/*  ���� ȭ���� ų���Դϴ�. */
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_2;
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_2: /* ���ø� �ϰ��� 3���� �Ѱܹ������� */
	{
		if (m_Stage3_OnesCheck)
		{
			/*���� 3���� �̵��� ��ǥ */
			m_Ratio = 0.0;
			XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK], m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));
			m_Stage3_CameraINFO[CAMAERA_LOOK] = _float3(-0.079f, -0.219f, 0.972f);
			m_Stage3_CameraINFO[CAMAERA_POSTION] = _float3(236.445f, 7.043f, 179.107f);
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/*  ���� �ĸַ����� ��ǥ�� �� �̵��� �Ұ� . */
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			pGameInstnace->StopSound(EATCUTSCENE);
			pGameInstnace->PlaySoundOnce(TEXT("EatCutScene.wav"), EATCUTSCENE);
			RELEASE_INSTANCE(CGameInstance);
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_3;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_3: /* �ĸַ����� �̵� �Ұ��� */
	{
		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_Stage3_CameraINFO[CAMAERA_LOOK]), m_Ratio);

		_vector vPostion = XMVectorCatmullRom(XMVectorSet(m_Stage3_CameraCurrentINFO[CAMAERA_POSTION].x, m_Stage3_CameraCurrentINFO[CAMAERA_POSTION].y - 15.f, m_Stage3_CameraCurrentINFO[CAMAERA_POSTION].z - 7.f, 1.f)
			, XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION])
			, XMLoadFloat3(&m_Stage3_CameraINFO[CAMAERA_POSTION])
			, XMVectorSet(m_Stage3_CameraCurrentINFO[CAMAERA_POSTION].x, m_Stage3_CameraCurrentINFO[CAMAERA_POSTION].y + 15.f, m_Stage3_CameraCurrentINFO[CAMAERA_POSTION].z + 7.f, 1.f)
			, m_Ratio);

		m_pTransform->Set_MyLook(vLook);
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
		if (1.0 <= m_Ratio)
		{
			/*  ���� : �Ѳ��� ����߸���.*/
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_4;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_4: /* �̵��� ������ �Ѳ��� ��������.*/
	{
		if (m_Stage3_OnesCheck)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CGiantCloche*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_GiantCloche"), 0))->Set_Giant_Move(true);
			RELEASE_INSTANCE(CGameInstance);
		}

		m_Stage3Cut_Scene = STAGE3_CUT_5;
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_5: /* �Ѳ� ������������ ���.*/
	{
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_6: /* �Ѳ��� �������� �θ���.*/
	{
		if (m_Stage3_OnesCheck)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			/* ��� ���ϵ� �켱 �ϴ� ���δ�.*/
			for (_uint i = 0; i < 16; ++i)
			{
				static_cast<CFruitSpawner*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_FruitSpawner"), i))->Stting_Create_Cook_And_Mesh_Setting();
			}
			RELEASE_INSTANCE(CGameInstance);

			Set_Camera_Shaking(true, 1.0, true);
			m_Stage3_OnesCheck = false;
		}
		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/* ȭ���� ���ڴ�.*/
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);

			/* �÷��̾� ���� �� ������ ���� �Ž� �ٸ��ɷ� ������ */
			RELEASE_INSTANCE(CGameInstance);
			m_Stage3Cut_Scene = STAGE3_CUT_7;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_7: /* ȭ���� Ű�鼭 .*/
	{
		if (m_Stage3_OnesCheck)
		{
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		if (1.0 <= m_Ratio)
		{
			/* ȭ���� Ű�鼭 �÷��̾�� ī�޶� ��ǥ ������ �Ұ� . */
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			/* ī�޶� ���� */
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(233.070f, 2.448f, 178.406f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.016f, 0.350f, 0.937f, 0.f));

			/* ���� ģ���� ���� */
			_vector vPostion = XMVectorSet(237.174f, /*1.744f*/2.232f, 192.740f, 1.f);
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingEnd"), SCENE_STAGE03, TEXT("Layer_CookingEnd"), &vPostion)))
			{
				RELEASE_INSTANCE(CGameInstance);
			}
			/* �÷��̾� ���� */
			static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_KeyCheck(true);
			CCharacterController* pPlayer_Controller = dynamic_cast<CCharacterController*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Controller")));
			CTransform* pPlayer_Transform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
			pPlayer_Controller->Set_FootPosition(_float3(236.160f, 1.744f, 180.161f));
			pPlayer_Transform->Set_MyLook(XMVectorSet(0.070f, 0.0f, 0.998f, 0.f));

			/* ȭ�� �ٽ� ���ֱ� */
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			
			RELEASE_INSTANCE(CGameInstance);
			m_Stage3Cut_Scene = STAGE3_CUT_8;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_8: /* ��ǥ�� ������ ���� ī�޶� �����̸鼭.*/
	{
		if (m_Stage3_OnesCheck)
		{
			m_Ratio = 0.0;
			XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));
			m_Stage3_CameraINFO[CAMAERA_POSTION] = _float3(236.825f, 2.448f, 178.470f);
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta * 0.25;

		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION]), XMLoadFloat3(&m_Stage3_CameraINFO[CAMAERA_POSTION]), m_Ratio);
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);
		if (1.0 <= m_Ratio)
		{
			/* ���� �Ѿ�� �Ѳ��� ������ ó���Ұ��� */
			m_Stage3_OnesCheck = true;
			m_Ratio = 0.0;
			m_Stage3Cut_Scene = STAGE3_CUT_9;

			/* ���� ó���Ұ� ���ϵ� �����ϰ� �丮 �Ȱ����� ����ǰ��ؾߵɵ� */
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_9: /* �Ѳ� ���� �Ѵ�.*/
	{
		if (m_Stage3_OnesCheck)
		{
			/* �̵� ��ǥ ���ð� + ī�޶� ����ŷ + �Ѳ� ������ */
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = false;
			XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK], m_pTransform->Get_State(CTransform::STATE_LOOK));
			XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION], m_pTransform->Get_State(CTransform::STATE_POSITION));
			m_Stage3_CameraINFO[CAMAERA_LOOK] = _float3(-0.309f, -0.320f, 0.896f);
			m_Stage3_CameraINFO[CAMAERA_POSTION] = _float3(240.465f, 5.575f, 182.302f);
			//Set_Camera_Shaking(true, 1.0, true);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CGiantCloche*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_GiantCloche"), 0))->Set_Giant_MoveUp(true);
			RELEASE_INSTANCE(CGameInstance);
		}

		m_Ratio += TimeDelta * 0.5;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_Stage3_CameraINFO[CAMAERA_LOOK]), m_Ratio);
		_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION]), XMLoadFloat3(&m_Stage3_CameraINFO[CAMAERA_POSTION]), m_Ratio);


		m_pTransform->Set_MyLook(vLook);
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);

		if (1.0 <= m_Ratio)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->StopSound(COOKEND);
			pGameInstance->PlaySoundOnce(TEXT("CookEnd.wav"), COOKEND);
			RELEASE_INSTANCE(CGameInstance);

			m_Stage3_OnesCheck = true;
			m_Ratio = 0.0;
			m_Stage3Cut_Scene = STAGE3_CUT_10;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_10: /* �Ѱϴ� ���������� ���.*/
	{
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_11: /* �Ѱϴ� ������ �Ҹ����̴�.*/
	{
		if (m_Stage3_OnesCheck)
		{
			/* ���� ������ ������ �����ٰ� �ٽ� �þ�� ó���Ұ� */
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = false;
			Set_Fovy(40.f, 1.f);
		}

		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
		{
			/* �ð� ������ ���� ���� ����*/
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_12;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CStage3BossDish*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Stage3BossDish"), 0))->Set_Render_BossDish();
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_12: /* ���� ���� �ϰ� ��ǥ �� ����.*/
	{
		if (m_Stage3_OnesCheck)
		{
			/* ���� ���� */
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
		{
			Set_Fovy(70.f, 1.f);
			m_Ratio = 0.0;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_uint SceneCookMesh = ((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Stage03PhaseNum();

			/* �ش� ���� ���� */
			switch (SceneCookMesh)
			{
			case 0:
				/* Start*/
				XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK], XMVectorSet(0.874f, -0.271f, -0.404f, 0.f));
				XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION], XMVectorSet(255.224f, 4.129f, 186.057f, 1.f));
				/* End */
				m_Stage3_CameraINFO[CAMAERA_LOOK] = _float3(0.909f, -0.344f, 0.236f);
				m_Stage3_CameraINFO[CAMAERA_POSTION] = _float3(255.224f, 4.251f, 179.838f);
				break;
			case 1:
				/* Start*/
				XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK], XMVectorSet(-0.029f, -0.253f, -0.967f, 0.f));
				XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION], XMVectorSet(248.269f, 4.396f, 169.378f, 1.f));
				/* End */
				m_Stage3_CameraINFO[CAMAERA_LOOK] = _float3(0.764f, -0.313f, -0.565f);
				m_Stage3_CameraINFO[CAMAERA_POSTION] = _float3(242.058f, 4.574f, 165.411f);
				break;
			case 2:
				/* Start*/
				XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK], XMVectorSet(-0.728f, -0.239f, -0.643f, 0.f));
				XMStoreFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION], XMVectorSet(229.749f, 4.311f, 165.357f, 1.f));
				/* End */
				m_Stage3_CameraINFO[CAMAERA_LOOK] = _float3(-0.098f, -0.277f, -0.956f);
				m_Stage3_CameraINFO[CAMAERA_POSTION] = _float3(224.309f, 4.605f, 168.035f);
				break;
			}

			/* ���� ģ���� ���� �ٲܰ� */
			for (_uint i = 0; i < 15; ++i)
				static_cast<CFruitSpawner*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_FruitSpawner"), i))->Cooking_EndCut_SceneAnimation();

			RELEASE_INSTANCE(CGameInstance);
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_13;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_13: /* ���� ���� �ϰ� �ƹ��͵� ���Ұ���.*/
	{
		if (m_Stage3_OnesCheck)
		{
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta;
		if (2.5 <= m_Ratio)
		{
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_14;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_14: /*  ���ϵ� �����ٰ��̴� .*/
	{
		m_Ratio += TimeDelta * 0.25;
		if (1.0 <= m_Ratio)
			m_Ratio = 1.0;

		_vector vLook = XMVectorLerp(XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_LOOK]), XMLoadFloat3(&m_Stage3_CameraINFO[CAMAERA_LOOK]), m_Ratio);
		_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_Stage3_CameraCurrentINFO[CAMAERA_POSTION]), XMLoadFloat3(&m_Stage3_CameraINFO[CAMAERA_POSTION]), m_Ratio);


		m_pTransform->Set_MyLook(vLook);
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPostion);

		if (1.0 <= m_Ratio)
		{
			/* ���� ���� �丮 �̵� */
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CCookingEnd*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_CookingEnd"), 0))->Setting_Poation();
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
			RELEASE_INSTANCE(CGameInstance);
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_15;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_15:
	{
		m_Ratio += TimeDelta;

		if (1.0 <= m_Ratio)
		{
			/* ��ǥ�� ���� */
			m_pTransform->Set_MyLook(XMVectorSet(0.422f, 0.385f, 0.821f, 0.f));
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(231.364f, 1.537f, 200.099f, 1.f));
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_16;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_16:
	{
		if (m_Stage3_OnesCheck)
		{
			/* ȭ���� ������ */
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			RELEASE_INSTANCE(CGameInstance);
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta;
		if (1.0 <= m_Ratio)
		{
			m_Ratio = 0.0;
			m_Stage3_OnesCheck = true;
			m_Stage3Cut_Scene = STAGE3_CUT_17;
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_17:
	{
		/* ������ �Ծ�� */
		if (m_Stage3_OnesCheck)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			((CNonBT_Otto*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Otto"), 0))->Set_EatMotion();
			((CNonBT_Ford*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Ford"), 0))->Set_EatMotion();
			((CNonBT_Hollis*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Hollis"), 0))->Set_EatMotion();

			RELEASE_INSTANCE(CGameInstance);
			m_Stage3_OnesCheck = false;
		}

		if (false == m_Stage3_OnesCheck)
		{

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_bool bEnd = ((CNonBT_Otto*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Otto"), 0))->IsIdle();
			RELEASE_INSTANCE(CGameInstance);

			if (true == bEnd)
			{
				m_Ratio += TimeDelta;
				if (2.0 <= m_Ratio)
				{
					m_Ratio = 0.0;
					m_Stage3_OnesCheck = true;
					m_Stage3Cut_Scene = STAGE3_CUT_18;
				}
			}
		}
		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_18: /* ������ �� �԰� ���� �θ�����  Fade Out�� õõ�� �ϸ鼭 ������.*/
	{
		if (m_Stage3_OnesCheck)
		{
			m_Ratio = 0.0f;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
			RELEASE_INSTANCE(CGameInstance);
			m_Stage3_OnesCheck = false;
		}

		m_Ratio += TimeDelta * 0.5;

		if (1.0 <= m_Ratio)
		{
			m_Stage3Cut_Scene = STAGE3_CUT_END;
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = nullptr;
			pPlayer = (CPlayer*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);

			if (nullptr == pPlayer)
				break;
			m_pTransform->Set_MyLook(XMVectorSet(0.0f, 0.f, 0.908f, 0.f));
			m_vBackUpDestLook = m_vBackUpLook = _float3(0.0f, 0.f, 0.908f);
			m_LerpSpeed = 1.0;

			pPlayer->Set_IsAnimUpdate(true);
			pPlayer->Set_KeyCheck(false);

			m_bIsLerp = true;
			m_bMouseMove = true;

			Set_CameraType(CAMERATYPE_DEPENDENT);
			m_Stage3_OnesCheck = true;

			/* ���� */
			pPlayer->Set_HpRender(SCENE_STAGE03, true);

			CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstnace->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Skill_UI"), 0));
			if (nullptr == pSkill_UI)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			pSkill_UI->Set_AllSkillRenderSet(true);


			CScene_Stage03* pStage = (CScene_Stage03*)pGameInstnace->Get_CurrentScene();
			if (((CPlayer*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Stage03PhaseNum() != 2)
			{
				pStage->Setting_StageUpdate(SCENE_STAGE04);
			}
			else
				pStage->Setting_StageUpdate(SCENE_STAGE05);

			RELEASE_INSTANCE(CGameInstance);
		}

		break;
	}
	case Client::CCamera_Fly::STAGE3_CUT_END:
	{
		Set_CameraType(CAMERATYPE_DEPENDENT);
		break;
	}
	}
}

void CCamera_Fly::Update_StaticMonsterView(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_eStaticMonster)
	{
	case Client::CCamera_Fly::MONSTER_CENSOR:
	{
		if (m_bMouseMove)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.553f, 3.820f, 9.539f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(0.032f, 0.190f, -0.981f, 0.f));
			CMonsterInfo::MONSTERINFO  tagMonsterInfo;
			ZeroMemory(&tagMonsterInfo, sizeof(CMonsterInfo::MONSTERINFO));
			tagMonsterInfo.iNumTextrue = CMonsterInfo::MONSTER_CENSOR;
			tagMonsterInfo.IsSpawn = &m_IsSpawned;
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterInfo"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"), &tagMonsterInfo);
			m_bMouseMove = false;
		}
		else
		{
			CEnemy* pEnemy = static_cast<CEnemy*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Censor"), 0));
			if (nullptr != pEnemy)
				m_IsSpawned = pEnemy->IsSpawned();
			_uint iLayerSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"));
			if (0 == iLayerSize)
				m_eStaticMonster = MONSTER_END;
		}
	}
	break;
	case Client::CCamera_Fly::MONSTER_CENSORB:
	{
		if (m_bMouseMove)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(35.f, 6.560f, 30.f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.884f, -0.224f, -0.411f, 0.f));
			m_bMouseMove = false;
			CMonsterInfo::MONSTERINFO  tagMonsterInfo;
			ZeroMemory(&tagMonsterInfo, sizeof(CMonsterInfo::MONSTERINFO));
			tagMonsterInfo.iNumTextrue = CMonsterInfo::MONSTER_CENSORB;
			tagMonsterInfo.IsSpawn = &m_IsSpawned;
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterInfo"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"), &tagMonsterInfo);
		}
		else
		{
			CEnemy* pEnemy = static_cast<CEnemy*>(pGameInstance->Get_GameObject(SCENE_STAGE04, TEXT("Layer_CensorB"), 0));
			if (nullptr != pEnemy)
				m_IsSpawned = pEnemy->IsSpawned();
			_uint iLayerSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"));

			if (0 == iLayerSize)
				m_eStaticMonster = MONSTER_END;
		}
	}
	break;
	case Client::CCamera_Fly::MONSTER_BADIDEA:
	{
		if (m_bMouseMove)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(21.f, 4.f, 34.f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.069f, -0.259f, -0.963f, 0.f));
			m_bMouseMove = false;
			CMonsterInfo::MONSTERINFO  tagMonsterInfo;
			ZeroMemory(&tagMonsterInfo, sizeof(CMonsterInfo::MONSTERINFO));
			tagMonsterInfo.iNumTextrue = CMonsterInfo::MONSTER_BADIDEA;
			tagMonsterInfo.IsSpawn = &m_IsSpawned;
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterInfo"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"), &tagMonsterInfo);
		}
		else
		{
			CEnemy* pEnemy = static_cast<CEnemy*>(pGameInstance->Get_GameObject(SCENE_STAGE04, TEXT("Layer_BadIdea"), 0));
			if (nullptr != pEnemy)
				m_IsSpawned = pEnemy->IsSpawned();
			_uint iLayerSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"));

			if (0 == iLayerSize)
				m_eStaticMonster = MONSTER_END;
		}
	}
	break;
	case Client::CCamera_Fly::MONSTER_DENTURE:
	{
		if (m_bMouseMove)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.683f, 6.795f, 26.583f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.964f, -0.036f, -0.263f, 0.f));
			m_bMouseMove = false;
			CMonsterInfo::MONSTERINFO  tagMonsterInfo;
			ZeroMemory(&tagMonsterInfo, sizeof(CMonsterInfo::MONSTERINFO));
			tagMonsterInfo.iNumTextrue = CMonsterInfo::MONSTER_DENTURE;
			tagMonsterInfo.IsSpawn = &m_IsSpawned;
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterInfo"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"), &tagMonsterInfo);
		}
		else
		{
			m_Ratio += TimeDelta;
			if (4.0 < m_Ratio)
				m_IsSpawned = true;
			_uint iLayerSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"));

			if (0 == iLayerSize)
			{
				/*����*/
				CSkill_Info* pSkill_Info = dynamic_cast<CSkill_Info*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_Info"), 4));
				if (nullptr == pSkill_Info)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				pSkill_Info->Set_IsRender(true);

				CSkill_UI* pSkill_UI = dynamic_cast<CSkill_UI*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Skill_UI"), 0));
				if (nullptr == pSkill_UI)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				pSkill_UI->Set_SkillRenderSet(1, true);
				m_eStaticMonster = MONSTER_END;
			}
		}
	}
	break;
	case Client::CCamera_Fly::MONSTER_REGRET:
	{
		if (m_bMouseMove)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(48.002f, 3.203f, 37.606f, 1.f));
			m_pTransform->Set_MyLook(XMVectorSet(-0.084f, 0.037f, 0.996f, 0.f));
			m_bMouseMove = false;
			CMonsterInfo::MONSTERINFO  tagMonsterInfo;
			ZeroMemory(&tagMonsterInfo, sizeof(CMonsterInfo::MONSTERINFO));
			tagMonsterInfo.iNumTextrue = CMonsterInfo::MONSTER_REGRET;
			tagMonsterInfo.IsSpawn = &m_IsSpawned;
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterInfo"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"), &tagMonsterInfo);
		}
		else
		{
			CEnemy* pEnemy = static_cast<CEnemy*>(pGameInstance->Get_GameObject(SCENE_STAGE01, TEXT("Layer_Doubt"), 0));
			if (nullptr != pEnemy)
				m_IsSpawned = pEnemy->IsSpawned();
			_uint iLayerSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"));
			if (0 == iLayerSize)
				m_eStaticMonster = MONSTER_END;
		}
	}
	break;
	case Client::CCamera_Fly::MONSTER_END:
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		if (nullptr == pPlayer)
			pPlayer->Set_IsAnimUpdate(true);
		m_bMouseMove = true;
		m_IsSpawned = false;
		m_Ratio = 0.0;
		Set_CameraType(CAMERATYPE_DEPENDENT);
	}
	break;
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}



void CCamera_Fly::Set_StaticMonsterType(STATICMONSTER eMonsterType)
{
	m_eStaticMonster = eMonsterType;
}

_bool CCamera_Fly::IsDefault()
{
	if (CAMERATYPE_DEPENDENT == m_eCameraType)
 		return true;
	else
		return false;
}

_vector CCamera_Fly::Compute_Pos_Dependent(_double dTimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval, _float4 vPlayerPos)
{
	_vector vDirToPlayer = m_pTransform->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&vPlayerPos);

	_float fCos = GET_VECTORDOT(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDirToPlayer);

	//if (fMinDot >= fCos)
	//{
	//	fCos = fMinDot;
	//	m_bLimitDown = true;
	//}
	//else if (fMaxDot <= fCos)
	//{
	//	fCos = fMaxDot;
	//	m_bLimitUp = true;
	//}
	if (-0.9f >= fCos)
	{
		fCos = -0.9f;
		m_bLimitDown = true;
	}
	else if (fMaxDot <= fCos)
	{
		fCos = fMaxDot;
		m_bLimitUp = true;
	}
	_float fOffset = (fCos + 0.3f)*-100.f;
	m_vFixAdjustPos.y = -100.f - fOffset;

	_float fTest = (fUpDis*(fCos - fMinDot) - fDownDis*(fCos - fMaxDot)) / (fMaxDot - fMinDot);

	if (true == m_bIsShaking)
	{
		_float fRandom[3];
		fRandom[0] = (rand() % 10 - 5)*0.005f;
		fRandom[1] = (rand() % 10 - 5)*0.005f;
		fRandom[2] = (rand() % 10 - 5)*0.005f;

		vInterval.x += fRandom[0];
		vInterval.y += fRandom[1];
		vInterval.z += fRandom[2];
	}

	vInterval.z = -fTest;
	return XMLoadFloat4(&vInterval);
}

_bool CCamera_Fly::IsMoveY(_long lMouseMove)
{
	if (true == m_bLimitUp)
	{
		if (0 > lMouseMove)
		{
			m_bLimitUp = false;
			return true;
		}
		else
			return false;
	}
	if (true == m_bLimitDown)
	{
		if (0 < lMouseMove)
		{
			m_bLimitDown = false;
			return true;
		}
		else
			return false;
	}
	return true;
}

void CCamera_Fly::Lerp_Once(_double TimaDelta, _vector vSourPos, _vector vDestPos, _vector vSourLook, _vector vDestLook, _float3* pOut_Look, _float3* pOut_Pos)
{

	m_Ratio += (_float)TimaDelta*m_LerpSpeed;
	//if (!(CAMERTYPE_PARKOUR == m_eCameraType))
	//	m_Ratio = 0.01;
	if (1.f <= m_Ratio)
	{
		m_bIsLerp = false;
		m_Ratio = 0.f;

		XMStoreFloat3(&*pOut_Pos, vDestPos);
		XMStoreFloat3(&*pOut_Look, vDestLook);
		return;
	}

	XMStoreFloat3(&*pOut_Pos, XMVectorLerp(vSourPos, vDestPos, (_float)m_Ratio));
	XMStoreFloat3(&*pOut_Look, XMVectorLerp(vSourLook, vDestLook, (_float)m_Ratio));
}

void CCamera_Fly::Lerp_Once(_double TimaDelta, _vector vSourPos, _vector vDestPos, _float3 * pOut_Pos)
{
	m_Ratio += (_float)TimaDelta*m_LerpSpeed;
	if (CAMERTYPE_PARKOUR == m_eCameraType)
		m_Ratio = 0.1f;
	if (CAMERATYPE_PARKOUR_END == m_eCameraType)
		m_Ratio = 0.1f;

	if (1.f <= m_Ratio)
	{
		m_bIsLerp = false;
		m_Ratio = 0.f;

		XMStoreFloat3(&*pOut_Pos, vDestPos);

		return;
	}


	XMStoreFloat3(&*pOut_Pos, (1.f - m_Ratio)*vSourPos + m_Ratio*vDestPos);
}

void CCamera_Fly::Lerp_DependentValue(_double TimaDelta)
{
	if (true == m_bLerpOnce)
	{
		m_dDependentTimeAcc += TimaDelta*m_dDependentLerpSpeed;

		if (1.f <= m_dDependentTimeAcc)
		{
			m_bLerpOnce = false;
			m_dDependentTimeAcc = 0.f;

			m_vDependentValue = m_vDestDependentValue;
			m_fDependentIntervalZ = m_fDestDependentIntervalZ;
		}

		XMStoreFloat4(&m_vDependentValue, XMVectorLerp(XMLoadFloat4(&m_vDependentValue), XMLoadFloat4(&m_vDestDependentValue), (_float)m_dDependentTimeAcc));
		m_fDependentIntervalZ = (1.f - m_dDependentTimeAcc)*(_float)m_fDependentIntervalZ + (_float)m_dDependentTimeAcc*m_fDestDependentIntervalZ;
	}
}

void CCamera_Fly::Lerp_InterverZ(_double TimeDelta)
{
	if (true == m_IsLerpZCheck)
	{
		m_lfRatioZ += TimeDelta * m_lfZInterverSpeed;

		if (1.0 <= m_lfRatioZ)
			m_lfRatioZ = 1.0;

		m_fLerpInterverZ = m_fLerpInterverStartZ * (1.f - (_float)m_lfRatioZ) + 1.f * (_float)m_lfRatioZ;

		if (1.0 <= m_lfRatioZ)
		{
			m_IsLerpZCheck = false;
			m_fLerpInterverZ = 1.f;
			m_lfZInterverSpeed = 0.0;
			m_lfRatioZ = 0.0;
		}
	}
}

void CCamera_Fly::Set_Default_Value()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iSceneIndex = pGameInstance->Get_CurrentSceneIndex();
	if (SCENE_DEMO == iSceneIndex)
	{
		Set_DependentType_Value(_float4(0.7f, 0.1f, 4.7f, 4.f), 1.6f);
		Set_Fovy(60.f);
	}
	else if (SCENE_STAGE01 == iSceneIndex)
	{
		Set_DependentType_Value(_float4(0.7f, -0.305f, 6.f, 3.f), 1.5f);
		Set_Fovy(70.f);
	}
	else if (SCENE_STAGE02 == iSceneIndex)
	{
		Set_DependentType_Value(_float4(0.7f, -0.305f, 6.f, 3.f), 1.5f);
		Set_Fovy(70.f);
	}
	else if (SCENE_STAGE03 == iSceneIndex)
	{
		Set_DependentType_Value(_float4(0.7f, -0.7f, 5.84f, 2.045f), 1.74f);
		Set_Fovy(90.f);
		Set_Far(200.f);
	}
	RELEASE_INSTANCE(CGameInstance);
}

_float4 CCamera_Fly::Get_StandardPos()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return _float4(0.f, 0.f, 0.f, 1.f);
	}
	_matrix PlayerWorldMatrix = pTransform->Get_WorldMatrix();
	_float4 vPos;
	XMStoreFloat4(&vPos, PlayerWorldMatrix.r[3]);



	//����
	if (SCENE_STAGE05 == pGameInstance->Get_CurrentSceneIndex())
	{
		if (true == m_IsLerpZCheck)
		{
			vPos.y += m_fLerpInterverZ;
			vPos.z -= m_fLerpInterverZ;
		}
		else
			vPos.y += m_fDependentIntervalZ;
	}
	else
		vPos.y += m_fDependentIntervalZ;




	//if (true == m_bIsFixY)
	//{
	//	_vector vOrthoPos = PlayerWorldMatrix.r[3];

	//	_matrix View = pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	//	_matrix Proj = pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
	//	vOrthoPos = XMVector3TransformCoord(vOrthoPos, View);
	//	vOrthoPos = XMVector3TransformCoord(vOrthoPos, Proj);

	//	_float4 vOrthoPos_Float4;
	//	XMStoreFloat4(&vOrthoPos_Float4, vOrthoPos);

	//	if (0.2f < vOrthoPos_Float4.y)
	//	{
	//		vOrthoPos_Float4.y = 0.2f;

	//		vOrthoPos = XMVector3TransformCoord(vOrthoPos,XMMatrixInverse(nullptr, Proj));
	//		vOrthoPos = XMVector3TransformCoord(vOrthoPos, XMMatrixInverse(nullptr, View));

	//		XMStoreFloat4(&vOrthoPos_Float4, vOrthoPos);

	//		_float fDisY = vOrthoPos_Float4.y - vPos.y;

	//		vPos.y = m_fBackUpPosY+fabs(fDisY);
	//	}
	//	else
	//		vPos.y = m_fBackUpPosY;
	//}
	//else
	//{
	//	m_fBackUpPosY = vPos.y;
	//}
	if (CAMERTYPE_PARKOUR == m_eCameraType)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
		_bool bIsJump = pPlayer->IsJumping();
		if (true == bIsJump)
		{

			//else if (true == m_bParkourSignal_Exit_Up)
			//{
			//	m_fBackUpPosY += 1.f;
			//	m_bParkourSignal_Exit_Up = false;
			//}
			//else if (true == m_bParkourSignal_Exit_Down)
			//{
			//	m_fBackUpPosY += (vPos.y - m_fBackUpPosY);
			//	m_bParkourSignal_Exit_Down = false;
			//}

			vPos.y = m_fBackUpPosY;
		}
		else
		{
			//if (true == m_bParkourSignal_Ledge)
			//{
			//	m_fBackUpPosY += (vPos.y - m_fBackUpPosY);
			//	m_bParkourSignal_Ledge = false;

			//	vPos.y = m_fBackUpPosY;
			//}
			//else
			m_fBackUpPosY = vPos.y;
		}
		//_vector vOrthoPos = PlayerWorldMatrix.r[3];

		//_matrix View = pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
		//_matrix Proj = pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
		//vOrthoPos = XMVector3TransformCoord(vOrthoPos, View);
		//vOrthoPos = XMVector3TransformCoord(vOrthoPos, Proj);

		//_float4 vOrthoPos_Float4;
		//XMStoreFloat4(&vOrthoPos_Float4, vOrthoPos);

		//if (-0.8f > vOrthoPos_Float4.y)
		//{
		//	vOrthoPos_Float4.y = -0.8f;

		//	vOrthoPos = XMVector3TransformCoord(vOrthoPos, XMMatrixInverse(nullptr, Proj));
		//	vOrthoPos = XMVector3TransformCoord(vOrthoPos, XMMatrixInverse(nullptr, View));

		//	XMStoreFloat4(&vOrthoPos_Float4, vOrthoPos);
		//	m_fBackUpPosY = vOrthoPos_Float4.y;
		//}
		//vPos.y = m_fBackUpPosY;
	}
	else
	{
		m_fBackUpPosY = vPos.y;
	}
	RELEASE_INSTANCE(CGameInstance);

	return vPos;
}

void CCamera_Fly::Lerp_Fovy(_double TimaDelta)
{
	if (true == m_bIsChangeFovy)
	{
		m_dFovyRatio += (_float)TimaDelta*m_dFovyLerpSpeed;
		if (1.f <= m_dFovyRatio)
		{
			if (true == m_bIsFovyReverse)
			{
				m_dFovyRatio = 0.f;
				m_CameraDesc.fFovy = m_fDestFovy;
				m_fSourFovy = m_fDestFovy;
				m_fDestFovy = m_fBackUpFovy;

				m_bIsFovyReverse = false;
			}
			else
			{
				m_CameraDesc.fFovy = m_fDestFovy;

				m_bIsChangeFovy = false;
				m_bIsFovyReverse = false;
				m_dFovyRatio = 0.f;
				m_dFovyLerpSpeed = 0.f;

				m_fDestFovy = 0.f;
				m_fSourFovy = 0.f;

			}
			return;
		}

		_float fFovy = (1 - (_float)m_dFovyRatio)*m_fSourFovy + m_fDestFovy*(_float)m_dFovyRatio;
		m_CameraDesc.fFovy = fFovy;
	}
}

void CCamera_Fly::Shaking(_double TimeDelta, _bool bIsFix, _float fPower)
{
	if (true == m_bIsShaking)
	{
		m_dShakingTimeAcc += TimeDelta;
		if (m_dShakingTimeMax < m_dShakingTimeAcc)
		{
			m_dShakingTimeAcc = 0.0;
			m_dShakingTimeMax = 0.0;
			m_bIsShaking = false;
			m_vBackUpShakePos = _float3(0.f, 0.f, 0.f);
			m_vBackUpShakeLook = _float3(0.f, 0.f, 0.f);
			return;
		}
		_float fRandom[3];
		fRandom[0] = (rand() % 10 - 5)*fPower;
		fRandom[1] = (rand() % 10 - 5)*fPower;
		fRandom[2] = (rand() % 10 - 5)*fPower;

		_float3 vPos;

		if (true == m_bIsFixShake)
			vPos = m_vBackUpShakePos;
		else
			XMStoreFloat3(&vPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
		vPos.x += fRandom[0];
		vPos.y += fRandom[1];
		vPos.z += fRandom[2];
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));

		_float3 vLook;

		_float fRandom2[3];
		fRandom2[0] = (rand() % 10 - 5)*fPower;
		fRandom2[1] = (rand() % 10 - 5)*fPower;
		fRandom2[2] = (rand() % 10 - 5)*fPower;
		if (true == m_bIsFixShake)
			vLook = m_vBackUpShakeLook;
		else
			XMStoreFloat3(&vLook, m_pTransform->Get_State(CTransform::STATE_LOOK));
		vLook.x += fRandom2[0];
		vLook.y += fRandom2[1];
		vLook.z += fRandom2[2];

		m_pTransform->Set_MyLook(XMLoadFloat3(&vLook));
	}
	else
	{
		XMStoreFloat3(&m_vBackUpShakePos, m_pTransform->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat3(&m_vBackUpShakeLook, m_pTransform->Get_State(CTransform::STATE_LOOK));
	}
}

void CCamera_Fly::Bone_PositionGet(const char * pBossBoneName, _float4 * pGetPosition, _bool bResetOn)
{
	if (true == bResetOn)
	{
		if (nullptr != m_pTargetWorldMatrix)
		{
			m_TargetOffsetMatrix = {};
			m_TargetPivotMatrix = {};
			m_pTargetCombinedMatrix = nullptr;
			m_pTargetWorldMatrix = nullptr;
		}
	}


	if (nullptr == m_pTargetWorldMatrix)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pBoss = pGameInstance->Get_GameObject(SCENE_STAGE05, L"Layer_FinalBoss", 0);
		if (nullptr == pBoss)
		{
			Safe_Release(pGameInstance);
			return;
		}
		Safe_Release(pGameInstance);

		CModel* pBossModel = nullptr;
		CTransform* pBossTransform = nullptr;

		if (m_eBossHit == BOSS_DISH_HIT_OTTOTWO)
		{
			pBossModel = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Model(CBoss_Goat::BOSS_B);
			pBossTransform = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Transform(CBoss_Goat::BOSS_B);
		}
		else if (m_eBossHit == BOSS_DISH_HIT_HOLLIESTWO)
		{
			pBossModel = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Model(CBoss_Goat::BOSS_S);
			pBossTransform = dynamic_cast<CBoss_Goat*>(pBoss)->Get_Transform(CBoss_Goat::BOSS_S);
		}

		m_TargetOffsetMatrix = pBossModel->Get_OffsetMatrix(pBossBoneName);
		m_TargetPivotMatrix = pBossModel->Get_PivotMatrix();
		m_pTargetCombinedMatrix = pBossModel->Get_CombinedMatrixPtr(pBossBoneName);
		m_pTargetWorldMatrix = pBossTransform->Get_WorldMatrixPtr();

		if (nullptr == m_pTargetWorldMatrix || nullptr == m_pTargetCombinedMatrix)
			*pGetPosition = _float4(0.f, 0.f, 0.f, 0.f);

	}

	if (nullptr != m_pTargetCombinedMatrix && nullptr != m_pTargetWorldMatrix)
	{
		_float4x4 vBoneMat;
		_matrix BoneMatrix = XMLoadFloat4x4(&m_TargetOffsetMatrix) * XMLoadFloat4x4(m_pTargetCombinedMatrix) * XMLoadFloat4x4(&m_TargetPivotMatrix) * XMLoadFloat4x4(m_pTargetWorldMatrix);

		XMStoreFloat4x4(&vBoneMat, BoneMatrix);

		*pGetPosition = *(_float4*)vBoneMat.m[3];
	}
}

void CCamera_Fly::Set_Camera_Shaking(_bool bCheck, _double dMaxTime, _bool bIsFix)
{
	m_bIsShaking = bCheck;
	m_dShakingTimeMax = dMaxTime;
	m_bIsFixShake = bIsFix;
}

void CCamera_Fly::IsLook_Fix(_bool bCheck, _float3 vLookFixDir, _bool bIsLimitTime, _double dLimitTime)
{
	m_vLookFixDir = vLookFixDir;
	m_bLookFix = bCheck;
	m_bIsLimitTime = bIsLimitTime;
	m_dLimitTime = dLimitTime;
}

CCamera_Fly * CCamera_Fly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCamera_Fly*	pInstance = new CCamera_Fly(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCamera_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Fly::Clone(void * pArg, _uint iSceneIndex)
{
	CCamera_Fly*	pInstance = new CCamera_Fly(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created CCamera_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Fly::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
}
