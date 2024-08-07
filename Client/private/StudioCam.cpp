#include "stdafx.h"
#include "..\public\StudioCam.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player_RightHand.h"
#include "Right_Hand_Anim.h"
#include "Player.h"
#include "Camera_Fly.h"


CStudioCam::CStudioCam(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CStudioCam::CStudioCam(const CStudioCam & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStudioCam::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CStudioCam::NativeConstruct(void * pArg)
{
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = LayerType::LAYERTYPE_PICK;

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

	m_isRender = true;
	m_bIsRenderingShadow = true;
	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;

	return S_OK;
}

_int CStudioCam::Tick(_double TimeDelta)
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
		if (m_isRender && m_pColliderCom)
		{
			m_pColliderCom->Remove_Actor();
		}
		m_isRender = false;
	}


	RELEASE_INSTANCE(CGameInstance);

	if (!m_isRender)
	{
		return 0;
	}

	if (m_pColliderCom)
	{
		if (m_isPicking)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
			_vector vPos = pPlayerTrans->Get_Position() - pPlayerTrans->Get_State(CTransform::STATE_UP) * 0.7f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pColliderCom->SetGravity(false);
			m_isMove = true;
			m_bPickingCheck = true;
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (m_isMove && !m_isPicking)
		{
			m_pColliderCom->SetGravity(true);
		}
		else
		{
			m_pColliderCom->Reset_Power();
		}

		if(!m_bPickingCheck)
			m_pColliderCom->Update_TransformPhysX();
	}

	if (true == m_bIsFluctuate)
		Fluctuate(TimeDelta);

	return 0;
}

_int CStudioCam::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_isEffectCreate)
	{
		m_DeleteTimeAcc += TimeDelta;
		if (0.5 < m_DeleteTimeAcc)
		{
			m_isDestroy = true;
		}
	}

	if (!m_isRender)
		return 0;

	if (m_pColliderCom)
	{
		if (!m_bPickingCheck)
			m_pColliderCom->Update_Transform();
	}
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CStudioCam::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	
	return S_OK;
}

void CStudioCam::Fluctuate(_double TimeDelta)
{
	m_fFluctuateSize += m_fFluctuateInterval;
	if (0 == m_iFluctuateStep)
	{
		if (1.5f < m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = -0.2f;
		}

	}
	else if (1 == m_iFluctuateStep)
	{
		if (0.7f > m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = 0.2f;
		}
	}
	else if (2 == m_iFluctuateStep)
	{
		if (1.3f < m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = -0.2f;
		}
	}
	else if (3 == m_iFluctuateStep)
	{
		if (0.85f > m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = 0.1f;
		}
	}
	else if (4 == m_iFluctuateStep)
	{
		if (1.15f < m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = -0.05f;
		}
	}
	else if (5 == m_iFluctuateStep)
	{
		if (0.95f > m_fFluctuateSize)
		{
			++m_iFluctuateStep;
			m_fFluctuateInterval = 0.05f;
		}
	}
	else if (6 == m_iFluctuateStep)
	{
		m_fFluctuateSize = 1.f;
		m_fFluctuateInterval = 0.2f;
		m_bIsFluctuate = false;
		m_iFluctuateStep = 0;
	}
	m_pTransformCom->Scaling(XMLoadFloat3(&_float3(m_fFluctuateSize, m_fFluctuateSize, m_fFluctuateSize)));
}

void CStudioCam::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	m_pColliderCom->Add_Force(vDir, eMode);
}

void CStudioCam::Setting_Throwing(_fvector vDir)
{
	m_isColController = false;
	m_pColliderCom->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
	m_isThrowing = true;
	m_bPickingCheck = false;
}

void CStudioCam::OnCollisionEnter(CCollisionObject & collision)
{
	if (!m_isMove)
		return;

	if (m_isEffectCreate)
	{
		m_isDestroy = true;
	}
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLANE)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pRightHandObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_RightHand", 0);
		if (nullptr == pRightHandObject)
		{
			MSGBOX("Failed to Finder RightHand : ClassName CStatic_Mesh : Funtion OnCollisionEnter");
			Safe_Release(pGameInstance);
			return;
		}

		if (CRight_Hand_Anim::RIGHT_HAND_STATE_GRAB != dynamic_cast<CPlayer_RightHand*>(pRightHandObject)->Get_HandState() &&
			!m_isColController)
		{
			if (!m_isEffectCreate)
			{
				m_isEffectCreate = true;
				CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstnace->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_HealthPack"), SCENE_STATIC, TEXT("Layer_HealthPack"), &m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
					return;
				RELEASE_INSTANCE(CGameInstance);
				if (FAILED(Create_Effect())) /* 뭔진 모르겠지만 이펙트 물통은 따로 다른걸로 처리 부탁드릴게요 */
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
		}

		Safe_Release(pGameInstance);
	}
}

void CStudioCam::OnCollisionStay(CCollisionObject & collision)
{
}

void CStudioCam::OnCollisionExit(CCollisionObject & collision)
{
}

void CStudioCam::OnTriggerEnter(CCollisionObject & collision)
{
}

void CStudioCam::SettingActorActive()
{
	if (!m_pColliderCom)
		return;

	if (m_isActorActive)
		m_pColliderCom->WakeUp();
	else
		m_pColliderCom->Sleep();
}

_bool CStudioCam::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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

void CStudioCam::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE03:
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
				m_pColliderCom->Remove_Actor();

			m_isRender = false;
		}
		break;
	}
	case SCENE_STAGE04:
	{
		//_int NextIndex = iPlayerCurlingIndex + 1;
		if (iPlayerCurlingIndex == m_iCurlingIndex /*|| NextIndex == m_iCurlingIndex*/)
		{
			if (!m_isRender && m_pColliderCom)
				m_pColliderCom->Add_Actor();

			m_isRender = true;
		}
		else
		{
			if (m_isRender && m_pColliderCom)
				m_pColliderCom->Remove_Actor();

			m_isRender = false;
		}
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CStudioCam::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	_float vLength = GET_FLOAT3LENGTH(vColSize);
	if (vLength > 0.f)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		//tColliderDesc.isVisualization = true;
		tColliderDesc.isGravity = false;

		tColliderDesc.isSceneQuery = true;
		if (m_eLayerType == LayerType::LAYERTYPE_PICK)
			tColliderDesc.fRestitution = 0.f;

		tColliderDesc.pGameObject = this;
		CBoxCollider::DESC tBoxColliderDesc;
		tBoxColliderDesc.tColliderDesc = tColliderDesc;
		tBoxColliderDesc.vScale = MULTI_FLOAT3(vColSize, 1.99f);

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
			return E_FAIL;

		_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
		m_pColliderCom->SetPivotMatrix(smatPviot);
		m_pColliderCom->Set_ShapeLayer(m_eLayerType);
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();

		m_isActorActive = false;
		SettingActorActive();
	}
	return S_OK;
}

HRESULT CStudioCam::SetUp_ConstantTable()
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

void CStudioCam::OnControllerCollisionHit(CCollisionObject & collision)
{
	if (m_eLayerType == LayerType::LAYERTYPE_PICK)
	{
		m_isMove = true;
		m_isColController = true;
	}
}

HRESULT CStudioCam::Create_Effect()
{
	/* 이펙트 추가 */
	return S_OK;
}

CStudioCam * CStudioCam::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStudioCam*	pInstance = new CStudioCam(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CStudioCam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStudioCam::Clone(void * pArg, _uint iSceneIndex)
{
	CStudioCam*	pInstance = new CStudioCam(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CStudioCam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStudioCam::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
