#include "stdafx.h"
#include "..\public\SlidingMesh.h"
#include "GameInstance.h"

CSlidingMesh::CSlidingMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CSlidingMesh::CSlidingMesh(const CSlidingMesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSlidingMesh::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CSlidingMesh::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	
	m_eType = *(EFFECT_TYPE*)pArg;
	
	return S_OK;
}

_int CSlidingMesh::Tick(_double TimeDelta)
{
	if (2.f < m_fTimeAcc)
		m_fTimeAcc = 0.f;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	_matrix ParentMat = XMLoadFloat4x4(&pTransform->Get_WorldFloat4x4());
	ParentMat.r[3] += pTransform->Get_State(CTransform::STATE_UP) * 1.3f + pTransform->Get_State(CTransform::STATE_LOOK) * 0.4f;

	XMVectorSetW(ParentMat.r[3], 1.f);
	
	_matrix RotMat = XMMatrixRotationAxis(pTransform->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

	_float4x4	fMat;
	XMStoreFloat4x4(&fMat, RotMat * ParentMat);
	m_pTransformCom->Set_WorldMatrix(fMat);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->Scaling(_float3(1.5f, 1.5f, 1.5f));

	m_fTimeAcc += static_cast<_float>(TimeDelta) * 0.1f;
	return _int();
}

_int CSlidingMesh::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CSlidingMesh::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_SILDINGMESH);
	return S_OK;
}

HRESULT CSlidingMesh::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_SlidingMesh"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlidingMesh::SetUp_ConstantTable()
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

	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pAlphaTexture->Get_SRV(6));
	m_pModelCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(1.f, 1.f, 0.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(1.f, 0.25f, 0.f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CSlidingMesh * CSlidingMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSlidingMesh*	pInstance = new CSlidingMesh(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CSlidingMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSlidingMesh::Clone(void * pArg, _uint iCurrentScene)
{
	CSlidingMesh*	pInstance = new CSlidingMesh(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CSlidingMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSlidingMesh::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAlphaTexture);
}
