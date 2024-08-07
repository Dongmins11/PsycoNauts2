#include "stdafx.h"
#include "..\public\CookingEnd.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"

CCookingEnd::CCookingEnd(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CCookingEnd::CCookingEnd(const CCookingEnd & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCookingEnd::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookingEnd::NativeConstruct(void * pArg)
{
	_vector pos;
	if (nullptr != pArg)
		pos = *(_vector*)pArg;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pSceneCookMesh = ((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Stage03PhaseNum();
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	if (0 == m_pSceneCookMesh)
	{
		m_pTransformCom->Scaling(XMVectorSet(0.5, 0.5, 0.5, 0.f));
		m_pTransformCom->RotationByRadian(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
	}
	if (1 == m_pSceneCookMesh)
	{
		m_pTransformCom->Scaling(XMVectorSet(2, 2, 2, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos + XMVectorSet(0.f, 1.2f, 0.f, 0.f));
	}
	if(2 == m_pSceneCookMesh)
		m_pTransformCom->Scaling(XMVectorSet(2, 2, 2, 0.f));


	m_isRender = true;
	m_bIsRenderingShadow = true;
	m_fRangeForFrustum = 3.f;

	return S_OK;
}

_int CCookingEnd::Tick(_double TimeDelta)
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
		m_isRender = false;
	}
	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

_int CCookingEnd::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	//if(m_RenderCook)
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CCookingEnd::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CCookingEnd::Setting_Poation()
{

	if (0 == m_pSceneCookMesh)
		m_pTransformCom->Scaling(XMVectorSet(0.2, 0.2, 0.2, 0.f));
	if (1 == m_pSceneCookMesh)
		m_pTransformCom->Scaling(XMVectorSet(1, 1, 1, 0.f));
	if (2 == m_pSceneCookMesh)
		m_pTransformCom->Scaling(XMVectorSet(1, 1, 1, 0.f));



	if(1!= m_pSceneCookMesh)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(236.07f, 1.26f, 204.25f, 1.f));
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(236.07f, 1.7f, 204.25f, 1.f));
}

void CCookingEnd::Setting_Count_And_HitCheck()
{
}

void CCookingEnd::Player_HitCheck(_double TimeDelta)
{
}

void CCookingEnd::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
}

void CCookingEnd::Setting_Throwing(_fvector vDir)
{
}

void CCookingEnd::OnCollisionEnter(CCollisionObject & collision)
{
}

void CCookingEnd::OnCollisionStay(CCollisionObject & collision)
{
}

void CCookingEnd::OnCollisionExit(CCollisionObject & collision)
{
}

void CCookingEnd::OnTriggerEnter(CCollisionObject & collision)
{
}

void CCookingEnd::SettingActorActive()
{
}

_bool CCookingEnd::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CCookingEnd::Stage_RenderBranch(_int iPlayerCurlingIndex)
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

HRESULT CCookingEnd::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	switch (m_pSceneCookMesh)
	{
	case 0:
		/* Com_Model     ´Þ°¿ Á©¸®*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_EggJelly0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Compt_CroqueMadame0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Compt_Batter0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CCookingEnd::SetUp_ConstantTable()
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

void CCookingEnd::OnControllerCollisionHit(CCollisionObject & collision)
{
}

CCookingEnd * CCookingEnd::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCookingEnd*	pInstance = new CCookingEnd(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CCookingEnd");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCookingEnd::Clone(void * pArg, _uint iSceneIndex)
{
	CCookingEnd*	pInstance = new CCookingEnd(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CCookingEnd");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCookingEnd::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
}
