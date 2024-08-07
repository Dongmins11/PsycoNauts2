#include "stdafx.h"
#include "..\public\Player_ParentAnim.h"
#include "Camera_Fly.h"

CPlayer_ParentAnim::CPlayer_ParentAnim()
{
}

void CPlayer_ParentAnim::Set_StateInfo(CPlayer * pPlayer, CTransform * pPlayerTransform, CModel * pPlayerModel, CSphereCollider * pPlayerCollider, CNavigation* pPlayerNavigation, CCharacterController* pCharacterController, CPlayer_LeftHand* pLeftHand, CPlayer_RightHand* pRightHand, CPlayerTrigger_Parkour* pTriggerParkour, _uint iScene)
{

	if (nullptr == pPlayer || nullptr == pPlayerTransform || nullptr == pPlayerModel || nullptr == pCharacterController || 0 > iScene)
	{
		MSGBOX("Failed to ParentAnim : Fuintion(Set_StateInfo) Checking Plz");
		return;
	}

	m_pPlayer = pPlayer;
	m_pTransform = pPlayerTransform;
	m_pModel = pPlayerModel;
	m_pColliderCom = pPlayerCollider;
	m_pNavigation = pPlayerNavigation;
	m_pCharacterController = pCharacterController;
	m_pTrigger_Parkour = pTriggerParkour;
	m_iScene = iScene;

	m_pLeftHand = pLeftHand;
	m_pRightHand = pRightHand;
	m_lfLookLerpSpeed = 1.0;

	m_lfMoveSpeed = 0.5;
}

void CPlayer_ParentAnim::Handle_Movement(const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc)
{
	for (_uint i = 0; i < pAnimKeyDesc.iKeyCount; ++i)
	{
		if (pAnimKeyDesc.iKey[i] == 'W' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_eCurrentLookState = LOOK_FRONT;

			for (_uint j = 0; j < pAnimKeyDesc.iKeyCount; ++j)
			{
				if (pAnimKeyDesc.iKey[j] == 'D' && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING)
				{
					m_eCurrentLookState = LOOK_RIGHT_UP;
					return;
				}
				else if (pAnimKeyDesc.iKey[j] == 'A' && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING)
				{
					m_eCurrentLookState = LOOK_LEFT_UP;
					return;
				}
			}
		}
		else if (pAnimKeyDesc.iKey[i] == 'S' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_eCurrentLookState = LOOK_BACK;
			for (_uint j = 0; j < pAnimKeyDesc.iKeyCount; ++j)
			{
				if (pAnimKeyDesc.iKey[j] == 'D' && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING)
				{
					m_eCurrentLookState = LOOK_RIGHT_DOWN;
					return;
				}
				else if (pAnimKeyDesc.iKey[j] == 'A' && pAnimKeyDesc.eKeyState[j] == CPlayer_AnimState::KEY_PRESING)
				{
					m_eCurrentLookState = LOOK_LEFT_DOWN;
					return;
				}
			}
		}
		else if (pAnimKeyDesc.iKey[i] == 'D' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_eCurrentLookState = LOOK_RIGHT;
			return;
		}
		else if (pAnimKeyDesc.iKey[i] == 'A' && pAnimKeyDesc.eKeyState[i] == CPlayer_AnimState::KEY_PRESING)
		{
			m_eCurrentLookState = LOOK_LEFT;
			return;
		}
	}
}

void CPlayer_ParentAnim::Move_Acceleration(_double TimeDelta)
{
	if (true == m_bIsMoveOn)
	{
		m_fMoveOffset += (_float)TimeDelta * 1.2f;
		if (1.f <= m_fMoveOffset)
			m_fMoveOffset = 1.f;
	}
	else
	{
		m_fMoveOffset -= (_float)TimeDelta * 2.f;
		if (0.f >= m_fMoveOffset)
			m_fMoveOffset = 0.f;
	}

}

void CPlayer_ParentAnim::SetUp_Look(_double TimeDelta)
{
	_vector vLook;

	XMStoreFloat3(&m_vDestRotation, Compute_DirVector());

	if (m_ePreLookState != m_eCurrentLookState)
	{
		m_bIsLefp = true;
		m_fRatio = 0.f;
		XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
	}
	if (true == m_bIsLefp)
		vLook = Lerp_Roation(TimeDelta*4.f);
	else
		vLook = XMLoadFloat3(&m_vDestRotation);

	m_pTransform->Set_MyLook(vLook);
}

