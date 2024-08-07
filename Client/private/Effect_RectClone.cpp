#include "stdafx.h"
#include "..\Public\Effect_RectClone.h"
#include "GameInstance.h"

CEffect_RectClone::CEffect_RectClone(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CEffect_RectClone::CEffect_RectClone(const CEffect_RectClone & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_RectClone::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_RectClone::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	PARTICLECREATE tParticleCreate;
	tParticleCreate = *(PARTICLECREATE*)pArg;

	ZeroMemory(&m_tParticleInfo, sizeof(EFFECTPARTICLEINFO));
	memcpy(&m_tParticleInfo, &tParticleCreate.tParticleInfo, sizeof(EFFECTPARTICLEINFO));

	if (FAILED(SetUp_Component(tParticleCreate.wstLayerTag)))
		return E_FAIL;

	m_pTransformCom->Scaling(_float3(m_tParticleInfo.vSize.x, 1.f, m_tParticleInfo.vSize.y));
	return S_OK;
}

_int CEffect_RectClone::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	return _int();
}

_int CEffect_RectClone::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (m_TimeAcc >= m_tParticleInfo.fLifeTime)
		m_TimeAcc = 0.0;

	m_TimeAcc += TimeDelta * m_pOptionCom->Get_EffectSpeed();
	m_LiveTimeAcc += TimeDelta * m_pOptionCom->Get_EffectSpeed();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_tParticleInfo.iOption)
	{
	case TYPE_OPTION:
		m_pVIBufferCom->Update_Option(&m_tParticleInfo, m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_BILLBOARD:
		m_pVIBufferCom->Update_Billboard(&m_tParticleInfo, pGameInstance->Get_CamPosition(), m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_SMOKE_RANDING:
		m_pVIBufferCom->Update_Smoke_Landing(&m_tParticleInfo, pGameInstance->Get_CamPosition(), m_TimeAcc, TimeDelta);
		break;
	case TYPE_SMOKE_DIE:
		m_pVIBufferCom->Update_Smoke_Die(&m_tParticleInfo, pGameInstance->Get_CamPosition(), m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_SPARK:
		m_pVIBufferCom->Update_Spark(&m_tParticleInfo, m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_SMOKE_WORK:
		m_pVIBufferCom->Update_Smoke_Working(&m_tParticleInfo, m_pOptionCom->Get_ParentTransform()->Get_Position(), pGameInstance->Get_CamPosition(), m_isInteraction, TimeDelta);
		break;
	case TYPE_SMOKE_OBJECT:
		m_pVIBufferCom->Update_Smoke_Object(&m_tParticleInfo, pGameInstance->Get_CamPosition(), m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_PAPER:
		m_pVIBufferCom->Update_Paper(&m_tParticleInfo, m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_FIRE:
		m_pVIBufferCom->Update_Fire(&m_tParticleInfo, pGameInstance->Get_CamPosition(), m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_PARTICLE:
		m_pVIBufferCom->Update_Particle(&m_tParticleInfo, m_pOptionCom->Get_ParentTransform()->Get_Position(), pGameInstance->Get_CamPosition(), 0.016);
		break;
	case TYPE_BULLETFIRE:
		m_pVIBufferCom->Update_BulletFire(&m_tParticleInfo, m_pOptionCom->Get_ParentTransform(), pGameInstance->Get_CamPosition(), TimeDelta);
		break;
	}

	if (m_tParticleInfo.iOption != TYPE_SMOKE_WORK &&
		m_tParticleInfo.iOption != TYPE_BULLETFIRE)
	{
		_float4x4 WorldMatrix;
		if (!m_isOnce)
		{
			if (m_pOptionCom->Get_NonBillboard())
			{
				XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_NormalParentMatrix());
				m_ParentMat._41 += m_pOptionCom->Get_MovePos().x;
				m_ParentMat._42 += m_pOptionCom->Get_MovePos().y;
				m_ParentMat._43 += m_pOptionCom->Get_MovePos().z;
			}
			else if (m_pOptionCom->Get_WorldPosEnable())
			{
				XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_WorldPosMatrix());
			}
			else
			{
				XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentBillboardMatrix(pGameInstance->Get_CamPosition()));
				m_ParentMat._41 += m_pOptionCom->Get_MovePos().x;
				m_ParentMat._42 += m_pOptionCom->Get_MovePos().y;
				m_ParentMat._43 += m_pOptionCom->Get_MovePos().z;
			}
		}


		if (m_pOptionCom->Get_Once())
			m_isOnce = true;

		if (m_pOptionCom->Get_NonRotion())
		{
			_float3 vPos = _float3(m_ParentMat._41, m_ParentMat._42, m_ParentMat._43);

			XMStoreFloat4x4(&m_ParentMat, XMMatrixIdentity());
			m_ParentMat._41 = vPos.x;
			m_ParentMat._42 = vPos.y;
			m_ParentMat._43 = vPos.z;
		}

		XMStoreFloat4x4(&WorldMatrix, XMLoadFloat4x4(&m_ParentMat));


		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	}

	RELEASE_INSTANCE(CGameInstance);

	if (!m_isRender)
		return 0;

	if (TYPE_PAPER == m_tParticleInfo.iOption)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
	else
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PARTICLE, this);
	}


	return _int();
}

HRESULT CEffect_RectClone::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBufferCom->Render(m_tParticleInfo.iPass);

	return S_OK;
}

HRESULT CEffect_RectClone::SetUp_Component(wstring	wstrVIBufferTag)
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_RectInstance_Clone"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Option */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_EffectOption"), TEXT("Com_Option"), (CComponent**)&m_pOptionCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_RectClone::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, m_tParticleInfo.iTexNum);

	m_pVIBufferCom->Set_RawValue("g_vColor", &m_tParticleInfo.vColor, sizeof(_float3));
	m_pVIBufferCom->Set_RawValue("g_vEndColor", &m_tParticleInfo.vEndColor, sizeof(_float3));

	m_pVIBufferCom->Set_RawValue("g_vMaxUV", &m_tParticleInfo.vMaxUV, sizeof(_float2));

	_float	fTimeAcc = (_float)m_TimeAcc / m_tParticleInfo.fLifeTime;
	m_pVIBufferCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));
	m_pVIBufferCom->Set_RawValue("g_isDistAlpha", &m_tParticleInfo.isDistAlpha, sizeof(_bool));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEffect_RectClone * CEffect_RectClone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_RectClone* pInstance = new CEffect_RectClone(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_RectClone::Clone(void * pArg, _uint iCurrentScene)
{
	CEffect_RectClone* pInstance = new CEffect_RectClone(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_RectClone::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pOptionCom);
}
