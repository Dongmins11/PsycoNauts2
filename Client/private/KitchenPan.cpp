#include "stdafx.h"
#include "..\public\KitchenPan.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Camera_Fly.h"
#include "GasBurner.h"

CKitchenPan::CKitchenPan(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CKitchenPan::CKitchenPan(const CKitchenPan & rhs)
	: CGameObject(rhs)
{
}

HRESULT CKitchenPan::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKitchenPan::NativeConstruct(void * pArg, _uint iCurrentScene)
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

	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = objinfo.fFrustumRange;
	_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);

	XMStoreFloat4x4(&m_WorldMatrix, objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(objinfo.fCollider_Extent, objinfo.fCollider_Center, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

	m_iPanPos[0] = _float3(242.5f, 11.f, 125.25f);
	m_iPanPos[1] = _float3(238.5f, 11.f, 125.25f);
	m_iPanPos[2] = _float3(235.f, 11.f, 125.25f);
	m_iPanPos[3] = _float3(231.5f, 11.f, 125.25f);
	m_iPanPos[4] = _float3(228.f, 11.f, 125.25f);

	SetUp_Trigger();

	return S_OK;
}

_int CKitchenPan::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (false == m_bRandStart && false == m_IsCookFinished)
	{
		char buf[100];
		m_iRandIndex = rand() % 5 + 1;
		sprintf_s(buf, "불 위치 : %f\n", (_double)m_iRandIndex);

		OutputDebugStringA(buf);
		m_bRandStart = true;
	}

	Move_Pan(TimeDelta);

	Rand_Make(TimeDelta);

	if (true == m_IsCollision)
		m_dCollisionTimeAcc += TimeDelta;

	if (m_pTargetFruit)
	{
		_float3 vMyPos = m_pTransformCom->Get_State_Float3(CTransform::STATE_POSITION);
		vMyPos = _float3(vMyPos.x, vMyPos.y + 1.f, vMyPos.z);

		m_pTargetFruit->Set_CookWarePosition(vMyPos);
		m_pTargetFruit->Update_CookingTime(m_dCollisionTimeAcc);
	}

	if (m_dCookFinishedTimeAcc <= m_dCollisionTimeAcc)
	{
		m_dCollisionTimeAcc = 0.0;
		m_IsCookFinished = true;
		m_IsCollision = false;
		if (m_pTargetFruit)
			m_pTargetFruit->Set_EndBaking();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_GAS_BURRER)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("BurrerOn.wav"), CHANNELID::PLAYER_GAS_BURRER)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		RELEASE_INSTANCE(CGameInstance);

		OutputDebugString(TEXT("요리완성"));
	}

	return _int();
}

_int CKitchenPan::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CKitchenPan::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

HRESULT CKitchenPan::SetUp_Trigger()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TriggerMat = XMMatrixScaling(1.3f, 0.5f, 1.3f) * XMMatrixIdentity();

	//_vector vRight = XMVectorSet(m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13, 0.f);
	//_vector vUp = XMVectorSet(m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23, 0.f);
	//_vector vLook = XMVectorSet(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33, (_float)(2<<(_uint)CookType::COOK_BAKE));	// 3열 : 그룹인덱스(BAKE)
	_vector vPos = XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42 + 1.f, m_WorldMatrix._43, (_uint)EnviromentTriggerType::ETTYPE_COOKING); // /4열 TriggerType

	_vector vLook = TriggerMat.r[2];
	vLook = XMVectorSetW(vLook, 3.f);

	TriggerMat.r[2] = vLook;
	TriggerMat.r[3] = vPos;

	//TriggerMat 
	// 3열 4행 : 3 (그룹 인덱스) CookType::COOK_BAKE
	// 4열 4행 : 10 (레이어 인덱스) EnviromentTriggerType::ETTYPE_COOKING

	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_ParkourTrigger"), SCENE_STAGE03, TEXT("Layer_KitchPan_Trigger"), &m_pEnvironTrigger, &TriggerMat);

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

void CKitchenPan::Move_Pan(_double TimeDelta)
{
	if (4 <= m_iPosIndex)
		m_iPosIndex = 4;

	if (0 >= m_iPosIndex)
		m_iPosIndex = 0;

	_float3 vMyPos = _float3(0.f, 0.f, 0.f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_IsMove)
	{
		m_pTransformCom->Set_State_Float3(CTransform::STATE_POSITION, m_iPanPos[m_iPosIndex]);
		vMyPos = m_pTransformCom->Get_State_Float3(CTransform::STATE_POSITION);
		vMyPos = _float3(vMyPos.x, vMyPos.y + 1.f, vMyPos.z);
		CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STAGE03, TEXT("Layer_KitchPan_Trigger"), TEXT("Com_Transform"), 0));
		if (nullptr == pTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		pTransform->Set_State_Float3(CTransform::STATE_POSITION, vMyPos);

	
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CKitchenPan::Rand_Make(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGasBurner* pGasBurner = dynamic_cast<CGasBurner*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_GasBurner"), m_iRandIndex));
	if (nullptr == pGasBurner)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (false == m_IsCookFinished && 0 != m_iRandIndex)
	{
		pGasBurner->Set_LightedOn(true);
		m_dRandTimer += TimeDelta;
	}


	if (5.0 <= m_dRandTimer)
	{
		m_dRandTimer = 0.0;
		m_bRandStart = false;
	}

	if ((m_iRandIndex == m_iPosIndex + 1) && false == m_IsCookFinished)
	{
		if (FAILED(pGameInstance->PlaySoundOnce(TEXT("FireEggBurner.wav"), CHANNELID::PLAYER_GAS_BURRER_FIRE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		//OutputDebugString(TEXT("후라이팬 == 불 위치"));
		m_IsCollision = true;
	}
	else
	{
		if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_GAS_BURRER_FIRE)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		m_IsCollision = false;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CKitchenPan::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	//CPhysXCollider::DESC tColliderDesc;
	//
	//tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	//tColliderDesc.isGravity = false;
	//tColliderDesc.isSceneQuery = false;		// interaction 
	//tColliderDesc.isTrigger = true;
	//tColliderDesc.pGameObject = this;
	//
	//CBoxCollider::DESC tBoxColliderDesc;
	//tBoxColliderDesc.tColliderDesc = tColliderDesc;
	////tBoxColliderDesc.vScale = MULTI_FLOAT3(vColSize, 1.99f);	// scale이 지름값을 가지므로 scale * 2 해야한다.
	////tBoxColliderDesc.vScale = vObjScale;
	//tBoxColliderDesc.vScale = _float3(vColSize.x * 8.f, vColSize.y * 5.f, vColSize.z * 8.f);
	//
	//
	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
	//	return E_FAIL;
	//
	//_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
	//m_pColliderCom->SetPivotMatrix(smatPviot);
	//m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_KITCHENPAN);
	//m_pColliderCom->Update_TransformPhysX();
	//m_pColliderCom->Update_Transform();

	return S_OK;
}

HRESULT CKitchenPan::SetUp_ConstantTable()
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

CKitchenPan * CKitchenPan::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CKitchenPan* pInstance = new CKitchenPan(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CKitchenPan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKitchenPan::Clone(void * pArg, _uint iSceneIndex)
{
	CKitchenPan* pInstance = new CKitchenPan(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CKitchenPan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKitchenPan::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);


}
