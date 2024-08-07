#include "stdafx.h"
#include "..\public\MoveCabinet.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"

CMoveCabinet::CMoveCabinet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CMoveCabinet::CMoveCabinet(const CMoveCabinet & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMoveCabinet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMoveCabinet::NativeConstruct(void * pArg,_uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName =TEXT("Layer_MoveCabinet");
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = (LayerType)objinfo.iPX_Index;

	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = objinfo.fFrustumRange;
	_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(objinfo.fCollider_Extent, objinfo.fCollider_Center, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;
	return S_OK;
}

_int CMoveCabinet::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	/* 확인용 입니다. */
	//Check();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bFall)/* 쓰러지다 */
		MoveFALL();
	if (true == m_bNextMove[0])
		NextUpMove();
	if (true == m_bNextMove[1])
		NextDownMove();
	if (true == m_bNextMove[2])
		LastUpMove();
	if (true == m_bNextMove[3])
		LastDownMove();

	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	{
		_int iPlayerCurlingIndex = 0;
		_int iDeleteCurlingIndex = 0;
		if (Setting_CurlingIndex(&iPlayerCurlingIndex, &iDeleteCurlingIndex))
		{
			if (m_iDeleteCurlingIndex == iDeleteCurlingIndex)
			{
				m_isDestroy = true;
				RELEASE_INSTANCE(CGameInstance);
				return 0;
			}
			Stage_RenderBranch(iPlayerCurlingIndex);
		}
	}
	else
	{
		if (m_isRender && m_pColliderCom)
		{
			m_pColliderCom->Remove_Actor();
		}
		m_isRender = false;
	}



	if (!m_isRender)
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}
	RELEASE_INSTANCE(CGameInstance);

	/*if (m_pColliderCom)
	{
		if (m_isPicking)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
			_vector vPos = pPlayerTrans->Get_Position() - pPlayerTrans->Get_State(CTransform::STATE_UP) * 0.7f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pColliderCom->SetGravity(false);
			m_isMove = true;
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (m_isMove && !m_isPicking)
		{
			m_pColliderCom->SetGravity(true);
		}
		else
		{
			m_pColliderCom->Reset_Power();
		}
		m_pColliderCom->Update_TransformPhysX();
	}*/

	//RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CMoveCabinet::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	/*if (m_pColliderCom)
	{
		m_pColliderCom->Update_Transform();
	}*/
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CMoveCabinet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (!(1 == m_iRenderPass && false == m_bIsRenderingShadow))
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		m_pModelCom->Render(m_iRenderPass);
	}
	return S_OK;
}

_bool CMoveCabinet::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* Player = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	if (Player)
	{
		*pCurlingIndex = Player->Get_CurlingIndex();
		*pDeleteIndex = Player->Get_Delete_CurlingIndex();
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
}

