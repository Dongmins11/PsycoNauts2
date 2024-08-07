#include "stdafx.h"
#include "..\public\Lock_Object.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Lock_2D.h"

CLock_Object::CLock_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)

{
}

CLock_Object::CLock_Object(const CLock_Object & rhs)
	: CGameObject(rhs)

{
}

HRESULT CLock_Object::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLock_Object::NativeConstruct(void * pArg, _uint iCurrentScene)
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

	if (!lstrcmp(m_StrLayerName.c_str(), L"Layer_Lock0"))
		m_IsLock = true;

	if (!lstrcmp(m_StrLayerName.c_str(), L"Layer_Chain0"))
		m_IsChain = true;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

	return S_OK;
}

_int CLock_Object::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	Dissolve(TimeDelta);

	return _int();
}

_int CLock_Object::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CLock_Object::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (false == m_IsEmissiveEnd && true == m_IsLock)
		m_pModelCom->Render_Custom_Light(m_iRenderPassArr, 2, m_isEmissiveActive);
	else if (true == m_IsEmissiveEnd && true == m_IsLock)
		m_pModelCom->Render(0);

	if (true == m_IsChain)
		m_pModelCom->Render(0);

	return S_OK;
}


void CLock_Object::Dissolve(_double TimeDelta)
{
	if (true == m_IsDisolveStart)
		Set_Dissolve(true, 0.4, 3, 0.1f, 0.5f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (true == m_bIsDissolve)
	{
		m_fDissolveTime += TimeDelta*m_dDissolveSpeed;
		if (1.f < m_fDissolveTime)
		{
			m_fDissolveTime = 0.f;
			m_bIsDissolve = false;
			m_isDestroy = true;

			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &m_fDissolveStart, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bIsDissolve, sizeof(_bool))))
				return;
		}
	}
	else
	{
		_bool bCheck = false;
		_float fTime = 0.f;
		_float fStartTime = 0.1f;
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &fTime, sizeof(_float))))
			return;
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &fStartTime, sizeof(_float))))
			return;
		if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &bCheck, sizeof(_bool))))
			return;
	}
	m_bBackUpDissolve = m_bIsDissolve;

	RELEASE_INSTANCE(CGameInstance);
}

void CLock_Object::Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed)
{
	m_bIsDissolve = bIsDissolve;
	m_dDissolveSpeed = dSpeed;
	m_iTextureIndex = iTextureIndex;
	m_fDissolveStart = fStart;
	m_fDissolveScaleSpeed = fScaleSpeed;
	if (false == m_bBackUpDissolve && true == m_bIsDissolve)
		m_vDissolveScale = m_vBackUpScale;
}

HRESULT CLock_Object::SetUp_Component()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	/* Com_Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLock_Object::SetUp_ConstantTable()
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

	if (true == m_bIsDissolve)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &m_fDissolveStart, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bIsDissolve, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_TextureValue("g_DissolveTexture", m_pTextureCom->Get_SRV(m_iTextureIndex))))
			return E_FAIL;
	}

	//±×¸²ÀÚ ¸Ê
	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CLock_Object * CLock_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLock_Object* pInstance = new CLock_Object(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CLock_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLock_Object::Clone(void * pArg, _uint iSceneIndex)
{
	CLock_Object* pInstance = new CLock_Object(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CLock_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLock_Object::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
