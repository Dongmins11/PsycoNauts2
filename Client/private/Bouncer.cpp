#include "stdafx.h"
#include "..\public\Bouncer.h"
#include "CollisionObject.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"


CBouncer::CBouncer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBouncer::CBouncer(const CBouncer & rhs)
	:CGameObject(rhs)
{
}

HRESULT CBouncer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBouncer::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	//m_iNavigationIndex = objinfo.iNavigationIndex;
	//m_iRenderPass = objinfo.iRenderPass;
	m_iRenderPass = 0;
	m_StrLayerName = objinfo.StrLayerName;										//TEXT("Layer_Compt_Bouncer0")
	m_StrModelName = objinfo.StrModelName;										//TEXT("Prototype_Component_Model_Compt_Bouncer0")
	m_StrObjectFullName = objinfo.StrObjectFullName;							//TEXT("Prototype_GameObject_Bouncer0")
	m_iPX_TYPE = (_uint)LayerType::LAYERTYPE_BOUNCER;							//objinfo.iPX_Index;
	m_eLayerType = (LayerType)m_iPX_TYPE;

	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(vColSize, vColPos, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

	m_pModelCom->Set_Animation(0);

	m_eCurAnimState = BOUNCER_IDLE;

	return S_OK;
}

_int CBouncer::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	UpdateAnimState(TimeDelta);

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();

	if (m_IsCollision)
		m_pModelCom->Set_Animation(m_iCurAnimation);

	if (m_isInteraction)
	{
		m_isInteraction = false;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BouncerEffect"),
			SCENE_STATIC, TEXT("Effect_BouncerEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_BouncerEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		_vector vPos =  m_pTransformCom->Get_State(CTransform::STATE_UP);
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BouncerEffect"), TEXT("Com_Option"), vPos);
		pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BouncerEffect"), TEXT("Com_Option"), 0.8);

		RELEASE_INSTANCE(CGameInstance);
	}

	m_pModelCom->Lerp_Update(TimeDelta * m_dAnimSpeed);

	return _int();
}

_int CBouncer::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_Transform();
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CBouncer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CBouncer::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_TRIGGER == collision.Get_Object()->Get_ObjectLayerType() || LayerType::LAYERTYPE_PLAYER == collision.Get_Object()->Get_ObjectLayerType())
	{
		CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
		dynamic_cast<CPlayer*>(pGameInstace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsBouncer(true, m_iLayerIndex);
		RELEASE_INSTANCE(CGameInstance);
		m_eCurAnimState = BOUNCER_BOUNCE;
		SetUp_AnimValue(BOUNCER_BOUNCE, 1.0);
		m_IsCollision = true;
		m_isInteraction = true;
		OutputDebugString(TEXT("충돌"));
	}
}

void CBouncer::UpdateAnimState(_double TimeDelta)
{
	switch (m_eCurAnimState)
	{
	case Client::CBouncer::BOUNCER_IDLE:
		SetUp_AnimValue(BOUNCER_IDLE, 1.0);
		break;
	case Client::CBouncer::BOUNCER_BOUNCE:
		SetUp_AnimValue(BOUNCER_BOUNCE, 1.0);
		SetUp_AnimFinish(BOUNCER_IDLE, BOUNCER_IDLE, 1.0);
		break;
	case Client::CBouncer::BOUNCER_END:
		break;
	}
}

void CBouncer::SetUp_AnimValue(_uint iAnimIndex, _double dAnimSpeed)
{
	m_iCurAnimation = iAnimIndex;
	m_dAnimSpeed = dAnimSpeed;
}

void CBouncer::SetUp_AnimFinish(BOUNCER eNextAnim, _uint iNextAnimIndex, _double dNextAnimSpeed)
{
	if (true == m_pModelCom->Get_IsAnimFinishied(m_iCurAnimation))
	{
		SetUp_AnimValue(iNextAnimIndex, dNextAnimSpeed);
		m_eCurAnimState = eNextAnim;
		m_pModelCom->Set_IsAnimLefp(true);
	}
}

HRESULT CBouncer::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Compt_Bouncer"));

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// PhysicsX Type 
	// Trigger : Signal
	// Collision : Static/Dynamic 
	// ex) Static : Plane(NaviMesh)
	// ex) Dynamic : Pick(Plant, WaterCooler..)

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isGravity = false;
	tColliderDesc.isSceneQuery = false;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isTrigger = true;								// Trigger ?

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = MULTI_FLOAT3(vObjScale, 1.99f);	// scale이 지름값을 가지므로 scale * 2 해야한다.

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_BOUNCER);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBouncer::SetUp_ConstantTable()
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

CBouncer * CBouncer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBouncer*	pInstance = new CBouncer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBouncer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBouncer::Clone(void * pArg, _uint iCurrentScene)
{
	CBouncer*	pInstance = new CBouncer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBouncer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBouncer::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

}
