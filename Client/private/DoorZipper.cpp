#include "stdafx.h"
#include "..\public\DoorZipper.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player.h"
#include "Player_RightHand.h"
#include "Camera_Fly.h"
#include "FadeInOut.h"

CDoorZipper::CDoorZipper(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CDoorZipper::CDoorZipper(const CDoorZipper & rhs)
	:CGameObject(rhs)
{
}

HRESULT CDoorZipper::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CDoorZipper::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_NavigationIndex = objinfo.iNavigationIndex;
	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_iPX_TYPE = objinfo.iPX_Index;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(SetUp_MyBoneMatrices("ZipperAttach_end")))
		return E_FAIL;


	m_pModelCom->Set_Animation(0);

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isPulling = true;

	return S_OK;
}

_int CDoorZipper::Tick(_double TimeDelta)
{
	
	if (FAILED(Animation_Playing(TimeDelta)))
		return -1;

	if (FAILED(Animation_Stop()))
		return -1;

	//CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	//
	//if (pGameInstnace->Key_Down(VK_NUMPAD7))
	//	m_bIsDissolve = true;
	//
	//RELEASE_INSTANCE(CGameInstance);

	/* 끝나면 컷씬을 진행할것 */
	Dissolve(TimeDelta);

	/* 끝나면 아저씨 보게 할것 */
	Blending(TimeDelta);

	if (false == m_isPicking)
	{
		m_pModelCom->Lerp_Update(0.0);
		Bone_MatrixCalculation();
		m_pCollider->Update_TransformPhysX(XMLoadFloat4x4(m_pBoneMatrix));
		m_pCollider->Update_Transform(m_pBoneMatrix);
	}

	return _int();
}

_int CDoorZipper::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	//m_pCollider->Update_Transform();
	
	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONSHADOW_LAST, this);

	return 0;
}

HRESULT CDoorZipper::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass , m_iSeparatePassIndex ,true,"LOBO_Zip");
	
	return S_OK;
}

void CDoorZipper::Dissolve(_double TimeDelta)
{
	if (true == m_bIsDissolve && false == m_isDestroy)
	{
		//m_isUpdateCollide = false;
		m_fDissolveTime -= TimeDelta*m_dDissolveSpeed;
		if (1.f > m_fDissolveTime)
		{
			m_iSeparatePassIndex = 9;
			//m_fDissolveTime = 0.f;
			m_bIsDissolve = true;
			//m_isDestroy = true;

			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &m_fDissolveStart, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bIsDissolve, sizeof(_bool))))
				return;
		}
		if (0.2 > m_fDissolveTime)
		{
			//m_bIsDissolve = false;
			//m_iSeparatePassIndex = 0;
			if (m_OnesFade)
			{
				m_OnesFade = false;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				static_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
				static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_18);
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		//else
		//{
		//	m_bIsDissolve = false;
		//	m_iSeparatePassIndex = 0;
		//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//	static_cast<CFadeInOut*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
		//	static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_13);
		//	RELEASE_INSTANCE(CGameInstance);
		//}
	}
	/*else
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
	m_bBackUpDissolve = m_bIsDissolve;*/
}

void CDoorZipper::Blending(_double TimeDelta)
{
	if (true == m_bBlending)
	{
		m_BlendingTimer += TimeDelta;

		if (0.5f > m_BlendingTimer)
		{
			m_iSeparatePassIndex = 10;

			if (FAILED(m_pModelCom->Set_RawValue("g_fBlendingTime", &m_BlendingTimer, sizeof(_float))))
				return;
		}
		else
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_13);
			RELEASE_INSTANCE(CGameInstance);
			m_BlendingTimer = 0.5; /* 0.5로 고정을 하고 */
			m_bBlending = false;
		}
	}
}

void CDoorZipper::Setting_Dissolve()
{

	//Set_Dissolve(true, 1.4, 3, 0.1f, 0.5f);
	m_bIsDissolve = true;
	//m_dDissolveSpeed = 1.4;
	//m_iTextureIndex = 3;
	//m_fDissolveStart = 1;
	//m_fDissolveScaleSpeed = 0.5f;
	//if (false == m_bBackUpDissolve && true == m_bIsDissolve)
	//	m_vDissolveScale = m_vBackUpScale;
}

