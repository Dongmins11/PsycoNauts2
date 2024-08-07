#include "stdafx.h"
#include "..\public\AnimatedShelv.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"

CAnimatedShelv::CAnimatedShelv(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CAnimatedShelv::CAnimatedShelv(const CAnimatedShelv & rhs)
	:CGameObject(rhs)
{
}

HRESULT CAnimatedShelv::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimatedShelv::NativeConstruct(void * pArg, _uint iCurrentScene)
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
	m_StrLayerName = TEXT("Layer_AnimatedShelv");
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
	XMStoreFloat4(&m_SettingPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_iRenderPassBackUp = m_iRenderPass;

	Setting_Lerp();
	m_isRender = true;
	
	return S_OK;
}

_int CAnimatedShelv::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	/* 확인용 입니다. */
	//Check();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (true == m_bMoveShelv)/* 쓰러지다 */
		MoveShelv();

	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	{
		_int iPlayerCurlingIndex = 0;
		_int iDeleteCurlingIndex = 0;
		if (Setting_CurlingIndex(&iPlayerCurlingIndex, &iDeleteCurlingIndex))
		{
			if (m_iDeleteCurlingIndex == iDeleteCurlingIndex)
			{
				if (nullptr != m_pEffect)
				{
					m_pEffect->Set_IsDestroy(true);
					Safe_Release(m_pEffect);
					m_pEffect = nullptr;
				}
				m_isDestroy = true;
				RELEASE_INSTANCE(CGameInstance);
				return 0;
			}
			Stage_RenderBranch(iPlayerCurlingIndex);
		}

		if (m_pColliderCom)
		{
			if (!m_pColliderCom->Get_Gravity())
				m_pColliderCom->Reset_Power();
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

	if (m_isInteraction)
	{
		//디졸브 효과시간 후 Destroy

		m_DissolveTimeAcc += TimeDelta;
		if (m_DissolveDelay <= m_DissolveTimeAcc)
		{
			if (nullptr != m_pEffect)
			{
				m_pEffect->Set_IsDestroy(true);
				Safe_Release(m_pEffect);
				m_pEffect = nullptr;
			}
			m_isDestroy = true;
		}

		if (!m_isEffectCreate)
		{
			m_isEffectCreate = true;

			if (FAILED(Create_Effect()))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
		}

	}

	if (!m_isRender)
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}

	if (m_pColliderCom)
	{
 		m_pColliderCom->Update_TransformPhysX();
	}
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CAnimatedShelv::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (m_pColliderCom)
	{
    	m_pColliderCom->Update_Transform();
	}
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CAnimatedShelv::Render()
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

void CAnimatedShelv::MoveShelv()
{
	RoationTime += 0.001f + RoationAccumulationTime;
	MoveTime += 0.001f + MoveAccumulationTime;
	if (1.01f >= RoationTime)
	{
		XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(m_SettingRoation.x, m_SettingRoation.y, m_SettingRoation.z, m_SettingRoation.w), XMVectorSet(m_SettingLeupRoation.x, m_SettingLeupRoation.y, m_SettingLeupRoation.z, m_SettingLeupRoation.w), RoationTime));
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, XMConvertToRadians(m_MoveRoation.x), 0.f));
	}
	if (1.01f >= MoveTime)
	{
		XMStoreFloat4(&m_MovePostion, XMVectorLerp(XMVectorSet(m_SettingPosition.x, m_SettingPosition.y, m_SettingPosition.z, m_SettingPosition.w), XMVectorSet(m_SettingLeupPosition.x, m_SettingLeupPosition.y, m_SettingLeupPosition.z, m_SettingLeupPosition.w), MoveTime));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MovePostion));
		
	}
	RoationAccumulationTime = RoationAccumulationTime *1.1f;
	MoveAccumulationTime = MoveAccumulationTime *1.1f;

	if (2.f < RoationTime && 2.f < MoveTime)
	{
		if (9 == m_iMYObjectCount)
			m_eLayerType = LayerType::LAYERTYPE_STATIC;
		else
			m_eLayerType = LayerType::LAYERTYPE_ATTAKABLE;

		m_bMoveShelv = false;
	}

	if (m_pColliderCom)
	{
		m_pColliderCom->SetGravity(true);
		m_pColliderCom->Reset_Power();
	}
}

void CAnimatedShelv::Check()
{
	//if (9 == m_iMYObjectCount) /* 찬장 */
	//{
		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);

		if (true == pGameInstnace->Key_Pressing('M'))
		{
			RoationTime += 0.001f + RoationAccumulationTime;
			MoveTime += 0.001f +    MoveAccumulationTime;
			if (1.01f >= RoationTime)
			{
				XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(m_SettingRoation.x, m_SettingRoation.y, m_SettingRoation.z, m_SettingRoation.w), XMVectorSet(m_SettingLeupRoation.x, m_SettingLeupRoation.y, m_SettingLeupRoation.z, m_SettingLeupRoation.w), RoationTime));
				m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 1.f, 0.f), _float3(0.f, XMConvertToRadians(m_MoveRoation.x),0.f));
			}
			if (1.01f >= MoveTime)
			{
				XMStoreFloat4(&m_MovePostion, XMVectorLerp(XMVectorSet(m_SettingPosition.x, m_SettingPosition.y, m_SettingPosition.z, m_SettingPosition.w), XMVectorSet(m_SettingLeupPosition.x, m_SettingLeupPosition.y, m_SettingLeupPosition.z, m_SettingLeupPosition.w), MoveTime));
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MovePostion));
			}
			RoationAccumulationTime = RoationAccumulationTime *1.1f;
			MoveAccumulationTime = MoveAccumulationTime *1.1f;
		}
		RELEASE_INSTANCE(CGameInstance);
	//}

}

