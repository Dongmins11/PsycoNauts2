#include "stdafx.h"
#include "..\public\DizziStar.h"
#include "GameInstance.h"

CDizziStar::CDizziStar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDizziStar::CDizziStar(const CDizziStar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDizziStar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CDizziStar::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	XMStoreFloat4x4(&m_MyWorldMat,XMMatrixIdentity());
	//m_MyWorldMat._11 *= 0.5f;
	//m_MyWorldMat._22 *= 0.5f;
	//m_MyWorldMat._33 *= 0.5f;

	m_isFadeIn = true;
	m_iMaxSpriteX = 2;
	m_iMaxSpriteY = 2;
	return S_OK;
}

_int CDizziStar::Tick(_double TimeDelta)
{	
	if (2.f <= m_LiveTimeAcc)
	{
		m_isDestroy = true;
		m_isRender = false;
		return 0;
	}
	_vector vRight = XMVector3Normalize(XMLoadFloat4((_float4*)&m_MyWorldMat.m[0][0])) * m_pOptionCom->Get_Scale().x;
	_vector vUp = XMVector3Normalize(XMLoadFloat4((_float4*)&m_MyWorldMat.m[1][0])) * m_pOptionCom->Get_Scale().y;
	_vector vLook = XMVector3Normalize(XMLoadFloat4((_float4*)&m_MyWorldMat.m[2][0])) * m_pOptionCom->Get_Scale().z;
	_vector vPosition = XMLoadFloat4((_float4*)&m_MyWorldMat.m[3][0]);

	_matrix	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), _float(XMConvertToRadians(90.0f) * TimeDelta));
	_matrix BoneMat = m_pOptionCom->Get_ParentBoneMatrix();

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	XMStoreFloat4((_float4*)&m_MyWorldMat.m[0][0], vRight);
	XMStoreFloat4((_float4*)&m_MyWorldMat.m[1][0], vUp);
	XMStoreFloat4((_float4*)&m_MyWorldMat.m[2][0], vLook);

	_float4x4 WorldMatrix;
	
	XMStoreFloat4((_float4*)&WorldMatrix.m[3][0], vPosition);
	
	XMStoreFloat4x4(&WorldMatrix, XMLoadFloat4x4(&m_MyWorldMat) * m_pOptionCom->Get_ParentBoneMatrix());
	
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_LiveTimeAcc += TimeDelta;

	if (1.f <= m_fTimeAcc)
		m_isFadeIn = false;
	else if (0.f >= m_fTimeAcc)
	{
		m_iCurSpriteX++;
		if (m_iCurSpriteX >= m_iMaxSpriteX)
		{
			m_iCurSpriteX = 0;
			m_iCurSpriteY++;
			if (m_iCurSpriteY > m_iMaxSpriteY)
				m_iCurSpriteY = 0;
		}

		m_isFadeIn = true;
	}

	if (m_isFadeIn)
		m_fTimeAcc += static_cast<_float>(TimeDelta) * 2.f;
	else
		m_fTimeAcc -= static_cast<_float>(TimeDelta) * 2.f;

	return _int();
}

_int CDizziStar::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CDizziStar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(6);
	return S_OK;
}

HRESULT CDizziStar::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_DizziStar"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Option */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_EffectOption"), TEXT("Com_Option"), (CComponent**)&m_pOptionCom)))
		return E_FAIL;
	return S_OK;
}

HRESULT CDizziStar::SetUp_ConstantTable()
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

	m_pModelCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(0.8f, 0.8f, 0.3f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));

	m_pModelCom->Set_RawValue("g_fImgMaxCountX", &m_iMaxSpriteX, sizeof(_int));
	m_pModelCom->Set_RawValue("g_fImgMaxCountY", &m_iMaxSpriteY, sizeof(_int));
	m_pModelCom->Set_RawValue("g_fCurIndexX", &m_iCurSpriteX, sizeof(_int));
	m_pModelCom->Set_RawValue("g_fCurIndexY", &m_iCurSpriteY, sizeof(_int));
	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CDizziStar * CDizziStar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDizziStar*	pInstance = new CDizziStar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CDizziStar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDizziStar::Clone(void * pArg, _uint iCurrentScene)
{
	CDizziStar*	pInstance = new CDizziStar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CDizziStar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDizziStar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pOptionCom);
}
