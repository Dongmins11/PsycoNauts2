#include "stdafx.h"
#include "..\public\PsyBLExit.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "FadeInOut.h"
#include "Scene_Loading.h"

CPsyBLExit::CPsyBLExit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPsyBLExit::CPsyBLExit(const CPsyBLExit & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPsyBLExit::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPsyBLExit::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = TEXT("Layer_PsyBLExit1");
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = (LayerType)objinfo.iPX_Index;

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
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	
	XMStoreFloat4(&m_MovePostion[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&m_MovePostion[1], XMVectorSet(-24.77f,10.24f,8.85f,1.f));
	m_isRender = true;
	return S_OK;
}

_int CPsyBLExit::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	Check(TimeDelta);


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
		{
			m_pColliderCom->Remove_Actor();
		}
		m_isRender = false;
	}



	if (!m_isRender)
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CPsyBLExit::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	/*if (m_pColliderCom)
	{
	m_pColliderCom->Update_Transform();
	}*/
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CPsyBLExit::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (!(1 == m_iRenderPass && false == m_bIsRenderingShadow))
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		m_pModelCom->Render(m_iRenderPass);
	}
	return S_OK;
}

_bool CPsyBLExit::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CPsyBLExit::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
	case SCENE_DEMO:
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
			{
				m_pColliderCom->Remove_Actor();
			}
			m_isRender = false;
		}
		break;
	}
	case SCENE_STAGE01:
	{
		if (iPlayerCurlingIndex == 0) /* 0번 것만 출력 할것 */
		{
			if (iPlayerCurlingIndex == m_iCurlingIndex/* || NextIndex == m_iCurlingIndex*/)
			{
				if (!m_isRender && m_pColliderCom)
					m_pColliderCom->Add_Actor();
				m_isRender = true;
			}
			else
			{
				if (m_isRender && m_pColliderCom)
				{
					m_pColliderCom->Remove_Actor();
				}
				m_isRender = false;
			}
		}
		else if (iPlayerCurlingIndex == 1)/* 0 ~ 1 출력 할것 */
		{
			_uint  OldIndex = iPlayerCurlingIndex - 1;
			if (iPlayerCurlingIndex == m_iCurlingIndex || OldIndex == m_iCurlingIndex)
			{
				if (!m_isRender && m_pColliderCom)
					m_pColliderCom->Add_Actor();
				m_isRender = true;
			}
			else
			{
				if (m_isRender && m_pColliderCom)
				{
					m_pColliderCom->Remove_Actor();
				}
				m_isRender = false;
			}
		}
		else if (iPlayerCurlingIndex == 2)/* 1 출력 할것 */
		{
			_uint curIndex = iPlayerCurlingIndex - 1;
			if (curIndex == m_iCurlingIndex)
			{
				if (!m_isRender && m_pColliderCom)
					m_pColliderCom->Add_Actor();
				m_isRender = true;
			}
			else
			{
				if (m_isRender && m_pColliderCom)
				{
					m_pColliderCom->Remove_Actor();
				}
				m_isRender = false;
			}
		}
		break;
	}
	case SCENE_STAGE02:
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
			{
				m_pColliderCom->Remove_Actor();
			}
			m_isRender = false;
		}
		break;
	}
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPsyBLExit::Check(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Pressing('Z'))
	//{
	//	m_MoveCheck = true;
	//	//m_MovePostion[0].y = m_MovePostion[0].y - 0.1f;
	//	//
	//	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MovePostion[0]));
	//}
	if (m_MoveCheck)
	{
		Time += /*TimeDelta **/ 0.16f + AccumulationTime;
		if (1.0f <= Time)
			Time = 1.0;

			XMStoreFloat4(&m_vMove, XMVectorLerp(XMLoadFloat4(&m_MovePostion[0]), XMLoadFloat4(&m_MovePostion[1]), Time));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vMove));
			
			if (true == m_bFade)
			{
				m_bFade = false;
				dynamic_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(1.f, 1.f, 1.f, 1.f), CFadeInOut::INOUT_OUT);
			}

			/* 페이드 인 진행 + 카메라 쉐이킹 */
			if (m_bCameraMove)
			{
				m_bCameraMove = false;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Camera_Shaking(true, 1.0);
				RELEASE_INSTANCE(CGameInstance);
			}
		
		if (1.0f <= Time)
		{
			pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_STAGE02));
		}
		AccumulationTime = AccumulationTime *1.1f;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPsyBLExit::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

HRESULT CPsyBLExit::SetUp_ConstantTable()
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

CPsyBLExit * CPsyBLExit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPsyBLExit*	pInstance = new CPsyBLExit(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPsyBLExit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPsyBLExit::Clone(void * pArg, _uint iCurrentScene)
{
	CPsyBLExit*	pInstance = new CPsyBLExit(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CPsyBLExit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPsyBLExit::Free()
{
	__super::Free();

	/*if (m_pColliderCom)
	m_pColliderCom->Remove_Actor();*/

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
