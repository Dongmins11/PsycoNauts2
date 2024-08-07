#include "stdafx.h"
#include "..\public\Blender.h"
#include "GameInstance.h"

CBlender::CBlender(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CBlender::CBlender(const CBlender & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlender::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlender::NativeConstruct(void * pArg)
{
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = TEXT("Layer_Blender");
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	//m_eLayerType = LayerType::LAYERTYPE_GRIDDLE;

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

	m_iRenderPass = 10;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;

	m_bIsRenderingShadow = true;

	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;

	return S_OK;
}

_int CBlender::Tick(_double TimeDelta)
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
		/*if (m_isRender && m_pColliderCom)
			m_pColliderCom->Remove_Actor();*/
		m_isRender = false;
	}

	if (m_isEffectCreate)
	{
		if (!m_isInteraction)
		{
			m_isInteraction = true;
			if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BlenderWave"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlenderWave"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}

			if(FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransformCom, TEXT("Layer_BlenderWave"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
		}
	}
	else
	{
		_uint iSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlenderWave"));
		if (0 < iSize)
		{
			CGameObject* pEffect = pGameInstance->Get_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlenderWave"), 0);

			if(nullptr != pEffect)
				pEffect->Set_IsInteraction(true);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

_int CBlender::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT_AFTER, this); /* 나중에 다른걸로 처리 부탁 */
	return 0;
}

HRESULT CBlender::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(8);
	return S_OK;
}

void CBlender::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
}

void CBlender::Setting_Throwing(_fvector vDir)
{
}

void CBlender::OnCollisionEnter(CCollisionObject & collision)
{
}

void CBlender::OnCollisionStay(CCollisionObject & collision)
{
}

void CBlender::OnCollisionExit(CCollisionObject & collision)
{
}

void CBlender::OnTriggerEnter(CCollisionObject & collision)
{
}

void CBlender::SettingActorActive()
{
}

_bool CBlender::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CBlender::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE03:
	{
		_int NextIndex = iPlayerCurlingIndex + 1;
		if (iPlayerCurlingIndex == m_iCurlingIndex || NextIndex == m_iCurlingIndex)
		{
			m_isRender = true;
		}
		else
		{
			m_isRender = false;
		}
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CBlender::HITCheck(_double TimeDelta)
{
}

HRESULT CBlender::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	return S_OK;
}

HRESULT CBlender::SetUp_ConstantTable()
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

	//_bool		bCheck = true;
	_bool		bEmissive = false;
	if (FAILED(m_pModelCom->Set_RawValue("g_bEmissive", &bEmissive, sizeof(_bool))))
		return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float3(0.079f, -0.997f, 0.0f), sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
	//	return E_FAIL;

	//
	//_float fAlpha = 0.4f;
	//if (FAILED(m_pModelCom->Set_RawValue("g_fBlendingTime", &fAlpha, sizeof(_float))))
	//	return E_FAIL;
	///*For. Shader On/Off*/

	////if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bCheck, sizeof(_bool))))
	////	return E_FAIL;
	//if (false == m_bIsRenderingShadow)
	//{
	//	bShadow = false;
	//}
	//if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
	//	return E_FAIL;
	//if (false == m_bIsNormalMapping)
	//{
	//	if (FAILED(m_pModelCom->Set_RawValue("g_bIsNormalMapping", &m_bIsNormalMapping, sizeof(_bool))))
	//		return E_FAIL;
	//}
	///*For. ShadowMap*/
	//_matrix PlayerWorld = XMMatrixIdentity();
	//PlayerWorld.r[3] = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	//_vector vPos = XMVector3TransformCoord(XMVectorSet(m_ArrKey[0], m_ArrKey[1], m_ArrKey[2], 1.f), PlayerWorld);
	//_float3 vPosFloat;
	//XMStoreFloat3(&vPosFloat, vPos);

	//_float3 vLightLook, vLightPos;
	//vLightLook = pGameInstance->Get_ShadowLightLook();
	//vLightPos = pGameInstance->Get_ShadowLightPos();

	//_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	//if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
	//	return E_FAIL;

	///*For. MotionBlur*/
	//if (true == m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR))
	//{
	//	m_pModelCom->Set_RawValue("g_OldWorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldWolrd)), sizeof(_float4x4));
	//	m_pModelCom->Set_RawValue("g_OldViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldView)), sizeof(_float4x4));

	//}
	//m_OldWolrd = m_pTransformCom->Get_WorldFloat4x4();
	//XMStoreFloat4x4(&m_OldView, pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBlender::OnControllerCollisionHit(CCollisionObject & collision)
{
}

CBlender * CBlender::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBlender*	pInstance = new CBlender(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBlender");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBlender::Clone(void * pArg, _uint iSceneIndex)
{
	CBlender*	pInstance = new CBlender(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CBlender");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlender::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
