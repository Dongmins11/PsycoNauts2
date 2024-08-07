#include "stdafx.h"
#include "..\public\Static_Mesh.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player_RightHand.h"
#include "Right_Hand_Anim.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "Enemy.h"
CStatic_Mesh::CStatic_Mesh(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CStatic_Mesh::CStatic_Mesh(const CStatic_Mesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStatic_Mesh::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CStatic_Mesh::NativeConstruct(void * pArg)
{

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = (LayerType)objinfo.iPX_Index;

	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = objinfo.fFrustumRange;
	_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);
	m_vBackUpScale = vObjScale;
	if (FAILED(SetUp_Component(objinfo.fCollider_Extent, objinfo.fCollider_Center, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;

	if (m_eLayerType == LayerType::LAYERTYPE_PICK)
	{
		/* 상자 의자 */
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box1"))
			|| !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box3")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box4")))
		{
			/* 상자 조잉 */
			if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
				m_iCurrentScene, TEXT("Layer_Env_PickingObject"),
				SCENE_STATIC, TEXT("Effect_Env_PickingObject"), &m_pEffect)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			Safe_AddRef(m_pEffect);

			if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, false, m_pTransformCom, TEXT("Layer_Env_PickingObject"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			pGameInstance->Setting_Effect_TransformPos(m_iCurrentScene, TEXT("Layer_Env_PickingObject"), TEXT("Com_Option"), CTransform::STATE_UP);
		}
		else
		{
			if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
				m_iCurrentScene, TEXT("Layer_Env_PickingObject"),
				SCENE_STATIC, TEXT("Effect_Env_PickingObject"), &m_pEffect)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			Safe_AddRef(m_pEffect);

			if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, false, m_pTransformCom, TEXT("Layer_Env_PickingObject"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			pGameInstance->Setting_Effect_TransformPos(m_iCurrentScene, TEXT("Layer_Env_PickingObject"), TEXT("Com_Option"), CTransform::STATE_UP);
		}


		RELEASE_INSTANCE(CGameInstance);
	}


	if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Floor1")))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_uint iSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Compt_Floor"));
		if (23 == iSize)
		{
			m_bReflect_Ramlt_Up_Dest = true;
			m_fReflect_Height = 0.67f;

			m_iRenderPass = 11;
			m_iRenderPassBackUp = m_iRenderPass;
		}
		RELEASE_INSTANCE(CGameInstance);

	}
	if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Stands0")))
	{
		m_bReflect_Ramlt_Down_Sour = true;
		m_fReflect_Height = -0.02f;
	}
	if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Stationlcon_Back0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_StationIcon_Back")) ||
		!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_StationIcon_Back")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_StationIcon_Back0")))
	{

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_uint iSize = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Compt_Stationlcon_Back"));
		if (0 == iSize)
		{
			m_bReflect_Ramlt_Down_Dest = true;
			m_fReflect_Height = -0.02f;

			m_iRenderPass = 11;
			m_iRenderPassBackUp = m_iRenderPass;
		}
		RELEASE_INSTANCE(CGameInstance);

	}
	m_bIsSetUpReflect = false;
	m_bIsSetUpShadow = false;
	return S_OK;
}

_int CStatic_Mesh::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	SetUp_IsShadow();
	SetUp_IsReflect();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


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
		//Stage_RenderBranch(iPlayerCurlingIndex);
	}
	else
	{
		if (m_isRender && m_pColliderCom)
		{
			m_pColliderCom->Remove_Actor();
		}
		m_isRender = false;
	}

	RELEASE_INSTANCE(CGameInstance);

	if (!m_isRender)
	{
		return 0;
	}

	if (m_pColliderCom)
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
		if (true == m_isUpdateCollide)
			m_pColliderCom->Update_TransformPhysX();
	}

	if (true == m_bIsFluctuate)
		Fluctuate(TimeDelta);

	//if (GetKeyState(VK_F7) & 0x8000)
	//{
	//	m_bIsDissolve = false;
	//	m_fDissolveTime = 0.0;
	//}
	//if (GetKeyState(VK_F8) & 0x8000)
	//{
	//	m_bIsDissolve = true;
	//}
	//if (true == m_bIsDissolve)
	//{
	//	m_isUpdateCollide = false;
	//	Dissolve_Scaling(TimeDelta);
	//	m_fDissolveTime += TimeDelta*m_dDissolveSpeed;
	//	if (1.5f < m_fDissolveTime)
	//	{
	//		m_fDissolveTime = 0.f;
	//		m_bIsDissolve = false;
	//		m_isDestroy = true;
	//		m_isRender = false;
	//	}
	//}
	//m_bBackUpDissolve = m_bIsDissolve;
	return 0;
}

