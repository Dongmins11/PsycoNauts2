#include "stdafx.h"
#include "..\public\Player_Blender_Climbing.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Blender_Climbing_Jump.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
#include "Blender2.h"



IMPLEMENT_SINGLETON(CPlayer_Blender_Climbing)

CPlayer_Blender_Climbing::CPlayer_Blender_Climbing()
{
}

void CPlayer_Blender_Climbing::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	_bool bMoveCheck = false;
	_bool Test = false;
	_bool bUpKeyCheck = false;
	_bool bDownKeyCheck = false;
	_bool bJump = false;

	BLENDER_MOVE PlayeBlenderMoveIndex = BLENDER_MOVE_END;

	m_bGravityCheck = true; //중력이 false 가 중력주는것 true 중력을없에는것
	m_MoveDir = { 0.f,0.f,0.f,0.f };


	First_ChangeMotion(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	Enter_Motion_Checking(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	if (true == m_bEnterMotionCheck)
	{
		for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
		{
			if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				bUpKeyCheck = true;
				bMoveCheck = true;
				PlayeBlenderMoveIndex = BLENDER_MOVE_UP;
				Belnder_Move_DirSetting(PlayeBlenderMoveIndex);
			}
			if ('S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				bMoveCheck = true;
				bDownKeyCheck = true;
				PlayeBlenderMoveIndex = BLENDER_MOVE_DOWN;
				Belnder_Move_DirSetting(PlayeBlenderMoveIndex);
			}
			if ('A' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				bMoveCheck = true;
				PlayeBlenderMoveIndex = BLENDER_MOVE_LEFT;
				Belnder_Move_DirSetting(PlayeBlenderMoveIndex);
			}
			if ('D' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			{
				bMoveCheck = true;
				PlayeBlenderMoveIndex = BLENDER_MOVE_RIGHT;
				Belnder_Move_DirSetting(PlayeBlenderMoveIndex);
			}
			//if ('I' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			//{
			//	Test = true;
			//}
			if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			{
				bJump = true;
			}
		}


		if (bJump && bUpKeyCheck) /* 올라가면서 점프 눌름 : 위로 올라감 */
		{
			if (true == Blender_to_Jump(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
				return;
		}
		if (bJump && false == bUpKeyCheck && true == bDownKeyCheck) /* 점프 했을때 떨어짐*/
		{
			if (true == Blender_to_Jump(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
				return;
		}

		if (Test)
		{
			if (true == Blender_to_Test(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
				return;
		}
		if (false == bMoveCheck)
		{
			Blender_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

		}
	}


	if (m_iPreRootIndex != m_iRootIndex)
	{
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		m_iPreRootIndex = m_iRootIndex;
	}

	m_bIsMoveOn = bMoveCheck;

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Blender_Climbing::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	//여기에서 함수호출에서 기능구현해도되고
	//m_pPlayer들고와서 기능을뽑아와도되고
	//pPlayer_Animstate에서 기능을들고와서 써도되고
	//부모에서 선언을해서 기능을사용도됨

	//if (true == m_bIsMoveOn)
	//	m_pTransform->Go_Straight(TimeDelta,m_pCharacterController);


	Blender_LOOK_Change(pPlayer_AnimState, TimeDelta); /* 계속해서 쳐다 볼수있게 처리 할것 */

	if (false == m_bIsMoveOn) /* 움직임이 없다 IDLE */
		Blender_ILDE(pPlayer_AnimState, TimeDelta);

	if(true == m_bIsMoveOn) /* 움직임을 가져 갈때 */
		Blender_MOVE(pPlayer_AnimState, TimeDelta);




	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
}

void CPlayer_Blender_Climbing::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bEnterMotionCheck = false;
	m_bFristCheck = false;
	m_bIsMoveOn = false;
	m_bGravityCheck = false;
	m_MoveDir = { 0.f,0.f,0.f,0.f };
	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Blender_Climbing::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	Reset_Anim(pPlayer_AnimState);
	m_pTransform->Set_Velocity(XMVectorZero());
}

void CPlayer_Blender_Climbing::First_ChangeMotion(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (false == m_bFristCheck)
	{
		m_iUpperIndex = PLAYER_CLIMB_ENTER;
		m_iRootIndex = PLAYER_CLIMB_ENTER;
		m_iBottomIndex = PLAYER_CLIMB_ENTER;

		m_iPreRootIndex = PLAYER_CLIMB_ENTER;

		m_bFristCheck = true;
		m_bEnterMotionCheck = false;


		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_CLIME)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("Ledge.wav"), CHANNELID::PLAYER_CLIME)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);


		_int TriggerNum = m_pPlayer->Get_Climbing_TriggerNum();

		if (-1 != TriggerNum)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_vector TriggerUp = static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum))->Get_STATE_Trigger(CBlender2::BLENDERSTATE_UP);
			RELEASE_INSTANCE(CGameInstance);

			XMStoreFloat4(&m_MoveDir, XMVector3Normalize(TriggerUp*0.1f));
			Blender_MOVE(pPlayer_AnimState, TimeDelta);
		}
	}

}

