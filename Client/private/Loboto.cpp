#include "stdafx.h"
#include "Loboto.h"
#include "GameInstance.h"
#include "MoveCabinet.h"
#include "OfficeWalls.h"
#include "DemoRect.h"
CLoboto::CLoboto(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CLoboto::CLoboto(const CLoboto & rhs)
	: CGameObject(rhs)
{
}

void CLoboto::SetUpPointNum(_int iNum)
{
	if (m_iTriggerCnt >= iNum)
		return;

	if (5 == iNum)
	{
		SetUpForDistanceEvent();
	}

	m_iTriggerCnt = iNum;
	m_lfLerpTimeAcc = 0.0;
	if (m_iTriggerCnt != -1)
	{
		m_iStartPointNum = m_iArrVecNum[m_iTriggerCnt];
		m_iEndPointNum = m_iArrVecNum[m_iTriggerCnt + 1];
	}

	SetAnimationByMoveStartPoint();
	//m_isMove = true;
}

void CLoboto::Set_Move(_bool isMove)
{
	m_isMove = isMove;
}

HRESULT CLoboto::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoboto::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	m_StrLayerName = TEXT("Layer_Loboto");
	m_eLayerType = LayerType::LAYERTYPE_NPC;

	_float3 vColSize = { 0.3f,0.5f,0.3f };
	_float3 vColPos = { 0,0.f,0 };
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = 1.f;
	_float3 vObjScale = { 1,1,1 };


	if (FAILED(SetUp_Component(vColSize, vColPos, vObjScale)))
		return E_FAIL;

	m_pModelCom->Set_Animation(1);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(8.96f, 0.12f, 16.53f, 1.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	SetUp_MovePoints();
	SetUpPointNum();
	m_isMove = false;
	return S_OK;
}

_int CLoboto::Tick(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(VK_RIGHT))
	{
		m_iCurrentMovePoint = 16;
		if (m_iCurrentMovePoint + 1 < m_vecMovePoints.size())
			m_lfLerpTimeMax = m_vecMovePoints[m_iCurrentMovePoint + 1].w;
		m_lfLerpTimeAcc = 0.0;

		if (m_iCurrentMovePoint == 17)
			m_isLerpForWall = true;

		m_isMove = true;
	}

	RELEASE_INSTANCE(CGameInstance);
	if (m_iCurrentMovePoint  >= 23)
	{
		m_isMove = false;
		m_isDestroy = true;
		return 0;
	}
	_double fLerpSpeed = 1.0;
	if (13 >= fLerpSpeed)
		fLerpSpeed = 1.5;
	if (m_isMove)
		MoveToPoint(TimeDelta*fLerpSpeed);

	SetUpAnimation();
	if (13 >= m_iCurrentMovePoint)
		m_dAnimSpeed = 0.8;
	else
		m_dAnimSpeed = 0.3;

	m_pModelCom->Lerp_Update(TimeDelta*m_dAnimSpeed);
	DemoRect();
	Destroy_RectEvent(TimeDelta);
	return 0;
}

_int CLoboto::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CLoboto::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (m_isRenderVP)
		m_pModelCom->Render(m_iRenderPass);
	else
		m_pModelCom->Render(m_iRenderPass, 4);

	return S_OK;
}


