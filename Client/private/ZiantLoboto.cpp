#include "stdafx.h"
#include "..\public\ZiantLoboto.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "Player_CutScene.h"
//#include "PhysXCollider.h"
CZiantLoboto::CZiantLoboto(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CZiantLoboto::CZiantLoboto(const CZiantLoboto & rhs)
	: CGameObject(rhs)
{
}

HRESULT CZiantLoboto::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CZiantLoboto::NativeConstruct(void * pArg, _uint iCurrentScene)
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
	m_eLayerType = (LayerType)objinfo.iPX_Index;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0);
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	XMStoreFloat3(&m_vDestPos, m_pTransformCom->Get_Position());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position() + XMVectorSet(0.f, -5.f, 0.f, 0.f));
	XMStoreFloat3(&m_vSourPos, m_pTransformCom->Get_Position());
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_eCurAnimstate = ANIM_READY;
	m_isRender = false;
	return S_OK;
}

_int CZiantLoboto::Tick(_double TimeDelta)
{
	if (GetKeyState('Z') & 0x8000)
	{
		m_bIsAppearEnd = false;
		m_bIsPunch = false;
		m_eCurAnimstate = ANIM_READY;
	}

	if (!m_bIsUpdate)
		return 0;

	UpdateAnimState(TimeDelta);
	m_pModelCom->Set_Animation(m_iCurAnimation);

	if(ANIM_READY != m_eCurAnimstate)
		m_pModelCom->Lerp_Update(TimeDelta*m_dAnimSpeed);

	return _int();
}

_int CZiantLoboto::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CZiantLoboto::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}


