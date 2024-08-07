#include "stdafx.h"
#include "..\public\BlenderHead.h"
#include "GameInstance.h"
CBlenderHead::CBlenderHead(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBlenderHead::CBlenderHead(const CBlenderHead & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlenderHead::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CBlenderHead::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_float3 vPos = _float3(0.f, 0.f, 0.f);
	memcpy(&vPos, pArg, sizeof(_float3));

	m_pTransformCom->Scaling(_float3(1.8f, 1.8f, 1.8f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x - 0.1f, vPos.y + 5.f, vPos.z, 1.f));
	m_fTimeAcc = 0.f;
	return S_OK;
}

_int CBlenderHead::Tick(_double TimeDelta)
{
	m_fTimeAcc += (_float)TimeDelta * 0.2f;

	return _int();
}

_int CBlenderHead::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CBlenderHead::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_BLENDWAVE);
	return S_OK;
}

HRESULT CBlenderHead::SetUp_Component()
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
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_BlendHead"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	/* Com_Noise */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"), TEXT("Com_Noise"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlenderHead::SetUp_ConstantTable()
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
	m_pModelCom->Set_TextureValue("g_NoiseMapTexture", m_pNoiseTexture->Get_SRV(4));
	m_pModelCom->Set_TextureValue("g_FadeMapTexture", m_pAlphaTexture->Get_SRV(68));
	m_pModelCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));
	
	_float3 vColor = _float3(1.f, 0.3f, 0.3f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(1.f, 0.0f, 0.0f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBlenderHead * CBlenderHead::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBlenderHead*	pInstance = new CBlenderHead(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBlenderHead");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBlenderHead::Clone(void * pArg, _uint iCurrentScene)
{
	CBlenderHead*	pInstance = new CBlenderHead(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBlenderHead");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlenderHead::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pAlphaTexture);
}
