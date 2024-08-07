#include "stdafx.h"
#include "..\public\Strawberry.h"

CStrawberry::CStrawberry(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CFruit(pDevice, pDeviceContext)
{
}

CStrawberry::CStrawberry(const CStrawberry & rhs)
	: CFruit(rhs)
{
}

HRESULT CStrawberry::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStrawberry::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_matrix	matrix;
	if (nullptr != pArg)
		matrix = *(_matrix*)pArg;

	m_NavigationIndex = 0;
	m_iRenderPass = 0;

	m_StrLayerName = TEXT("Layer_STRAWBERRY");
	Setting_ModelNames();
	m_StrObjectFullName = TEXT("Prototpye_GameObject_STRAWBERRY");
	m_iPX_TYPE = LayerType::LAYERTYPE_PICK;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(WorldMatrixSetting(matrix)))
		return E_FAIL;

	if (FAILED(PhysColliderBox_Setting()))
		return E_FAIL;
	m_pModelCom[m_eCurrentModelType]->Set_Animation(0);

	//쉐이더를 적용할 객체면 렌더패스 0
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	m_fRangeForFrustum = 1.f;

	m_iCurlingIndex = 0; /* 확인용 원래 1 */
	m_iDeleteCurlingIndex = 0;
	m_isRender = true;

	////테스트코드
	//m_iCookingNumber = (2 << (_uint)CookType::COOK_BLEND);
	return S_OK;
}

_int CStrawberry::Tick(_double TimeDelta)
{
	if (__super::Tick(TimeDelta))
		return 0;

	Curling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fRangeForFrustum, m_pColliderCom);

	m_pModelCom[m_eCurrentModelType]->Lerp_Update(TimeDelta);

	if (FAILED(Create_Effect()))
		return -1;

	if (FAILED(Change_Effect()))
		return -1;

	return 0;
}

_int CStrawberry::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	//그림자 적용 O -> RENDER_SHADOW
	//그림자 적용 X -> RENDER_NONALPHA
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CStrawberry::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom[m_eCurrentModelType]->Render(m_iRenderPass);

	return S_OK;
}

void CStrawberry::Setting_ModelNames()
{
	m_StrModelName[FMT_IDLE] = TEXT("Prototype_Component_Model_Strawberry0");
	m_StrModelName[FMT_BOILED] = TEXT("Prototype_Component_Model_Strawberry0");
	m_StrModelName[FMT_CUTTING] = TEXT("Prototype_Component_Model_Strawberry0");
	m_StrModelName[FMT_BLENDED] = TEXT("Prototype_Component_Model_StrawberryBlended0");
	m_StrModelName[FMT_GRIDDLED] = TEXT("Prototype_Component_Model_Strawberry0");
}

HRESULT CStrawberry::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* Com_Model*/
	for (_uint i = 0; i < FMT_END; ++i)
	{
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName[i].c_str(), m_szModelLayerTags[i], (CComponent**)&m_pModelCom[i])))
			return E_FAIL;

		if (FAILED(SetUp_BoneMatrices("HeadPoint", i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStrawberry::SetUp_ConstantTable()
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
	_float2		vRimRatio = m_pRendererCom->Get_RimRatio();
	//노말텍스쳐 맵핑의 여부 
	_bool		bIsMapping = true;

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_vLightDir", &_float3(0.079f, -0.997f, 0.0f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_vLightPos", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_bRimlight", &bRimlight, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_bIsNormalMapping", &bIsMapping, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_vRimRatio", &vRimRatio, sizeof(_float2))))
		return E_FAIL;

	//그림자 맵
	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	if (FAILED(m_pModelCom[m_eCurrentModelType]->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

	/*모션블러 : 스태틱 메쉬만 모션블러 킬 것이다.*/

	//if (true == m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR))
	//{
	//	m_pModelCom->Set_RawValue("g_OldWorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldWolrd[2])), sizeof(_float4x4));
	//	m_pModelCom->Set_RawValue("g_OldViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldView[2])), sizeof(_float4x4));

	//}
	//m_OldWolrd[0] = m_pTransformCom->Get_WorldFloat4x4();
	//XMStoreFloat4x4(&m_OldView[0], pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));

	//m_OldWolrd[1] = m_OldWolrd[0];
	//m_OldView[1] = m_OldView[0];

	//m_OldWolrd[2] = m_OldWolrd[1];
	//m_OldView[2] = m_OldView[1];

	//if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
	//	return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStrawberry::Create_Effect()
{
	if (!m_isInteraction)
	{
		m_isInteraction = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
			m_iCurrentScene, TEXT("Layer_Repeat_FruitEffect"),
			SCENE_STATIC, TEXT("Effect_Repeat_FruitEffect"), &m_pEffect)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		Safe_AddRef(m_pEffect);

		if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, false, m_pTransformCom, TEXT("Layer_Repeat_FruitEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		pGameInstance->Setting_Effect_TransformPos(m_iCurrentScene, TEXT("Layer_Repeat_FruitEffect"), TEXT("Com_Option"), CTransform::STATE_UP, 0.4f);

		RELEASE_INSTANCE(CGameInstance);
	}
	return S_OK;
}

HRESULT CStrawberry::Change_Effect()
{
	if (m_isFinishCooking)
	{
		if (!m_isChangeEffect)
		{
			m_isChangeEffect = true;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (nullptr != m_pEffect)
			{
				m_pEffect->Set_IsDestroy(true);
				Safe_Release(m_pEffect);
				m_pEffect = nullptr;
			}

			if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
				m_iCurrentScene, TEXT("Layer_Repeat_CookingCompleteStarRect"),
				SCENE_STATIC, TEXT("Effect_Repeat_CookingCompleteStarRect"), &m_pEffect)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			Safe_AddRef(m_pEffect);

			if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, false, m_pTransformCom, TEXT("Layer_Repeat_CookingCompleteStarRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			pGameInstance->Setting_Effect_TransformPos(m_iCurrentScene, TEXT("Layer_Repeat_CookingCompleteStarRect"), TEXT("Com_Option"), CTransform::STATE_UP, 0.4f);

			RELEASE_INSTANCE(CGameInstance);
		}
	}
	return S_OK;
}

CStrawberry * CStrawberry::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStrawberry*	pInstance = new CStrawberry(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CStrawberry");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStrawberry::Clone(void * pArg, _uint iCurrentScene)
{
	CStrawberry*	pInstance = new CStrawberry(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CStrawberry");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStrawberry::Free()
{
	__super::Free();
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	for (_uint i = 0; i < FMT_END; ++i)
	{
		Safe_Release(m_pModelCom[i]);

	}

	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}