void CPlayer_Blender_Climbing::Enter_Motion_Checking(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (m_iRootIndex == PLAYER_CLIMB_ENTER && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_CLIMB_ENTER))
	{
		if(false == m_bEnterMotionCheck)
			m_bEnterMotionCheck = true;
	}
}

void CPlayer_Blender_Climbing::Blender_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_iUpperIndex = PLAYER_CLIMB_IDLE;
	m_iRootIndex = PLAYER_CLIMB_IDLE;
	m_iBottomIndex = PLAYER_CLIMB_IDLE;
}

_bool CPlayer_Blender_Climbing::Blender_to_Jump(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	/* 일시적으로 움직이기 위해서 */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_CLIME_JUMP)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	if (FAILED(pGameInstance->PlaySoundOnce(TEXT("JumpOne.wav"), CHANNELID::PLAYER_CLIME_JUMP)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	RELEASE_INSTANCE(CGameInstance);
	


	Reset_Anim(pPlayer_AnimState);
	m_pTransform->Set_Velocity(XMVectorZero());
	//만약필요한경우에만
	pPlayer_AnimState->SetUp_State(CPlayer_Blender_Climbing_Jump::Get_Instance());
	CPlayer_Blender_Climbing_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);

	return true;
}

_bool CPlayer_Blender_Climbing::Blender_to_Test(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	/* 일시적으로 움직이기 위해서 */
	Reset_Anim(pPlayer_AnimState);
	
	m_pCharacterController->Set_Position(_float3(279.f, 6.f, 157.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(287.5f, 19.4f, 195.3f)), 1.f));
	m_pTransform->Set_Velocity(XMVectorZero());
	m_pPlayer->Set_Climbing_TriggerCheck(false,-1);
	//pPlayer_AnimState-> Get_ or Set
	//만약필요한경우에만
	
	pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
	CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
	return true;
}

void CPlayer_Blender_Climbing::Belnder_Move_DirSetting(BLENDER_MOVE playerMove)
{
	_int TriggerNum = m_pPlayer->Get_Climbing_TriggerNum();
	
	if (-1 != TriggerNum)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		//if (-1 == TriggerNum)
		//{
		//	RELEASE_INSTANCE(CGameInstance);
		//	return;
		//}

		_vector TriggerRight = static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum))->Get_STATE_Trigger(CBlender2::BLENDERSTATE_RIGHT);
		_vector TriggerUp = static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum))->Get_STATE_Trigger(CBlender2::BLENDERSTATE_UP);

		RELEASE_INSTANCE(CGameInstance);

		switch (playerMove)
		{
		case Client::CPlayer_Blender_Climbing::BLENDER_MOVE_LEFT:
		{
			XMStoreFloat4(&m_MoveDir, XMVector3Normalize((TriggerRight*0.1f)));
			m_iUpperIndex = PLAYER_CLIMB_UP_LEFT;
			m_iRootIndex = PLAYER_CLIMB_UP_LEFT;
			m_iBottomIndex = PLAYER_CLIMB_UP_LEFT;
			/* 왼쪽 */
			break;
		}
		case Client::CPlayer_Blender_Climbing::BLENDER_MOVE_RIGHT:
		{
			XMStoreFloat4(&m_MoveDir, XMVector3Normalize(TriggerRight * -0.1f));
			m_iUpperIndex = PLAYER_CLIMB_UP_RIGHT;
			m_iRootIndex = PLAYER_CLIMB_UP_RIGHT;
			m_iBottomIndex = PLAYER_CLIMB_UP_RIGHT;
			/* 오른쪽  */
			break;
		}
		case Client::CPlayer_Blender_Climbing::BLENDER_MOVE_UP:
		{
			XMStoreFloat4(&m_MoveDir, XMVector3Normalize(TriggerUp*0.1f));
			m_iUpperIndex = PLAYER_CLIMB_UP;
			m_iRootIndex = PLAYER_CLIMB_UP;
			m_iBottomIndex = PLAYER_CLIMB_UP;
			/* 위 */
			break;
		}
		case Client::CPlayer_Blender_Climbing::BLENDER_MOVE_DOWN:
		{
			XMStoreFloat4(&m_MoveDir, XMVector3Normalize(TriggerUp) * -0.1f);
			m_iUpperIndex = PLAYER_CLIMB_DOWN;
			m_iRootIndex = PLAYER_CLIMB_DOWN;
			m_iBottomIndex = PLAYER_CLIMB_DOWN;
			/* 아래 */
			break;
		}
		}
	}

}