HRESULT CLoboto::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Loboto"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 4.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoboto::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoboto::SetUp_MovePoints()
{
	m_vecMovePoints.reserve(23);

	m_vecMovePoints.emplace_back(_float4(8.96f, 0.12f, 16.53f, 7.f));	// 0 Start
	m_vecMovePoints.emplace_back(_float4(9.22f, 0.15f, 46.51f, 1.f));	// 0 End, 1 Start
	m_vecMovePoints.emplace_back(_float4(7.65f, 0.10f, 52.19f, 1.f));	// 1 - 1 [2]
	m_vecMovePoints.emplace_back(_float4(3.55f, 0.15f, 54.69f, 1.f));	// 1 - 2 [3]
	m_vecMovePoints.emplace_back(_float4(-8.45f, 0.10f, 62.32f, 1.f));	// 1 End, 2 Start
	m_vecMovePoints.emplace_back(_float4(-13.17f, 0.10f, 64.23f, 1.f));	// 2 - 1 [5]
	m_vecMovePoints.emplace_back(_float4(-20.95f, 0.12f, 66.45f, 1.f));	// 2 - 2 [6]
	m_vecMovePoints.emplace_back(_float4(-25.13f, 0.15f, 71.77f, 1.f));	// 2 - 3 [7]
	m_vecMovePoints.emplace_back(_float4(-79.85f, 0.10f, 71.24f, 1.f));	// 2 End, 3 Start
	m_vecMovePoints.emplace_back(_float4(-82.10f, 0.10f, 82.00f, 1.f));	// 3 End, 4 Start
	m_vecMovePoints.emplace_back(_float4(-82.10f, 0.10f, 107.50f, 1.f)); // 4 - 1 [10]
	m_vecMovePoints.emplace_back(_float4(-115.09f, 0.132f, 107.241f, 1.f));// 4 End [11]
	m_vecMovePoints.emplace_back(_float4(0.f, 0.f, 0.f, 1.f));// 6 Start [12]
	m_vecMovePoints.emplace_back(_float4(-152.98f, 0.12f, 108.13f, 1.f));// 6 End 7 Start [13]

	m_vecMovePoints.emplace_back(_float4(-155.7f, -0.03f, 107.63f, 1.0f));// 7 - 1 [14]
	m_vecMovePoints.emplace_back(_float4(-156.2f, -0.156f, 109.4f, 1.0f));// 7 - 2 [15]
	m_vecMovePoints.emplace_back(_float4(-157.8f, -0.156f, 110.13f, 1.0f));// 7 - 3 [16]
	m_vecMovePoints.emplace_back(_float4(-160.f, -0.156f, 110.124f, 1.0f));// 7 - 4 [17]
	m_vecMovePoints.emplace_back(_float4(-161.f, 1.183f, 110.124f, 1.0f));// 7 - 5 [18]
	m_vecMovePoints.emplace_back(_float4(-161.f, 3.63f, 108.36f, 1.0f));// 7 - 6 [19]
	m_vecMovePoints.emplace_back(_float4(-161.f, 5.28f, 107.4f, 1.0f));// 7 - 7 [20]
	m_vecMovePoints.emplace_back(_float4(-161.f, 7.22f, 106.01f, 1.0f));// 7 [21]
	m_vecMovePoints.emplace_back(_float4(-161.f, 9.3f, 106.86f, 1.0f));// 7 [22]
	m_vecMovePoints.emplace_back(_float4(-161.f, 11.34f, 109.27f, 1.0f));// 7 - End [23]

	return S_OK;
}

void CLoboto::MoveToPoint(_double TimeDelta)
{
	//m_iCurrentMovePoint
	if (m_iTriggerCnt + 2 > LOBO_MAXMOVEARR)
	{
		m_isMove = false;
		m_isDestroy = true;
		return;
	}
	//m_iCurrentMovePoint

	// Distance Event를 해줄 TriggerCnt값이 4
	// 만약 TriggerEvent일때
	if (m_iTriggerCnt != 4)
	{
		if (m_iCurrentMovePoint >= m_iEndPointNum)
		{
			SetAnimationByMoveEndPoint();
			m_isMove = false;
			if (m_iCurrentMovePoint == m_vecMovePoints.size() - 1)
				m_isDestroy = true;
			return;
		}

		m_lfLerpTimeAcc += TimeDelta;
		_float fRatio = (_float)m_lfLerpTimeAcc / (_float)m_lfLerpTimeMax;
		_vector vStart = XMLoadFloat4(&m_vecMovePoints[m_iCurrentMovePoint]);
		_vector vEnd = XMLoadFloat4(&m_vecMovePoints[m_iCurrentMovePoint + 1]);
		vStart = XMVectorSetW(vStart, 1.f);
		vEnd = XMVectorSetW(vEnd, 1.f);

		_vector vPos = XMQuaternionSlerp(vStart, vEnd, fRatio);
		_vector vDir = vPos - vStart;
		if (m_iCurrentMovePoint == 9)
		{
			_tchar szBuff[260] = TEXT("");
			swprintf_s(szBuff, 260, TEXT("fRatio:  %.2f\n"), fRatio);
			OutputDebugStringW(szBuff);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (fRatio >= 0.3f &&fRatio <= 0.3f + TimeDelta)
			{
				static_cast<CMoveCabinet*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveCabinet"), 0))->Set_FallStart();
			}
			if (fRatio >= 0.315f &&fRatio <= 0.315f + TimeDelta)
			{
				static_cast<CMoveCabinet*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveCabinet"), 1))->Set_FallStart();
			}
			if (fRatio >= 0.5f &&fRatio <= 0.5f + TimeDelta)
			{
				static_cast<CMoveCabinet*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveCabinet"), 2))->Set_FallStart();
			}
			if (fRatio >= 0.6f &&fRatio <= 0.6f + TimeDelta)
			{
				static_cast<CMoveCabinet*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_MoveCabinet"), 3))->Set_FallStart();
			}
			RELEASE_INSTANCE(CGameInstance);
		}
		if (m_iCurrentMovePoint < 17)
		{
			vDir = XMVectorSetY(vDir, 0.f);
		}
		else if (m_iCurrentMovePoint >= 17)
			vDir = XMVectorSetX(vDir, 0.f);


		if (m_iTriggerCnt != 5)
		{
			if (m_iCurrentMovePoint >= 17)
			{
				if (!m_isLerpForWall)
				{
					m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vDir));
					_vector Look = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					_vector Up = m_pTransformCom->Get_State(CTransform::STATE_UP);
					_vector Right = XMVector3Cross(Up, Look);
					m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(Right));
				}
				else
				{
					m_pTransformCom->RotationPerSec(XMVectorSet(0.f, 0.f, 1.f, 0.f), -TimeDelta);

					if (fRatio >= 1.f)
					{
						m_isLerpForWall = false;
					}
				}

			}
			else
			{
				m_pTransformCom->Set_MyLook(XMVector3Normalize(vDir));
			}
		}
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		if (fRatio >= 1.f)
		{
			if (m_iCurrentMovePoint + 1 < m_vecMovePoints.size())
				m_lfLerpTimeMax = m_vecMovePoints[m_iCurrentMovePoint + 1].w;
			++m_iCurrentMovePoint;
			m_lfLerpTimeAcc = 0.0;
			if (m_iCurrentMovePoint == 17)
				m_isLerpForWall = true;
		}
	}
	else // 만약 DistanceEvent일때
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);

		_float fDistance = GET_VECTORLENGTH(pPlayerTransform->Get_Position() - m_pTransformCom->Get_Position());
		_float fMoveDist = 0.f;
		if (fDistance < 10.f)
		{
			fMoveDist = 10.f - fDistance;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position() - XMVectorSet(1.f, 0.f, 0.f, 0.f)*fMoveDist);
		}
		//if (pGameInstance->Key_Down(VK_HOME))
		//{
		//	_tchar szBuff[260] = TEXT("");
		//	swprintf_s(szBuff, 260, TEXT("Distance: %.2f \n"), fDistance);
		//	OutputDebugString(szBuff);
		//}
		if (!m_isUpdateDistanceAnim)
		{
			m_pModelCom->Set_IsAnimLefp(true);
			UpdateAnimState(LA_DISTANCE);
			m_isUpdateDistanceAnim = true;

		}
		RELEASE_INSTANCE(CGameInstance);
	}

}

