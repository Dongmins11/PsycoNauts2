#include "stdafx.h"
#include "..\public\GasBurner.h"
#include "GameInstance.h"
#include "Camera_Fly.h"

CGasBurner::CGasBurner(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)

{
}

CGasBurner::CGasBurner(const CGasBurner & rhs)
	: CGameObject(rhs)

{
}

HRESULT CGasBurner::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGasBurner::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

	return S_OK;
}

_int CGasBurner::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (true == m_bLightedOn)
	{
		m_dRandTimeAcc += TimeDelta;

		if (false == m_isInteraction)
		{
			m_isInteraction = true;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BurnnerFireSmall"),
				SCENE_STATIC, TEXT("Effect_BurnnerFireSmall"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}

			// 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransformCom, TEXT("Layer_BurnnerFireSmall"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.5;
			pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BurnnerFireSmall"), TEXT("Com_Option"), vPos);

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BurnnerFire"),
				SCENE_STATIC, TEXT("Effect_BurnnerFire"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}

			// 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, true, m_pTransformCom, TEXT("Layer_BurnnerFire"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			vPos = m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.0f;
			pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BurnnerFire"), TEXT("Com_Option"), vPos);

			RELEASE_INSTANCE(CGameInstance);
		}
	}

	if(5.0 <= m_dRandTimeAcc)
	{
		m_dRandTimeAcc = 0.0;
		m_bLightedOn = false;
		m_isInteraction = false;
	}


	return _int();
}

_int CGasBurner::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CGasBurner::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

HRESULT CGasBurner::SetUp_Component()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_GasBurner"));

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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGasBurner::SetUp_ConstantTable()
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

	//그림자 맵
	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CGasBurner * CGasBurner::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGasBurner* pInstance = new CGasBurner(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CGasBurner");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGasBurner::Clone(void * pArg, _uint iSceneIndex)
{
	CGasBurner* pInstance = new CGasBurner(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CKitchenPanButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CGasBurner::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
