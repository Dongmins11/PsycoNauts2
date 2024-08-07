#include "stdafx.h"
#include "..\public\Cracked_Denture.h"
#include "GameInstance.h"

CCracked_Denture::CCracked_Denture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CCracked_Denture::CCracked_Denture(const CCracked_Denture & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCracked_Denture::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCracked_Denture::NativeConstruct(void* _pArg, _uint iSceneIndex)
{
	if (FAILED(__super::NativeConstruct(_pArg, iSceneIndex)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		ZeroMemory(&m_tDentureInfo, sizeof(DENTUREINFO));
		m_tDentureInfo = *(DENTUREINFO*)_pArg;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_float x = GET_RANDOMFLOAT(-1.f, 1.f);
	_float y = GET_RANDOMFLOAT(0.5f, 1.f);
	_float z = GET_RANDOMFLOAT(-1.f, 1.f);

	m_pTransformCom->Set_WorldMatrix(m_tDentureInfo.WorldMatrix);

	_vector vDir = XMVector3Normalize(XMLoadFloat3(&_float3(x, y, z)));

	m_pBoxCollider->Add_Force(vDir*7.f);

	m_pBoxCollider->Update_TransformPhysX();

	return S_OK;
}

_int CCracked_Denture::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (m_AccTime > 2.0)
		m_isDestroy = true;

	m_AccTime += TimeDelta;
	m_pBoxCollider->Update_TransformPhysX();

	return 0;
}

_int CCracked_Denture::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pBoxCollider->Update_Transform();

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CCracked_Denture::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

HRESULT CCracked_Denture::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_tchar szModelPrototypeTag[MAX_PATH] = TEXT("");
	swprintf_s(szModelPrototypeTag, MAX_PATH, TEXT("Prototype_Component_Model_DentureCracked%d"), m_tDentureInfo.MeshType);

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
	tColliderDesc.isTrigger = false;
	tColliderDesc.fMass = 5.f;
	tColliderDesc.fRestitution = 0.1f;
	tColliderDesc.fStaticFriction = 1.f;
	tColliderDesc.fDynamicFriction = 1.f;

	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pBoxCollider, &tBoxColliderDesc)))
		return E_FAIL;

	m_pBoxCollider->Update_TransformPhysX();

	return S_OK;
}

HRESULT CCracked_Denture::SetUp_ConstantTable()
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


CCracked_Denture * CCracked_Denture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCracked_Denture*	pInstance = new CCracked_Denture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CCracked_Denture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCracked_Denture::Clone(void * pArg, _uint iSceneIndex)
{
	CCracked_Denture*	pInstance = new CCracked_Denture(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CCracked_Denture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCracked_Denture::Free()
{
	__super::Free();
	if (nullptr != m_pBoxCollider)
		m_pBoxCollider->Remove_Actor();
	Safe_Release(m_pBoxCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
