#include "stdafx.h"
#include "..\public\ChargeWave.h"
#include "GameInstance.h"

CChargeWave::CChargeWave(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CChargeWave::CChargeWave(const CChargeWave & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChargeWave::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CChargeWave::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(_float3(1.f, 1.f, 0.2f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 10.f, 1.f));

	return S_OK;
}

_int CChargeWave::Tick(_double TimeDelta)
{
	if (2.f <= m_fTimeAcc)
	{
		m_isDestroy = true;
		m_isRender = false;
		return 0;
	}

	m_fTimeAcc += (_float)TimeDelta;

	return _int();
}

_int CChargeWave::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CChargeWave::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(8);
	return S_OK;
}

HRESULT CChargeWave::SetUp_Component()
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
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_ChargeWave"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;


	return S_OK;
}

HRESULT CChargeWave::SetUp_ConstantTable()
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
	
	_float fFadeSet = 1.8f;
	m_pModelCom->Set_RawValue("g_fFadeSet", &fFadeSet, sizeof(_float));
	
	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pAlphaTexture->Get_SRV(48));
	m_pModelCom->Set_TextureValue("g_NoiseMapTexture", m_pAlphaTexture->Get_SRV(5));

	_float3 vColor = _float3(1.f, 0.4f, 0.0f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));

	_float	vDecrease = 0.3f;
	m_pModelCom->Set_RawValue("g_fDecreaseColor", &vDecrease, sizeof(_float));


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CChargeWave * CChargeWave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CChargeWave*	pInstance = new CChargeWave(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CChargeWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChargeWave::Clone(void * pArg, _uint iCurrentScene)
{
	CChargeWave*	pInstance = new CChargeWave(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CChargeWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChargeWave::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAlphaTexture);
}
