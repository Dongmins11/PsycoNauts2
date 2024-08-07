#include "stdafx.h"
#include "..\public\KitchenPanButton.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Camera_Fly.h"
#include "KitchenPan.h"
#include "Player.h"

CKitchenPanButton::CKitchenPanButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CKitchenPanButton::CKitchenPanButton(const CKitchenPanButton & rhs)
	: CGameObject(rhs)
{
}

HRESULT CKitchenPanButton::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKitchenPanButton::NativeConstruct(void * pArg, _uint iCurrentScene)
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

	m_iPX_TYPE = (_uint)LayerType::LAYERTYPE_KITCHENPANBUTTON;
	m_eLayerType = (LayerType)m_iPX_TYPE;/*(LayerType)objinfo.iPX_Index;*/

	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	//m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	//if (m_fRangeForFrustum <= 0.f)
	//	m_fRangeForFrustum = objinfo.fFrustumRange;
	_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(vColSize, vColPos, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

	m_vInitialPos = m_pTransformCom->Get_Position();

	return S_OK;
}

_int CKitchenPanButton::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	if (m_bCheck[m_iLayerIndex])
	{
		m_dTimeAcc[m_iLayerIndex] += TimeDelta;
		Move_Update(TimeDelta);
	}

	if (0.3 <= m_dTimeAcc[m_iLayerIndex])
	{
		m_dTimeAcc[m_iLayerIndex] = 0.0;
		m_bCheck[m_iLayerIndex] = false;
	}

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();


	return _int();
}

_int CKitchenPanButton::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_Transform();
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CKitchenPanButton::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CKitchenPanButton::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_TRIGGER == collision.Get_Object()->Get_ObjectLayerType() || LayerType::LAYERTYPE_PLAYER == collision.Get_Object()->Get_ObjectLayerType())
	{
		if (0 == m_iLayerIndex)
		{
			CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
			dynamic_cast<CKitchenPan*>(pGameInstace->Get_GameObject(SCENE_STAGE03, TEXT("Layer_KitchenPan"), 0))->Set_Move(true, -1);
			dynamic_cast<CPlayer*>(pGameInstace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsKitchenButton(true, m_iLayerIndex);
			RELEASE_INSTANCE(CGameInstance);
		//	OutputDebugString(TEXT("충돌<-"));
			m_bCheck[m_iLayerIndex] = true;
		}
		if (1 == m_iLayerIndex)
		{
			CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
			dynamic_cast<CKitchenPan*>(pGameInstace->Get_GameObject(SCENE_STAGE03, TEXT("Layer_KitchenPan"), 0))->Set_Move(true, 1);
			dynamic_cast<CPlayer*>(pGameInstace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsKitchenButton(true, m_iLayerIndex);
			RELEASE_INSTANCE(CGameInstance);
		//	OutputDebugString(TEXT("충돌->"));
			m_bCheck[m_iLayerIndex] = true;

		}
		if (2 == m_iLayerIndex)
		{
			CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
			dynamic_cast<CKitchenPan*>(pGameInstace->Get_GameObject(SCENE_STAGE03, TEXT("Layer_KitchenPan"), 0))->Set_Move(true, 0);
			dynamic_cast<CPlayer*>(pGameInstace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsKitchenButton(true, m_iLayerIndex);
			RELEASE_INSTANCE(CGameInstance);
			//OutputDebugString(TEXT("충돌X"));
			m_bCheck[m_iLayerIndex] = true;

		}

	}
}

HRESULT CKitchenPanButton::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_iLayerIndex = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_KitchenPanButton"));

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

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isGravity = false;
	tColliderDesc.isSceneQuery = false;		// interaction 
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = MULTI_FLOAT3(vColSize, 1.99f);	// scale이 지름값을 가지므로 scale * 2 해야한다.
																//oxColliderDesc.vScale = vObjScale;
																//tBoxColliderDesc.vScale = _float3(vColSize.x * 8.f, vColSize.y * 5.f, vColSize.z * 8.f);


	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_KITCHENPANBUTTON);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CKitchenPanButton::SetUp_ConstantTable()
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

void CKitchenPanButton::Move_Update(_double TimeDelta)
{
	_vector vAxis = XMVectorSet(0.f, -1.0f, 0.f, 0.f);

	_vector vPos = m_pTransformCom->Get_Position();

	if (0.1 > m_dTimeAcc[m_iLayerIndex])
	{
		m_pTransformCom->Go_ByAxis(vAxis, TimeDelta, vPos);
	}
	if ((0.1 <= m_dTimeAcc[m_iLayerIndex] && 0.2 > m_dTimeAcc[m_iLayerIndex]))
	{
		m_pTransformCom->Go_ByAxis(-vAxis, TimeDelta, vPos);
	}
	if (0.2 <= m_dTimeAcc[m_iLayerIndex])
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitialPos);
	}

}

CKitchenPanButton * CKitchenPanButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CKitchenPanButton* pInstance = new CKitchenPanButton(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CKitchenPanButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CKitchenPanButton::Clone(void * pArg, _uint iSceneIndex)
{
	CKitchenPanButton* pInstance = new CKitchenPanButton(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CKitchenPanButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKitchenPanButton::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	if (nullptr != m_pColliderCom)
	{
		m_pColliderCom->Remove_Actor();
		Safe_Release(m_pColliderCom);
	}
}
