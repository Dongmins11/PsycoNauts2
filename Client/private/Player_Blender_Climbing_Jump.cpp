#include "stdafx.h"
#include "..\public\Player_Blender_Climbing_Jump.h"
#include "Player_IdleAnim.h"
#include "Player_RunAnim.h"
#include "Player_Jump.h"
#include "Camera_Fly.h"
#include "Blender2.h"
#include "Player_Blender_Climbing.h"
#include "Player_Blender_Climbing_JumpEnd.h"

IMPLEMENT_SINGLETON(CPlayer_Blender_Climbing_Jump)

CPlayer_Blender_Climbing_Jump::CPlayer_Blender_Climbing_Jump()
{
}

void CPlayer_Blender_Climbing_Jump::HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_bGravityCheck = true;
	Setting_Initialize();

	_bool bUpKeyCheck = false;
	_bool bDownKeyCheck = false;
	_bool bJump = false;
	_bool Test = false;

	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if ('W' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			bUpKeyCheck = true;
		if ('S' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
			bDownKeyCheck = true;
		if (VK_SPACE == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
			bJump = true;
		//if ('I' == pAnimKeyDesc.iKey[i] && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_DOWN)
		//	Test = true;
	}

	if (VK_LSHIFT == pAnimKeyDesc.iOnceKey && pAnimKeyDesc.eOneceKeyState == CPlayer_AnimState::KEY_DOWN)
	{
		if (true == Blender_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
			return;
	}
	
	if (Test)
	{
		Reset_Anim(pPlayer_AnimState);
		m_pCharacterController->Set_Position(_float3(279.f, 6.f, 157.f));
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&_float3(287.5f, 19.4f, 195.3f)), 1.f));
		m_pTransform->Set_Velocity(XMVectorZero());
		m_pPlayer->Set_Climbing_TriggerCheck(false, -1);
		pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
		CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	if (bJump && bUpKeyCheck && false == m_KeyUpCheck) /* 올라가면서 점프 눌름 : 위로 올라감 */
	{
		/* m_bJumpMenu[BLENDERJUMP_BACK] flase랑 같은때 처리-> 다음틱에 들어와서 w를 누를수도 있기 때문에 아예 못들어오게 막기 */
		/* 멤버 변수에 점프 가능 체크 */
		m_KeyUpCheck = true;

		m_bJumpMenu[BLENDERJUMP_UP] = true;

		m_iUpperIndex = PLAYER_CLIMB_JUMP;
		m_iRootIndex = PLAYER_CLIMB_JUMP;
		m_iBottomIndex = PLAYER_CLIMB_JUMP;
		/* Jump 애니 메이션 진행 */
	}
	if (bJump && false == bUpKeyCheck && true == bDownKeyCheck) /* 점프 했을때 떨어짐*/
	{
		/* 기본 점프 */
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(70.f, 1.f);
		RELEASE_INSTANCE(CGameInstance);
		Reset_Anim(pPlayer_AnimState);
		Reverse_Look(TimeDelta);
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Jump::Get_Instance());
		CPlayer_Jump::Get_Instance()->Set_BlendJumpTest(true);
		CPlayer_Jump::Get_Instance()->Set_PlayerMoveValue(0.6f);
		CPlayer_Jump::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return;
	}

	if (m_iRootIndex == PLAYER_CLIMB_JUMP && true == m_pModel->Get_IsRootAnimFinishied(PLAYER_CLIMB_JUMP))
	{
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		m_iUpperIndex = PLAYER_FALLING;
		m_iRootIndex = PLAYER_FALLING;
		m_iBottomIndex = PLAYER_FALLING;
	}


	if (true == Blender_ClimbingJump_to_Blender_Climbing(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
		return;

	if (bJump && bUpKeyCheck && true == m_bJumpMenu[BLENDERJUMP_UP]) /* 마지막 점프로 탈출을 하기 위해서 */
	{
		if (true == Blender_ClimbingJump_to_Blender_ClimbingJumpEnd(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
			return;
	}

	//if (true == Blender_to_Idle(pPlayer_AnimState, pAnimKeyDesc, TimeDelta))
	//	return;

	//if (true == m_bJumpMenu[BLENDERJUMP_BACK])
	//	Jump_DOWN(pPlayer_AnimState, TimeDelta);

	//First_JumpStateCheck(); /* 점프 애니메이션 세팅 */

	m_pModel->Set_UpperAnimation(m_iUpperIndex);
	m_pModel->Set_RootAnimation(m_iRootIndex);
	m_pModel->Set_BottomAnimation(m_iBottomIndex);
}

void CPlayer_Blender_Climbing_Jump::Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	m_fLerpTimer += TimeDelta + m_fAccLerpTimer;

	if (1.0 >= m_fLerpTimer)
	{
		_float3 NewPlayerPostion;
		XMStoreFloat3(&NewPlayerPostion, XMVectorLerp(XMLoadFloat3(&m_vPlayerCurrentPostion), XMLoadFloat3(&m_vPlayerNextEndPostion), (_float)m_fLerpTimer));
		m_pCharacterController->Set_FootPosition(NewPlayerPostion);

		m_fAccLerpTimer += m_fAccLerpTimer;
	}
	else
		m_bHadleInput = true;
	

	m_pModel->Upper_Update(TimeDelta);
	m_pModel->Root_Update(TimeDelta);
	m_pModel->Bottom_Update(TimeDelta);
	
}
void CPlayer_Blender_Climbing_Jump::Reset_Anim(CPlayer_AnimState * pPlayer_AnimState)
{
	m_iUpperIndex = 0;
	m_iRootIndex = 0;
	m_iBottomIndex = 0;

	m_bInitialize = false;
	m_bHadleInput = false;
	m_KeyUpCheck = false;

	m_bJumpMenu[BLENDERJUMP_UP] = false;

	for (_uint i = 0; i < CLIMBING_JUMP_STATE_END; ++i)
		m_bAnimation_STATE[i] = false;

	pPlayer_AnimState->Set_FallSpeed(1.f);
	pPlayer_AnimState->Set_BackUpY(0.f);
	pPlayer_AnimState->Set_JumpTimeAcc(0.f);
	pPlayer_AnimState->Set_JumpValue(0.f);
	pPlayer_AnimState->Set_LandCheck(false);
	pPlayer_AnimState->Set_FallCheck(false);
	pPlayer_AnimState->Set_JumpCheck(false);

	m_vPlayerCurrentPostion = _float3(0.f, 0.f, 0.f);
	m_vPlayerNextEndPostion = _float3(0.f, 0.f, 0.f);

	m_fLerpTimer = 0.0;
	//m_fAccLerpTimer = 0.001;

	pPlayer_AnimState->Set_AllLerpCheckTrue();
}

void CPlayer_Blender_Climbing_Jump::Dead_Reset(CPlayer_AnimState * pPlayer_AnimState)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(70.f, 1.f);
	RELEASE_INSTANCE(CGameInstance);

	Reset_Anim(pPlayer_AnimState);
	m_pTransform->Set_Velocity(XMVectorZero());
}

void CPlayer_Blender_Climbing_Jump::Setting_Initialize()
{
	if (false == m_bInitialize)
	{
		m_JumpHeight = 3.f;
		m_fAccLerpTimer= 0.00001;
		m_KeyUpCheck = false;
		m_bJumpMenu[BLENDERJUMP_UP] = false;
		m_bInitialize = true;
		//m_bFirstCheck = false;
		m_bHadleInput = false;

		for (_uint i = 0; i < CLIMBING_JUMP_STATE_END; ++i)
			m_bAnimation_STATE[i] = false;

		m_fLerpTimer = 0.0;
		XMStoreFloat3(&m_vPlayerCurrentPostion, m_pTransform->Get_State(CTransform::STATE_POSITION));

		_float3 vNextPostion;
		XMStoreFloat3(&vNextPostion, m_pTransform->Get_State(CTransform::STATE_POSITION));
		
		XMStoreFloat3(&m_vPlayerNextEndPostion, XMVectorSet(vNextPostion.x, vNextPostion.y + m_JumpHeight, vNextPostion.z,1.f));
	}
}

_bool CPlayer_Blender_Climbing_Jump::Blender_to_Idle(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (PALYER_LANDING == m_iRootIndex)
	{
		if (true == m_pModel->Get_IsRootAnimFinishied(PALYER_LANDING))
		{
			m_iUpperIndex = 0;
			m_iRootIndex = 0;
			m_iBottomIndex = 0;

			Reset_Anim(pPlayer_AnimState);
			//m_bFirstCheck = false;
			pPlayer_AnimState->SetUp_State(CPlayer_IdleAnim::Get_Instance());
			CPlayer_IdleAnim::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
			return true;
		}
	}
	return false;
}

_bool CPlayer_Blender_Climbing_Jump::Blender_to_Test(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	return _bool();
}

_bool CPlayer_Blender_Climbing_Jump::Blender_ClimbingJump_to_Blender_Climbing(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	if (true == m_bHadleInput)
	{
		Reset_Anim(pPlayer_AnimState);
		m_pTransform->Set_Velocity(XMVectorZero());
		//만약필요한경우에만
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Blender_Climbing::Get_Instance());
		CPlayer_Blender_Climbing::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

_bool CPlayer_Blender_Climbing_Jump::Blender_ClimbingJump_to_Blender_ClimbingJumpEnd(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta)
{
	/* 조건 처리 */
	if (true == m_pPlayer->Get_Climbing_TriggerEndCheck())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(70.f, 1.f);
		RELEASE_INSTANCE(CGameInstance);

		Reset_Anim(pPlayer_AnimState);
		m_pTransform->Set_Velocity(XMVectorZero());
		//만약필요한경우에만
		pPlayer_AnimState->Set_AllLerpCheckTrue();
		pPlayer_AnimState->SetUp_State(CPlayer_Blender_Climbing_JumpEnd::Get_Instance());
		/*flase 다시 넣어주기 */
		CPlayer_Blender_Climbing_JumpEnd::Get_Instance()->HandleInput(pPlayer_AnimState, pAnimKeyDesc, TimeDelta);
		return true;
	}
	return false;
}

void CPlayer_Blender_Climbing_Jump::Jump_DOWN(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	if (false == pPlayer_AnimState->Get_JumpCheck())
	{
		pPlayer_AnimState->Set_JumpPower(m_fJumpPower);
		pPlayer_AnimState->Set_LandCheck(false);
		pPlayer_AnimState->Set_JumpCheck(true);
	}
}

void CPlayer_Blender_Climbing_Jump::First_JumpStateCheck()
{
	//if (false == m_bFirstCheck)
	//{
	//	m_fJumpPower = 6.85f;
	//	m_iRandomState = rand() % 3;
	//	Set_BlendSpeed(); /* 스피드를 정해 줄것 */
	//	if (true == m_bJumpMenu[BLENDERJUMP_UP])
	//	{

	//	}
	//	m_bFirstCheck = true;
	//}
}

void CPlayer_Blender_Climbing_Jump::Fall_Animation()
{



}


void CPlayer_Blender_Climbing_Jump::Land_Animation()
{
}

void CPlayer_Blender_Climbing_Jump::Set_BlendSpeed()
{
	m_pModel->Set_UpperAnimBlendSpeed(PLAYER_JUMP_UP, 5.f);
	m_pModel->Set_RootAnimBlendSpeed(PLAYER_JUMP_UP, 5.f);
	m_pModel->Set_BottomAnimBlendSpeed(PLAYER_JUMP_UP, 5.f);
}

void CPlayer_Blender_Climbing_Jump::Player_Move_NonMoveOffset(_double TimeDelta)
{
	SetUp_Look(TimeDelta);
	m_pTransform->Go_Straight(TimeDelta * m_lfMoveSpeed, m_pCharacterController);

	m_pModel->Upper_Update(TimeDelta * 0.8);
	m_pModel->Root_Update(TimeDelta * 0.8);
	m_pModel->Bottom_Update(TimeDelta * 0.8);
}

void CPlayer_Blender_Climbing_Jump::Blender_StartAnimation(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
{
	/* 조건으로 스타트 애니메이션을 출력 */
	/* 이후 애니메이션이 끝나는 키프레임때 bool값 하나 true로 변경 */
	/* m_fLerpTimer = true; */
}


void CPlayer_Blender_Climbing_Jump::Free()
{
}