void CLoboto::SetUpForDistanceEvent()
{
	m_lfLerpTimeAcc = 0.0;
	m_vecMovePoints[12] = STOREFLOAT4(m_pTransformCom->Get_Position());
	m_iCurrentMovePoint = 12;
	m_lfLerpTimeMax = m_vecMovePoints[m_iCurrentMovePoint].w;
}

void CLoboto::SetAnimationByMoveStartPoint()
{
	switch (m_iCurrentMovePoint)
	{
	case 1:
	case 4:
	case 8:
		UpdateAnimState(LA_WALK);
		break;
	case 9:
	{
		UpdateAnimState(LA_TALK);
		break;
	}
	case 13:
		UpdateAnimState(LA_LAUGH);
		break;
	}
}

void CLoboto::SetAnimationByMoveEndPoint()
{
	switch (m_iCurrentMovePoint)
	{
	case 1:
		UpdateAnimState(LA_IDLE);
		break;
	case 4:
		UpdateAnimState(LA_LOOKAROUND);
		break;
	case 8:
		UpdateAnimState(LA_IDLE);
		break;
	case 9:
	{
		m_isRenderVP = false;
		m_pTransformCom->RotationByRadian(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
		UpdateAnimState(LA_IDLE);
		break;
	}
	case 11:
	{
		m_pTransformCom->RotationByRadian(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));
		UpdateAnimState(LA_DISTANCE_IDLE);
		break;
	}
	}
}

void CLoboto::SetUpAnimation()
{
	switch (m_iCurAnimNum)
	{
	case CLoboto::LA_IDLE:
		m_pModelCom->Set_Animation(1);
		break;
	case CLoboto::LA_WALK:
		m_pModelCom->Set_Animation(0);
		break;
	case CLoboto::LA_LOOKAROUND:
	{
		if (m_pModelCom->Get_IsAnimFinishied(2) || m_pModelCom->Get_IsAnimFinishied(3))
		{
			m_pModelCom->Set_IsAnimLefp(true);
			m_isLookAroundLeft = !m_isLookAroundLeft;
		}

		if (!m_isLookAroundLeft)
		{
			m_pModelCom->Set_Animation(2);
		}
		else
			m_pModelCom->Set_Animation(3);
		break;
	}
	case LA_TALK:
	{
		m_pModelCom->Set_Animation(4);
		if (m_pModelCom->Get_IsAnimFinishied(4))
		{
			m_pModelCom->Set_IsAnimLefp(true);
			m_pModelCom->Set_Animation(6);
			UpdateAnimState(LA_RUN);
			m_isMove = true;
		}
		break;
	}
	case LA_RUN:
	{
		m_pModelCom->Set_Animation(6);
		break;
	}
	case CLoboto::LA_DISTANCE_IDLE:
	{
		m_pModelCom->Set_Animation(7);
		break;
	}
	case CLoboto::LA_DISTANCE:
	{
		m_pModelCom->Set_Animation(8);
		if (m_pModelCom->Get_IsAnimFinishied(8))
		{
			m_pModelCom->Set_IsAnimLefp(true);
			m_pModelCom->Set_Animation(7);
			UpdateAnimState(LA_MOCKING);
		}
		break;
	}
	case LA_MOCKING:
	{
		m_pModelCom->Set_Animation(9);
		break;
	}
	case LA_LAUGH:
	{
		m_pModelCom->Set_Animation(10);
		if (m_pModelCom->Get_IsAnimFinishied(10))
		{
			m_pModelCom->Set_IsAnimLefp(true);
			m_pModelCom->Set_Animation(1);
			UpdateAnimState(LA_WALK);
			m_isMove = true;
		}
		break;
	}
	default:
		break;
	}
}

