#include "stdafx.h"
#include "..\public\VentDoor.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "CollisionObject.h"
CVentDoor::CVentDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CVentDoor::CVentDoor(const CVentDoor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CVentDoor::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CVentDoor::NativeConstruct(void * pArg, _uint iCurrentScene)
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
	m_StrLayerName = TEXT("Layer_VentDoor");
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

	m_isRender = true;

	/*For.Dissolve*/
	m_vBackUpScale = vObjScale;

	return S_OK;
}

_int CVentDoor::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	Dissolve(TimeDelta);

	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	//{
	//	_int iPlayerCurlingIndex = 0;
	//	_int iDeleteCurlingIndex = 0;
	//	if (Setting_CurlingIndex(&iPlayerCurlingIndex, &iDeleteCurlingIndex))
	//	{
	//		if (m_iDeleteCurlingIndex == iDeleteCurlingIndex)
	//		{
	//			m_isDestroy = true;
	//			RELEASE_INSTANCE(CGameInstance);
	//			return 0;
	//		}
	//		Stage_RenderBranch(iPlayerCurlingIndex);
	//	}
	//}
	//else
	//{
	//	//if (m_isRender && m_pColliderCom)
	//	//{
	//	//	m_pColliderCom->Remove_Actor();
	//	//}
	//	m_isRender = false;
	//}


	////RELEASE_INSTANCE(CGameInstance);

	//if (!m_isRender)
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return 0;
	//}

	//if (m_pColliderCom)
	//{
	//	if (true == m_isUpdateCollide)
	//		m_pColliderCom->Update_TransformPhysX();
	//}

	//RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CVentDoor::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	//if (m_pColliderCom)
	//{
	//	if (true == m_isUpdateCollide)
	//		m_pColliderCom->Update_Transform();
	//}
	if (false == m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CVentDoor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(m_iRenderPass)))
		return E_FAIL;


	return S_OK;
}

//void CVentDoor::OnCollisionEnter(CCollisionObject & collision)
//{
//	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLANE)
//	{
//		if (false == m_isDestroy)
//		{
//			if (true == m_bFall && false == m_bIsDissolve)
//				Set_Dissolve(true, 1.4, 3, 0.1f, 0.5f);
//
//		}
//	}
//}

void CVentDoor::Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed)
{
	m_bIsDissolve = bIsDissolve;
	m_dDissolveSpeed = dSpeed;
	m_iTextureIndex = iTextureIndex;
	m_fDissolveStart = fStart;
	m_fDissolveScaleSpeed = fScaleSpeed;
	if (false == m_bBackUpDissolve && true == m_bIsDissolve)
		m_vDissolveScale = m_vBackUpScale;
}

_bool CVentDoor::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
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
	RELEASE_INSTANCE(CGameInstance);
}

void CVentDoor::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE02:
	{
		_int NextIndex = iPlayerCurlingIndex + 1;
		if (iPlayerCurlingIndex == m_iCurlingIndex || NextIndex == m_iCurlingIndex)
		{
			//if (!m_isRender && m_pColliderCom)
			//	m_pColliderCom->Add_Actor();
			m_isRender = true;
		}
		else
		{
			//if (m_isRender && m_pColliderCom)
			//{
			//	m_pColliderCom->Remove_Actor();
			//}
			m_isRender = false;
		}
		break;
	}
	default:
		break;
	}
}


void CVentDoor::Dissolve(_double TimeDelta)
{
	if (true == m_bIsDissolve && false == m_isDestroy)
	{
		m_isUpdateCollide = false;
		//Dissolve_Scaling(TimeDelta);
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
}

void CVentDoor::Dissolve_Scaling(_double TimeDelta)
{
	m_vDissolveScale.x += TimeDelta*m_fDissolveScaleSpeed;
	m_vDissolveScale.y += TimeDelta*m_fDissolveScaleSpeed;
	m_vDissolveScale.z += TimeDelta*m_fDissolveScaleSpeed;

	m_pTransformCom->Scaling(XMLoadFloat3(&m_vDissolveScale));
}

HRESULT CVentDoor::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
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

	//_float vLength = GET_FLOAT3LENGTH(vColSize);
	//if (vLength > 0.f)
	//{
	//	CPhysXCollider::DESC tColliderDesc;

	//	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	//	//tColliderDesc.isVisualization = true;
	//	tColliderDesc.isGravity = false;

	//	tColliderDesc.isSceneQuery = true;
	//	if (m_eLayerType == LayerType::LAYERTYPE_PICK)
	//		tColliderDesc.fRestitution = 0.f;

	//	tColliderDesc.pGameObject = this;
	//	CBoxCollider::DESC tBoxColliderDesc;
	//	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	//	tBoxColliderDesc.vScale = MULTI_FLOAT3(vColSize, 1.99f);

	//	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
	//		return E_FAIL;

	//	_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
	//	m_pColliderCom->SetPivotMatrix(smatPviot);
	//	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	//	m_pColliderCom->Update_TransformPhysX();
	//	m_pColliderCom->Update_Transform();

	//	m_isActorActive = false;
	//	SettingActorActive();
	//}

	/* Com_Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVentDoor::SetUp_ConstantTable()
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

	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

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

CVentDoor * CVentDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVentDoor*	pInstance = new CVentDoor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CVentDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CVentDoor::Clone(void * pArg, _uint iCurrentScene)
{
	CVentDoor*	pInstance = new CVentDoor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CVentDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVentDoor::Free()
{
	__super::Free();

	//if (m_pColliderCom)
	//	m_pColliderCom->Remove_Actor();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}
