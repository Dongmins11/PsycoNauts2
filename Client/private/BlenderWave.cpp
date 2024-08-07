#include "stdafx.h"
#include "..\public\BlenderWave.h"
#include "GameInstance.h"

CBlenderWave::CBlenderWave(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBlenderWave::CBlenderWave(const CBlenderWave & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlenderWave::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CBlenderWave::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;


	return S_OK;
}

_int CBlenderWave::Tick(_double TimeDelta)
{

	if (!m_isInteraction)
	{
		m_fUVTimeAcc += (_float)TimeDelta * 0.5f;
		if (1.f <= m_fTimeAcc)
		{
			m_fTimeAcc = 1.f;
			if (!m_isEffectCreate)
			{
				m_isEffectCreate = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				_float3 vPos;
				XMStoreFloat3(&vPos, m_pTransformCom->Get_Position());
				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BlenderHead"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlenderHead"), &vPos)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return -1;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		else
		{
			m_fTimeAcc += (_float)TimeDelta * 0.5f;
		}
	}
	else
	{
		if (0.f >= m_fTimeAcc)
		{
			m_isDestroy = true;
			m_fTimeAcc = 0.f;
		}
		else
		{
			if (m_isEffectCreate)
			{
				m_fUVTimeAcc = 1.f;
				m_isEffectCreate = false;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BlenderHead"), 0)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return -1;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
			m_fUVTimeAcc -= (_float)TimeDelta * 0.5f;
			m_fTimeAcc -= (_float)TimeDelta * 0.5f;
		}
	}

	if (FAILED(Call_Inheritance()))
		return -1;

	return _int();
}

_int CBlenderWave::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CBlenderWave::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_BLENDWAVE);
	return S_OK;
}

HRESULT CBlenderWave::Call_Inheritance()
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_pOptionCom)
		return E_FAIL;
	/* 可记 矫累 */
	_matrix LocalMatrix = XMMatrixIdentity();
	LocalMatrix.r[3] = XMVectorSet(0.f, 2.8f, 0.f, 1.f);

	/* 可记 场 */
	_float4x4 WorldMatrix;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!m_isOnce)
	{
		if (m_pOptionCom->Get_NonBillboard())
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentMatrix());
		else
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentBillboardMatrix(pGameInstance->Get_CamPosition()));
	}

	if (m_pOptionCom->Get_Once())
		m_isOnce = true;

	RELEASE_INSTANCE(CGameInstance);
	XMStoreFloat4x4(&WorldMatrix, LocalMatrix * XMLoadFloat4x4(&m_ParentMat));
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

HRESULT CBlenderWave::SetUp_Component()
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
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_BlendWave"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	/* Com_Option */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_EffectOption"), TEXT("Com_Option"), (CComponent**)&m_pOptionCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlenderWave::SetUp_ConstantTable()
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
	m_pModelCom->Set_RawValue("g_fUVTimeAcc", &m_fUVTimeAcc, sizeof(_float));

	_float3 vColor = _float3(1.f, 0.3f, 0.3f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(1.f, 0.0f, 0.0f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBlenderWave * CBlenderWave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBlenderWave*	pInstance = new CBlenderWave(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBlenderWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBlenderWave::Clone(void * pArg, _uint iCurrentScene)
{
	CBlenderWave*	pInstance = new CBlenderWave(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBlenderWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlenderWave::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAlphaTexture);
	Safe_Release(m_pOptionCom);
}