_int CStatic_Mesh::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_isEffectCreate)
	{
		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
		//m_bIsDissolve = true;
		m_DeleteTimeAcc += TimeDelta;
		if (0.5 < m_DeleteTimeAcc)
		{
			m_isDestroy = true;
		}
	}

	if (m_isDestroy)
	{
		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
	}

	if (!m_isRender)
		return 0;

	if (m_pColliderCom && m_isUpdateCollide)
	{
		m_pColliderCom->Update_Transform();
	}
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//if (SCENE_STAGE03 == pGameInstance->Get_CurrentSceneIndex())
	//{
	//	if (true == m_bReflect_Ramlt_Down_Dest)
	//		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT_DEST, this);

	//	else
	//	{
	//		if(true ==m_bIsRenderingShadow)
	//			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	//		else
	//		{
	//			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	//		}
	//	}
	//}
	//else
	//{
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	//}

	if (true == m_bReflect_Ramlt_Down_Dest)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT_DEST, this);
	else
	{
		if (false == m_bIsRenderingShadow)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		else
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		}
	}

	if (true == m_bReflect_Ramlt_Down_Sour || true == m_bReflect_Sour)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this);

	//RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CStatic_Mesh::Render()
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

void CStatic_Mesh::Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed)
{
	m_bIsDissolve = bIsDissolve;
	m_dDissolveSpeed = dSpeed;
	m_iTextureIndex = iTextureIndex;
	m_fDissolveStart = fStart;
	m_fDissolveScaleSpeed = fScaleSpeed;
	if (false == m_bBackUpDissolve && true == m_bIsDissolve)
		m_vDissolveScale = m_vBackUpScale;

}

void CStatic_Mesh::Fluctuate(_double TimeDelta)
{
	m_fFluctuateSize += m_fFluctuateInterval;
	if (0 == m_iFluctuateStep)
	{
		if (1.5f < m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = -0.2f;
		}

	}
	else if (1 == m_iFluctuateStep)
	{
		if (0.7f > m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = 0.2f;
		}
	}
	else if (2 == m_iFluctuateStep)
	{
		if (1.3f < m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = -0.2f;
		}
	}
	else if (3 == m_iFluctuateStep)
	{
		if (0.85f > m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = 0.1f;
		}
	}
	else if (4 == m_iFluctuateStep)
	{
		if (1.15f < m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = -0.05f;
		}
	}
	else if (5 == m_iFluctuateStep)
	{
		if (0.95f > m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = 0.05f;
		}
	}
	else if (6 == m_iFluctuateStep)
	{
		m_fFluctuateSize = 1.f;
		m_fFluctuateInterval = 0.2f;
		m_bIsFluctuate = false;
		m_iFluctuateStep = 0;
	}
	m_pTransformCom->Scaling(XMLoadFloat3(&_float3(m_fFluctuateSize, m_fFluctuateSize, m_fFluctuateSize)));

}

void CStatic_Mesh::Dissolve_Scaling(_double TimeDelta)
{
	m_vDissolveScale.x += TimeDelta*m_fDissolveScaleSpeed;
	m_vDissolveScale.y += TimeDelta*m_fDissolveScaleSpeed;
	m_vDissolveScale.z += TimeDelta*m_fDissolveScaleSpeed;

	m_pTransformCom->Scaling(XMLoadFloat3(&m_vDissolveScale));

}