void CPlayer_Blender_Climbing::Blender_LOOK_Change(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	_int TriggerNum = m_pPlayer->Get_Climbing_TriggerNum();
	if (-1 != TriggerNum)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float3 vTargetPos;
		_float3 vPlayerPos;
		//CTransform* TargetTransform = (CTransform*)(static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), TriggerNum)))->Get_Components(TEXT("Com_Transform"));
		
		_vector pos = XMVectorZero();
		if (0 == TriggerNum)
		{
			pos = XMVectorSet(277.32f, 8.40f, 142.79f, 1.f); /* 바라보는 방향 */
			//pos = XMVectorSet(277.65f, 8.40f, 155.63f, 1.f);
		}
		else if (1 == TriggerNum)
		{
			//pos = XMVectorSet(288.49f, 8.43f, 155.87f, 1.f);
			pos = XMVectorSet(290.49f, 8.43f, 154.45f, 1.f); /* 바라보는 방향 */
		}

		XMStoreFloat3(&vTargetPos, /*TargetTransform->Get_Position()*/pos);
		XMStoreFloat3(&vPlayerPos, m_pTransform->Get_Position());
		
		vTargetPos = _float3(vTargetPos.x, vPlayerPos.y, vTargetPos.z);
		m_pTransform->Set_MyLook(XMVector3Normalize(XMLoadFloat3(&vTargetPos) - XMLoadFloat3(&vPlayerPos)));
	
		//_vector vPlayerLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK) * -1.f);
		//_float	PlayerLookScale = m_pTransform->Get_ScaleFloat3().z / 2.f;
		//_vector NewLook = vPlayerLook * PlayerLookScale;

		//m_pCharacterController->Set_FootPosition()
		////XMStoreFloat3(&vPos, XMLoadFloat3(&vPos) + XMVector3Normalize(NewLook));
	
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPlayer_Blender_Climbing::Blender_ILDE(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	/* 움직임 없이 아이들 모션 + */
}

void CPlayer_Blender_Climbing::Blender_MOVE(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_int TriggerNum = m_pPlayer->Get_Climbing_TriggerNum();
	
	if (-1 == TriggerNum)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	
	CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
	CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
	_float3 vVelocity;
	_float4 vUpRayDirection;
	_vector vTriggerLook; 
	vTriggerLook = static_cast<CBlender2*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender2"), m_pPlayer->Get_Climbing_TriggerNum()))->Get_STATE_Trigger(CBlender2::BLENDERSTATE_LOOK);
	vTriggerLook *= -1.f;

	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, m_pTransform->Get_Position());
	
	

	_float3 vOriginPos = _float3(vPlayerPos.x, vPlayerPos.y + 0.9f, vPlayerPos.z);
	vOriginPos = STOREFLOAT3(XMLoadFloat3(&vOriginPos) -vTriggerLook);

	XMStoreFloat3(&tDescIn.vOrigin, XMLoadFloat3(&vOriginPos) + XMVector3Normalize(XMLoadFloat4(&m_MoveDir))*0.05f);
	XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(vTriggerLook));

	tDescIn.fDistance = 10.f;

	tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
	tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_CLIMBINGPLANE);
	CGameObject* pHitObject = nullptr;
	tDescOut.ppHitObject = &pHitObject;

	if (pGameInstance->Raycast(tDescIn, tDescOut))
	{
		if (pHitObject)
		{
			_vector vDir = XMVector3Normalize(-vTriggerLook);
			_float3 vPos = _float3(tDescOut.vHitPos.x, tDescOut.vHitPos.y - 0.9f, tDescOut.vHitPos.z);
			
			/* 확인 해봄 */
			//CTransform* TargetTransform = (CTransform*)pHitObject->Get_Components(TEXT("Com_Transform"));
			//_float3 fTargetPos;
			//XMStoreFloat3(&fTargetPos, TargetTransform->Get_Position());
			//_vector TargetUp = XMVector3Normalize(TargetTransform->Get_State(CTransform::STATE_UP));
			//TargetUp *= -1.f;
			//_vector Dir;
			//Dir = TargetUp * (TargetTransform->Get_ScaleFloat3().y / 2.f);
			//XMStoreFloat3(&fTargetPos, XMLoadFloat3(&fTargetPos) + Dir);
			////XMStoreFloat3(&fTargetPos, TargetTransform->Get_Position());
			//if(fTargetPos.y >= vPos.y)
			m_pCharacterController->Set_FootPosition(vPos);
		
			//_double dAdjustSpeed = 0.1;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_Blender_Climbing::Free()
{
}

