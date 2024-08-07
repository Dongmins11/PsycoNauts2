#include "stdafx.h"
#include "..\public\PyroPainting.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player_RightHand.h"
#include "Right_Hand_Anim.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "Enemy.h"
#include "Lock_2D.h"

CPyroPainting::CPyroPainting(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPyroPainting::CPyroPainting(const CPyroPainting & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPyroPainting::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPyroPainting::NativeConstruct(void * pArg)
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
	m_eLayerType = LayerType::LAYERTYPE_NPC; /*  타입 이넘 */

	//objinfo.fCollider_Extent = 0.5f;
	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	//if (m_fRangeForFrustum <= 0.f)
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

	m_isRender = false;
	m_bIsRenderingShadow = true;
	

	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;


	return S_OK;
}

_int CPyroPainting::Tick(_double TimeDelta)
{

	if (0 > (__super::Tick(TimeDelta)))
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

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();

	CLock_2D* pLock_2D = dynamic_cast<CLock_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Lock_2D"), 0));
	if (nullptr == pLock_2D)
	{
		RELEASE_INSTANCE(CGameInstance);
		return -1;
	}
	if (true == pLock_2D->Get_DisolveCheck())
		m_IsLockOpen = true;

	RELEASE_INSTANCE(CGameInstance);

	if (!m_isRender)
	{
		return 0;
	}


	if (true == m_bIsFluctuate)
		Fluctuate(TimeDelta);




	if (true == m_bBrainStromBurningCheck)
	{
		m_fBurningTimeAcc += (_float)(TimeDelta * 0.7);

		if (!m_isInteraction)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_isInteraction = true;
			if (0 == m_iLayerIndex)
			{
				/* For. Layer_BlueSpark 이펙트 생성 */
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FireEffectRect"),
					SCENE_STATIC, TEXT("Effect_FireEffectRect"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				/* For. Layer_BlueSpark 상속 */
				if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransformCom, TEXT("Layer_FireEffectRect"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			}
			else
			{
				/* For. Layer_BlueSpark 이펙트 생성 */
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_FireEffectRect"),
					SCENE_STATIC, TEXT("Effect_FireEffectRect"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				/* For. Layer_BlueSpark 상속 */
				if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_FireEffectRect"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			}
			RELEASE_INSTANCE(CGameInstance);
		}

		if (2.f <= m_fBurningTimeAcc)
		{
			m_fBurningTimeAcc = 0.0f;
			m_bBrainStromBurningCheck = false;
			m_isDestroy = true;
		}
	}



	return 0;
}

_int CPyroPainting::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_pColliderCom)
		m_pColliderCom->Update_Transform();
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	
	return 0;
}

HRESULT CPyroPainting::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	if (!m_isRender)
		return 0;

	if (!(1 == m_iRenderPass && false == m_bIsRenderingShadow))
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		m_pModelCom->Render(m_iRenderPass);
	}
	return S_OK;
}

void CPyroPainting::Fluctuate(_double TimeDelta)
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

void CPyroPainting::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	m_pColliderCom->Add_Force(vDir, eMode);

}

void CPyroPainting::Setting_Throwing(_fvector vDir)
{
	//m_pColliderCom->SetType(CPhysXScene_Manager::TYPE_DYNAMIC);
	m_isColController = false;
	//m_pColliderCom->Add_Force(vDir*m_pTransformCom->Get_TransFormDescSpeed()+XMVectorSet(0.f, 4.9f, 0.f, 0.f));
	m_pColliderCom->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
	m_isThrowing = true;
}

void CPyroPainting::OnCollisionEnter(CCollisionObject & collision)
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

void CPyroPainting::OnCollisionStay(CCollisionObject & collision)
{
}

void CPyroPainting::OnCollisionExit(CCollisionObject & collision)
{
}

void CPyroPainting::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_BRAINSTROM)
	{
		if (false == m_bBrainStromBurningCheck && 0 == m_iLayerIndex)
			m_bBrainStromBurningCheck = true;
		else if (false == m_bBrainStromBurningCheck && 1 == m_iLayerIndex)
		{
			if (true == m_IsLockOpen)
				m_bBrainStromBurningCheck = true;
		}
	}
}

void CPyroPainting::SettingActorActive()
{
	if (!m_pColliderCom)
		return;

	if (m_isActorActive)
		m_pColliderCom->WakeUp();
	else
		m_pColliderCom->Sleep();

}

_bool CPyroPainting::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CPyroPainting::Stage_RenderBranch(_int iPlayerCurlingIndex)
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


HRESULT CPyroPainting::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE02, TEXT("Layer_PyroPainting1"));
	RELEASE_INSTANCE(CGameInstance);

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
		tColliderDesc.isTrigger = true;
		tColliderDesc.isSceneQuery = false;
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

HRESULT CPyroPainting::SetUp_ConstantTable()
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
	if (GetKeyState('C') & 0x8000)
	{
		m_ArrKey[2] -= 0.5f;
	}
	if (GetKeyState('V') & 0x8000)
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
	
	if (true == m_bBrainStromBurningCheck)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fBurningTimeAcc, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bBrainStromBurningCheck, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_TextureValue("g_DissolveTexture", m_pTextureCom->Get_SRV(m_iTextureIndex))))
			return E_FAIL;
	}

	/*For. Shader On/Off*/

	//if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bCheck, sizeof(_bool))))
	//	return E_FAIL;
	if (false == m_bIsRenderingShadow)
	{
		bShadow = false;
	}
	if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
		return E_FAIL;
	/*if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;*/
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

void CPyroPainting::OnControllerCollisionHit(CCollisionObject & collision)
{
	if (m_eLayerType == LayerType::LAYERTYPE_PICK)
	{
		m_isMove = true;
		m_isColController = true;
	}
}

HRESULT CPyroPainting::Create_Effect()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


CPyroPainting * CPyroPainting::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPyroPainting*	pInstance = new CPyroPainting(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPyroPainting");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPyroPainting::Clone(void * pArg, _uint iSceneIndex)
{
	CPyroPainting*	pInstance = new CPyroPainting(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPyroPainting");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPyroPainting::Free()
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
}