void CStatic_Mesh::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	m_pColliderCom->Add_Force(vDir, eMode);

}

void CStatic_Mesh::Setting_Throwing(_fvector vDir)
{
	m_isColController = false;
	m_pColliderCom->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
	m_isThrowing = true;
}

void CStatic_Mesh::OnCollisionEnter(CCollisionObject & collision)
{
	if (!m_isMove)
		return;

	if (m_isEffectCreate)
		m_isDestroy = true;

	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLANE)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pRightHandObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_RightHand", 0);
		if (nullptr == pRightHandObject)
		{
			MSGBOX("Failed to Finder RightHand : ClassName CStatic_Mesh : Funtion OnCollisionEnter");
			Safe_Release(pGameInstance);
			return;
		}

		if (CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB != dynamic_cast<CPlayer_RightHand*>(pRightHandObject)->Get_HandState() &&
			!m_isColController)
		{
			if (!m_isEffectCreate)
			{
				m_isEffectCreate = true;

				if (FAILED(Create_Effect()))
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
		}

		Safe_Release(pGameInstance);
	}

}

void CStatic_Mesh::OnCollisionStay(CCollisionObject & collision)
{
}

void CStatic_Mesh::OnCollisionExit(CCollisionObject & collision)
{
}

void CStatic_Mesh::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_TRIGGER == collision.Get_Object()->Get_ObjectLayerType())
	{
		OutputDebugString(TEXT("렉트 이벤트 트리거 충돌!!!!"));
	}
}

void CStatic_Mesh::SettingActorActive()
{
	if (!m_pColliderCom)
		return;

	if (m_isActorActive)
		m_pColliderCom->WakeUp();
	else
		m_pColliderCom->Sleep();

}

_bool CStatic_Mesh::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CStatic_Mesh::Stage_RenderBranch(_int iPlayerCurlingIndex)
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
	case SCENE_STAGE01:
	{
		if (iPlayerCurlingIndex == 0) /* 0번 것만 출력 할것 */
		{
			if (iPlayerCurlingIndex == m_iCurlingIndex/* || NextIndex == m_iCurlingIndex*/)
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
		}
		else if (iPlayerCurlingIndex == 1)/* 0 ~ 1 출력 할것 */
		{
			_uint  OldIndex = iPlayerCurlingIndex - 1;
			if (iPlayerCurlingIndex == m_iCurlingIndex || OldIndex == m_iCurlingIndex)
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
		}
		else if (iPlayerCurlingIndex == 2)/* 1 출력 할것 */
		{
			_uint curIndex = iPlayerCurlingIndex - 1;
			if (curIndex == m_iCurlingIndex)
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
		}
		break;
	}
	case SCENE_STAGE02:
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
	case SCENE_STAGE03:
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
	case SCENE_STAGE04:
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
	case SCENE_STAGE05:
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

