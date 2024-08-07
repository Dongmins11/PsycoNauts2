#include "stdafx.h"
#include "..\public\Fruits.h"
#include "GameInstance.h"

CFruits::CFruits(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CFruits::CFruits(const CFruits & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFruits::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		ZeroMemory(&m_tFruitsInfo, sizeof(FRUITSINFO));
		m_tFruitsInfo = *(FRUITSINFO*)_pArg;

		m_eType = m_tFruitsInfo.eFruitsType;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_tFruitsInfo.vPos));


	_uint iRandAnim = rand() % 2 +16;
	m_pModelCom->Set_Animation(iRandAnim);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Blob"), m_iCurrentScene, TEXT("Layer_Blob"), &m_pBlob, &m_WorldMatrix);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CFruits::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CFruits::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	m_pModelCom->Lerp_Update(TimeDelta);
	if(!m_isThrowing)
		m_pBoxCollider->SetGravity(false);

	if (m_pBoxCollider)
	{
		if (m_isPicking)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
			_vector vPos = pPlayerTrans->Get_Position() - pPlayerTrans->Get_State(CTransform::STATE_UP) * 0.7f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			if (nullptr != m_pBlob)
			{
				m_pBlob->Set_IsDestroy(true);
				m_pBlob = nullptr;
			}
			RELEASE_INSTANCE(CGameInstance);
		}
		m_pBoxCollider->Update_TransformPhysX();
	}

	return 0;
}

_int CFruits::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (m_isThrowing)
		m_pBoxCollider->Update_Transform();

	return 0;
}

HRESULT CFruits::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CFruits::Setting_Throwing(_fvector vDir)
{
	m_isThrowing = true;
	m_pBoxCollider->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
}

CFruits::FRUITTYPE CFruits::Get_Type()
{
	return m_eType;
}

HRESULT CFruits::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_tchar szModelPrototypeTag[MAX_PATH] = TEXT("");
	swprintf_s(szModelPrototypeTag, MAX_PATH, TEXT("Prototype_Component_Model_Fruits%d"), m_tFruitsInfo.eFruitsType);

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, szModelPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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
	tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = true;

	tColliderDesc.isSceneQuery = true;
	tColliderDesc.isTrigger = true;

	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(2.f,2.f, 2.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pBoxCollider, &tBoxColliderDesc)))
		return E_FAIL;

	m_pBoxCollider->Update_TransformPhysX();

	m_pBoxCollider->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_PICK);

	return S_OK;
}

HRESULT CFruits::SetUp_ConstantTable()
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
	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &XMLoadFloat4x4(&WorldMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&ViewMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&ProjMatrix), sizeof(_float4x4));

	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float4(-1.f, -1.f, -1.f, 0.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CFruits * CFruits::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CFruits*	pInstance = new CFruits(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CFruits");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFruits::Clone(void * pArg, _uint iSceneIndex)
{
	CFruits*	pInstance = new CFruits(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CJudge_AttTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFruits::Free()
{
	__super::Free();
	if (nullptr != m_pBlob)
	{
		m_pBlob->Set_IsDestroy(true);
		m_pBlob = nullptr;
	}
	if (m_pBoxCollider)
		m_pBoxCollider->Remove_Actor();
	Safe_Release(m_pBoxCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