void CMoveCabinet::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
	case SCENE_DEMO:
	{
		_int NextIndex = iPlayerCurlingIndex + 1;
		if (iPlayerCurlingIndex == m_iCurlingIndex || NextIndex == m_iCurlingIndex)
		{
			if (!m_isRender && m_pColliderCom)
				m_pColliderCom->Add_Actor();
			m_isRender = true;
		}
		else
		{
			if (m_isRender && m_pColliderCom)
			{
				m_pColliderCom->Remove_Actor();
			}
			m_isRender = false;
		}
		break;
	}
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CMoveCabinet::MoveFALL()
{
	Time += 0.001f + AccumulationTime;
	if (1.01f >= Time)
	{
		XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), Time));
		CanbinetCountMove();
	}
	else
	{
		m_bFall = false;
		m_bNextMove[0] = true;

		if (0 == m_iMYObjectCount)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Camera_Shaking(true, 1.f);
			RELEASE_INSTANCE(CGameInstance);
		}
		//이펙트 생성
		_vector vPos;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		switch (m_iMYObjectCount)
		{
		case 0:
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetSmallRect1"),
				SCENE_STATIC, TEXT("Effect_MoveCabinetSmallRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			vPos = XMVectorSet(-84.76f, 0.2f, 86.69f, 1.f);
			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetSmallRect1"), vPos, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			break;
		case 1:
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetSmallRect2"),
				SCENE_STATIC, TEXT("Effect_MoveCabinetSmallRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			vPos = XMVectorSet(-80.49f, 0.2f, 86.7f, 1.f);
			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetSmallRect2"), vPos, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			break;
		case 2:
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetBigRect1"),
				SCENE_STATIC, TEXT("Effect_MoveCabinetBigRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			vPos = XMVectorSet(-84.03f, 0.2f, 96.86f, 1.f);
			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetBigRect1"), vPos, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			break;
		case 3:
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetBigRect2"),
				SCENE_STATIC, TEXT("Effect_MoveCabinetBigRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			vPos = XMVectorSet(-81.05f, 0.2f, 95.845f, 1.f);
			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MoveCabinetBigRect2"), vPos, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			break;
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	AccumulationTime = AccumulationTime *1.1f;	
}
void CMoveCabinet::CanbinetCountMove()
{
	switch (m_iMYObjectCount)
	{
	case 0:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, 0.f, XMConvertToRadians(m_MoveRoation.x)));
		break;
	case 1:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, XMConvertToRadians(180.f), XMConvertToRadians(m_MoveRoation.x)));
		break;
	case 2:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(XMConvertToRadians(m_MoveRoation.x), XMConvertToRadians(257.f), 0.f));
		break;
	case 3:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(XMConvertToRadians(m_MoveRoation.x), XMConvertToRadians(90.f), 0.f));
		break;
	}
}


void CMoveCabinet::NextUpMove()
{
	
	m_fNextTime[0] += 0.2f + m_NextAccumulationTime[0];
	if (1.0f >= m_fNextTime[0])
	{
		switch (m_iMYObjectCount)/* 왼아래		오아래		왼위	오위*/
		{
		case 0:
			XMStoreFloat4(&m_vNextMoveRoation[0], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-80.f, 0.f, 0.f, 0.f), m_fNextTime[0]));
			break;																							
		case 1:																								
			XMStoreFloat4(&m_vNextMoveRoation[0], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-84.f, 0.f, 0.f, 0.f), m_fNextTime[0]));
			break;																							
		case 2:																								
			XMStoreFloat4(&m_vNextMoveRoation[0], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-78.f, 0.f, 0.f, 0.f), m_fNextTime[0]));
			break;																							
		case 3:																								
			XMStoreFloat4(&m_vNextMoveRoation[0], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-80.f, 0.f, 0.f, 0.f), m_fNextTime[0]));
			break;
		}
		NextCanbinetCountMove(0);
	}
	else
	{
		m_bNextMove[0] = false;
		m_bNextMove[1] = true;
	}

	m_NextAccumulationTime[0] = m_NextAccumulationTime[0]*1.1f;
}