void CStatic_Mesh::SetUp_IsShadow()
{
	if (false == m_bIsSetUpShadow)
	{
		m_bIsSetUpShadow = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iSceneIndex = pGameInstance->Get_CurrentSceneIndex();


		if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_TileFloor0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_TileFloor1")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_TileFloor2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_TileFloor3")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_TileFloor4")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Ceiling0")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Light0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Light1")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Light2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Light3")) ||
			/*!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeFloor0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeFloor1"))||*/
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall1")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall3")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall4")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall5")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall6")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall7")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall8")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall9")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall10")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall11")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall12")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Projector")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Projector0")))
		{
			m_bIsRenderingShadow = false;
		}
		else
		{
			m_bIsRenderingShadow = true;
		}

		//if (SCENE_STAGE03 == iSceneIndex)
		//	m_bIsRenderingShadow = false;
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CStatic_Mesh::SetUp_IsReflect()
{
	if (false == m_bIsSetUpReflect)
	{
		m_bIsSetUpReflect = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_uint iSceneIndex = pGameInstance->Get_CurrentSceneIndex();
		//if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Floor3")))
		//{
		//	m_bReflect_Bath_Sour = true;
		//	m_fReflect_Height = 15.f;
		//}
		//if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Floor0")))
		//{
		//	m_bReflect_Bath_Dest = true;
		//	m_iRenderPass = 9;
		//	m_iRenderPassBackUp = m_iRenderPass;
		//	m_fReflect_Height = 15.f;

		//}
		//if (SCENE_STAGE03 == iSceneIndex)
		//{

		//}
		//else if (SCENE_STAGE04 == iSceneIndex)
		//{
		if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_CerealSign1")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_CerealSign2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_HostZone1")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_StudioCam0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_CrunchBox0")))
		{

			m_bReflect_Sour = true;
			m_fReflect_Height = 1.f;
		}
		if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Floor1")))
		{
			if (SCENE_STAGE04 == m_iCurrentScene)
			{
				m_bReflect_Dest = true;
				m_fReflect_Height = 1.f;

				m_iRenderPass = 11;
				m_iRenderPassBackUp = m_iRenderPass;
				m_bIsRenderingShadow = true;
			}
		}
		//}
		//else if (SCENE_STAGE05 == iSceneIndex)
		//{
		if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_BossBackground0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_BossBackground1")) ||
			!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_BossBackground2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Flower0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Flower"))
			|| !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Flower0")))
		{
			m_bReflect_Sour = true;
			m_fReflect_Height = 0.1f;
		}
		if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Compt_Floor2")))
		{
			m_bReflect_Dest = true;
			m_fReflect_Height = 0.1f;

			m_iRenderPass = 11;
			m_iRenderPassBackUp = m_iRenderPass;
			m_bIsRenderingShadow = true;
		}
		//}
		RELEASE_INSTANCE(CGameInstance);
	}
}


HRESULT CStatic_Mesh::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	/* Com_Navigation */
	//CNavigation::NAVIDESC		NaviDesc;
	//ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));
	//
	//NaviDesc.iCurrentIndex = 0;
	//
	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	//	return E_FAIL;
	_float vLength = GET_FLOAT3LENGTH(vColSize);
	if (vLength > 0.f)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		//tColliderDesc.isVisualization = true;
		tColliderDesc.isGravity = false;

		tColliderDesc.isSceneQuery = true;
		if (m_eLayerType == LayerType::LAYERTYPE_PICK)
			tColliderDesc.fRestitution = 0.f;

		tColliderDesc.pGameObject = this;
		CBoxCollider::DESC tBoxColliderDesc;
		tBoxColliderDesc.tColliderDesc = tColliderDesc;
		tBoxColliderDesc.vScale = MULTI_FLOAT3(vColSize, 1.99f);

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
			return E_FAIL;

		_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
		m_pColliderCom->SetPivotMatrix(smatPviot);
		m_pColliderCom->Set_ShapeLayer(m_eLayerType);
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();

		m_isActorActive = false;
		SettingActorActive();
	}

	/* Com_Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	return S_OK;
}

HRESULT CStatic_Mesh::SetUp_ConstantTable()
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

	_bool		bMotionBlur = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR);
	_bool		bShadow = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_SHADOW);
	_bool		bRimlight = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_RIMLIGHT);
	if (SCENE_DEMO == m_iCurrentScene)
	{
		bRimlight = false;
	}
	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix, ReflectMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	if (2 != m_iRenderPass)
	{
		XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	}
	else if (2 == m_iRenderPass)
	{
		//_float3 vTempPos = STOREFLOAT3(m_pTransformCom->Get_Position());
		//_float3 vBackUpPos = vTempPos;
		//_float	fY = vTempPos.y;
		//fY -= (fY - m_fReflect_Height)*2.f;
		//vTempPos.y = fY;

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vTempPos));
		//XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());


		XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_ReflectionView(m_fReflect_Height)));

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vBackUpPos));
	}

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));


	if (11 == m_iRenderPass)
	{
		XMStoreFloat4x4(&ReflectMatrix, XMMatrixTranspose(dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_ReflectionView(m_fReflect_Height)));
		m_pModelCom->Set_RawValue("g_ReflectMatrix", &ReflectMatrix, sizeof(_float4x4));

		if (FAILED(m_pModelCom->Set_TextureValue("g_ReflectTexture", pGameInstance->Get_SRV(TEXT("Target_Reflect")))))
			return E_FAIL;
		_float fReflectRatio = 0.03f;
		_uint iSceneIndex = pGameInstance->Get_CurrentSceneIndex();
		if (SCENE_STAGE05 == iSceneIndex)
		{
			if (FAILED(m_pModelCom->Set_RawValue("g_fReflectRatio", &fReflectRatio, sizeof(_float))))
				return E_FAIL;

			m_bIsNormalMapping = false;
		}

	}

	_bool		bCheck = true;
	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float3(0.079f, -0.997f, 0.0f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bRimlight, sizeof(_bool))))
		return E_FAIL;
	_float2 vRimRatio = m_pRendererCom->Get_RimRatio();

	if (FAILED(m_pModelCom->Set_RawValue("g_vRimRatio", &vRimRatio, sizeof(_float2))))
		return E_FAIL;

	if (false == m_bIsRenderingShadow)
	{
		bShadow = false;
	}
	if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
	if (false == m_bIsNormalMapping)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_bIsNormalMapping", &m_bIsNormalMapping, sizeof(_bool))))
			return E_FAIL;
	}
	/*For. ShadowMap*/
	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

	/*For. MotionBlur*/
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

