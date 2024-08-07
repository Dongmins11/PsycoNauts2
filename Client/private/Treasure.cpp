#include "stdafx.h"
#include "..\public\Treasure.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player.h"
#include "Camera_Fly.h"

CTreasure::CTreasure(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTreasure::CTreasure(const CTreasure & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTreasure::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasure::NativeConstruct(void * pArg)
{
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = TEXT("Layer_Treasure");
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = LayerType::LAYERTYPE_TREASURE;

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

_int CTreasure::Tick(_double TimeDelta)
{
	if (0 >(__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_isInteraction)
	{
		m_isInteraction = true;

		if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			m_iCurrentScene, TEXT("Layer_Env_HealpackObject"),
			SCENE_STATIC, TEXT("Effect_Env_HealpackObject"), &m_pEffect)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		Safe_AddRef(m_pEffect);

		if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, false, m_pTransformCom, TEXT("Layer_Env_HealpackObject"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		pGameInstance->Setting_Effect_TransformPos(m_iCurlingIndex, TEXT("Layer_Env_HealpackObject"), TEXT("Com_Option"), CTransform::STATE_UP);
	}

	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	{
		_int iPlayerCurlingIndex = 0;
		_int iDeleteCurlingIndex = 0;
		if (Setting_CurlingIndex(&iPlayerCurlingIndex, &iDeleteCurlingIndex))
		{
			if (m_iDeleteCurlingIndex == iDeleteCurlingIndex)
			{
				m_isDestroy = true;
				if (nullptr != m_pEffect)
				{
					m_pEffect->Set_IsDestroy(true);
					Safe_Release(m_pEffect);
					m_pEffect = nullptr;
				}
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

	if (m_bMoveScale)
		ScaleMove(TimeDelta);

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();

	
	//if (m_HitCheck) /* true */
	//	Hit_Check(TimeDelta);
	
	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

_int CTreasure::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CTreasure::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CTreasure::Setting_Count_And_HitCheck()
{
}

void CTreasure::Hit_Check(_double TimeDelta)
{
	if (true == m_HitCheck) /* 충돌이 되었다. */
	{
		m_Hit_Delay_Timer += TimeDelta;
		if (1.0 < m_Hit_Delay_Timer)
		{
			m_HitCheck = false;
			m_Hit_Delay_Timer = 0.0;
		}
	}
}

void CTreasure::ScaleMove(_double TimeDelta)
{
	if (true == m_bScaleDown && false == m_bScaleEnd) /* 작아졌다 커져라 해줘야겠다. */
	{
		m_Scale_Timer += TimeDelta * 4.0;
		if (1.0 >= m_Scale_Timer)
		{
			_vector vScale = XMVectorLerp(XMVectorSet(1.f, 1.f, 1.f,0.f), XMVectorSet(1.f, 0.7f, 1.f,0.f) , m_Scale_Timer);
			m_pTransformCom->Scaling(vScale);
		}
		else
		{
			m_bScaleDown = false;
			m_Scale_Timer = 0.0;
		}
	}
	
	if(false == m_bScaleDown && false == m_bScaleEnd)
	{
		m_Scale_Timer += TimeDelta * 4.0;
		if (1.0 >= m_Scale_Timer)
		{
			_vector vScale = XMVectorLerp(XMVectorSet(1.f, 0.7f, 1.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 0.f), m_Scale_Timer);
			m_pTransformCom->Scaling(vScale);
		}
		else
		{
			m_bScaleEnd = true;
			//m_bMoveScale = false; /* 다시 Hit 처리가 될수있게 해줄것이다 .*/
			m_bScaleDown = true;
			m_Scale_Timer = 0.0;
		}
	}

	if (true == m_bScaleEnd && false == m_bScaleUp)
	{
		m_Scale_Timer += TimeDelta * 4.0;
		if (1.0 >= m_Scale_Timer)
		{
			_vector vScale = XMVectorLerp(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(1.f, 0.8f, 1.f, 0.f), m_Scale_Timer);
			m_pTransformCom->Scaling(vScale);
		}
		else
		{
			m_bScaleUp = true;
			m_Scale_Timer = 0.0;
		}
	}

	if (true == m_bScaleEnd && true == m_bScaleUp)
	{
		m_Scale_Timer += TimeDelta * 4.0;
		if (1.0 >= m_Scale_Timer)
		{
			_vector vScale = XMVectorLerp(XMVectorSet(1.f, 0.8f, 1.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 0.f), m_Scale_Timer);
			m_pTransformCom->Scaling(vScale);
		}
		else
		{
			m_bScaleUp = false;  /* 초기화 */
			m_bScaleDown = true;  /* 초기화 */
			m_bScaleEnd = false; /* 초기화 */
			m_bMoveScale = false; /* 다시 Hit 처리가 될수있게 해줄것이다 .*/
			m_Scale_Timer = 0.0;
		}
	}
}

void CTreasure::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	m_pColliderCom->Add_Force(vDir, eMode);
}

void CTreasure::Setting_Throwing(_fvector vDir)
{
}

void CTreasure::OnCollisionEnter(CCollisionObject & collision)
{
	//if (!m_isMove)
	//	return;

	//if (LayerType::LAYERTYPE_PLAYER_SKILL == collision.Get_Object()->Get_ObjectLayerType())
	//{
	//	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	//
	//	RELEASE_INSTANCE(CGameInstance);
	//}
}

void CTreasure::OnCollisionStay(CCollisionObject & collision)
{
}

void CTreasure::OnCollisionExit(CCollisionObject & collision)
{
}

void CTreasure::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_SKILL == collision.Get_Object()->Get_ObjectLayerType())
	{
		if (0 == m_TreasureHP)
		{
			/* 죽으면서 힐팩  이벤트 해줘~ */
			m_isDestroy = true;
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstnace->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_HealthPack"), SCENE_STATIC, TEXT("Layer_HealthPack"), &m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
				return;
			RELEASE_INSTANCE(CGameInstance);
		}
		if (false == m_bMoveScale && 0 < m_TreasureHP) /* 딜레이도 줘야되기 떄문이다. */
		{
			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
			pGameInstnace->StopSound(JELLYHIT);

			if (1 == m_TreasureHP)
				pGameInstnace->PlaySoundOnce(TEXT("JellyHit0.wav"), JELLYHIT);
			else
				pGameInstnace->PlaySoundOnce(TEXT("JellyHit1.wav"), JELLYHIT);
			
			RELEASE_INSTANCE(CGameInstance);
			//m_HitCheck = true;
			m_bMoveScale = true; /* 크기의 변동을 줄것이다. */
			--m_TreasureHP;
		}
	}
}

void CTreasure::SettingActorActive()
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

_bool CTreasure::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CTreasure::Stage_RenderBranch(_int iPlayerCurlingIndex)
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

HRESULT CTreasure::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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
	tColliderDesc.isSceneQuery = true; /* 물리충돌을 끄겠다. */
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

HRESULT CTreasure::SetUp_ConstantTable()
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

void CTreasure::OnControllerCollisionHit(CCollisionObject & collision)
{
	//if (LayerType::LAYERTYPE_PLAYER_SKILL== collision.Get_Object()->Get_ObjectLayerType())
	//{
	//	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	//	
	//	RELEASE_INSTANCE(CGameInstance);
	//}
}

CTreasure * CTreasure::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTreasure*	pInstance = new CTreasure(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CTreasure");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTreasure::Clone(void * pArg, _uint iSceneIndex)
{
	CTreasure*	pInstance = new CTreasure(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CTreasure");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTreasure::Free()
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