void CPlayer_ParentAnim::SetUp_SlideLook(_double TimeDelta)
{
	_vector vLook;

	XMStoreFloat3(&m_vDestRotation, Compute_Slide_DirVector());

	if (m_ePreLookState != m_eCurrentLookState)
	{
		m_bIsLefp = true;
		m_fRatio = 0.f;
		XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
	}
	if (true == m_bIsLefp)
		vLook = Lerp_Roation(TimeDelta * 0.5);
	else
		vLook = XMLoadFloat3(&m_vDestRotation);

	m_pTransform->Set_MyLook(vLook);

}

void CPlayer_ParentAnim::SetUp_Cam_to_LookAt(_double TimeDelta)
{
	if (false == m_bFirstRotationCheck)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pCameraFly = pGameInstance->Get_GameObject(m_iScene, L"Layer_Camera", 0);
		if (nullptr == pCameraFly)
		{
			MSGBOX("Failed to Finder CamFly : Function Player_ParentAnim : Compute_DirVector");
			Safe_Release(pGameInstance);
			return;
		}
		Safe_Release(pGameInstance);

		_matrix CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();
		_float3 vCamLook;

		XMStoreFloat3(&vCamLook, CamWorldMat.r[2]);
		vCamLook.y = 0.f;

		XMStoreFloat3(&m_vDestRotation, XMLoadFloat3(&vCamLook));

		m_bFirstRotationCheck = true;
	}

	_vector vLook;

	if (m_ePreLookState != m_eCurrentLookState)
	{
		m_bIsLefp = true;
		m_fRatio = 0.f;
		XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
	}
	if (true == m_bIsLefp)
		vLook = Lerp_Roation(TimeDelta*6.f);
	else
		vLook = XMLoadFloat3(&m_vDestRotation);

	m_pTransform->Set_MyLook(vLook);

}

void CPlayer_ParentAnim::Reverse_Look(_double TimeDelta)
{
	_vector vLook;

	vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);

	m_pTransform->Set_MyLook(-vLook);
}

_vector CPlayer_ParentAnim::Lerp_Roation(_double TimeDelta)
{
	if (!XMVectorGetX(XMVectorEqual(XMLoadFloat3(&m_vDestBackUp), XMLoadFloat3(&m_vDestRotation))))
		XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));

	_float fCos = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&m_vSourRotation)), XMVector3Normalize(XMLoadFloat3(&m_vDestRotation))));
	if (-0.9f >= fCos)
		m_bReverse = true;

	_vector vLook;

	if (false == m_bReverse)
	{
		m_fRatio += (_float)TimeDelta * 2.0f;
		if (1.f <= m_fRatio)
		{
			m_fRatio = 0.f;
			m_bFirstRotationCheck = false;
			m_bIsLefp = false;
			return XMLoadFloat3(&m_vDestRotation);
		}

		_float fRad = XMScalarACos(fCos);
		_float fSinRad = XMScalarSin(fRad);
		{
			if (0.f == fSinRad)
			{
				XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
				return XMLoadFloat3(&m_vSourRotation);
			}
		}
		vLook = (XMScalarSin(fabsf(fRad)*(1 - m_fRatio)) / fabsf(fSinRad))*XMVector3Normalize(XMLoadFloat3(&m_vSourRotation)) + (XMScalarSin(fabsf(fRad)*m_fRatio) / fabsf(fSinRad))* XMVector3Normalize(XMLoadFloat3(&m_vDestRotation));
	}
	else
	{
		m_fRatio += (_float)TimeDelta;
		if (1.f <= m_fRatio)
		{
			m_fRatio = 0.f;
			m_bFirstRotationCheck = false;
			m_bIsLefp = false;
			m_bReverse = false;
			return XMLoadFloat3(&m_vDestRotation);
		}
		m_pTransform->RotationPerSec(m_pTransform->Get_State(CTransform::STATE_UP), TimeDelta);
		vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	}
	m_vDestBackUp = m_vDestRotation;
	m_bReverse = false;

	return vLook;
}

