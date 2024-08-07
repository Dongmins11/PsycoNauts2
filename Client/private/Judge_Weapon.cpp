#include "stdafx.h"
#include "..\public\Judge_Weapon.h"
#include "GameInstance.h"

CJudge_Weapon::CJudge_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CJudge_Weapon::CJudge_Weapon(const CJudge_Weapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CJudge_Weapon::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		ZeroMemory(&m_tWeaponInfo, sizeof(WEAPONINFO));
		m_tWeaponInfo = *(WEAPONINFO*)_pArg;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	XMStoreFloat4x4(&m_NewPivotMatrix, XMMatrixIdentity()*XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f))*XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f)));

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	return S_OK;
}

HRESULT CJudge_Weapon::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CJudge_Weapon::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_IsDrop)
	{
		_float4x4 CalculateMatrix;
		XMStoreFloat4x4(&CalculateMatrix, XMLoadFloat4x4(&m_NewPivotMatrix)*XMLoadFloat4x4(m_tWeaponInfo.pParentMatrix));
		m_pTransformCom->Set_WorldMatrix(CalculateMatrix);
		m_pBoxCollider->SetGravity(false);
		m_pBoxCollider->Sleep();
		m_IsReadyDrop = false;
	}
	else
	{
		m_IsReadyDrop = true;
		m_pBoxCollider->SetGravity(true);
		if(!m_isDestroy)
		m_pBoxCollider->Update_TransformPhysX();
	}

	if (m_IsRotation)
	{
		m_fDegree -= (_float)TimeDelta*540.f;
		if (110.f > m_fDegree)
		{
			m_fDegree = 110.f;
		}
	}
	else
	{
		m_fDegree = 270.f;
	}

	XMStoreFloat4x4(&m_NewPivotMatrix, XMMatrixIdentity()*XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f))*XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_fDegree)));

	return 0;
}

_int CJudge_Weapon::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

 	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (m_IsDrop&&m_IsReadyDrop)
		m_pBoxCollider->Update_Transform();

	return 0;
}

HRESULT CJudge_Weapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CJudge_Weapon::Set_Drop(_bool _bDrop)
{
	m_IsDrop = _bDrop;
	m_pBoxCollider->SetTrigger(!_bDrop);
}

_bool CJudge_Weapon::IsDrop()
{
	return m_IsDrop;
}

_bool CJudge_Weapon::IsRotate()
{
	return m_IsRotation;
}

void CJudge_Weapon::SetOn_Rotate(_bool _bRotate)
{
	m_IsRotation = _bRotate;
}

HRESULT CJudge_Weapon::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_tchar szModelPrototypeTag[MAX_PATH] = TEXT("");
	swprintf_s(szModelPrototypeTag, MAX_PATH, TEXT("Prototype_Component_Model_JudgeWeapon%d"), m_tWeaponInfo.iWeaponType);

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

	tColliderDesc.isSceneQuery = false;
	tColliderDesc.isTrigger = true;

	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pBoxCollider, &tBoxColliderDesc)))
		return E_FAIL;

	m_pBoxCollider->Update_TransformPhysX();

	return S_OK;
}

HRESULT CJudge_Weapon::SetUp_ConstantTable()
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


CJudge_Weapon * CJudge_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CJudge_Weapon*	pInstance = new CJudge_Weapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CJudge_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJudge_Weapon::Clone(void * pArg, _uint iSceneIndex)
{
	CJudge_Weapon*	pInstance = new CJudge_Weapon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CJudge_AttTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJudge_Weapon::Free()
{
	__super::Free();
	if(m_pBoxCollider)
		m_pBoxCollider->Remove_Actor();
	Safe_Release(m_pBoxCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
