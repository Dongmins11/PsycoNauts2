#include "stdafx.h"
#include "..\public\GiantCloche.h"
#include "GameInstance.h"
#include "Camera_Fly.h"

CGiantCloche::CGiantCloche(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CGiantCloche::CGiantCloche(const CGiantCloche & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGiantCloche::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGiantCloche::NativeConstruct(void * pArg)
{
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	//m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = TEXT("Layer_GiantCloche");
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
	Setting_Move_Postion();

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;
	m_bIsRenderingShadow = true;

	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;

	return S_OK;
}

_int CGiantCloche::Tick(_double TimeDelta)
{
	if (0 >(__super::Tick(TimeDelta)))
		return -1;

	if (true == m_bGiant_Move)
		Postion_Down_Lerp(TimeDelta);

	if(true == m_bGiantUp_Move)
		Postion_Up_Lerp(TimeDelta);


	return 0;
}

_int CGiantCloche::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CGiantCloche::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CGiantCloche::Setting_Count_And_HitCheck()
{
}

void CGiantCloche::Player_HitCheck(_double TimeDelta)
{
}

void CGiantCloche::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
}

void CGiantCloche::Setting_Throwing(_fvector vDir)
{
}

void CGiantCloche::OnCollisionEnter(CCollisionObject & collision)
{
}

void CGiantCloche::OnCollisionStay(CCollisionObject & collision)
{
}

void CGiantCloche::OnCollisionExit(CCollisionObject & collision)
{
}

void CGiantCloche::OnTriggerEnter(CCollisionObject & collision)
{
}

void CGiantCloche::SettingActorActive()
{
}

_bool CGiantCloche::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
{
	return _bool();
}

void CGiantCloche::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
}

void CGiantCloche::Setting_Move_Postion()
{
	XMStoreFloat3(&m_MovePostion[START_POSTION], m_pTransformCom->Get_Position());
	
	m_MovePostion[ENDING_POSTION] = _float3(236.378815f, 1.97839785f, 192.333298f);
}

void CGiantCloche::Postion_Down_Lerp(_double TimeDelta)
{
	m_AccTimer += TimeDelta;

	if (1.0 <= m_AccTimer)
		m_AccTimer = 1.0;

	_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_MovePostion[START_POSTION]), XMLoadFloat3(&m_MovePostion[ENDING_POSTION]), m_AccTimer);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPostion);

	if (1.0 <= m_AccTimer)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Stage3_CUT(CCamera_Fly::STAGE3_CUT_6);
		RELEASE_INSTANCE(CGameInstance);
		m_AccTimer = 0.0;
		m_bGiant_Move = false;
	}
}

void CGiantCloche::Postion_Up_Lerp(_double TimeDelta)
{
	m_AccTimer += TimeDelta * 0.25;

	if (1.0 <= m_AccTimer)
		m_AccTimer = 1.0;
	
	_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_MovePostion[ENDING_POSTION]), XMLoadFloat3(&m_MovePostion[START_POSTION]), m_AccTimer);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPostion);

	if (1.0 <= m_AccTimer)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Stage3_CUT(CCamera_Fly::STAGE3_CUT_11);
		RELEASE_INSTANCE(CGameInstance);
		m_AccTimer = 0.0;
		m_bGiantUp_Move = false;
	}
}

HRESULT CGiantCloche::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
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

	return  S_OK;
}

HRESULT CGiantCloche::SetUp_ConstantTable()
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

void CGiantCloche::OnControllerCollisionHit(CCollisionObject & collision)
{
}

CGiantCloche * CGiantCloche::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGiantCloche*	pInstance = new CGiantCloche(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CGlantCloche");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGiantCloche::Clone(void * pArg, _uint iSceneIndex)
{
	CGiantCloche*	pInstance = new CGiantCloche(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CGlantCloche");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGiantCloche::Free()
{
	__super::Free();

	//if (m_pColliderCom)
	//	m_pColliderCom->Remove_Actor();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