HRESULT CDoorZipper::Animation_Playing(_double TimeDelta)
{
	if (false == m_isUpdate && true == m_isPicking)
	{
		m_pModelCom->Lerp_Update(TimeDelta * 0.5);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		if (0 > pGameInstance->Get_LayerSize(SCENE_STATIC, TEXT("Layer_RightHand")))
		{
			MSGBOX("Failed to CDoorZipper : Funtion : Animation_Playing");
			Safe_Release(pGameInstance);
			return E_FAIL;
		}


		CTransform* pRightHandTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);

		CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);

		if (nullptr == pRightHandTrans || nullptr == pPlayerTrans)
		{
			MSGBOX("Failed to CDoorZipper : Funtion : Animation_Playing");
			Safe_Release(pGameInstance);
			return E_FAIL;
		}


		Bone_MatrixCalculation();

		pRightHandTrans->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)&m_ColiderMatirx.m[3]) - (XMVector3Normalize(XMLoadFloat4((_float4*)&m_ColiderMatirx.m[2]) * 0.2f)) - (XMVector3Normalize(XMLoadFloat4((_float4*)&m_ColiderMatirx.m[1]) * 0.2f)));

		pRightHandTrans->Set_MyLook(-m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_float4 vPlayerLook;

		XMStoreFloat4(&vPlayerLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pPlayerTrans->Get_State(CTransform::STATE_POSITION));
		vPlayerLook.y = 0.f;

		pPlayerTrans->Set_MyLook(XMLoadFloat4(&vPlayerLook));


		Safe_Release(pGameInstance);
	}

	return S_OK;
}

HRESULT CDoorZipper::Animation_Stop()
{
	if (true == m_pModelCom->Get_IsAnimFinishied(0) && false == m_isUpdate)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
		CGameObject* pRightHandObject = pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_RightHand"), 0);

		if (nullptr == pRightHandObject || nullptr == pPlayerObject)
		{
			MSGBOX("Failed to CDoorZipper : Funtion : Animation_Stop");
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		dynamic_cast<CPlayer*>(pPlayerObject)->Set_GrabSuccessCheck(true);
		dynamic_cast<CPlayer_RightHand*>(pRightHandObject)->Set_PullingSuccessCheck(true);

		Safe_Release(pGameInstance);

		m_isUpdate = true;
	}


	return S_OK;
}

void CDoorZipper::Bone_MatrixCalculation()
{
	_matrix ColiderMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedMatrix) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(&m_pTransformCom->Get_WorldFloat4x4());
	XMStoreFloat4x4(&m_ColiderMatirx, ColiderMatrix);
	m_pBoneMatrix = &m_ColiderMatirx;
}

HRESULT CDoorZipper::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	CPhysXCollider::DESC tColliderDesc;
	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.isSceneQuery = true;

	tColliderDesc.pGameObject = this;
	


	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = m_pTransformCom->Get_ScaleFloat3();

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pCollider, &tBoxColliderDesc)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_PICK;

	m_pCollider->Set_ShapeLayer(m_eLayerType);

	m_pCollider->Update_TransformPhysX();
	m_pCollider->Update_Transform();

	/* Com_Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoorZipper::SetUp_MyBoneMatrices(const char * pBoneName)
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("Failed to CPlayer_RightHand : Funtion SetUp_MyBoneMatrices");
		return E_FAIL;
	}

	m_OffsetMatrix = m_pModelCom->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_pCombinedMatrix = m_pModelCom->Get_CombinedMatrixPtr(pBoneName);
	m_MyWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	return S_OK;
}

HRESULT CDoorZipper::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

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

	//if (true == m_bBlending)
	//{
		if (FAILED(m_pModelCom->Set_RawValue("g_fBlendingTime", &m_BlendingTimer, sizeof(_float))))
			return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDoorZipper * CDoorZipper::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDoorZipper*	pInstance = new CDoorZipper(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CDoorZipper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDoorZipper::Clone(void * pArg, _uint iCurrentScene)
{
	CDoorZipper*	pInstance = new CDoorZipper(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CDoorZipper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoorZipper::Free()
{
	__super::Free();


	if (m_pCollider)
		m_pCollider->Remove_Actor();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
