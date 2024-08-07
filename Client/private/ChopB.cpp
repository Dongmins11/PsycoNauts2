#include "stdafx.h"
#include "..\public\ChopB.h"
#include "GameInstance.h"

CChopB::CChopB(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CChopB::CChopB(const CChopB & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChopB::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CChopB::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	

	return S_OK;
}

_int CChopB::Tick(_double TimeDelta)
{
	if (2.f < m_fTimeAcc)
	{
		m_isDestroy = true;
		m_isRender = false;
		return 0;
	}


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));

	m_pTransformCom->Set_WorldMatrix(pTransform->Get_WorldFloat4x4());
	RELEASE_INSTANCE(CGameInstance);

	m_fTimeAcc += static_cast<_float>(TimeDelta) * 15.0f;
	return _int();
}

_int CChopB::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);
	
	return 0;
}

HRESULT CChopB::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(4);
	return S_OK;
}

HRESULT CChopB::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_ChopB"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	/* Com_Noise */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"), TEXT("Com_Noise"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;


	return S_OK;
} 

HRESULT CChopB::SetUp_ConstantTable()
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

	m_pModelCom->Set_TextureValue("g_NoiseMapTexture", m_pNoiseTexture->Get_SRV(4));
	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pAlphaTexture->Get_SRV(5));
	m_pModelCom->Set_TextureValue("g_FadeMapTexture", m_pAlphaTexture->Get_SRV(68));
	m_pModelCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(0.8f, 0.25f, 0.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CChopB * CChopB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CChopB*	pInstance = new CChopB(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CChopB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChopB::Clone(void * pArg, _uint iCurrentScene)
{
	CChopB*	pInstance = new CChopB(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CChopB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChopB::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);
}