void CLoboto::UpdateAnimState(_uint iNextAnim)
{
	m_iNextAnimNum = (LOBOANIM)iNextAnim;

	if (m_iCurAnimNum != m_iNextAnimNum)
	{
		m_pModelCom->Set_IsAnimLefp(true);
		m_iCurAnimNum = m_iNextAnimNum;
	}
}

void CLoboto::DemoRect()
{
	if (11 <= m_iCurrentMovePoint && 13 >= m_iCurrentMovePoint)
	{
		//m_vBackUpPos = m_vPos = _float3(-116.09f, 3.002f, 107.241f);
		//m_vBackUpPos = m_vPos = _float3(-116.09f, 0.132f, 103.391f);
		//m_vBackUpPos = m_vPos = _float3(-116.09f, 0.132f, 110.931f);

		m_vecMovePoints.emplace_back(_float4(-115.09f, 0.132f, 107.241f, 1.f));// 4 End [11]

		_float3 vPos;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		COfficeWalls* pOfficeWalls_Left = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 0));
		if (nullptr == pOfficeWalls_Left)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		XMStoreFloat3(&vPos, m_pTransformCom->Get_Position());
		vPos.x += -1.f;
		vPos.z += -3.85f;
		pOfficeWalls_Left->Set_Pos(vPos);

		COfficeWalls* pOfficeWalls_Right = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 1));
		if (nullptr == pOfficeWalls_Right)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		XMStoreFloat3(&vPos, m_pTransformCom->Get_Position());
		vPos.x += -1.f;
		vPos.z += 3.69f;
		pOfficeWalls_Right->Set_Pos(vPos);

		CDemoRect* pDemoRect = dynamic_cast<CDemoRect*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_DemoRect"), 0));
		if (nullptr == pDemoRect)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		XMStoreFloat3(&vPos, m_pTransformCom->Get_Position());
		vPos.x += -1.f;
		vPos.y += 2.87f;
		pDemoRect->Set_Pos(vPos);

		RELEASE_INSTANCE(CGameInstance);

		if (false == m_bIsDestroyDemoRect && (13 == m_iCurrentMovePoint))
		{
			m_dAnimSpeed = 2.0;
			m_bActiveDestroyDemoRect = true;
		}
		
	}
}

void CLoboto::Destroy_RectEvent(_double TimeDelta)
{
	if (false == m_bIsDestroyDemoRect)
	{
		if (true == m_bActiveDestroyDemoRect)
		{
			m_dDestoryTimeAcc += TimeDelta;

			if (0.05f < m_dDestoryTimeAcc)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				COfficeWalls* pOfficeWalls_Left = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 0));
				if (nullptr == pOfficeWalls_Left)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				COfficeWalls* pOfficeWalls_Right = dynamic_cast<COfficeWalls*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_OfficeWalls"), 1));
				if (nullptr == pOfficeWalls_Right)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				CDemoRect* pDemoRect = dynamic_cast<CDemoRect*>(pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_DemoRect"), 0));
				if (nullptr == pDemoRect)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				pOfficeWalls_Left->Set_IsDestroy(true);
				pOfficeWalls_Right->Set_IsDestroy(true);
				pDemoRect->Set_IsDestroy(true);

				RELEASE_INSTANCE(CGameInstance);

				m_dDestoryTimeAcc = 0.0;
				m_bActiveDestroyDemoRect = false;
				m_bIsDestroyDemoRect = true;
			}
		}
	}
}

void CLoboto::Set_MoveStartLoboto(_bool bMove)
{
	m_isMove = bMove;

	m_iCurAnimNum = LOBOANIM::LA_WALK;

	m_pModelCom->Set_Animation(0);
}

CLoboto * CLoboto::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLoboto*	pInstance = new CLoboto(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CLoboto");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoboto::Clone(void * pArg, _uint iCurrentScene)
{
	CLoboto*	pInstance = new CLoboto(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CLoboto");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoboto::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
