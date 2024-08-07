#include "stdafx.h"
#include "..\public\Blade.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player_RightHand.h"
#include "Right_Hand_Anim.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "Enemy.h"

CBlade::CBlade(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBlade::CBlade(const CBlade & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlade::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBlade::NativeConstruct(void * pArg)
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

	if (FAILED(SetUp_Component(objinfo.fCollider_Extent, objinfo.fCollider_Center, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	if (FAILED(Setting_MyCount()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;


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
		!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeWall12")))
	{
		m_bIsRenderingShadow = false;
	}
	else
	{
		m_bIsRenderingShadow = true;
	}

	if (!lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeFloor0")) || !lstrcmp(m_StrLayerName.c_str(), TEXT("Layer_OfficeFloor1")))
		m_bIsNormalMapping = false;

	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;


	return S_OK;
}

_int CBlade::Tick(_double TimeDelta)
{
	if (0 >(__super::Tick(TimeDelta)))
		return -1;

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
		switch (m_iMyCount)
		{
		case 0:
		case 1:
			m_fMove -= XMConvertToRadians(2.f);
			m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMove);
			break;
		case 2:
		case 3:
			m_fMove -= XMConvertToRadians(2.f);
			m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMove);
			break;
		case 4:
		case 5:
		case 6:
			m_pTransformCom->RotationPerSec(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)), m_fMove);
			break;
		}

		if (m_HitCheck) /* 충돌이 되었습니다.*/
		{
			m_HitCheckTimer += TimeDelta;
			if (1.0 <= m_HitCheckTimer)
			{
				m_HitCheck = false;
				m_HitCheckTimer = 0.0;
			}
		}

		m_pColliderCom->Update_TransformPhysX();
	}

	if (true == m_bIsFluctuate)
		Fluctuate(TimeDelta);

	return 0;
}

_int CBlade::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_Transform();
	}
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CBlade::Render()
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

void CBlade::Fluctuate(_double TimeDelta)
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

void CBlade::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	m_pColliderCom->Add_Force(vDir, eMode);
}

void CBlade::Setting_Throwing(_fvector vDir)
{
	m_isColController = false;
	m_pColliderCom->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
	m_isThrowing = true;
}

void CBlade::OnCollisionEnter(CCollisionObject & collision)
{
	if (!m_isMove)
		return;
	/*if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
	_uint iA = 0;
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
	}

	Safe_Release(pGameInstance);
	}*/

}

void CBlade::OnCollisionStay(CCollisionObject & collision)
{
}

void CBlade::OnCollisionExit(CCollisionObject & collision)
{
}

void CBlade::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_TRIGGER == collision.Get_Object()->Get_ObjectLayerType())
	{
		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);

		if (false == m_HitCheck) /* 딜레이를 주기 위해서 */
		{
			m_HitCheck = true;

			if (false == static_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_HitingCheck())
			{
				static_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HitCheck(true);

				if (true == m_isPulling && false == static_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_IsKnockBack())
					static_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsKnockBack(true);
			}
			//OutputDebugString(TEXT("BLADE 트리거 충돌!!!!"));
		}
		RELEASE_INSTANCE(CGameInstance);
		/* Player와 충돌이 되었다.*/
	}
}

void CBlade::SettingActorActive()
{
	if (!m_pColliderCom)
		return;

	if (m_isActorActive)
		m_pColliderCom->WakeUp();
	else
		m_pColliderCom->Sleep();

}

_bool CBlade::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CBlade::Stage_RenderBranch(_int iPlayerCurlingIndex)
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
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBlade::Setting_MyCount()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iMyCount = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Compt_Blender"));

	switch (m_iMyCount)
	{
	case 0: /* 믹서기쪽 Right Down*/
		m_fMove = -1.f;
		break;
	case 1: /* 믹서기쪽 Left  Down*/
		m_fMove = -1.f;
		break;
	case 2: /* 믹서기쪽 Right Up 문제*/
			//m_pTransformCom->RotationByRadian(XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
		m_fMove = -1.3f;
		break;
	case 3: /* 믹서기쪽 Left  Up  문제*/
		m_fMove = -1.2f;
		break;
	case 4: /* 돼지 가는 길 0 문제*/
		m_isPulling = true;
		m_fMove = -0.016f;
		break;
	case 5: /* 돼지 가는 길 1*/
		m_isPulling = true;
		m_fMove = -0.025f;
		break;
	case 6: /* 돼지 가는 길 2 문제*/
		m_isPulling = true;
		m_fMove = -0.03f;
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


HRESULT CBlade::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	_float vLength = GET_FLOAT3LENGTH(vColSize);
	if (vLength > 0.f)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		//tColliderDesc.isVisualization = true;
		tColliderDesc.isGravity = false;
		tColliderDesc.isTrigger = true;/* 트리거 출동*/
		tColliderDesc.isSceneQuery = true;
		if (m_eLayerType == LayerType::LAYERTYPE_BLADE)
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
	return S_OK;
}

HRESULT CBlade::SetUp_ConstantTable()
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

	/*For. Shader On/Off*/

	//if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bCheck, sizeof(_bool))))
	//	return E_FAIL;
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
	_matrix PlayerWorld = XMMatrixIdentity();
	PlayerWorld.r[3] = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	_vector vPos = XMVector3TransformCoord(XMVectorSet(m_ArrKey[0], m_ArrKey[1], m_ArrKey[2], 1.f), PlayerWorld);
	_float3 vPosFloat;
	XMStoreFloat3(&vPosFloat, vPos);

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

void CBlade::OnControllerCollisionHit(CCollisionObject & collision)
{
	if (m_eLayerType == LayerType::LAYERTYPE_PICK)
	{
		m_isMove = true;
		m_isColController = true;
	}
}

CBlade * CBlade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBlade*	pInstance = new CBlade(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CStatic_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBlade::Clone(void * pArg, _uint iSceneIndex)
{
	CBlade*	pInstance = new CBlade(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CStatic_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlade::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