void CStatic_Mesh::OnControllerCollisionHit(CCollisionObject & collision)
{
	if (m_eLayerType == LayerType::LAYERTYPE_PICK)
	{
		m_isMove = true;
		m_isColController = true;
	}
}

HRESULT CStatic_Mesh::Create_Effect()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box1"))
		|| !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box2")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box3")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_Box4")))
	{
		/* 상자 조잉 */
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperParticleRect"),
			SCENE_STATIC, TEXT("Effect_JudgePaperAttackRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperParticleRect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return false;
		}

		pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperAttackRect"), TEXT("Com_Option"), true);
	}

	/* WhiteStarRect */
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"),
		SCENE_STATIC, TEXT("Effect_WhiteStarRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. WhiteStarRect 상속 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_WhiteStarRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	_float3 vPos = _float3(0.f, 1.f, 0.f);
	pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), XMLoadFloat3(&vPos));
	pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), true);
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_WhiteStarRect"), TEXT("Com_Option"), 0.5f);

	/* Smoke_Gray */
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"),
		SCENE_STATIC, TEXT("Effect_Smoke_Gray"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Smoke_Gray 이펙트 상속 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_Smoke_Gray"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), true);
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Gray"), TEXT("Com_Option"), 0.8f);

	/* For. Layer_BlueSpark 이펙트 생성 */
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"),
		SCENE_STATIC, TEXT("Effect_BlueSpark"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Layer_BlueSpark 상속 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_BlueSpark"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), true);
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 2.f);

	/* For. Layer_GrabBrokenRect 이펙트 생성 */
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"),
		SCENE_STATIC, TEXT("Effect_GrabBrokenRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlueSpark"), TEXT("Com_Option"), 1.5f);

	/* For. Layer_GrabBrokenRect 이펙트 상속 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_GrabBrokenRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_GrabBrokenRect"), TEXT("Com_Option"), 0.8f);

	if (FAILED(pGameInstance->StopSound(CHANNELID::STATICK_MESH)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->PlaySoundOnce(TEXT("GrabBrokenTwo.wav"), CHANNELID::STATICK_MESH)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


CStatic_Mesh * CStatic_Mesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStatic_Mesh*	pInstance = new CStatic_Mesh(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CStatic_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStatic_Mesh::Clone(void * pArg, _uint iSceneIndex)
{
	CStatic_Mesh*	pInstance = new CStatic_Mesh(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CStatic_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStatic_Mesh::Free()
{
	__super::Free();

	//Safe_Delete(m_StrLayerName);
	//Safe_Delete(m_StrModelName);
	//Safe_Delete(m_StrObjectFullName);

	//Safe_Release(m_pNavigationCom);
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);

	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}