HRESULT CZiantLoboto::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CZiantLoboto::SetUp_ConstantTable()
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
	bRimlight = false;
	bShadow = false;
	bMotionBlur = false;

	if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bRimlight, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CZiantLoboto::Setting_CurlingIndex(_int * pCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* Player = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	if (Player)
	{
		*pCurlingIndex = Player->Get_CurlingIndex();
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
}

void CZiantLoboto::UpdateAnimState(_double TimeDelta)
{
	switch (m_eCurAnimstate)
	{
	case Client::CZiantLoboto::ANIM_READY:
		m_dTimeAcc += TimeDelta;
		if (m_dMaxReadyTime < m_dTimeAcc)
		{
			m_dTimeAcc = 0.0;
			m_dAnimSpeed = 0.8;
			m_eCurAnimstate = ANIM_APPEAR;
			m_iCurAnimation = APPEAR;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Stage0_End_State(CCamera_Fly::STAGE0_END_PUSHBACK);
			RELEASE_INSTANCE(CGameInstance);
		}
		break;
	case Client::CZiantLoboto::ANIM_APPEAR:
		m_iCurAnimation = APPEAR;
		Appear(TimeDelta);
		if (true == m_pModelCom->Get_IsAnimFinishied(m_iCurAnimation))
		{
			m_eCurAnimstate = ANIM_PUNCH;
			m_pModelCom->Set_IsAnimLefp(true);
			m_dAnimSpeed = 1.5;
			m_iCurAnimation = PUNCH;
			m_pModelCom->Set_AnimBlendSpeed(APPEAR, 2.0);
		}
		break;
	case Client::CZiantLoboto::ANIM_PUNCH:
		m_iCurAnimation = PUNCH;
		if (false == m_bIsPunch && PUNCH_FRAME <= m_pModelCom->Get_AnimationCurrentKeyFrame())
		{
			Punch();
			m_bIsPunch = true;
		}
		if (true == m_pModelCom->Get_IsAnimFinishied(m_iCurAnimation))
		{
			m_eCurAnimstate = ANIM_LAUGH;
			m_pModelCom->Set_IsAnimLefp(true);
			m_dAnimSpeed = 1.5;
			m_iCurAnimation = LAUGH;
			m_pModelCom->Set_AnimBlendSpeed(PUNCH, 10.0);
			m_bIsPunch = false;
		}
		break;
	case Client::CZiantLoboto::ANIM_LAUGH:
		m_iCurAnimation = LAUGH;
		if (true == m_pModelCom->Get_IsAnimFinishied(m_iCurAnimation))
		{
			m_eCurAnimstate = ANIM_PUNCH_TWO;
			m_pModelCom->Set_IsAnimLefp(true);
			m_dAnimSpeed = 2.0;
			m_iCurAnimation = PUNCH_TWO;
		}
		break;
	case Client::CZiantLoboto::ANIM_PUNCH_TWO:
		m_iCurAnimation = PUNCH_TWO;
		if (false == m_bIsPunch && PUNCH_FRAME <= m_pModelCom->Get_AnimationCurrentKeyFrame())
		{
			Punch_Two();
			m_bIsPunch = true;
		}
		if (true == m_pModelCom->Get_IsAnimFinishied(m_iCurAnimation))
		{
			m_eCurAnimstate = ANIM_IDLE;
			m_pModelCom->Set_IsAnimLefp(true);
			m_pModelCom->Set_AnimBlendSpeed(PUNCH_TWO, 3.0);
			m_dAnimSpeed = 0.5;
			m_iCurAnimation = IDLE;
			m_bIsPunch = false;

		}
		break;
	case Client::CZiantLoboto::ANIM_IDLE:
		m_iCurAnimation = IDLE;
		//if (true == m_pModelCom->Get_IsAnimFinishied(m_iCurAnimation))
		//{
		//	m_eCurAnimstate = ANIM_PUNCH;
		//	m_pModelCom->Set_IsAnimLefp(true);
		//	m_dAnimSpeed = 0.5;
		//	m_iCurAnimation = PUNCH;
		//	m_bIsUpdate = false;
		//}
		break;
	case Client::CZiantLoboto::ANIM_END:
		m_bIsUpdate = false;
		break;
	default:
		break;
	}
}

void CZiantLoboto::Appear(_double TimeDelta)
{
	if (false == m_bIsAppearEnd)
	{
		m_dRatio += TimeDelta*m_dLerpSpeed;

		if (1.0 < m_dRatio)
		{
			m_bIsAppearEnd = true;
			m_dRatio = 0.0;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vDestPos));
			return;
		}

		_vector vPos = XMVectorLerp(XMLoadFloat3(&m_vSourPos), XMLoadFloat3(&m_vDestPos), (_float)m_dRatio);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
}

void CZiantLoboto::Punch()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Fly* pCam = ((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0));
	if (nullptr == pCam)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	pCam->Set_Stage0_End_State(CCamera_Fly::STAGE0_END_PUNCH);
	pCam->Set_Camera_Shaking(true, 0.7f, false);

	CPlayer_CutScene* pPlayerCutScene = ((CPlayer_CutScene*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_Player_CutScene"), 0));
	if (nullptr == pPlayerCutScene)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	pPlayerCutScene->Punch();
	RELEASE_INSTANCE(CGameInstance);
}

void CZiantLoboto::Punch_Two()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Fly* pCam = ((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0));
	if (nullptr == pCam)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	pCam->Set_Stage0_End_State(CCamera_Fly::STAGE0_END_PUNCHTWO);
	pCam->Set_Camera_Shaking(true, 0.7f,false);

	CPlayer_CutScene* pPlayerCutScene = ((CPlayer_CutScene*)pGameInstance->Get_GameObject(SCENE_DEMO, TEXT("Layer_Player_CutScene"), 0));
	if (nullptr == pPlayerCutScene)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	pPlayerCutScene->Punch();
	RELEASE_INSTANCE(CGameInstance);
}


CZiantLoboto * CZiantLoboto::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CZiantLoboto*	pInstance = new CZiantLoboto(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CZiantLoboto::Clone(void * pArg, _uint iCurrentScene)
{
	CZiantLoboto*	pInstance = new CZiantLoboto(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CZiantLoboto::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