void CMoveCabinet::NextDownMove()
{
	m_fNextTime[1] += 0.3f + m_NextAccumulationTime[1];
	if (1.0f >= m_fNextTime[1])
	{
		switch (m_iMYObjectCount)/* 왼아래		오아래		왼위	오위*/
		{
		case 0:
			XMStoreFloat4(&m_vNextMoveRoation[1], XMVectorLerp(XMVectorSet(-80.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[1]));
			break;
		case 1:
			XMStoreFloat4(&m_vNextMoveRoation[1], XMVectorLerp(XMVectorSet(-84.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[1]));
			break;
		case 2:
			XMStoreFloat4(&m_vNextMoveRoation[1], XMVectorLerp(XMVectorSet(-78.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[1]));
			break;
		case 3:
			XMStoreFloat4(&m_vNextMoveRoation[1], XMVectorLerp(XMVectorSet(-80.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[1]));
			break;
		}
		NextCanbinetCountMove(1);
	}
	else
	{
		m_bNextMove[1] = false;
		m_bNextMove[2] = true;
	}

	m_NextAccumulationTime[1] = m_NextAccumulationTime[1] * 1.1f;
}

void CMoveCabinet::LastUpMove()
{
	m_fNextTime[2] += 0.2f + m_NextAccumulationTime[2];
	if (1.0f >= m_fNextTime[2])
	{
		switch (m_iMYObjectCount)/* 왼아래		오아래		왼위	오위*/
		{
		case 0:
			XMStoreFloat4(&m_vNextMoveRoation[2], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-84.f, 0.f, 0.f, 0.f), m_fNextTime[2]));
			break;
		case 1:
			XMStoreFloat4(&m_vNextMoveRoation[2], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-86.f, 0.f, 0.f, 0.f), m_fNextTime[2]));
			break;
		case 2:
			XMStoreFloat4(&m_vNextMoveRoation[2], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-82.f, 0.f, 0.f, 0.f), m_fNextTime[2]));
			break;
		case 3:
			XMStoreFloat4(&m_vNextMoveRoation[2], XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-84.f, 0.f, 0.f, 0.f), m_fNextTime[2]));
			break;
		}
		NextCanbinetCountMove(2);
	}
	else
	{
		m_bNextMove[2] = false;
		m_bNextMove[3] = true;
	}

	m_NextAccumulationTime[2] = m_NextAccumulationTime[0] * 1.1f;
}

void CMoveCabinet::LastDownMove()
{
	m_fNextTime[3] += 0.3f + m_NextAccumulationTime[3];
	if (1.0f >= m_fNextTime[3])
	{
		switch (m_iMYObjectCount)/* 왼아래		오아래		왼위	오위*/
		{
		case 0:
			XMStoreFloat4(&m_vNextMoveRoation[3], XMVectorLerp(XMVectorSet(-84.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[3]));
			break;
		case 1:
			XMStoreFloat4(&m_vNextMoveRoation[3], XMVectorLerp(XMVectorSet(-86.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[3]));
			break;
		case 2:
			XMStoreFloat4(&m_vNextMoveRoation[3], XMVectorLerp(XMVectorSet(-82.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[3]));
			break;
		case 3:
			XMStoreFloat4(&m_vNextMoveRoation[3], XMVectorLerp(XMVectorSet(-84.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), m_fNextTime[3]));
			break;
		}
		NextCanbinetCountMove(3);
	}
	else
	{
		m_bNextMove[3] = false;
	}

	m_NextAccumulationTime[3] = m_NextAccumulationTime[3] * 1.1f;
}

void CMoveCabinet::NextCanbinetCountMove(_uint RoationCount)
{
	switch (m_iMYObjectCount)
	{
	case 0:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, 0.f, XMConvertToRadians(m_vNextMoveRoation[RoationCount].x)));
		break;
	case 1:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, XMConvertToRadians(180.f), XMConvertToRadians(m_vNextMoveRoation[RoationCount].x)));
		break;
	case 2:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(XMConvertToRadians(m_vNextMoveRoation[RoationCount].x), XMConvertToRadians(257.f), 0.f));
		break;
	case 3:
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(XMConvertToRadians(m_vNextMoveRoation[RoationCount].x), XMConvertToRadians(90.f), 0.f));
		break;
	}
}

