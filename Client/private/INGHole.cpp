#include "stdafx.h"
#include "..\public\INGHole.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"

CINGHole::CINGHole(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CINGHole::CINGHole(const CINGHole & rhs)
	: CGameObject(rhs)
{
}

HRESULT CINGHole::NativeConstruct_Prototype()
{
	if(FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CINGHole::NativeConstruct(void * pArg)
{
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = TEXT("Layer_INGHole");
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = LayerType::LAYERTYPE_STATIC;

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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE05, TEXT("Layer_INGHole"));

	RELEASE_INSTANCE(CGameInstance);

	if (0 == m_iLayerSize)
	{
		m_isEmissiveActive[0] = false;
		m_iRenderPassArr[0] = 0;
		m_isEmissiveActive[1] = false;
		m_iRenderPassArr[1] = 0 ;
		m_isEmissiveActive[2] = true;
		m_iRenderPassArr[2] = 7;
	}

	return S_OK;
}

_int CINGHole::Tick(_double TimeDelta)
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
	}
	else
	{
		//if (m_isRender && m_pColliderCom)
		//	m_pColliderCom->Remove_Actor();

		m_isRender = false;
	}

	RELEASE_INSTANCE(CGameInstance);

	m_TimeAcc += TimeDelta;
	if (FAILED(Effect_Create()))
		return -1;

	return _int();
}

_int CINGHole::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CINGHole::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;
	
	m_pModelCom->Render_Custom_Light(m_iRenderPassArr, 3, m_isEmissiveActive);
	return S_OK;
}

void CINGHole::Setting_Count_And_HitCheck()
{
}

void CINGHole::Player_HitCheck(_double TimeDelta)
{
}

void CINGHole::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
}

void CINGHole::Setting_Throwing(_fvector vDir)
{
}

void CINGHole::OnCollisionEnter(CCollisionObject & collision)
{
}

void CINGHole::OnCollisionStay(CCollisionObject & collision)
{
}

void CINGHole::OnCollisionExit(CCollisionObject & collision)
{
}

void CINGHole::OnTriggerEnter(CCollisionObject & collision)
{
}

void CINGHole::SettingActorActive()
{
}

_bool CINGHole::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CINGHole::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE05:
	{
		_int NextIndex = iPlayerCurlingIndex + 1;
		if (iPlayerCurlingIndex == m_iCurlingIndex || NextIndex == m_iCurlingIndex)
		{
			/*
			if (!m_isRender && m_pColliderCom)
				m_pColliderCom->Add_Actor();*/

			m_isRender = true;
		}
		else
		{
			//if (m_isRender && m_pColliderCom)
			//	m_pColliderCom->Remove_Actor();

			m_isRender = false;
		}
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CINGHole::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	//CPhysXCollider::DESC tColliderDesc;

	//tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	////tColliderDesc.isVisualization = true;
	//tColliderDesc.isGravity = false;
	//tColliderDesc.isTrigger = true;/* 트리거 출동*/
	//tColliderDesc.isSceneQuery = false; /* 물리충돌을 끄겠다. */
	//tColliderDesc.fRestitution = 0.f;

	//tColliderDesc.pGameObject = this;
	//CBoxCollider::DESC tBoxColliderDesc;
	//tBoxColliderDesc.tColliderDesc = tColliderDesc;
	//tBoxColliderDesc.vScale = MULTI_FLOAT3(vColSize, 1.99f);

	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider0"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
	//	return E_FAIL;

	//_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
	//m_pColliderCom->SetPivotMatrix(smatPviot);
	//m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	//m_pColliderCom->Update_TransformPhysX();
	//m_pColliderCom->Update_Transform();

	//m_isActorActive = false;

	//SettingActorActive();

	/* Com_LightTexture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_LightTexture"), TEXT("Com_LightTexture"), (CComponent**)&m_pLightMapCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CINGHole::SetUp_ConstantTable()
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

	_float		fTimeAcc = (_float)m_TimeAcc;
	m_pModelCom->Set_RawValue("g_fUVTimeAcc", &fTimeAcc, sizeof(_float));
	_float		fBlue = 0.2f;
	m_pModelCom->Set_RawValue("g_fBlue", &fBlue, sizeof(_float));

	m_pModelCom->Set_TextureValue("g_LightImgTexture", m_pLightMapCom->Get_SRV(2));

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

void CINGHole::OnControllerCollisionHit(CCollisionObject & collision)
{
}

HRESULT CINGHole::Effect_Create()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_uint iSize = 0;
	switch (m_iLayerSize)
	{
	case 0:
		//iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SpinWormhole0"));
		//if (0 < iSize)
		//	m_isInteraction = true;
		//else
		//	m_isInteraction = false;

		if (!m_isInteraction)
		{
			m_isInteraction = true;
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_SpinHole"),
				m_iCurrentScene, TEXT("Layer_SpinWormhole0"),
				SCENE_STATIC, TEXT("Effect_SpinWormhole"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, true, m_pTransformCom, TEXT("Layer_SpinWormhole0"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			pGameInstance->Setting_Effect_TransformPos(m_iCurrentScene, TEXT("Layer_SpinWormhole0"), TEXT("Com_Option"), CTransform::STATE_LOOK);
		}
		break;

	case 1:
		//iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SpinWormhole1"));
		//if (0 < iSize)
		//	m_isInteraction = true;
		//else
		//	m_isInteraction = false;

		if (!m_isInteraction)
		{
			m_isInteraction = true;
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_SpinHole"),
				m_iCurrentScene, TEXT("Layer_SpinWormhole1"),
				SCENE_STATIC, TEXT("Effect_SpinWormhole"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, true, m_pTransformCom, TEXT("Layer_SpinWormhole1"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			pGameInstance->Setting_Effect_TransformPos(m_iCurrentScene, TEXT("Layer_SpinWormhole1"), TEXT("Com_Option"), CTransform::STATE_LOOK);
		}
		break;
	}
	


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CINGHole * CINGHole::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CINGHole*	pInstance = new CINGHole(pDevice, pDeviceContext);
	
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CINGHole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CINGHole::Clone(void * pArg, _uint iSceneIndex)
{
	CINGHole*	pInstance = new CINGHole(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CINGHole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CINGHole::Free()
{
	__super::Free();

	//if (m_pColliderCom)
	//	m_pColliderCom->Remove_Actor();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pLightMapCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
