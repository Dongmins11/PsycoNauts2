#include "stdafx.h"
#include "..\public\OfficeWalls.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"

COfficeWalls::COfficeWalls(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

COfficeWalls::COfficeWalls(const COfficeWalls & rhs)
	: CGameObject(rhs)
{
}

HRESULT COfficeWalls::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT COfficeWalls::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	memcpy(&m_eWallType, pArg, sizeof(WALLTYPE));

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;
	if (WALLTYPE_LEFT == m_eWallType)
	{
		m_vBackUpPos = m_vPos = _float3(-116.09f, 0.132f, 103.391f);
		m_fRad = -2.715f;
	}
	else
	{
		m_vBackUpPos = m_vPos = _float3(-116.09f, 0.132f, 110.931f);
		m_fRad = -0.63f;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPos));
	m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRad);
	m_fSourSpeed[0] = 0.2f;
	m_fDestSpeed[0] = 1.7f;

	m_fSourSpeed[1] = 0.2f;
	m_fDestSpeed[1] = 1.5f;

	m_fMaxTime[0] = 2.3f;
	m_fMaxTime[1] = 2.f;

	m_vScale = _float3(1.f, 1.f, 1.f);
	return S_OK;
}

_int COfficeWalls::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	//if (GetKeyState('C') & 0x8000)
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vBackUpPos));
	//Update(TimeDelta);
	return _int();
}

_int COfficeWalls::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if(false==m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT COfficeWalls::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void COfficeWalls::Set_RectMove(RECTMOVE eRectMove)
{
	m_eRectMove = eRectMove;
	m_bIsLerp = true;
}

HRESULT COfficeWalls::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_OfficeWall6"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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

HRESULT COfficeWalls::SetUp_ConstantTable()
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
	if (GetKeyState('N') & 0x8000)
	{
		m_ArrKey[2] -= 0.5f;
	}
	if (GetKeyState('M') & 0x8000)
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
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float3(0.012f, -1.f, 0.001f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bCheck, sizeof(_bool))))
	//	return E_FAIL;
	if (true == m_bIsRenderingShadow)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
			return E_FAIL;
	}
	//if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
	//	return E_FAIL;
	_matrix PlayerWorld = XMMatrixIdentity();
	PlayerWorld.r[3] = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	_vector vPos = XMVector3TransformCoord(XMVectorSet(m_ArrKey[0], m_ArrKey[1], m_ArrKey[2], 1.f), PlayerWorld);
	_float3 vPosFloat;
	XMStoreFloat3(&vPosFloat, vPos);

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(_float3(0.012f, -1.f, 0.001f), _float3(m_ArrKey[0] - 4.338f, m_ArrKey[1] + 54.306f, m_ArrKey[2] + 43.769f));

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

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

void COfficeWalls::Update(_double TimeDelta)
{
	switch (m_eRectMove)
	{
	case Client::COfficeWalls::RECTMOVE_NORMAL:
	{
		m_dTimeAcc += TimeDelta;
		if (m_fMaxTime[RECTMOVE_NORMAL] < m_dTimeAcc)
		{
			m_dTimeAcc = 0.0;
			m_fRatio = 0.f;
			m_bIsLerp = true;
			m_eRectMove = RECTMOVE_END;
		}
		_float fSpeed = 0.f;
		if (true == m_bIsLerp)
		{
			m_fRatio += (_float)TimeDelta;
			if (1.f < m_fRatio)
			{
				m_bIsLerp = false;
				fSpeed = m_fDestSpeed[RECTMOVE_NORMAL];
				m_fRatio = 0.f;
			}

			fSpeed = (1 - m_fRatio)*m_fSourSpeed[RECTMOVE_NORMAL] + m_fRatio*m_fDestSpeed[RECTMOVE_NORMAL];
		}
		else
		{
			fSpeed = m_fDestSpeed[RECTMOVE_NORMAL];
		}
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Go_Straight(TimeDelta*fSpeed);
		m_pTransformCom->Set_MyLook(vLook);

	}
	break;
	case Client::COfficeWalls::RECTMOVE_SPEEDUP:
	{
		m_dTimeAcc += TimeDelta;
		if (m_fMaxTime[RECTMOVE_SPEEDUP] < m_dTimeAcc)
		{
			m_dTimeAcc = 0.0;
			m_fRatio = 0.f;
			m_bIsLerp = true;
			m_eRectMove = RECTMOVE_END;
			//m_isDestroy = true;

		}

		_float fSpeed = 0.f;
		if (true == m_bIsLerp)
		{
			m_fRatio += (_float)TimeDelta;
			if (1.f < m_fRatio)
			{
				m_bIsLerp = false;
				fSpeed = m_fDestSpeed[RECTMOVE_SPEEDUP];
				m_fRatio = 0.f;
			}

			fSpeed = (1 - m_fRatio)*m_fSourSpeed[RECTMOVE_SPEEDUP] + m_fRatio*m_fDestSpeed[RECTMOVE_SPEEDUP];
		}
		else
		{
			fSpeed = m_fDestSpeed[RECTMOVE_SPEEDUP];
		}
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Go_Straight(TimeDelta*fSpeed);
		m_pTransformCom->Set_MyLook(vLook);
	}
	break;
	case Client::COfficeWalls::RECTMOVE_END:
		break;
	default:
		break;
	}
}

COfficeWalls * COfficeWalls::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	COfficeWalls*	pInstance = new COfficeWalls(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : COfficeWalls");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * COfficeWalls::Clone(void * pArg, _uint iCurrentScene)
{
	COfficeWalls*	pInstance = new COfficeWalls(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : COfficeWalls");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COfficeWalls::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

}