_vector CPlayer_ParentAnim::Lerp_Slide_Rotation(_double TimeDelta)
{
	if (!XMVectorGetX(XMVectorEqual(XMLoadFloat3(&m_vDestBackUp), XMLoadFloat3(&m_vDestRotation))))
		XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));

	_float fCos = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&m_vSourRotation)), XMVector3Normalize(XMLoadFloat3(&m_vDestRotation))));

	_vector vLook;

	if (false == m_bReverse)
	{
		_float fRad = XMScalarACos(fCos);
		_float fSinRad = XMScalarSin(fRad);

		if (0.f == fSinRad)
		{
			XMStoreFloat3(&m_vSourRotation, m_pTransform->Get_State(CTransform::STATE_LOOK));
			return XMLoadFloat3(&m_vSourRotation);
		}

		vLook = (XMScalarSin(fabsf(fRad)*(1 - 0.02)) / fabsf(fSinRad))*XMVector3Normalize(XMLoadFloat3(&m_vSourRotation)) + (XMScalarSin(fabsf(fRad)*0.02) / fabsf(0.02))* XMVector3Normalize(XMLoadFloat3(&m_vDestRotation));
	}

	m_vDestBackUp = m_vDestRotation;

	return vLook;
}

_vector CPlayer_ParentAnim::Compute_DirVector()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pCameraFly = pGameInstance->Get_GameObject(m_iScene, L"Layer_Camera", 0);
	if (nullptr == pCameraFly)
	{
		MSGBOX("Failed to Finder CamFly : Function Player_ParentAnim : Compute_DirVector");
		Safe_Release(pGameInstance);
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}
	Safe_Release(pGameInstance);

	_matrix CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();
	_vector	vCameraVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float4 vCamPos;
	switch (m_eCurrentLookState)
	{
	case CPlayer_ParentAnim::LOOK_FRONT:
		vCameraVector = CamWorldMat.r[2];
		break;
	case CPlayer_ParentAnim::LOOK_LEFT_UP:
		vCameraVector = CamWorldMat.r[2] - CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_RIGHT_UP:
		vCameraVector = CamWorldMat.r[2] + CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_BACK:
		vCameraVector = -CamWorldMat.r[2];
		break;
	case CPlayer_ParentAnim::LOOK_LEFT_DOWN:
		vCameraVector = -CamWorldMat.r[2] - CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_RIGHT_DOWN:
		vCameraVector = -CamWorldMat.r[2] + CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_LEFT:
		vCameraVector = -CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_RIGHT:
		vCameraVector = CamWorldMat.r[0];
		break;
	}
	XMStoreFloat4(&vCamPos, vCameraVector);
	vCamPos.y = 0.f;
	return XMVector3Normalize(XMLoadFloat4(&vCamPos));
}

_vector CPlayer_ParentAnim::Compute_Slide_DirVector()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pCameraFly = pGameInstance->Get_GameObject(m_iScene, L"Layer_Camera", 0);
	if (nullptr == pCameraFly)
	{
		MSGBOX("Failed to Finder CamFly : Function Player_ParentAnim : Compute_DirVector");
		Safe_Release(pGameInstance);
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}
	Safe_Release(pGameInstance);

	_matrix CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();
	_vector	vCameraVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float4 vCamPos;

	switch (m_eCurrentLookState)
	{
	case CPlayer_ParentAnim::LOOK_FRONT:
		vCameraVector = CamWorldMat.r[2];
		break;
	case CPlayer_ParentAnim::LOOK_LEFT_UP:
		vCameraVector = CamWorldMat.r[2] - CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_RIGHT_UP:
		vCameraVector = CamWorldMat.r[2] + CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_LEFT:
		vCameraVector = CamWorldMat.r[2] - CamWorldMat.r[0];
		break;
	case CPlayer_ParentAnim::LOOK_RIGHT:
		vCameraVector = CamWorldMat.r[2] + CamWorldMat.r[0];
		break;
	}
	XMStoreFloat4(&vCamPos, vCameraVector);
	vCamPos.y = m_pPlayer->Get_UpRayDirection().y * 2.f;
	return XMVector3Normalize(XMLoadFloat4(&vCamPos));
}

