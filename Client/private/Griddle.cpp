#include "stdafx.h"
#include "..\public\Griddle.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player.h"
#include "Camera_Fly.h"

CGriddle::CGriddle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CGriddle::CGriddle(const CGriddle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGriddle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGriddle::NativeConstruct(void * pArg)
{
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = TEXT("Layer_Griddle");
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = LayerType::LAYERTYPE_GRIDDLE;

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


	Setting_Count_And_HitCheck();

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;

	m_bIsRenderingShadow = true;

	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;

	return S_OK;
}

_int CGriddle::Tick(_double TimeDelta)
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
	}
	else
	{
		if (m_isRender && m_pColliderCom)
				m_pColliderCom->Remove_Actor();
		
		m_isRender = false;
	}

	/* 만약 세기를 0~1 사이로 하게 된다면 */

	m_AccTimer -= TimeDelta;
	if (m_Start_RenderTimer <= m_AccTimer && m_End_RenderTimer >= m_AccTimer)/* 시작 시간보단 크지만 끝나는 시간보단 작다.*/
	{
		/* 이펙ㅌ츠 시작 */

		if (!m_isInteraction)
		{
			m_isInteraction = true;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				m_iCurrentScene, TEXT("Layer_Repeat_HeatingRect"),
				SCENE_STATIC, TEXT("Effect_Repeat_HeatingRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, false, m_pTransformCom, TEXT("Layer_Repeat_HeatingRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			pGameInstance->Setting_Effect_TransformPos(m_iCurrentScene, TEXT("Layer_Repeat_HeatingRect"), TEXT("Com_Option"), CTransform::STATE_UP);
			RELEASE_INSTANCE(CGameInstance);
		}

		m_Emissive_RenderCheck = true; /* 이떄 쌔기 1*/
		m_RenderInterpolate = false;
		/* 랜더 패스 몇번 이다  추가 가능 */
	}
	else
		m_RenderInterpolate = true; /* 여기서 다른 랜더 패스로 뺴줘서 해줘도 상관없을듯 */


	if (true == m_RenderInterpolate)
	{
		/* 랜더 패스 몇번 이다  추가 하였다면 여기서 다시 약해질수있게 처리  m_Inter_Timer 이값으로 하면될거임 */
		m_Inter_Timer -= TimeDelta;
		if (0.0 >= m_Inter_Timer)
		{
			/* 꺼질떄 */
			m_isInteraction = false;

			m_Inter_Timer = 1.0;
			m_RenderInterpolate = false;
			m_Emissive_RenderCheck = false;
		}
	}
	if (m_AccTimer <= 0.0) /* 모두 지나갔다.*/
		m_AccTimer = 8.0;

	if (m_pColliderCom) /* 피직스 Wlord로*/
		m_pColliderCom->Update_TransformPhysX();

	Player_HitCheck(TimeDelta); /*Hit Delay Time */

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

_int CGriddle::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CGriddle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CGriddle::Setting_Count_And_HitCheck()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iMyCount = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Griddle"));

	switch (m_iMyCount)
	{
	case 0:
		m_Start_RenderTimer = 0.0;
		m_End_RenderTimer = 1.0;
		break;
	case 1:
		m_Start_RenderTimer = 1.0;
		m_End_RenderTimer = 2.0;
		break;
	case 2:
		m_Start_RenderTimer = 2.0;
		m_End_RenderTimer = 3.0;
		break;
	case 3:
		m_Start_RenderTimer = 3.0;
		m_End_RenderTimer = 4.0;
		break;
	case 4:
		m_Start_RenderTimer = 4.0;
		m_End_RenderTimer = 5.0;
		break;
	case 5:
		m_Start_RenderTimer = 5.0;
		m_End_RenderTimer = 6.0;
		break;
	case 6:
		m_Start_RenderTimer = 6.0;
		m_End_RenderTimer = 7.0;
		break; /* 한번 자르기 */
	case 7:
		m_Start_RenderTimer = 0.0;
		m_End_RenderTimer = 1.0;
		break;
	case 8:
		m_Start_RenderTimer = 1.0;
		m_End_RenderTimer = 2.0;
		break;
	case 9:
		m_Start_RenderTimer = 2.0;
		m_End_RenderTimer = 3.0;
		break;
	case 10:
		m_Start_RenderTimer = 3.0;
		m_End_RenderTimer = 4.0;
		break;
	case 11:
		m_Start_RenderTimer = 4.0;
		m_End_RenderTimer = 5.0;
		break;
	case 12:
		m_Start_RenderTimer = 5.0;
		m_End_RenderTimer = 6.0;
		break;
	case 13:
		m_Start_RenderTimer = 6.0;
		m_End_RenderTimer = 7.0;
		break; /* 2번 자르기  절반 */
	case 14:
		m_Start_RenderTimer = 0.0;
		m_End_RenderTimer = 1.0;
		break;
	case 15:
		m_Start_RenderTimer = 1.0;
		m_End_RenderTimer = 2.0;
		break;
	case 16:
		m_Start_RenderTimer = 2.0;
		m_End_RenderTimer = 3.0;
		break;
	case 17:
		m_Start_RenderTimer = 3.0;
		m_End_RenderTimer = 4.0;
		break;
	case 18:
		m_Start_RenderTimer = 4.0;
		m_End_RenderTimer = 5.0;
		break;
	case 19:
		m_Start_RenderTimer = 5.0;
		m_End_RenderTimer = 6.0;
		break;
	case 20:
		m_Start_RenderTimer = 6.0;
		m_End_RenderTimer = 7.0;
		break; /* 3번 자르기 */
	case 21:
		m_Start_RenderTimer = 0.0;
		m_End_RenderTimer = 1.0;
		break;
	case 22:
		m_Start_RenderTimer = 1.0;
		m_End_RenderTimer = 2.0;
		break;
	case 23:
		m_Start_RenderTimer = 2.0;
		m_End_RenderTimer = 3.0;
		break;
	case 24:
		m_Start_RenderTimer = 3.0;
		m_End_RenderTimer = 4.0;
		break;
	case 25:
		m_Start_RenderTimer = 4.0;
		m_End_RenderTimer = 5.0;
		break;
	case 26:
		m_Start_RenderTimer = 5.0;
		m_End_RenderTimer = 6.0;
		break;
	case 27:
		m_Start_RenderTimer = 6.0;
		m_End_RenderTimer = 7.0;
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CGriddle::Player_HitCheck(_double TimeDelta)
{
	if (true == m_HitCheck) /* 충돌이 되었다. */
	{
		m_Hit_Delay_Timer += TimeDelta;
		if (0.5 < m_Hit_Delay_Timer)
		{
			m_HitCheck = false;
			m_Hit_Delay_Timer = 0.0;
		}
	}
}

void CGriddle::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	m_pColliderCom->Add_Force(vDir, eMode);
}

void CGriddle::Setting_Throwing(_fvector vDir)
{
	m_isColController = false;
	m_pColliderCom->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
	m_isThrowing = true;
}

void CGriddle::OnCollisionEnter(CCollisionObject & collision)
{
	if (!m_isMove)
		return;

}

void CGriddle::OnCollisionStay(CCollisionObject & collision)
{
}

void CGriddle::OnCollisionExit(CCollisionObject & collision)
{
}

void CGriddle::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_TRIGGER == collision.Get_Object()->Get_ObjectLayerType())
	{
		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
		if (false == m_HitCheck && true == m_Emissive_RenderCheck) /* 딜레이를 주기 위해서 */
		{
			if (false == static_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_HitCheck())
			{
				m_HitCheck = true;
				static_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HitCheck(true);
				OutputDebugString(TEXT("Griddle 트리거 충돌!!!!"));
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CGriddle::SettingActorActive()
{
	if (!m_pColliderCom)
		return;

	if (m_isActorActive)
	{
		m_pColliderCom->WakeUp();
	}
	else
	{
		m_pColliderCom->Sleep();
	}
}

_bool CGriddle::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CGriddle::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
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
				m_pColliderCom->Remove_Actor();
			
			m_isRender = false;
		}
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CGriddle::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	//tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = false;
	tColliderDesc.isTrigger = true;/* 트리거 출동*/
	tColliderDesc.isSceneQuery = false; /* 물리충돌을 끄겠다. */
	tColliderDesc.fRestitution = 0.f;

	tColliderDesc.pGameObject = this;
	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = MULTI_FLOAT3(vColSize, 1.99f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider0"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;
	
	_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	m_isActorActive = false;
	
	SettingActorActive();	
	return S_OK;
}

HRESULT CGriddle::SetUp_ConstantTable()
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
	if (FAILED(m_pModelCom->Set_RawValue("g_bEmissive", &m_Emissive_RenderCheck, sizeof(_bool))))
		return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_fEmissiveRatio", &m_Inter_Timer, sizeof(_float))))
	//	return E_FAIL;

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

void CGriddle::OnControllerCollisionHit(CCollisionObject & collision)
{
}

CGriddle * CGriddle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGriddle*	pInstance = new CGriddle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CGriddle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGriddle::Clone(void * pArg, _uint iSceneIndex)
{
	CGriddle*	pInstance = new CGriddle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CGriddle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGriddle::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
