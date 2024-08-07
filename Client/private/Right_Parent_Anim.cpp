#include "stdafx.h"
#include "..\public\Right_Parent_Anim.h"
#include "GameInstance.h"
#include "Player_RightHand.h"
#include "Right_Hand_Anim.h"
#include "Camera_Fly.h"

CRight_Parent_Anim::CRight_Parent_Anim()
{
}

void CRight_Parent_Anim::SetUp_StateInfo(CPlayer_RightHand* pRightHand, CTransform* pTransformCom, CModel* pModelCom, CCollider* pRenderColider, CSphereCollider* pSphereColider, _uint iMyScene)
{
	m_pRightHand = pRightHand;
	m_pTransformCom = pTransformCom;
	m_pModelCom = pModelCom;
	m_pRenderColiderCom = pRenderColider;
	m_pColliderCom = pSphereColider;
	m_iCurScene = iMyScene;
}

void CRight_Parent_Anim::SetUp_BoneTargetMatrix(const char * pBoneName)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform*	pTargetTransform = (CTransform*)pGameInstance->Get_Components(m_iCurScene, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTargetTransform)
	{
		MSGBOX("Failed to SetupBoneTargetMatrix : Funtion CRight_Parent_Anim");
		Safe_Release(pGameInstance);
		return;
	}
	CModel*		pModel = (CModel*)pGameInstance->Get_Components(m_iCurScene, TEXT("Layer_Player"), TEXT("Com_Model"), 0);
	if (nullptr == pModel)
	{
		MSGBOX("Failed to SetupBoneTargetMatrix : Funtion CRight_Parent_Anim");
		Safe_Release(pGameInstance);
		return;
	}
	m_pMyWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	m_OffsetMatrix = pModel->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix = pModel->Get_PivotMatrix();
	m_pCombinedMatrix = pModel->Get_CombinedMatrixPtr(pBoneName);
	m_pTargetWorldMatrix = pTargetTransform->Get_WorldMatrixPtr();

	m_vDistance = _float4(2.f, 2.f, 0.3f, 1.f);

	Safe_Release(pGameInstance);
}

void CRight_Parent_Anim::Bone_MatrixCalculation(_double TimeDelta)
{
	if (nullptr != m_pCombinedMatrix || nullptr != m_pTargetWorldMatrix)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		
		CGameObject* pCameraFly = pGameInstance->Get_GameObject(m_iCurScene, L"Layer_Camera", 0);
		if (nullptr == pCameraFly)
		{
			MSGBOX("Failed to Finder CamFly : Function Right_Parent_Anim : Bone_MatrixCalculation ");
			Safe_Release(pGameInstance);
			return;
		}

		_matrix			MyMatrix;
		_vector			PlayerPosition;
		_vector			TargetBonePosition;
		_float4			LerpPosition;
		_matrix			CamWorldMat;
		_long			MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE::MMS_X))
			m_pTransformCom->RotationPerSec(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), TimeDelta * MouseMove * 0.08);

		CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();

		MyMatrix = XMLoadFloat4x4(m_pMyWorldMatrix);
		
		PlayerPosition = XMLoadFloat4(&*(_float4*)&m_pTargetWorldMatrix->m[3]);

		MyMatrix.r[3] = PlayerPosition;

		TargetBonePosition = XMVector3TransformCoord(XMLoadFloat4(&m_vDistance), MyMatrix);

		XMStoreFloat4(&LerpPosition ,XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), TargetBonePosition, 0.3));

		LerpPosition.y += 0.3f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&LerpPosition));

		m_pTransformCom->Set_MyLook(CamWorldMat.r[2]);

		Safe_Release(pGameInstance);

	}
}

void CRight_Parent_Anim::Free()
{
}
