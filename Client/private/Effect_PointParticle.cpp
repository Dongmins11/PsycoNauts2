#include "stdafx.h"
#include "..\Public\Effect_PointParticle.h"
#include "GameInstance.h"

CEffect_PointParticle::CEffect_PointParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CEffect_PointParticle::CEffect_PointParticle(const CEffect_PointParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_PointParticle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_PointParticle::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	PARTICLECREATE tParticleCreate;
	tParticleCreate = *(PARTICLECREATE*)pArg;

	ZeroMemory(&m_tParticleInfo, sizeof(EFFECTPARTICLEINFO));
	memcpy(&m_tParticleInfo, &tParticleCreate.tParticleInfo, sizeof(EFFECTPARTICLEINFO));

	if (FAILED(SetUp_Component(tParticleCreate.wstLayerTag)))
		return E_FAIL;


//	m_pTransformCom->Scaling(_float3(m_tParticleInfo.vSize.x, 1.f, m_tParticleInfo.vSize.y));
	return S_OK;
}

_int CEffect_PointParticle::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (m_TimeAcc >= m_tParticleInfo.fLifeTime)
		m_TimeAcc = 0.0;

	m_TimeAcc += TimeDelta * m_pOptionCom->Get_EffectSpeed();
	m_LiveTimeAcc += TimeDelta * m_pOptionCom->Get_EffectSpeed();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_tParticleInfo.iOption)
	{
	case TYPE_DEFUALT:
		m_pVIBufferCom->Update_Default(&m_tParticleInfo, m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_ONEPOINT:
		m_pVIBufferCom->Update_OnePoint(&m_tParticleInfo, m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_SPIRAL:
		m_pVIBufferCom->Update_Spiral(&m_tParticleInfo, m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	case TYPE_RANDOMPOS:
		m_pVIBufferCom->Update_RandomPos(&m_tParticleInfo, m_pTransformCom, m_TimeAcc, TimeDelta);
		break;
	}

	if (m_LiveTimeAcc >= m_tParticleInfo.fCreateTime)
	{
		m_isRender = false;
		m_isDestroy = true;
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}
	_float4x4 WorldMatrix;
	if (!m_isOnce)
	{
		if (m_pOptionCom->Get_NonBillboard())
		{
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentMatrix());
			m_ParentMat._41 += m_pOptionCom->Get_MovePos().x;
			m_ParentMat._42 += m_pOptionCom->Get_MovePos().y;
			m_ParentMat._43 += m_pOptionCom->Get_MovePos().z;
			if (m_pOptionCom->Get_SetTransformPos())
			{
				m_ParentMat._41 += +XMVectorGetX(m_pOptionCom->Get_ParentTransform()->Get_State((CTransform::STATE)m_pOptionCom->Get_TransformPosType()));
				m_ParentMat._42 += +XMVectorGetY(m_pOptionCom->Get_ParentTransform()->Get_State((CTransform::STATE)m_pOptionCom->Get_TransformPosType()));
				m_ParentMat._43 += +XMVectorGetZ(m_pOptionCom->Get_ParentTransform()->Get_State((CTransform::STATE)m_pOptionCom->Get_TransformPosType()));
			}
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
			if (m_pOptionCom->Get_SetTransformPos())
			{
				m_ParentMat._41 += +XMVectorGetX(m_pOptionCom->Get_ParentTransform()->Get_State((CTransform::STATE)m_pOptionCom->Get_TransformPosType()));
				m_ParentMat._42 += +XMVectorGetY(m_pOptionCom->Get_ParentTransform()->Get_State((CTransform::STATE)m_pOptionCom->Get_TransformPosType()));
				m_ParentMat._43 += +XMVectorGetZ(m_pOptionCom->Get_ParentTransform()->Get_State((CTransform::STATE)m_pOptionCom->Get_TransformPosType()));
			}
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


	RELEASE_INSTANCE(CGameInstance);

	//m_pVIBufferCom->Update_Test(TimeDelta);


	return _int();
}

_int CEffect_PointParticle::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PARTICLE, this);

	return _int();
}

HRESULT CEffect_PointParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBufferCom->Render(m_tParticleInfo.iPass);
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	return S_OK;
}

HRESULT CEffect_PointParticle::SetUp_Component(wstring	wstrVIBufferTag)
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	wstring wstrPrototypeTag = TEXT("Prototype_Component_");
	wstrPrototypeTag += wstrVIBufferTag;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, wstrPrototypeTag.c_str(), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
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

HRESULT CEffect_PointParticle::SetUp_ConstantTable()
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
	m_pVIBufferCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_vector));
	m_pVIBufferCom->Set_RawValue("g_isDistAlpha", &m_tParticleInfo.isDistAlpha, sizeof(_bool));

	m_pVIBufferCom->Set_RawValue("g_vColor", &m_tParticleInfo.vColor, sizeof(_float3));
	m_pVIBufferCom->Set_RawValue("g_vEndColor", &m_tParticleInfo.vEndColor, sizeof(_float3));
	m_pVIBufferCom->Set_RawValue("g_vSize", &m_tParticleInfo.vSize, sizeof(_float2));

	_float	fTimeAcc = (_float)m_TimeAcc / m_tParticleInfo.fLifeTime;
	m_pVIBufferCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEffect_PointParticle * CEffect_PointParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_PointParticle* pInstance = new CEffect_PointParticle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_PointParticle::Clone(void * pArg, _uint iCurrentScene)
{
	CEffect_PointParticle* pInstance = new CEffect_PointParticle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created CEffect_PointParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_PointParticle::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pOptionCom);
}