void CMoveCabinet::Check()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing('Z'))
	{
		if (0 == m_iMYObjectCount)
		{
			Time += 0.001f + AccumulationTime;
			if (1.01f >= Time)
			{
				XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), Time));
				m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, 0.f, XMConvertToRadians(m_MoveRoation.x)));
			}
			AccumulationTime = AccumulationTime *1.1f;
		}
	}
	if (pGameInstance->Key_Pressing('X'))
	{
		if (1 == m_iMYObjectCount)
		{
			Time += 0.001f + AccumulationTime;
			if (1.01f >= Time)
			{
				XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), Time));
				m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, XMConvertToRadians(180.f), XMConvertToRadians(m_MoveRoation.x)));
			}
			AccumulationTime = AccumulationTime *1.1f;
		}
	}
	if (pGameInstance->Key_Pressing('C'))
	{
		if (2 == m_iMYObjectCount)
		{
			Time += 0.001f + AccumulationTime;
			if (1.01f >= Time)
			{
				XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), Time));
				m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(XMConvertToRadians(m_MoveRoation.x), XMConvertToRadians(257.f), 0.f));
			}
			AccumulationTime = AccumulationTime *1.1f;
		}
	}
	if (pGameInstance->Key_Pressing('V'))
	{
		if (3 == m_iMYObjectCount)
		{
			Time += 0.001f + AccumulationTime;
			if (1.01f >= Time)
			{
				XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), Time));
				m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(XMConvertToRadians(m_MoveRoation.x), XMConvertToRadians(90.f), 0.f));
			}
			AccumulationTime = AccumulationTime *1.1f;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMoveCabinet::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	m_vColPos = vColPos;
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint LayerSize = pGameInstance->Get_LayerSize(m_iCurrentScene, TEXT("Layer_MoveCabinet"));
	m_iMYObjectCount = LayerSize;
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CMoveCabinet::SetUp_ConstantTable()
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
	if (GetKeyState('Y') & 0x8000)
	{
		m_ArrKey[0] -= 0.5f;
	}
	if (GetKeyState('U') & 0x8000)
	{
		m_ArrKey[0] += 0.5f;
	}
	if (GetKeyState('H') & 0x8000)
	{
		m_ArrKey[1] -= 0.5f;
	}
	if (GetKeyState('J') & 0x8000)
	{
		m_ArrKey[1] += 0.5f;
	}
	if (GetKeyState('N') & 0x8000)
	{
		m_ArrKey[2] -= 0.5f;
	}
	if (GetKeyState('M') & 0x8000)
	{
		m_ArrKey[2] += 0.5f;
	}
	_bool		bMotionBlur = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR);
	_bool		bShadow = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_SHADOW);
	_bool		bRimlight = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_RIMLIGHT);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	_bool		bCheck = true;
	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float3(0.012f, -1.f, 0.001f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bCheck, sizeof(_bool))))
	//	return E_FAIL;
	if (true == m_bIsRenderingShadow)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
			return E_FAIL;
	}
	//if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
	//	return E_FAIL;
	_matrix PlayerWorld = XMMatrixIdentity();
	PlayerWorld.r[3] = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	_vector vPos = XMVector3TransformCoord(XMVectorSet(m_ArrKey[0], m_ArrKey[1], m_ArrKey[2], 1.f), PlayerWorld);
	_float3 vPosFloat;
	XMStoreFloat3(&vPosFloat, vPos);

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(_float3(0.012f, -1.f, 0.001f), _float3(m_ArrKey[0] - 4.338f, m_ArrKey[1] + 54.306f, m_ArrKey[2] + 43.769f));

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

	if (true == m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR))
	{
		m_pModelCom->Set_RawValue("g_OldWorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldWolrd)), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_OldViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldView)), sizeof(_float4x4));

	}
	m_OldWolrd = m_pTransformCom->Get_WorldFloat4x4();
	XMStoreFloat4x4(&m_OldView, pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMoveCabinet * CMoveCabinet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoveCabinet*	pInstance = new CMoveCabinet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CMoveCabinet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMoveCabinet::Clone(void * pArg, _uint iCurrentScene)
{
	CMoveCabinet*	pInstance = new CMoveCabinet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg,iCurrentScene)))
	{
		MSGBOX("Failed to Created : CMoveCabinet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoveCabinet::Free()
{
	__super::Free();

	/*if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();*/

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