void CPlayer_ParentAnim::Player_Move(_double TimeDelta)
{
	SetUp_Look(TimeDelta);
	m_pTransform->Go_Straight(TimeDelta * m_lfMoveSpeed * m_fMoveOffset, m_pCharacterController);
}

void CPlayer_ParentAnim::Player_Move_NonMoveOffset(_double TimeDelta)
{
	SetUp_Look(TimeDelta);
	m_pTransform->Go_Straight(TimeDelta * m_lfMoveSpeed, m_pCharacterController);
}

void CPlayer_ParentAnim::Player_Look_Reverse_Move(_double TimeDelta)
{
	m_pTransform->Go_Straight(TimeDelta * m_lfMoveSpeed, m_pCharacterController);
}

//void CPlayer_ParentAnim::Set_Position_RightHand(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta)
//{
//	if (nullptr != m_pHitObject)
//	{
//		_vector HitObjectPos = ((CTransform*)m_pHitObject->Get_Components(TEXT("Com_Transform")))->Get_Position();
//
//		CTransform* pRightHandTransform = (CTransform*)m_pRightHand->Get_Components(TEXT("Com_Transform"));
//		if (nullptr == pRightHandTransform)
//		{
//			MSGBOX("Failed to Get_Component : Funtion : CPlayer_IdleAnim::Set_Position_RightHand ");
//			return;
//		}
//		pRightHandTransform->Set_State(CTransform::STATE_POSITION, HitObjectPos);
//		m_pRightHand->Set_ChangeAnimCheck(true);
//		m_pRightHand->Set_HandState(CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB);
//	}
//}

//void CPlayer_ParentAnim::Grab_RayCast_Ready(CPlayer_AnimState * pPlayer_AnimState)
//{
//	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
//	Safe_AddRef(pGameInstance);
//
//	if (nullptr == m_pHitObject)
//	{
//		_vector vRayPos, vRayDir;
//		vRayPos = pGameInstance->Get_RayPos();
//		vRayDir = XMVector3Normalize(pGameInstance->Get_RayDir());
//		const _tchar* pLayerTag;
//
//		CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
//		CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
//		XMStoreFloat3(&tDescIn.vOrigin, vRayPos);
//		XMStoreFloat3(&tDescIn.vUnitDir, vRayDir);
//		tDescIn.fDistance = 50.f;
//		tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC;
//		tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PICK);
//		tDescOut.ppHitObject = &m_pHitObject;
//		if (pGameInstance->Raycast(tDescIn, tDescOut))
//		{
//			if (m_pHitObject)
//			{
//				//Setting_Throwing
//				pPlayer_AnimState->Set_GrabCheck(true);
//				pPlayer_AnimState->Set_GrabAttack(true);
//				pLayerTag = m_pHitObject->Get_LayerName();
//				m_pHitObject->Set_IsPicking(true);
//			}
//		}
//	}
//	Safe_Release(pGameInstance);
//}
//
//void CPlayer_ParentAnim::Grab_RayCasting(CPlayer_AnimState * pPlayer_AnimState)
//{
//	if (nullptr != m_pHitObject && true == pPlayer_AnimState->Get_GrabAttackCheck())
//	{
//		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
//		Safe_AddRef(pGameInstance);
//
//		_vector vDir1 = pGameInstance->Get_CamPosition() - ((CTransform*)m_pHitObject->Get_Components(TEXT("Com_Transform")))->Get_Position();
//		_vector vDir2 = XMVector3Normalize(pGameInstance->Get_RayDir())*30.f;
//		m_pHitObject->Setting_Throwing(XMVector3Normalize(vDir1 + vDir2));
//		m_pHitObject->Set_IsPicking(false);
//		m_pHitObject = nullptr;
//
//		pPlayer_AnimState->Set_GrabCheck(false);
//
//		if (true == pPlayer_AnimState->Get_GrabAttackCheck())
//			pPlayer_AnimState->Set_GrabAttack(false);
//
//		Safe_Release(pGameInstance);
//	}
//
//}

HRESULT CPlayer_ParentAnim::SetUp_HeightOnNavigation()
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
	vPos.y = m_pNavigation->OnNavigationGetY(XMLoadFloat3(&vPos));
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
	return S_OK;

}

void CPlayer_ParentAnim::Free()
{

}