void CAnimatedShelv::Setting_Lerp()
{
	switch (m_iMYObjectCount)
	{
	case 0:
		m_SettingLeupPosition = _float4(-54.9f, 2.75f, 74.82f, 1.f);
		m_SettingLeupRoation = _float4(180.f, 0.f, 0.f, 0.f);
		m_SettingRoation= _float4(90.f, 0.f, 0.f, 0.f);
		break;
	case 1:
		m_SettingLeupPosition = _float4(-54.89f, 1.71f, 71.68f, 1.f);
		m_SettingLeupRoation = _float4(270.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(180.f, 0.f, 0.f, 0.f);
		break;
	case 2:
		m_SettingLeupPosition = _float4(-54.96f, 0.57f, 71.72f, 1.f);
		m_SettingLeupRoation = _float4(270.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(180.f, 0.f, 0.f, 0.f);
		break;
	case 3:
		m_SettingLeupPosition = _float4(-54.68f, 2.84f, 71.22f, 1.f);
		m_SettingLeupRoation = _float4(180.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(90.f, 0.f, 0.f, 0.f);
		break;
	case 4:
		m_SettingLeupPosition = _float4(-54.82f, 2.79f, 73.31f, 1.f);
		m_SettingLeupRoation = _float4(90.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(0.f, 0.f, 0.f, 0.f);
		break;
	case 5:
		m_SettingLeupPosition = _float4(-55.21f, 0.55f, 75.01f, 1.f);
		m_SettingLeupRoation = _float4(180.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(90.f, 0.f, 0.f, 0.f);
		break;
	case 6:
		m_SettingLeupPosition = _float4(-55.08f, 0.56f, 73.49f, 1.f);
		m_SettingLeupRoation = _float4(270.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(180.f, 0.f, 0.f, 0.f);
		break;
	case 7:
		m_SettingLeupPosition = _float4(-54.88f, 0.57f, 70.39f, 1.f);
		m_SettingLeupRoation = _float4(270.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(180.f, 0.f, 0.f, 0.f);
		break;
	case 8:
		m_SettingLeupPosition = _float4(-54.83f, 1.71f, 70.40f, 1.f);
		m_SettingLeupRoation = _float4(270.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(180.f, 0.f, 0.f, 0.f);
		break;
	case 9:
		m_SettingLeupPosition = _float4(-55.f, 0.f, 72.3f, 1.f);
		m_SettingLeupRoation = _float4(90.f, 0.f, 0.f, 0.f);
		m_SettingRoation = _float4(0.f, 0.f, 0.f, 0.f);
		break;
	}
}

_bool CAnimatedShelv::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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
	RELEASE_INSTANCE(CGameInstance);

}

void CAnimatedShelv::Stage_RenderBranch(_int iPlayerCurlingIndex)
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

void CAnimatedShelv::Set_AnimatedShelvStart()
{
	if (1.01f > RoationTime && 1.01f > MoveTime)
		m_bMoveShelv = true;
}

HRESULT CAnimatedShelv::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint LayerSize = pGameInstance->Get_LayerSize(m_iCurrentScene, TEXT("Layer_AnimatedShelv"));
	m_iMYObjectCount = LayerSize;
	
	if (m_iMYObjectCount != 9)
	{
		if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			m_iCurrentScene, TEXT("Layer_Env_BrokenObject"),
			SCENE_STATIC, TEXT("Effect_Env_BrokenObject"), &m_pEffect)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		Safe_AddRef(m_pEffect);

		if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, false, m_pTransformCom, TEXT("Layer_Env_BrokenObject"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		pGameInstance->Setting_Effect_TransformPos(m_iCurlingIndex, TEXT("Layer_Env_PickingObject"), TEXT("Com_Option"), CTransform::STATE_UP);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CAnimatedShelv::SetUp_ConstantTable()
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

HRESULT CAnimatedShelv::Create_Effect()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iMYObjectCount)
	{
	case 0:
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect0"),
			SCENE_STATIC, TEXT("Effect_PaperRect0"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect0"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 1:
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect1"),
			SCENE_STATIC, TEXT("Effect_PaperRect1"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect1"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 2://터졌었음
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect2"),
			SCENE_STATIC, TEXT("Effect_PaperRect2"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect2"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 3:
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect3"),
			SCENE_STATIC, TEXT("Effect_PaperRect3"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect3"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 4:
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect4"),
			SCENE_STATIC, TEXT("Effect_PaperRect4"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect4"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 5:
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect5"),
			SCENE_STATIC, TEXT("Effect_PaperRect5"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect5"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 6:

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect6"),
			SCENE_STATIC, TEXT("Effect_PaperRect6"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect6"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 7:

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect7"),
			SCENE_STATIC, TEXT("Effect_PaperRect7"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect7"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	case 8:
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PaperRect8"),
			SCENE_STATIC, TEXT("Effect_PaperRect8"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_PaperRect8"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CAnimatedShelv * CAnimatedShelv::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CAnimatedShelv*	pInstance = new CAnimatedShelv(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CAnimatedShelv");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAnimatedShelv::Clone(void * pArg, _uint iCurrentScene)
{
	CAnimatedShelv*	pInstance = new CAnimatedShelv(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CAnimatedShelv");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimatedShelv::Free()
{
	__super::Free();

	if (m_pColliderCom)
	m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}
